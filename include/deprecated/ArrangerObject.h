#ifndef _SEQOBJ
#define _SEQOBJ

#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <InterfaceKit.h>
#endif

#include "Time.h"
#include "ObjectView.h"
#include "BaseVal.h"
#include "EnvelopeDrawer.h"

class StreamItem;
class StreamLogEntry;
class TimeArrangeView;
class Voice;
class Pool;
class Application;
class QuaObject;
class Schedulable;
class Instance;

class ArrangerObject: public ObjectView, public EnvelopeDrawer
{
public:
							ArrangerObject(char *lbl,
									Schedulable *s,
									Instance *i,
									TimeArrangeView *tv,
									rgb_color c);
							ArrangerObject(char *lbl,
									Schedulable *s,
									short chan,
									Time st, Time dur,
									TimeArrangeView *tv,
									rgb_color c);
							~ArrangerObject();
							
	virtual void			MouseDown(BPoint p);
	virtual void			MouseMoved(BPoint, uint32, const BMessage *);
	virtual void			MouseUp(BPoint);
	
	short					tracking;
	BRect					track_bnd;
	Envelope				*track_envelope;
	BPoint					track_lastp;
	float					track_deltax;
	StreamItem				*track_streamitem;
	BRect					track_rect;
	BMessage				*track_msg;

	virtual void			Draw(BRect);
	virtual void			MessageReceived(BMessage *);
	virtual void			FrameMoved(BPoint parentPoint);
	virtual void			FrameResized(float w, float h);
	virtual ArrangerObject	*Clone(short c, Time t);
	
	virtual void			Select(bool sel=TRUE);

	virtual float			TimeToPixel(float x);
	virtual float			ValToPixel(float x);
	virtual float			PixelToTime(float x);
	virtual float			PixelToVal(float x);
	virtual void			SetEnvelope(Envelope *e);

	void					DrawDisplayedControlVariable(BRect r);
	virtual void			DrawNewEnvSegment(Envelope *e, short pt);
	virtual void			DrawNewEnvSegPosition(Envelope *e, short pt, bool);

	bool					MouseDownControlVariable(BPoint where, ulong, ulong);
	bool					MouseDownControlVariableMenu(BPoint where);
	bool					MouseDownEnvelopePoint(BPoint where, bool add);
	void					ZotParameterMenu(BMenu *q, StabEnt *s);
	
	short					ObjectHeight();
	
	inline	TimeArrangeView	*Arranger()
		{ return (TimeArrangeView *)container; }

	base_type_t				type;
	ArrangerObject			*next;
	Instance				*instance;
	
	float					baseY;
	float					scaleY;
	
	ControlVariable			*displayedControlVariable;
};

#endif