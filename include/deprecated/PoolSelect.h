#ifndef _POOLSELECT
#define _POOLSELECT

class Qua;
class Pool;

class PoolSelect: public BControl
{
public:
					PoolSelect(BRect frame, const char *name,
						char *lbl,
		         		Pool *p,
						BMessage *mess, Qua *q,
						ulong f1, ulong f2);
					~PoolSelect();
	virtual void	Draw(BRect reg);
	virtual void	MouseDown(BPoint pt);
	virtual void	MessageReceived(BMessage *msg);
	virtual void	AttachedToWindow();
	void			SetDivider(float xCoordinate);
	
	BTextView		*textview;
	BRect			textrect;
	float			divider;
	
	Qua			*uberQua;
	Pool			*selected;
};

#endif