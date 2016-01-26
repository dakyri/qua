#ifndef _JOYPANEL
#define _JOYPANEL

#include "Controller.h"
#include "QuaJoystick.h"

class QuaJoystickPort;
class XYView;

class JoyPanel : public ControlPanel
{
public:
					JoyPanel(BRect, float, char *, QuaJoystickPort *j);
					~JoyPanel();
					
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	virtual void	Draw(BRect r);
	virtual void	ArrangeChildren();
	virtual void	FrameResized(float x, float y);
	
	void			DrawAxleRect(short i);
	void			AddAxleRect(short i);
	void			DelAxleRect(short i);
	void			MoveAxleRect(short i, short j);
	BPoint			PosToPixel(float x, float y);
	
	QuaJoystickPort	*myJoy;
	short			nAxles;

	XYView			**axes;

};

enum {
};

#endif