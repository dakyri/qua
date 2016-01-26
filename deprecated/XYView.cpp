
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif


#include "XYView.h"
#include "Colors.h"
#include "QuaTypes.h"

XYView::XYView(BRect r, float maxw, float *x, float *y, BPoint mi, BPoint ma):
	Panel(r, maxw, "xyview", B_PULSE_NEEDED)
{
	tracking = MOUSE_TRACK_NOT;

	r.OffsetTo(0,0);
	xp = x;
	yp = y;
	xy.x = *x;
	xy.y = *y;
	min = mi;
	max = ma;
	drect.Set(r.left+1,r.top+1,r.right-1,r.bottom-1);
	thePoint = XYToPixel(BPoint(*x,*y));
}

BPoint
XYView::XYToPixel(BPoint p)
{
	if		(p.x<min.x) p.x=min.x;
	else if (p.x>max.x) p.x=max.x;
	if		(p.y<min.y) p.y=min.y;
	else if (p.y>max.y) p.y=max.y;
	
	return BPoint(	drect.left +
						(p.x - min.x)*(drect.bottom-drect.top)/(max.x - min.x),
					drect.bottom -
						(p.y - min.y)*(drect.right-drect.left)/(max.y - min.y));
}

BPoint
XYView::PixelToXY(BPoint p)
{
	return BPoint(	min.x +
						(p.x - drect.left)*(max.x - min.x)/(drect.bottom-drect.top),
					max.y - 
						(drect.bottom - p.y)*(max.x - min.x)/(drect.bottom-drect.top)
						);
}

void
XYView::MouseDown(BPoint p)
{
	ulong	buts;

	if (drect.Contains(p)) {
		BRegion		reg;
		reg.Include(drect);
		ConstrainClippingRegion(&reg);
		SetPoint(p);

		tracking = MOUSE_TRACK_MOVE_POINT;
		SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
	}
}


void
XYView::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_POINT) {
		SetPoint(pt);
	}
}

void
XYView::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_POINT) {
		ConstrainClippingRegion(nullptr);
	}
	tracking = MOUSE_TRACK_NOT;
}

void
XYView::Draw(BRect r)
{
	LoweredBox(this, drect, purple, true);
	
	BRegion		reg;
	reg.Include(drect);
	ConstrainClippingRegion(&reg);
	SetPoint(thePoint);
	ConstrainClippingRegion(nullptr);
}

void
XYView::SetPoint(BPoint newPt)
{
	if (newPt.x < drect.left) newPt.x = drect.left;
	else if (newPt.x < drect.right) newPt.x = drect.right;
	if (newPt.y < drect.top) newPt.y = drect.top;
	else if (newPt.y < drect.right) newPt.y = drect.bottom;

	SetHighColor(purple);
	SetLowColor(green);
	StrokeArc(thePoint,2,2,0,360,B_SOLID_HIGH);
	thePoint = newPt;
	StrokeArc(thePoint,2,2,0,360,B_SOLID_LOW);
	xy = PixelToXY(thePoint);
	*xp = xy.x;
	*yp = xy.y;
}

void
XYView::SetXY(float x, float y)
{
	if (x < min.x) x = min.x;
	else if (x > max.x) x = max.x;
	if (y < min.y) y = min.y;
	else if (y > max.y) y = max.y;
	

	xy.x = x;
	xy.y = y;
	SetHighColor(purple);
	SetLowColor(green);
	StrokeArc(thePoint,2,2,0,360,B_SOLID_HIGH);
	thePoint = XYToPixel(xy);
	StrokeArc(thePoint,2,2,0,360,B_SOLID_LOW);
	
//	fprintf(stderr, "setxy %.2g %.2g %.2g %.2g (%.2g %.2g %.2g %.2g)                    \r",
//				x, y, thePoint.x, thePoint.y,
//				drect.left,drect.top,drect.right,drect.bottom);
}

void
XYView::Pulse()
{
	if (*xp != xy.x || *yp != xy.y) {
//		fprintf(stderr, "pulse %.2g %.2g\r", *xp, *yp);
		SetXY(*xp,*yp);
	}
}