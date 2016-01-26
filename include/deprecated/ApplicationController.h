#ifndef _APP_CTL_EDITOR
#define _APP_CTL_EDITOR

class Application;
class NumCtrl;
class ApplicationBlock;

#include "Controller.h"

class ApplicationView : public ControlPanel
{
public:
					ApplicationView(BRect B, Application *A);
					~ApplicationView();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	
	Application			*myApplication;
};

class ApplicationController: public Controller
{
public:
					~ApplicationController();
					ApplicationController(BRect inRect, Application *V);
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);
	
	ApplicationView		*edit;
	Application			*myApplication;
};

#endif