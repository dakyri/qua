#ifndef _SAMPLECONTROL
#define _SAMPLECONTROL

class Sample;
class NumCtrl;

#include "Controller.h"

class SampleControlView : public ControlPanel
{
public:
					SampleControlView(Sample *A, BRect B);
					~SampleControlView();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	
	Sample			*mySample;
	BMessenger		RightHere;
};

class SampleController: public Controller
{
public:
						~SampleController();
						SampleController(Sample *V, BRect inRect);
	virtual bool		QuitRequested();
	virtual void		MenusBeginning();
	virtual void		MessageReceived(BMessage *msg);
	
	SampleControlView		*edit;
	Sample				*mySample;
};

#endif