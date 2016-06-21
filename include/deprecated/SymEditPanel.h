#ifndef _SYMEDIT
#define _SYMEDIT

#include "Panel.h"

class StabEnt;
class BlockGraph;
class SymEditBlockView;
class Qua;
class Block;
class NameCtrl;
class Executable;
class DataObjectsView;

#undef BLOCKGRAPH

BMenuItem *AddTypeMenuItem(BHandler *h, BMenu *sub, short type, char *lbl);

class SymEditPanel: public Panel
{
public:
					SymEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq, char nme);

	virtual void	Draw(BRect r);
	virtual void	MessageReceived(BMessage *inMsg);
	virtual void	MouseDown(BPoint p);
	virtual void	ArrangeChildren();
	virtual void	FrameResized(float, float);
	virtual void	AttachedToWindow();

	void			ShuffleSymEditChildren();
	void			EditMenu(BPoint p);
	
	void			ReScale(float x);
	float			origH, origW;
		
	StabEnt			*mySym;
	BStringView		*vName;
	Qua				*uberQua;
	
	DataObjectsView	*dataChildrenView;
};

class ExecutableEditPanel: public SymEditPanel
{
public:
						ExecutableEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq);
	virtual void		ArrangeChildren();
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		MouseDown(BPoint);
	virtual void		Draw(BRect);

#ifdef BLOCKGRAPH
	BlockGraph			*myBlock;
#else
	SymEditBlockView	*myBlock;	
#endif
};

class ScopeEditPanel: public SymEditPanel
{
public:
						ScopeEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq);
	virtual void		ArrangeChildren();
	virtual void		Draw(BRect r);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		MouseDown(BPoint);
		
	virtual void		SetDisplayMode(short dm);
	
#ifdef BLOCKGRAPH
	BlockGraph			*myBlock;
#else
	SymEditBlockView	*myBlock;	
#endif
};

class SchedulableEditPanel: public SymEditPanel
{
public:
						SchedulableEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq);
	virtual void		ArrangeChildren();
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		Draw(BRect);
	virtual void		MouseDown(BPoint);
	
#ifdef BLOCKGRAPH
	BlockGraph			*myBlock;
#else
	SymEditBlockView	*myBlock;	
#endif
};

class ChannelEditPanel: public SymEditPanel
{
public:
						ChannelEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq);
	virtual void		ArrangeChildren();
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		MouseDown(BPoint);
	virtual void		Draw(BRect);
};

class ModeCtrl;

class DataEditPanel: public SymEditPanel
{
public:
						DataEditPanel(BRect r, float maxw, StabEnt *S, Qua *q);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		ArrangeChildren();
	virtual void		MouseDown(BPoint);
	
	ModeCtrl			*typeSelect;
};


enum {
	DELETE_SYMBOL = 'adds',
	ADD_SYMBOL = 'dels',
	MOVE_SYMBOL = 'movs'
};

#endif