#ifndef _SCHEDULABLECONTROLLER
#define _SCHEDULABLECONTROLLER

class Schedulable;
class Method;
class QuasiStack;
class Instance;
class SelectorPanel;

#include "Controller.h"

enum {
	SP_VIEW_MINIMAL=0
};

class SchedulablePanel: public ControlPanel
{
public:
					SchedulablePanel(BRect r, Schedulable *S, bool multi);
					~SchedulablePanel();
	virtual void	AttachedToWindow();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	Draw(BRect);
	
	virtual void	FrameResized(float, float);
	virtual void	ArrangeChildren();
	void			AddFramePanel(QuasiStack *n);
	void			AddInstance(Instance *);
	void			DeleteInstance(Instance *);
	void			Select(Instance *i);
	void			ZoomInstance(Instance *i);
	void			UnzoomInstance(Instance *i);
	
	SelectorPanel	*selector;

	Instance		*selectedInstance;
	Schedulable		*schedulable;
	
	bool			multiInstance;
	short			viewMode;
};

enum {
	DISPLAY_INSTANCE = 'dins'
};

#endif