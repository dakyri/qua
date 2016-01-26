#ifndef _ENVELOPEDRAWER
#define _ENVELOPEDRAWER
#ifdef __INTEL__
#include <InterfaceKit.h>
#include <SupportKit.h>
#endif
class Envelope;

class EnvelopeDrawer
{
public:
					EnvelopeDrawer(BView *v,
									rgb_color b,
									rgb_color l,
									rgb_color p,
									rgb_color s);
	
	virtual float	TimeToPixel(float x) = nullptr;
	virtual float	ValToPixel(float x) = nullptr;
	virtual float	PixelToTime(float x) = nullptr;
	virtual float	PixelToVal(float x) = nullptr;
	virtual void	SetEnvelope(Envelope *e) = nullptr;
	virtual void	DrawNewEnvSegment(Envelope *e, short pt)=nullptr;
	virtual void	DrawNewEnvSegPosition(Envelope *e, short pt, bool)=nullptr	;

	void			DrawEnvSegment(Envelope *envelope, short i, short j=-1, bool scrub=false);
	void			DrawEnvSegmentPt(Envelope *envelope, short i, bool scrub=false);
	bool			SelectEnvSegmentPoint(Envelope *envelope, BPoint w);
				
	short			selectedPt;
	BView			*view;
	rgb_color		bgColor,
					lineColor,
					ptColor,
					selectedPtColor;
};

#endif