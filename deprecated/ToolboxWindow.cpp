
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "Toolbox.h"
#include "Colors.h"
#include "messid.h"
#include "GlblMessid.h"
#include "TiledView.h"
#include "ObjectView.h"
#include "Panel.h"
#include "RosterView.h"
#include "Quapp.h"
#include "Sym.h"
#include "Quaddon.h"
#include "Template.h"
#include "Method.h"
#include "QuasiStack.h"

class ToolboxBackView: public TiledView
{
public:
	ToolboxBackView(BRect r):
		TiledView(r, "toolbox", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(orange);
	}
	
	virtual void
	Draw(BRect r)
	{
		TiledView::Draw(r);
	}
};
	



ToolboxWindow::ToolboxWindow(BRect r, char *title):
	BWindow(r, title,
			B_TITLED_WINDOW,
			B_WILL_ACCEPT_FIRST_CLICK
			|B_NOT_CLOSABLE
			|B_NOT_ZOOMABLE
//			|B_NOT_RESIZABLE
		)
{
	back = new ToolboxBackView(Bounds());
	AddChild(back);
	
	palette = new ColorSelectView();
	back->AddChild(palette);
	
	palette->MoveTo(MARGIN, MARGIN);
	BRect	pr = palette->Frame();

	BRect	lbr(pr.right+MARGIN, MARGIN,
				r.right-r.left-MARGIN,r.bottom-r.top-MARGIN);
	theLibrary = new RosterView(lbr, "rosterview");
	ResizeTo(lbr.right+MARGIN, lbr.bottom+MARGIN);
	back->AddChild(theLibrary);
	
	SetPulseRate(1000000.0);
}

ToolboxWindow::~ToolboxWindow()
{
}


// Palette

ColorSelectObjectView::ColorSelectObjectView(BRect r):
	ObjectView(r, "palette", nullptr, "Palette", nullptr, B_WILL_DRAW|B_FRAME_EVENTS)
{
	csv = new ColorSelectView();
	csv->MoveTo(2, 17);
	SetViewColor(mdGray);
	SetDisplayMode(OBJECT_DISPLAY_SMALL);
}

bool
ColorSelectObjectView::SetDisplayMode(short md)
{
	displayMode = md;
	if (displayMode == OBJECT_DISPLAY_SMALL) {
		RemoveChild(csv);
		ResizeTo(60, 16);
		Invalidate();
		return true;
	} else if (displayMode == OBJECT_DISPLAY_BIG) {
		if (csv->Parent()== nullptr)
			AddChild(csv);
		BRect	pr = csv->Frame();
		ResizeTo(pr.right+2, pr.bottom+2);
		Invalidate();
		return true;
	}
	return false;
}

void
ColorSelectObjectView::Draw(BRect r)
{
	BRect s = Bounds();
	RaisedBox(	this,
				BRect(
						s.left+1, s.top+1, s.right-1, s.bottom-1),
				ViewColor()
		);
	SetHighColor(black);
	SetLowColor(ViewColor());
	DrawString("Color Palette", BPoint(2, 12));
}

void
ColorSelectObjectView::MouseDown(BPoint p)
{
	ObjectView::MouseDown(p);
}

SymListView::SymListView(BRect rect, char *lbl, ObjectViewContainer *c, char *nm, uint32 rsz, uint32 flgs):
	ObjectView(rect, lbl, c, nm, rsz, flgs)
{
}

void
SymListView::ClearSymList()
{
	sli		*sp;
	for (short i=0; i<syms.CountItems(); i++) {
		sp = (sli *)syms.ItemAt(i);
		delete sp;
	}
	syms.MakeEmpty();
}

sli::sli(BRect r, StabEnt *s, BBitmap *i)
{
	rect = r;
	sym = s;
	icon = i;
}

void
SymListView::AddSymItem(BRect r, StabEnt *s, BBitmap *i)
{
	syms.AddItem(new sli(r, s, i));
}

sli *
SymListView::ItemForSym(StabEnt *s)
{
	sli		*sp;
	for (short i=0; i<syms.CountItems(); i++) {
		sp = (sli *)syms.ItemAt(i);
		if (sp->sym == s)
			return sp;
	}
	return nullptr;
}


