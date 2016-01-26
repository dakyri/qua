#ifndef _CHANNELOBJECTVIEW
#define _CHANNELOBJECTVIEW


#if defined(WIN32)

#elif defined(_BEOS)

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <InterfaceKit.h>
#endif

#endif

#include <stdio.h>


class Qua;
#include "ObjectView.h"

class ChannelObjectView: public ObjectView
{
	virtual void		Draw(BRect rect);
	virtual void		MouseDown(BPoint pt);
	virtual void		FrameResized(float, float);
	virtual void		MessageReceived(BMessage *);
	virtual void		KeyDown(const char *c, int32 nb);

	virtual void		Edit();

	void				JigHotAreas(float,float);
	void				DrawArrowButton(BRect,
								BPoint from, BPoint to,
								float d, rgb_color c);
	void				DrawButton(short);
	short				TrackHeight();
	BRect				thruEnableRect;
	BRect				inEnableRect;
	BRect				outEnableRect;
 	BRect				inRect;
 	BRect				outRect;

// windowish stuff	
	void				ArrangeChildren();
	ChannelView			*channelView;
}

#endif