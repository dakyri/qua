#ifndef _XYVIEW
#define _XYVIEW

#include "Panel.h"

class XYView: public Panel
{
public:
					XYView(BRect, float maxw, float *, float *, BPoint, BPoint);
	virtual void	Draw(BRect r);
	virtual void	Pulse();
	
	virtual void	MouseDown(BPoint p);
	virtual void	MouseMoved(BPoint, uint32, const BMessage *);
	virtual void	MouseUp(BPoint);

	short			tracking;
	
	void			SetPoint(BPoint);
	void			SetXY(float, float);
	BPoint			XYToPixel(BPoint);
	BPoint			PixelToXY(BPoint);
	
	BRect			drect;
	BPoint			min, max;
	BPoint			thePoint;
	BPoint			xy;
	
	float			*xp, *yp;
};

#endif
