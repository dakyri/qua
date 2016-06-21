#ifndef _ENVELOPEPANEL
#define _ENVELOPEPANEL

#include "Controller.h"
#include "EnvelopeDrawer.h"
#include "Time.h"

class NumCtrl;
class TimeCtrl;
class ModeCtrl;
class Envelope;
class NameCtrl;
class ControlVariable;
class TPot;
class PotEnvelope;

enum {
	SET_ENV_TYPE		= 'envt',
	SET_ENV_SEG_TYPE	= 'envr',
	SET_ENV_SEG_TIME	= 'envc',
	SET_ENV_SEG_VAL		= 'envv',
 };

enum {
	DISPLAY_NOT = 0,
	DISPLAY_POT = 1,
	DISPLAY_ENV = 2
};

class EnvelopePanel: public Panel, public EnvelopeDrawer
{
public:
					EnvelopePanel(BRect r, BRect p, float maxw, Envelope *e, short mode);
					~EnvelopePanel();
	virtual void	MessageReceived(BMessage *m);
	virtual void	AttachedToWindow();
	
	virtual void	FrameResized(float x, float y);
	virtual void	Draw(BRect region);
    virtual void	KeyDown(const char *c, int32 nb);
    virtual void	Pulse();
    
	virtual void	MouseDown(BPoint p);
	virtual void	MouseMoved(BPoint, uint32, const BMessage *);
	virtual void	MouseUp(BPoint);

	short			tracking;
	
	void			DrawEnvEdit();
	void			SelectEnvSegment(int i);
	void			DisplaySelected();
	void			UpdateDisplay(Time &t);
	
	void			SetDisplayMode(short);
	
	virtual float	TimeToPixel(float x);
	virtual float	ValToPixel(float x);
	virtual float	PixelToTime(float x);
	virtual float	PixelToVal(float x);
	virtual void	SetEnvelope(Envelope *e);
	virtual void	DrawNewEnvSegment(Envelope *e, short pt);
	virtual void	DrawNewEnvSegPosition(Envelope *e, short pt, bool);

	float			baseX,
					baseY,
					scaleX,
					scaleY;
	
	BStringView		*envName;
	Envelope		*envelope;
	NumCtrl			*ptVal;
	TimeCtrl		*ptTime;
	ModeCtrl		*ptType;
	ModeCtrl		*envType;
	
	BRect			editRegion;
	BRect			envRect;
	BRect			potRect;

	short			displayMode;
	PotEnvelope		*tPot;
	BRect			displayOptionTab;
};

#endif