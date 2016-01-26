#ifndef _POOLCONTROL
#define _POOLCONTROL

class Pool;
class NumCtrl;

#include "Controller.h"

class PoolControlView : public ControlPanel
{
public:
					PoolControlView(Pool *A, BRect B);
					~PoolControlView();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	
	Pool			*myPool;
};

class PoolController: public Controller
{
public:
						~PoolController();
						PoolController(Pool *V, BRect inRect);
	virtual bool		QuitRequested();
	virtual void		MenusBeginning();
	virtual void		MessageReceived(BMessage *msg);
	
	PoolControlView		*edit;
	Pool				*myPool;
};

#endif