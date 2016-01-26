#ifndef _POOLEDIT
#define _POOLEDIT

class Pool;
class Stream;
class StreamItem;
class Stream3dView;

class PoolEditView: public BView
{
public:
					PoolEditView(BRect rect, Pool *P);
					
	Stream3dView	*StreamView;
	Pool			*pool;
};

class Editor: public BWindow
{
public:
					Editor(BRect, char *);
};


class PoolEditor: public Editor
{
public:
					~PoolEditor();
					PoolEditor(BRect inRect, Pool *P);
	virtual bool	QuitRequested();
	BScrollBar		*vert,
					*horz;
	PoolEditView	*edit;
	Pool			*pool;
};

#endif