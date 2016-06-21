#ifndef _INSTANCECONTROLPANEL
#define _INSTANCECONTROLPANEL

class QuasiStack;
class Instance;

#include "Controller.h"


class InstancePanel : public ControlPanel
{
public:
							InstancePanel(BRect B, Instance *A, bool displayC);
							~InstancePanel();
	virtual void			MessageReceived(BMessage *msg);
	virtual void			MouseDown(BPoint pt);
	virtual void			AttachedToWindow();
	virtual void			Draw(BRect);
	
	void					AddFramePanel(QuasiStack *v, float &len, float &wid);
	
	Instance				*instance;
	bool					isSingular;
};

class FramePanel: public ControlPanel
{
public:
							FramePanel(BRect r, float maxw, QuasiStack *V, Qua *q);
							~FramePanel();
	virtual void			MessageReceived(BMessage *msg);
	virtual void			AttachedToWindow();
	virtual void			Draw(BRect);
	
	float					maxWidth;
	QuasiStack				*frame;
};

class GroupPanel: public ControlPanel
{
public:
							GroupPanel(BRect r, float maxw, StabEnt *S);
	class StabEnt			*groupSym;
	virtual void			Draw(BRect);
	virtual void			ArrangeChildren();
};

#endif