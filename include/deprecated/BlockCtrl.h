#ifndef _BLOCKCTRL
#define _BLOCKCTRL

class BlockCtrl;
class Block;
class Qua;
class StabEnt;
class TvParser;

class BlockView: public BTextView
{
public:
					BlockView(BRect frame, const char *name,
         					ulong resizingMode, ulong flags,
         					Qua *q);
    				~BlockView();
    virtual void	KeyDown(const char *c, int32 nb);
	virtual void	MakeFocus(bool flag = TRUE);
	virtual bool	AcceptsDrop(BMessage *m);
	virtual bool	CanEndLine(int32);
	virtual Block	*ExpressionValue(StabEnt *S);
	virtual Block	*BlockValue(StabEnt *S);
	virtual void	Draw(BRect reg);
	
	void			ResizeToFit();
	void			SetValue(Block *B);
	
	void			SelectNextToken();
	void			SelectPrevToken();
	void			IncrementCurrentToken();
	void			DecrementCurrentToken();
	void			MinimizeCurrentToken();
	void			MaximizeCurrentToken();
	void			HomeCurrentToken();

    void			DrawCompButton();

	bool			needsCompile;
	BRect			compButton;
	
    BlockCtrl		*controller;
    TvParser		*parser;
    bool			selectedToken;
};


class BlockCtrl: public BControl
{
public:
					BlockCtrl(BRect frame, const char *name,
		         			 char *label, 
		         			 const char *text,
							BMessage *msg,
							ulong type,
							ulong f1, ulong f2,
							Qua *q);
					~BlockCtrl();
	virtual void	Draw(BRect reg);
	virtual void	MakeFocus(bool flag = TRUE);
	
	void 			SetDivider(float xCoordinate);
	char			*Text();
	void			SetText(const char *txt);
	void			SetValue(Block *V);
	Block			*ExpressionValue(StabEnt *S);
	Block			*BlockValue(StabEnt *S);
	void			MakeEditable(bool wh=FALSE);
	
	void			Invoke() { BControl::Invoke(); }
	BlockView		*textview;
	BRect			textrect;
	float			divider;
	ulong			blockType;
};

#define EDEDGE	8


#endif