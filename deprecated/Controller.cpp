#include "Controller.h"
#include "StdDefs.h"
#include "Colors.h"
#include "ControlVar.h"
#include "TiledView.h"
#include "Quapp.h"

class Controller: public BWindow
{
public:
				Controller(BRect rect, char *nm);
	void		AddControlPanel(Panel *p);
	void		AddMenuBar(BMenuBar *p);
	BView		*backView;
};

extern flag debug_ctl;
extern FILE *dbfp;

class ControllerBackView: public TiledView
{
public:
	ControllerBackView(BRect r):
		TiledView(r, "back", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetTilingPattern(*quapp->backgroundPattern);
	}
	
	virtual void	MouseDown(BPoint where)
	{
		MakeFocus(TRUE);
	}
};

Controller::Controller(BRect rect, char *nm)
	: BWindow( rect, nm, B_TITLED_WINDOW,
				B_NOT_RESIZABLE|B_WILL_ACCEPT_FIRST_CLICK )
{
	backView = new ControllerBackView(Bounds());
	backView->SetViewColor(orange);
	AddChild(backView);
}

void
Controller::AddControlPanel(Panel *p)
{
	Lock();
	BRect wrect = Bounds();
	int			len = wrect.bottom - wrect.top;
	int			wid = wrect.right - wrect.left;
	
	wrect = p->Bounds();
	if (debug_ctl)
		fprintf(dbfp, "my size %d %d panel(%g,%g,%g,%g)\n",
				len, wid, wrect.left,wrect.top,wrect.right,wrect.bottom);
	
	p->MoveTo(MARGIN, len-MARGIN);
	backView->AddChild(p);

	len += (wrect.bottom - wrect.top);
	wid = Max(wid, wrect.right - wrect.left + 2*MARGIN);
	ResizeTo(wid,len);
	backView->MoveTo(0,0);
	Unlock();
}

void
Controller::AddMenuBar(BMenuBar *p)
{
	backView->AddChild(p);
	Lock();
	BRect Box = p->Bounds();
	float len = Box.bottom;
	float wid = Box.right;
	ResizeTo(wid, len);
	Unlock();
}
