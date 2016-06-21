#ifndef _TIMEVIEW
#define _TIMEVIEW

#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <InterfaceKit.h>
#endif

#include "Time.h"
#include "Metric.h"
#include "ObjectView.h"

class Instance;
class Qua;
class Channel;
class TimeArrangeView;
class NumCtrl;
class TimeCtrl;
class AQuarium;
class Metric;

class TimeCursor
{
public:
			TimeCursor(TimeArrangeView *tv);
	void	Draw(BRect wr);
	void	SetTime(Time t);

	Time			time;
	TimeArrangeView	*the_view;
};


#define LABEL_HEIGHT	8
#define PIXELS_PER_QUANT	2

#define DFLT_OBJECT_HEIGHT	50
#define DFLT_TRACK_HEIGHT	50
#define	DFLT_OBJVIEW_WIDTH	250
#define	DFLT_CHANNELVIEW_WIDTH	60


class ChannelView: public ObjectViewContainer
{
public:
					ChannelView(BRect area, char *name, Qua *q);
					~ChannelView();
	virtual void	Draw(BRect area);
	virtual void	MouseDown(BPoint where);
	virtual void	FrameResized(float, float);
	virtual void	MessageReceived(BMessage *message);
	virtual void	ArrangeChildren();
	
	bool			MessageDropped(BMessage *message,BPoint where,
									BPoint delta);
									
	short			TrackHeight();
									
	short			channelWidth;
	Qua				*uberQua;								
};



class TimeArrangeBarView: public BView
{

public:
							TimeArrangeBarView(BRect area, class TimeArrangeView *);
							~TimeArrangeBarView();
	virtual void			Draw(BRect area);
    virtual void			KeyDown(const char *c, int32 nb);
	virtual void			MouseDown(BPoint where);
	virtual void			MessageReceived(BMessage *message);
	virtual void			FrameResized(float width, float h);
	
	class TimeArrangeView	*arrange;
};	

class TimeArrangeView: public ObjectViewContainer
{

public:
					TimeArrangeView(BRect area, char *name, Qua *q);
					~TimeArrangeView();
	virtual void	Draw(BRect area);
    virtual void	KeyDown(const char *c, int32 nb);
	virtual void	MouseDown(BPoint where);
	virtual void	ScrollTo(BPoint where);
	virtual void	MessageReceived(BMessage *message);
	virtual void	FrameResized(float width, float h);
	
	short			PointToChannel(BPoint point);
	float			ChannelToPixel(short);
	Time			PixelToTime(float d);
	float			TimeToPixel(Time &t);
	BRect			ObjectRect(Time &startt, Time &dur, short chan);
	
	short 			TrackHeight();
	
	bool			MessageDropped(BMessage *message,BPoint where,
									BPoint delta);
	void			RemoveArrangerObject(class ArrangerObject *p, bool andD);
	void			AddArrangerObject(class ArrangerObject *p, BPoint qh);
	void			AddArrangerObject(class QuaObject *p, BPoint qh);
	void			MoveArrangerObject(class ArrangerObject *p, BPoint w);
	void			RemoveArrangerObject(class QuaObject *p, bool andDel);
	ArrangerObject	*FindArrangerObject(Instance *i);

	void			SetScale(float sc);
	TimeCursor		*current_time_cursor;
	ArrangerObject	*arrangerObjects;
	
	Metric			*metric;
	void			SetMetric(Metric *M);
	
	long			nChannel;
	float			fPixelsPerBeat;
	float			scale,
					maxTime;
	Qua				*uberQua;
};

class Channel;

class TiledView;

class SequencerWindow : public BWindow
{
public:
					SequencerWindow( BRect inRect, float, float,
									const char* inTitle,
									 ulong inFlags, Qua *q );
					~SequencerWindow();
	void			AddChannel(Channel *i);
	virtual bool	QuitRequested( );
	virtual void	MessageReceived(BMessage *inMsg);
	virtual void	MenusBeginning();
	virtual void	FrameResized(float width, float h);
	void			SetTempo(float);
	void			SetMetric(class Metric *);
	void			SetTime(class Time);	
	virtual void	SetTarget(BHandler *);
	void			CloseSubwindows();
	
	void			ArrangeChildren();

	TiledView		*backView;
	
	short			trackHeight;
	
	AQuarium		*aquarium;
	ChannelView		*channelView;
	TimeArrangeView	*arrange;
	TimeArrangeBarView	*barView;
	BMenuBar		*menuBar;
	
	BScrollBar		*arrangeScroll;
	BScrollBar		*vertScroll;
	
	BButton*		bGo;
	BButton*		bPause;
	BButton*		bRec;
	BButton*		bFf;
	BButton*		bRew;

	TimeCtrl		*dTCount;
	TimeCtrl		*dBBQCount;
	NumCtrl			*dTempoCount;

	BMenu			*mFileMenu;
	BMenu			*mEditMenu;
	BMenu			*mWinMenu;
	
	Qua				*uberQua;
};


inline short
TimeArrangeView::TrackHeight()
{
	return Window()?
		((SequencerWindow*)Window())->trackHeight:
		DFLT_TRACK_HEIGHT;
}

inline short
ChannelView::TrackHeight()
{
	return Window()?
		((SequencerWindow*)Window())->trackHeight:
		DFLT_TRACK_HEIGHT;
}

#endif