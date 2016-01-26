#ifndef _VOICECTL
#define _VOICECTL

class Voice;
class Source;
class QuasiStack;

#include "Controller.h"

class VoiceController: public Controller
{
public:
					VoiceController(Voice *V, BRect r);
					~VoiceController();
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);
	
	Voice			*myVoice;
};

class VoiceCtlView: public ControlPanel
{
public:
					VoiceCtlView(Voice *V, BRect r);
					~VoiceCtlView();
	virtual void	MessageReceived(BMessage *msg);
	
	Voice			*myVoice;
};
class LocusCtlView: public ControlPanel
{
public:
					LocusCtlView(QuasiStack *V, BRect r);
					~LocusCtlView();
	virtual void	MessageReceived(BMessage *msg);
	
	QuasiStack		*myStack;
};

#endif