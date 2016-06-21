#ifndef _PORTOBJECT
#define _PORTOBJECT

#include "SymObject.h"
//#include "QuaInsert.h"

class PortObject: public SymObject
{
public:
					PortObject(	QuaPort *,
								BRect r,
								BBitmap *,
								BBitmap *,
								rgb_color, float ex=0);
	virtual void	Draw(BRect r);
	virtual void	MouseDown(BPoint p);
	virtual bool	SetDisplayMode(short);
	virtual void	FrameMoved(BPoint);
	
	QuaPort			*quaPort;
};

#endif

