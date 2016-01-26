#ifndef _SAMPLEDITOR
#define _SAMPLEDITOR

#include "DataEditor.h"
#include "RWLock.h"

class Method;
class QuasiStack;
class BlockGraph;
class FramePanel;
class Sample;
class ToggleButton;
class Shortlist;
class SampleBuffer;
class TimeCtrl;
class MyCheckBox;
class SampleTake;
class SampleTakeView;
class RenderStatusView;

#include "ScrawleBar.h"

class SampleView: public ScrawleView
{
public:
						SampleView(BRect r, Sample *s);
						~SampleView();
						
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
	bigtime_t			track_oldpr;
	long				track_first_frame;
	
	virtual void		ScaleX(float xscale);
	virtual void		ScaleY(float yscale);
	float				scaleX;
	float				scaleY;

	size_t				Generate(float *outSig, long nFramesReqd, short nChannels);
	
	Sample				*mySample;
	SampleTake			*editTake;
	
	long				framesPerPixel;
	long				yPsPerChannel;
	long				leftFrame;
	long				rightFrame;
	long				leftDrawFrame;
	long				rightDrawFrame;
	
	static long			DrawerWrapper(void *data);
	status_t			DrawSampleBits();
	status_t			DrawCurrentPixel();
	long				XPixToFrame(float x);
	float				FrameToX(long fr);
	float				SampleToY(short channel, float sam);
	void				Select(long, long);
	void				Refresh();
	void				SetupDisplay();
	
	bool				stillHere;
	SampleBuffer		*currentDrawBuffer;
	thread_id			drawThread;
	float				*minPix;
	float				*maxPix;
	float				width, height;
	long				leftDRF;
	bool				drawReq;
	
	bool				doLoop;
	
	long				leftSelectedFrame;
	long				rightSelectedFrame;
	long				sampleDuration;
	
	long				currentPlayFrame;
	SampleBuffer		*currentPlayBuffer;
	
	SchLock				drawLock;
};

class SampleEditor: public DataEditor
{
public:
						SampleEditor(BRect r, float maxw, Sample *s);
						~SampleEditor();
						
	virtual void		ArrangeChildren();
	virtual void		FrameResized(float, float);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		AttachedToWindow();
	virtual bool		QuitRequested();
	virtual void		Draw(BRect r);
	
	bool				GetKthChunks(int, int, Shortlist *);
	size_t				Generate(float *outSig, long nFramesReqd, short nChannels);
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
	
	Sample				*mySample;
	SampleTake			*editTake;
	bool				isPreviewing;

	SampleTakeView		*takeView;	
	RenderStatusView	*statusView;
	
	SampleView			*sampleView;
	ScrawleBar			*sampleScroll;
	
	bool				isRendering;
	thread_id			renderThread;
	static long			RenderWrapper(void *data);
	long				Renderer();
};


inline float
SampleView::FrameToX(long fr)
{
	return fr / framesPerPixel;
}

#define SAMPLERANGE	2.0

inline float
SampleView::SampleToY(short channel, float sam)
{
	return (channel + 0.5 +	(sam/SAMPLERANGE))*yPsPerChannel;
}

inline long
SampleView::XPixToFrame(float x)
{
	return x * framesPerPixel;
}


#endif