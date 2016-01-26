#ifndef _FLT_CTL_EDITOR
#define _FLT_CTL_EDITOR

class NumCtrl;
class QuasiStack;
class Stream;
class RWLock;

#include "Controller.h"

enum {
	FC_MODE_NORM = 0,
	FC_MODE_CLEAR = 1,
	FC_MODE_COPY = 2
};

class StabEnt;

class FilterBlockCtlView: public ControlPanel
{
public:
					FilterBlockCtlView(char *nm, StabEnt *it,
							Block *V,
							QuaQua *A,
							BRect r,
							RWLock *lock,
							short cpy,
							Stream *S,
							QuasiStack *N);
					~FilterBlockCtlView();
	virtual void	MessageReceived(BMessage *msg);
	void			AddFilterBlock(Source *S);
	void			DelFilterBlock(short w);
	
	Block			*theBlock;
	RWLock			*theBlockLock;
	QuaQua			*uberQua;
	Stream			*theStream;
	QuasiStack		*theStack;
	StabEnt			*theThing;
	short			fcMode;
};

#endif