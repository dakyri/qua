#ifndef _SYMOBJECT
#define _SYMOBJECT

#include "Edittable.h"
#include "ObjectView.h"

class StabEnt;

class SymObject: public ObjectView, public Edittable
{
public:
							SymObject(
									StabEnt *,
									BRect r,
									BBitmap *,
									BBitmap *,
									ObjectViewContainer *,
									rgb_color);
							~SymObject();
							
	virtual void			MouseDown(BPoint w);
	virtual void			Draw(BRect region);

	virtual void			Edit();

	inline	StabEnt			*Symbol() { return sym; }
	
	virtual bool			SetDisplayMode(short);

	StabEnt					*sym;
	
	BBitmap					*bicon;
	BBitmap					*sicon;

	BPoint					iconPoint;
};

#endif