void
SymListView::MouseDown(BPoint p)
{
	sli	*theone = nullptr;
	for (short i=0; i<syms.CountItems(); i++) {
		sli *sp = (sli *)syms.ItemAt(i);
		if (sp->rect.Contains(p))
			theone = sp;
	}
	if (theone) {
		BRect		r = theone->rect;
		r.OffsetTo(0,0);
		BBitmap		*dragmap = new BBitmap(r, B_CMAP8, true);
		BView		*v = new BView(r, "offv", nullptr, B_WILL_DRAW);
		dragmap->AddChild(v);
		dragmap->Lock();
		v->SetHighColor(black);
		v->SetLowColor(ViewColor());
		v->FillRect(r, B_SOLID_LOW);
		v->DrawBitmap(theone->icon, BPoint(0,0));
		v->DrawString(theone->sym->name, BPoint(17,14));
		dragmap->Unlock();
		
		BMessage	dmsg(ADD_SYMBOL);

		dmsg.AddPointer("symbol", theone->sym);
		DragMessage(&dmsg, dragmap, B_OP_BLEND, p - theone->rect.LeftTop());
	} else {
		ObjectView::MouseDown(p);
	}
}

void
SymListView::DrawList()
{
	SetHighColor(black);
	SetLowColor(ViewColor());
	SetFontSize(8);
	sli		*sp;
	BRegion	r;
	for (short i=0; i<syms.CountItems(); i++) {
		sp = (sli *)syms.ItemAt(i);
		r.Include(sp->rect);
		ConstrainClippingRegion(&r);
		FillRect(sp->rect, B_SOLID_LOW);
		if (sp->icon) {
			DrawBitmap(sp->icon, sp->rect.LeftTop()+BPoint(0,0));
		}
		DrawString(sp->sym->name, sp->rect.LeftBottom()+BPoint(16+1,-3));
		StrokeRect(sp->rect);
		ConstrainClippingRegion(nullptr);
	}
}

// AudioFXObjectView

AudioFXObjectView::AudioFXObjectView(BRect r):
	SymListView(r, "audiofx", nullptr, "Audio FX", nullptr, B_WILL_DRAW|B_FRAME_EVENTS)
{
	SetViewColor(lavender);
	Refresh();
	SetDisplayMode(OBJECT_DISPLAY_SMALL);
}

bool
AudioFXObjectView::SetDisplayMode(short md)
{
	displayMode = md;
	if (displayMode == OBJECT_DISPLAY_SMALL) {
//		RemoveChild(csv);
		ResizeTo(60, 16);
		Invalidate();
		return true;
	} else if (displayMode == OBJECT_DISPLAY_BIG) {
		BRect	pr;
		Refresh();
		if (syms.CountItems() > 0) {
			sli	*l = (sli *)syms.ItemAt(syms.CountItems()-1);
			pr = l->rect;
			ResizeTo(pr.right+2, pr.bottom+2);
		} else {
			ResizeTo(60, 16);
		}
			
		Invalidate();
		return true;
	}
	return false;
}

void
AudioFXObjectView::Refresh()
{
	Quaddon		*p = quapp->audioFX;
	BRect		r = Bounds();
	short		i = 0;
	
	float		w = StringWidth("Audio FX")+4;

	while (p) {
		if (StringWidth(p->sym->name) + 16 + 4 > w)
			w = StringWidth(p->sym->name) + 16 + 4;
		p = p->next;
	}
	
	p = quapp->audioFX;
	while (p) {
		AddSymItem(
			BRect(r.left+2, 16 + i*SLITEM_HEIGHT, w-2, 16 + i*SLITEM_HEIGHT+SLITEM_HEIGHT),
			p->sym,
			quapp->audioSmallIcon);
		i++;
		p = p->next;
	}
}

void
AudioFXObjectView::Draw(BRect r)
{
	BRect s = Bounds();
	RaisedBox(	this,
				BRect(
						s.left+1, s.top+1, s.right-1, s.bottom-1),
				ViewColor()
		);
	SetHighColor(black);
	SetLowColor(ViewColor());
	SetFontSize(8);
	DrawString("Audio FX", BPoint(2, 12));
	if (displayMode == OBJECT_DISPLAY_BIG) {
		DrawList();
	}
}


// TemplateObjectView

TemplateObjectView::TemplateObjectView(BRect r):
	SymListView(r, "templates", nullptr, "Templates", nullptr, B_WILL_DRAW|B_FRAME_EVENTS)
{
	SetViewColor(mauve);
	SetDisplayMode(OBJECT_DISPLAY_SMALL);
}

bool
TemplateObjectView::SetDisplayMode(short md)
{
	displayMode = md;
	if (displayMode == OBJECT_DISPLAY_SMALL) {
//		RemoveChild(csv);
		ResizeTo(60, 16);
		Invalidate();
		return true;
	} else if (displayMode == OBJECT_DISPLAY_BIG) {
		Refresh();
		BRect pr;
		if (syms.CountItems() > 0) {
			sli	*l = (sli *)syms.ItemAt(syms.CountItems()-1);
			pr = l->rect;
			ResizeTo(pr.right+2, pr.bottom+2);
		} else {
			ResizeTo(60, 16);
		}
			
		Invalidate();
		return true;
	}
	return false;
}

