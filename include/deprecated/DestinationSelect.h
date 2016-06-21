#ifndef _DESTSELECT
#define _DESTSELECT

class Qua;
#include "Destination.h"
class Output;
class DestinationSelect: public BControl
{
public:
					DestinationSelect(BRect frame, const char *name,
						char *lbl,
						Output *d,
						BMessage *mess, Qua *q,
						ulong f1, ulong f2);
					~DestinationSelect();
	virtual void	Draw(BRect reg);
	virtual void	MouseDown(BPoint pt);
	virtual void	MessageReceived(BMessage *msg);
	virtual void	AttachedToWindow();
	void			SetDivider(float xCoordinate);
	
	BTextView		*textview;
	BRect			textrect;
	float			divider;
	
	Qua				*uberQua;
	Output			*uberThar;
};

enum {
	DESTINATION_SELECT = 'dsel'
};

#endif