#ifndef _TOOLBOX
#define _TOOLBOX

#include "version.h"
#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <InterfaceKit.h>
#endif

class TiledView;
class RosterView;
class ObjectViewContainer;
class ColorSelectView;

#include "ObjectView.h"

class StabEnt;

struct sli {
					sli(BRect, StabEnt *, BBitmap *);
					
	StabEnt			*sym;
	BRect			rect;
	BBitmap			*icon;
};

#define SLITEM_HEIGHT	16

class SymListView: public ObjectView
{
public:
					SymListView(BRect rect, char *lbl, ObjectViewContainer *, char *nm, uint32 rsz, uint32 flgs);
	BList			syms;
	
	virtual void	MouseDown(BPoint p);

	virtual void	Refresh()=nullptr;
	void			DrawList();
	void			ClearSymList();	
	void			AddSymItem(BRect, StabEnt *, BBitmap *);	
	sli 			*ItemForSym(StabEnt *s);
};


class AudioFXObjectView: public SymListView
{
public:
					AudioFXObjectView(BRect r);

	virtual void	Draw(BRect r);
	virtual bool	SetDisplayMode(short);
	
	virtual void	Refresh();
};

class TemplateObjectView: public SymListView
{
public:
					TemplateObjectView(BRect r);

	virtual void	Draw(BRect r);
	virtual bool	SetDisplayMode(short);
	
	virtual void	Refresh();
};

class LibraryObjectView: public SymListView
{
public:
					LibraryObjectView(BRect r);

	virtual void	Draw(BRect r);
	virtual bool	SetDisplayMode(short);
	
	virtual void	Refresh();
};

class ColorSelectObjectView: public ObjectView
{
public:
					ColorSelectObjectView(BRect r);
	virtual void	MouseDown(BPoint);
	virtual void	Draw(BRect r);
	virtual bool	SetDisplayMode(short);
	
	ColorSelectView	*csv;
};

class ColorSelectView: public BView
{
public:
					ColorSelectView();
					~ColorSelectView();
	rgb_color		palette[256];
	virtual void	MouseDown(BPoint p);
	virtual void	Draw(BRect r);
};

class ToolboxWindow: public BWindow
{
public:
						ToolboxWindow(BRect rect, char *);
						~ToolboxWindow();

	RosterView			*theLibrary;
	ColorSelectView		*palette;
	TiledView			*back;
};

#endif