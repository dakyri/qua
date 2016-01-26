#ifndef _CEBLOCKVIEW
#define _CEBLOCKVIEW

#include "BlockCtrl.h"

class SymEditBlockView: public BlockView
{
public:
					SymEditBlockView(BRect, Qua *);

	virtual void	FrameResized(float, float);
    virtual void	KeyDown(const char *c, int32 nb);
    virtual void	Draw(BRect r);
    virtual void	MouseDown(BPoint p);
    
    virtual Block	*BlockValue(StabEnt *);
};

class GraphBlockView: public SymEditBlockView
{
public:
					GraphBlockView(BRect, Qua *);

	virtual void	FrameResized(float, float);
    virtual void	KeyDown(const char *c, int32 nb);
    virtual void	Draw(BRect r);
    virtual void	MouseDown(BPoint p);
    
    virtual Block	*BlockValue(StabEnt *);
};

enum {
	COMPILE_BLOCK = 'comp'
};

#endif