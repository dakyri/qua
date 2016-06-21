#ifndef _ROSTERTHINGY
#define _ROSTERTHINGY

#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <MediaKit.h>
#include <InterfaceKit.h>
#endif

#include "ObjectView.h"
#include "qua_version.h"

class PortObject;
class QuaNexion;
class QuaInsert;

class RosterView: public ObjectViewContainer
{
public:
						RosterView(BRect r, char *);
						~RosterView();
					
	virtual	void		MessageReceived(BMessage *inMsg);
	virtual void		AttachedToWindow();

	virtual void		AddChild(BView *v, BView *s=nullptr);
	
	QuaNexion			*AddQuaNexion(
								ObjectView *i,
								QuaInsert* mi,
								ObjectView *o,
								QuaInsert *mo,
								rgb_color, uchar, uchar);
};

#endif