void
TemplateObjectView::Draw(BRect r)
{
	BRect s = Bounds();
	RaisedBox(	this,
				BRect(
						s.left+1, s.top+1, s.right-1, s.bottom-1),
				ViewColor()
		);
	SetHighColor(black);
	SetLowColor(ViewColor());
	SetFontSize(8);
	DrawString("Templates", BPoint(2, 12));
	if (displayMode == OBJECT_DISPLAY_BIG) {
		DrawList();
	}
}


void
TemplateObjectView::Refresh()
{
	Template	*p = quapp->templates;
	BRect		r = Bounds();
	short		i = 0;
	
	float		w = StringWidth("Templates")+4;

	while (p) {
		if (StringWidth(p->sym->name) + 16 + 4 > w)
			w = StringWidth(p->sym->name) + 16 + 4;
		p = p->next;
	}
	
	p = quapp->templates;
	while (p) {
		AddSymItem(
			BRect(r.left+2, 16 + i*SLITEM_HEIGHT, w-2, 16 + i*SLITEM_HEIGHT+SLITEM_HEIGHT),
			p->sym,
			p->sicon?p->sicon:quapp->quaSmallIcon);
		i++;
		p = p->next;
	}
}

// LibraryObjectView

LibraryObjectView::LibraryObjectView(BRect r):
	SymListView(r, "library", nullptr, "Library", nullptr, B_WILL_DRAW|B_FRAME_EVENTS)
{
	SetViewColor(seablue);
	SetDisplayMode(OBJECT_DISPLAY_SMALL);
}

bool
LibraryObjectView::SetDisplayMode(short md)
{
	displayMode = md;
	if (displayMode == OBJECT_DISPLAY_SMALL) {
//		RemoveChild(csv);
		ResizeTo(65, 16);
		Invalidate();
		return true;
	} else if (displayMode == OBJECT_DISPLAY_BIG) {
		BRect	pr;
		Refresh();
		if (syms.CountItems() > 0) {
			sli	*l = (sli *)syms.ItemAt(syms.CountItems()-1);
			pr = l->rect;
			ResizeTo(pr.right+2, pr.bottom+2);
		} else {
			ResizeTo(65, 16);
		}
			
		Invalidate();
		return true;
	}
	return false;
}

void
LibraryObjectView::Draw(BRect r)
{
	BRect s = Bounds();
	RaisedBox(	this,
				BRect(
						s.left+1, s.top+1, s.right-1, s.bottom-1),
				ViewColor()
		);
	SetHighColor(black);
	SetLowColor(ViewColor());
	SetFontSize(8);
	DrawString("Library Methods", BPoint(2, 12));
	if (displayMode == OBJECT_DISPLAY_BIG) {
		DrawList();
	}
}

void
LibraryObjectView::Refresh()
{
	Method		*p = quapp->methods;
	BRect		r = Bounds();
	short		i = 0;
	
	float		w = StringWidth("Library Methods")+4;

	while (p) {
		if (StringWidth(p->sym->name) + 16 + 4 > w)
			w = StringWidth(p->sym->name) + 16 + 4;
		p = p->next;
	}
	
	p = quapp->methods;
	while (p) {
		AddSymItem(
			BRect(r.left+2, 16 + i*SLITEM_HEIGHT, w-2, 16 + i*SLITEM_HEIGHT+SLITEM_HEIGHT),
			p->sym,
			quapp->quaSmallIcon);
		i++;
		p = p->next;
	}
}

// ColorSelectView

ColorSelectView::ColorSelectView():
	BView(BRect(0,0,8*8, 32*8), "", nullptr, B_WILL_DRAW)
{
	int			i, j;
	BScreen		*scr = new BScreen();

	for (i=0; i<256; i++)
		palette[i] = scr->ColorMap()->color_list[i];
	delete scr; 
}

void
ColorSelectView::Draw(BRect r)
{
	int			i, j;
	BRect		b=Bounds();
	for (i=0; i<8; i++) {
		float	w=(b.right-b.left)/8,h=(b.bottom-b.top)/32;
		for (j=0; j<32; j++) {
			SetHighColor(palette[i*32+j]);
			FillRect(BRect(i*w, j*h, i*w+w, j*h+h));
			SetHighColor(black);
			StrokeRect(BRect(i*w, j*h, i*w+w, j*h+h));
		}
	}
}

ColorSelectView::~ColorSelectView()
{
}

void
ColorSelectView::MouseDown(BPoint p)
{
	int		x =(((int)p.x) / 8);
	int		y =(((int)p.y) / 8);
	int ind = x*32 + y;
	BRect	r(x*8,y*8,x*8+8,y*8+8);
	BMessage	*msg = new BMessage(SET_COLOR);
	msg->AddData("color", B_RAW_TYPE,
					&palette[ind], sizeof(palette[ind]));
	DragMessage(msg, r);
}

