#ifndef _STREAMEDITOR
#define _STREAMEDITOR

#include "DataEditor.h"
#include "Note.h"

class Pool;
class StreamTakeView;
class StreamTake;
class Stream;
class BarBituate;
class PianoBituate;
class NoteView;
class StreamItem;
class StreamNote;
class CtrlBit;

class StreamView: public BView
{
public:
						StreamView(BRect r, Stream *s, Metric *m, short, short);
						~StreamView();
						
	virtual void		Draw(BRect r);
	virtual void		FrameMoved(BPoint);
	virtual void		FrameResized(float,float);
	virtual void		ScrollTo(BPoint);
	virtual void		MessageReceived(BMessage*);
	virtual void		KeyDown(const char *, int32);

	virtual void		MouseDown(BPoint p);
	virtual void		MouseMoved(BPoint, uint32, const BMessage *);
	virtual void		MouseUp(BPoint);

	short				tracking;
	StreamItem			*track_noteon;
	StreamNote			*track_keypress;
	Time				track_lastt;
	BRect				track_lastr;
	float				track_lastdur;
	pitch_t				track_lastpitch;
	BPoint				track_delta;
	CtrlBit				*track_ctrl;
	
		
	Stream				*myStream;
	Metric				*metric;
	float				width, height;
	
	StreamItem			*selected;
	BList				agglomerate;
	
	void				SetStream(Stream *stream);

	float				pixelsPerBeat;
	float				yPsPerMidiNoteUnit;

	long				XPixToTick(float x);
	float				TickToXPix(long t);
	float				ValueToYPix(short val);
	short				YPixToValue(float y);
	
	void				DrawNote(StreamItem *, BRect);
	BRect				StreamItemRect(StreamItem *);
	CtrlBit				*BitForController(short ct);
	CtrlBit				*BitAtPoint(BPoint);
	void				BuildOtherBits();
	
	BList				otherBits;
	
	void				Select(long, long);
	
	bool				doLoop;
	
	Time				leftSelectedTime;
	Time				rightSelectedTime;
	Time				currentPlayTime;
	
	SchLock				drawLock;
};

class PoolEditor: public DataEditor
{
public:
						PoolEditor(BRect r, float maxw,Pool *p);
						~PoolEditor();
						
	virtual void		ArrangeChildren();
	virtual void		FrameResized(float, float);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		AttachedToWindow();
	virtual bool		QuitRequested();
	virtual void		Draw(BRect r);

	status_t			Preview(bool start);
	status_t			Render(bool start);
	status_t			HaltRender();
	status_t			Cut();
	status_t			Copy();
	status_t			Paste();
	status_t			Clear();
	status_t			Crop();
	status_t			Delete();
	status_t			Overwrite();
	
	Pool				*myPool;
	
	bool				isPreviewing;
	StreamTakeView		*takeView;
	StreamView			*streamView;
	
	BScrollBar			*timeScroll;
	BScrollBar			*valueScroll;
	BarBituate			*barbituate;
	PianoBituate		*pianobituate;
};

#endif