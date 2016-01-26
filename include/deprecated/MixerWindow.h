#ifndef _MIXER_WINDOW
#define _MIXER_WINDOW

#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <InterfaceKit.h>
#endif


class NumCtrl;
class ColorCtrl;
class Panel;
class ChannelSelector;
class Channel;
class Qua;

#include "TiledView.h"

class MixerView : public TiledView
{
public:
					MixerView(BRect frame, Qua *);
					~MixerView();
	virtual void	MessageReceived(BMessage *in);
	virtual void	Draw(BRect region);
	virtual void	AttachedToWindow();
	virtual void	FrameResized(float x, float y);
	
	void			SetTargets();
	void			AddSchedulablePanel(Panel *p);
	void			DeleteSchedulablePanel(Panel *p);
	void			AddChannel(Channel *q);
	void			DeleteChannel(Channel *q);
	void			ArrangeChildren();
	
	ChannelSelector	*channelSelector;
	Qua				*uberQua;
	BView			*back;
	float			lastPanelBottom;
	BScrollBar		*verticalScroll;		
};

class MixerWindow : public BWindow
{
public:
					MixerWindow(BRect,
								const char* inTitle,
								ulong inFlags,
								MixerView *v);
					~MixerWindow();
					
	virtual bool	QuitRequested( );
	virtual void	FrameResized(float x, float y);
};

#define FULL_MIXER_WIDTH	600

#endif