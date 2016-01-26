#ifndef _BLOCKDRAW
#define _BLOCKDRAW

class Qua;
class StabEnt;
class Block;

class BlockGraph: public BControl
{
public:
					BlockGraph(BRect r, Stackable *, Qua *uq);
	virtual void	MouseDown(BPoint p);
	virtual void	Draw(BRect p);
	virtual void	MessageReceived(BMessage *);
	virtual void	SetViewColor(rgb_color);
	virtual void	AttachedToWindow();
	
	Block			*BlockValue(StabEnt *);
	void			SetValue(Block *);
	void			ResizeToFit();
	
	Block			*MouseDownBlock(Block *b, BPoint p);
	void			SetupDisplay(Block *b, float x, float y, short kind, short);
	void			ClearDisplay(Block *b);
	void			DrawBlock(Block *b, BRect r, short);
	
	Qua				*qua;
	Block			*block;
	Stackable		*stackable;
};


void DrawArrowHead(BView *v, BPoint from, BPoint to, float d);

#endif