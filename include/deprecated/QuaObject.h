#ifndef _QUAOBJ
#define _QUAOBJ

#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <InterfaceKit.h>
#endif

class ArrangerObject;
class TimeArrangeView;
class Pool;
class Voice;
class Application;
class Method;
class Schedulable;
class Sample;
class StreamLogEntry;
class Stream;
class AQuarium;
class Qua;

#include "SymObject.h"

class QuaObject: public SymObject
{
public:
							QuaObject(StabEnt *sym,
									BRect r,
									BBitmap *,
									BBitmap *,
									ObjectViewContainer *a,
									Qua *q,
									rgb_color);
							~QuaObject();

	virtual void			MessageReceived(BMessage *msg);
	virtual void			MouseDown(BPoint w);
	virtual void			Draw(BRect rect);

	virtual void			Edit();
	

	void					SetName(char *nm);

	Qua						*uberQua;
	Schedulable				*schedulable;
						
	QuaObject				*next;
};

#endif