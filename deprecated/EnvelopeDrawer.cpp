#include "EnvelopeDrawer.h"
#include "Envelope.h"
#include "Sym.h"

EnvelopeDrawer::EnvelopeDrawer(BView *v,
								rgb_color b,
								rgb_color l,
								rgb_color p,
								rgb_color s)
{
	view = v;					
	bgColor = b;
	lineColor = l;
	ptColor = p;
	selectedPtColor = s;
	selectedPt = 0;
}


void
EnvelopeDrawer::DrawEnvSegmentPt(Envelope *envelope, short i, bool scrub)
{
	if (view->Window()) {
		float	x = TimeToPixel(envelope->segment[i].time.ticks);
		float	y = ValToPixel(envelope->segment[i].val);
		
		if (scrub) {
			view->SetHighColor(bgColor);
		} else if (i == selectedPt) {
			view->SetHighColor(ptColor);
		} else {
			view->SetHighColor(selectedPtColor);
		}
		view->StrokeArc(BPoint(x,y), 2, 2, 0, 360);
	}
}


void
EnvelopeDrawer::DrawEnvSegment(Envelope *envelope, short i, short j, bool scrub)
{
	if (view->Window()) {
		if (j==-1) {
			j = i+1;
		} else if (j==-2) {
			;
		}
		
		BRect bnd = view->Bounds();
		
		if (scrub)
			view->SetHighColor(bgColor);
		else
			view->SetHighColor(lineColor);
			
		if (i == envelope->nSeg-1 || j == -2) {
			view->StrokeLine(BPoint(
								TimeToPixel(envelope->segment[i].time.ticks),
								ValToPixel(envelope->segment[i].val)),
							BPoint(
								bnd.right,
								ValToPixel(envelope->segment[i].val)));
		} else if (i >= 0 && i < envelope->nSeg) {
	
			switch (envelope->segment[i].type) {
			case ENV_SEG_TYPE_FIXED:
				view->StrokeLine(
							BPoint(	TimeToPixel(envelope->segment[i].time.ticks),
									ValToPixel(envelope->segment[i].val)),
							BPoint(	TimeToPixel(envelope->segment[j].time.ticks),
									ValToPixel(envelope->segment[i].val)));
				break;
			case ENV_SEG_TYPE_LINEAR:
			default:
				view->StrokeLine(
							BPoint(	TimeToPixel(envelope->segment[i].time.ticks),
									ValToPixel(envelope->segment[i].val)),
							BPoint(	TimeToPixel(envelope->segment[j].time.ticks),
									ValToPixel(envelope->segment[j].val)));
				break;
			}
		} else {
	//		reportError("Mild alarm in drawSeg");
		}
	}
}


bool
EnvelopeDrawer::SelectEnvSegmentPoint(Envelope *envelope, BPoint where)
{
	for (short i=0; i<envelope->nSeg; i++) {
		float	x = TimeToPixel(envelope->segment[i].time.ticks);
		float	y = ValToPixel(envelope->segment[i].val);
		if (Abs(x-where.x) < 4.0 && Abs(y-where.y) < 4.0) {
			selectedPt = i;
			return true;
		}
	}
	return false;
}
