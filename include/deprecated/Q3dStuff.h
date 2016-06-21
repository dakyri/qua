#ifndef _3dSTUFF
#define _3dSTUFF

#include <3dCube.h>
#include <3dView.h>
#include <3dRadialLens.h>

class Item3dModel: public B3dCube
{
public:
					Item3dModel(B3dWorld *w, StreamItem *p);
	B3dVector		*ItemCube(StreamItem *p);
	virtual void	TouchDown(
						B3dTouchDesc *	touch,
						B3dVector *		touchOnScreen,
						uint32			buttons);
	virtual void	TouchMoved(	
						B3dTouchDesc *	touch,
						B3dVector *		touchOnScreen,
						uint32			buttons);
	
};

class Grid3dModel: public B3dCube
{
public:
					Grid3dModel(B3dWorld *w, int Note);
	B3dVector		*GridCube();
	virtual void	TouchDown(
						B3dTouchDesc *	touch,
						B3dVector *		touchOnScreen,
						uint32			buttons);
	virtual void	TouchMoved(	
						B3dTouchDesc *	touch,
						B3dVector *		touchOnScreen,
						uint32			buttons);
	short			myNote;
};

class Stream3dView : public B3dView
{
public:
					Stream3dView(BRect rect, Stream *S, B3dUniverse *u);
	virtual void	KeyDown(const char * ch, int32 nb);
	virtual void	MouseDown(BPoint pt);
	virtual void	AttachedToWindow();
	void			MoveTo(uchar note, Time &t);
	
	Stream			*myStream;
};

#endif