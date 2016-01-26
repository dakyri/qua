#ifndef _DATAEDITOR
#define _DATAEDITOR

#include "Panel.h"

class StabEnt;
class BlockGraph;
class Qua;
class ToggleButton;
class TimeCtrl;
class MyCheckBox;
class Schedulable;
class QuasiStack;
class FramePanel;
class BlockGraph;
class Method;

#include "RWLock.h"
#include "Stacker.h"
#include "TimeKeeper.h"

class SymEditBlockView;

class DataEditor: public Panel, public TimeKeeper, public Stacker
{
public:
						DataEditor(BRect r, float maxw, Schedulable *, Metric *, long);
						~DataEditor();
					
	virtual void		Draw(BRect r);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		MouseDown(BPoint p);
	virtual void		ArrangeChildren();
	virtual void		FrameResized(float, float);
	virtual bool		QuitRequested();

	void				ReScale(float x, float y);
	float				origH, origW;
	
	Schedulable			*schedulable;
	
	ToggleButton		*previewButton;
	MyCheckBox			*loopButton;
	TimeCtrl			*durView;
	TimeCtrl			*lSelView;
	TimeCtrl			*rSelView;

	Method				*offlineFilter;
	QuasiStack			*filterStack;
	SymEditBlockView	*filterEdit;
	FramePanel			*filterControls;
	BMenuBar			*editBar;
	BMenu				*editMenu;
	
	SchLock				schlock;
};

enum {
	SELECT_PREVIW_TAKE = 'takp',
	SELECT_MAIN_TAKE = 'takm',
	SAMPLE_DURATION = 'sdur',
	REGION_SELECT = 'ssel',
	PREVIEW_REGION = 'prev',
	PREVIEW_LOOP = 'loop',
	SELECT_ALL = 'rall',
	CUT_REGION = 'rcut',
	COPY_REGION = 'rcpy',
	PASTE_REGION = 'rpst',
	OVERWRITE_REGION = 'rovr',
	CLEAR_REGION = 'rclr',
	DELETE_REGION = 'rdlt',
	CROP_REGION = 'ragy',
	RENDER_REGION = 'rend',
	HALT_RENDER = 'halt'
};

#endif