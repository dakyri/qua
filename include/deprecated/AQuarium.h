#ifndef _AQUARIUM
#define _AQUARIUM

class Qua;
class QuaObject;

#include "ObjectView.h"
#include "RosterView.h"

class ColorSelectObjectView;
class TemplateObjectView;
class LibraryObjectView;
class AudioFXObjectView;

class AQuarium: public RosterView
{

public:
							AQuarium(BRect area, char *name, Qua *q);
							~AQuarium();
	virtual void			Draw(BRect area);
	virtual void			MouseDown(BPoint where);
	virtual void			KeyDown(const char *c, int32 nb);
	virtual void			MessageReceived(BMessage *);
	
	bool					MessageDropped(BMessage *message,BPoint where,
									BPoint delta);
									
	Qua						*uberQua;
	ColorSelectObjectView	*palette;
	TemplateObjectView		*templateView;
	LibraryObjectView		*libraryView;
	AudioFXObjectView		*audiofxView;
};

#endif