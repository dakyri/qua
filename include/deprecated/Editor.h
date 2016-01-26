#ifndef _EDITOR_H
#define _EDITOR_H

class StabEnt;
class Qua;
class SymEditPanel;
class DataEditor;
class ScrawleBar;
class ScrawleView;

#include "TiledView.h"

class EditBackView: public TiledView
{
public:
					EditBackView(BRect);
	virtual void	Draw(BRect r);

	virtual void	MouseDown(BPoint p);
	virtual void	MouseMoved(BPoint, uint32, const BMessage *);
	virtual void	MouseUp(BPoint);

	short			tracking;
	BPoint			track_pt;
	BRect			track_cr;
	BRect			track_dr;
};

class Editor: public BWindow
{
public:
					Editor(BRect r, Qua *q, StabEnt *S);
					~Editor();
					
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *inMsg);
	virtual void	FrameResized(float, float);
	
	EditBackView	*back;
	SymEditPanel	*ctrlEdit;
	ScrawleBar		*ctrlScroll;
	ScrawleView		*ctrlBack;
	DataEditor		*dataEdit;
	ScrawleBar		*dataScroll;
};

#endif