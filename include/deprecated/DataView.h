#ifndef _DATAVIEW
#define _DATAVIEW

class StabEnt;
class ScrawleBar;

class DataListView: public BListView
{
public:
						DataListView(BRect, char *, StabEnt *);
	virtual void		KeyDown(const char *, int32);
	virtual bool		InitiateDrag(BPoint p, int32 index, bool sel);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		SelectionChanged();
	virtual void		MouseDown(BPoint p);
	virtual void		DeleteSelection();
	
	StabEnt				*sym;
};

class DataObjectsView: public BView
{
public:
						DataObjectsView(BRect, char *, StabEnt *, ulong, ulong);
	virtual void		Draw(BRect r);
	virtual void		FrameResized(float, float);
	void				AddDataItem(StabEnt *p, short, short);
	void				RemoveDataItem(StabEnt *p);
//	virtual void		Select(Take *take, bool)=0;
//	virtual void		Delete(Take *take)=0;

	virtual void		MouseDown(BPoint p);
	virtual void		MouseMoved(BPoint, uint32, const BMessage *);
	virtual void		MouseUp(BPoint);

	short				tracking;
	BPoint				track_delta;
	
	DataListView		*listView;
	ScrawleBar			*dataScroll;
	StabEnt				*sym;
};

class DataListItem: public BListItem
{
public:
						DataListItem(StabEnt *t, short);
	virtual status_t	Perform(perform_code d, void *arg);
	virtual	void		Update(BView *owner, const BFont *font);
	virtual void		DrawItem(BView *o, BRect ir, bool dr);
	
	StabEnt				*sym;
	short				indent;
	BRect				rect;
};


#endif