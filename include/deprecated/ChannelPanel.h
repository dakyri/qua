#ifndef _CHANNELCONTROL
#define _CHANNELCONTROL

class Channel;
class NumCtrl;
class SelectorPanel;
class FramePanel;

#include "Controller.h"

class ChannelPanel : public ControlPanel
{
public:
					ChannelPanel(BRect B, float w, Channel *A);
					~ChannelPanel();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	virtual void	Draw(BRect r);
	virtual void	ArrangeChildren();
	virtual void	FrameResized(float x, float y);
	
	FramePanel		*AddFramePanel(QuasiStack *);
	void			AddDestinationPanel(bool, ControlPanel *);
	void			RemoveDestinationPanel(bool);
	
	Channel			*myChannel;
	ControlPanel	*rxPortPanel;
	ControlPanel	*txPortPanel;
	FramePanel		*txBlockPanel;
	FramePanel		*rxBlockPanel;
};


class ChannelSelector: public ControlPanel
{
public:
					ChannelSelector(BRect r, Qua *q);
					~ChannelSelector();
					
	virtual void	MessageReceived(BMessage *msg);
	virtual void	Draw(BRect r);
	virtual void	FrameResized(float, float);
	virtual void	ArrangeChildren();
	virtual void	AttachedToWindow();
	
	void			DisplayChannel(Channel *c);
	void			UndisplayChannel(Channel *c);
	void			AddChannel(Channel *C);
	void			DeleteChannel(Channel *C);
	
	SelectorPanel	*selector;
	Qua				*uberQua;
};

enum {
	DISPLAY_CHANNEL = 'dsch'
};

#endif