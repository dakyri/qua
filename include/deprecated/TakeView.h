#ifndef _TAKEVIEW
#define _TAKEVIEW

class SampleTake;
class Sample;
class StreamTake;
class Pool;

class TakeListView: public BListView
{
public:
						TakeListView(BRect, char *);
	virtual void		KeyDown(const char *, int32);
	virtual bool		InitiateDrag(BPoint p, int32 index, bool sel);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		SelectionChanged();
	virtual void		MouseDown(BPoint p);
	virtual void		DeleteSelection();
};

class TakeView: public BView
{
public:
						TakeView(BRect, char *, ulong, ulong);
	virtual void		Draw(BRect r);
	virtual void		FrameResized(float, float);
	virtual void		Select(Take *take, bool)=0;
	virtual void		Delete(Take *take)=0;
	
	TakeListView		*listView;
};

class TakeListItem: public BListItem
{
public:
	BRect				rect;
	Take				*myTake;
};

class SampleTakeItem: public TakeListItem
{
public:
						SampleTakeItem(SampleTake *t);
	virtual status_t	Perform(perform_code d, void *arg);
	virtual	void		Update(BView *owner, const BFont *font);
	virtual void		DrawItem(BView *o, BRect ir, bool dr);
	
	inline SampleTake	*Take() { return (SampleTake *)myTake; }
};

class SampleTakeView: public TakeView
{
public:
						SampleTakeView(BRect r, Sample *s);
	
	void				AddTake(SampleTake *take);
	virtual void		Select(Take *take, bool);
	virtual void		Delete(Take *take);
	virtual void		MessageReceived(BMessage *inMsg);

	Sample				*mySample;
};


class StreamTake;
class Pool;

class StreamTakeItem: public TakeListItem
{
public:
						StreamTakeItem(StreamTake *t);
	virtual	void		Update(BView *owner, const BFont *font);
	virtual status_t	Perform(perform_code d, void *arg);
	virtual void		DrawItem(BView *o, BRect ir, bool dr);
	
	inline StreamTake	*Take() { return (StreamTake *)myTake; }
};

class StreamTakeView: public TakeView
{
public:
						StreamTakeView(BRect r, Pool *s);
	
	void				AddTake(StreamTake *take);
	virtual void		Select(Take *take, bool);
	virtual void		Delete(Take *take);

	Pool				*myPool;
};

enum {
	MOVE_TAKE	= 'mtak'
};

#endif