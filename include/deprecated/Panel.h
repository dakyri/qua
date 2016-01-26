#ifndef _PANEL
#define _PANEL

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <InterfaceKit.h>
#endif



#define MARGIN 10
#define MIXER_ROW_HEIGHT	33
#define MIXER_MARGIN	3

class ControlVariable;

class Panel: public BView
{
public:
					Panel(BRect r, float maxw, char*nm=nullptr, ulong fl=nullptr);
	virtual void	FrameResized(float x, float y);
	virtual void	ArrangeChildren();

	virtual void	SetDisplayMode(short dm);
	virtual void	MouseDown(BPoint p);
	virtual void	MessageReceived(BMessage *msg);
	virtual void	RunControlMenu(BPoint p);
					
	void			GetItemRect(
							float indent,
							float item_width,
							float item_height,
							float sep,
							BRect &r);
	void			SetControlDefaults(BControl *n, float swid, ControlVariable *p=nullptr, BMessage *msg=nullptr);

	float			myHeight;
	float			myWidth;
	BPoint			thePoint;
	float			rowHeight;
	float			maxWidth;
	short			displayMode;
};
enum {
	PANEL_DISPLAY_NIKON=0,
	PANEL_DISPLAY_SMALL=1,
	PANEL_DISPLAY_BIG=2
};


#endif