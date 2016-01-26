#include "version.h"

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif


#include "DataView.h"
#include "Sym.h"
#include "SymEditPanel.h"
#include "ScrawleBar.h"
#include "Colors.h"
#include "KeyVal.h"

#define CTLCTLSPOT	3
#define CTLCTLLEN	25
#define NAMESPOT	92
#define TYPESPOT	(CTLCTLSPOT+CTLCTLLEN+5)

DataObjectsView::DataObjectsView(BRect r, char *nm, StabEnt *s, ulong f1, ulong f2):
	BView(r, nm, f1, f2)
{
	sym = s;
	tracking = MOUSE_TRACK_NOT;

	BRect t = Bounds();
	SetViewColor(mdGray);
	listView =	new DataListView(
						BRect(t.left+3,t.top+3,
							t.right-B_V_SCROLL_BAR_WIDTH-1-3,t.bottom-3),
						"data", sym);
	AddChild(listView);
	BRect	rr =  listView->Frame();
	dataScroll = new ScrawleBar(
						BRect(	rr.right+1,t.top+3,
								t.right-3, rr.bottom),
						"", listView, nullptr, 0, 100, 4, B_VERTICAL);
	ResizeTo(rr.right+B_V_SCROLL_BAR_WIDTH+3, rr.bottom+3);
	AddChild(dataScroll);
}

void
DataObjectsView::FrameResized(float x, float y)
{
//	fprintf(stderr, "resize %g %g\n", x, y);
//	SymEditPanel *p = ((SymEditPanel *)Parent());
//	if (p) p->ArrangeChildren();
}

void
DataObjectsView::MouseDown(BPoint p)
{
	BRect	r = Bounds();
	
	if (p.x >= r.right-5 && p.y >= r.bottom-5) {
		ulong	buts = true;
		BPoint	track_delta = r.RightBottom() - p;

		tracking = MOUSE_TRACK_RESIZE;
		SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

		ResizeTo(p.x + track_delta.x,p.y + track_delta.y);
	}
}


void
DataObjectsView::MouseMoved(BPoint p, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
		ResizeTo(p.x + track_delta.x,p.y + track_delta.y);
	}
}

void
DataObjectsView::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
		Invalidate();
		SymEditPanel *p = ((SymEditPanel *)Parent());
		if (p) p->ArrangeChildren();
	}
	tracking = MOUSE_TRACK_NOT;
}

void
DataObjectsView::Draw(BRect r)
{
	BRect	s=Bounds();
	RaisedBox(this, BRect(1,1,s.right-1,s.bottom-1), ViewColor(), false);
	LoweredBox(this, listView->Frame(), ViewColor(), false);
}

void
DataObjectsView::AddDataItem(StabEnt *p, short pos, short i)
{
	DataListItem	*di = new DataListItem(p, i);
	listView->AddItem(di, i);
	for (StabEnt *q= p->children; q!=nullptr; q=q->sibling) {
		if (!q->isDeleted && !q->isClone && !q->isHidden) {
			AddDataItem(q, pos, i+1);
		}
	}
	if (listView->CountItems() > 0) {
		dataScroll->SetRange(0, 10*listView->CountItems());
	}
}

void
DataObjectsView::RemoveDataItem(StabEnt *p)
{
	for (short i=0; i<listView->CountItems(); i++) {
		DataListItem	*di=(DataListItem *)listView->ItemAt(i);
		if (di->sym == p) {
			listView->RemoveItem(di);
			break;
		}
	}
	for (StabEnt *q= p->children; q!=nullptr; q=q->sibling) {
		if (!q->isDeleted && !q->isClone && !q->isHidden) {
			RemoveDataItem(q);
		}
	}
	if (listView->CountItems() > 0) {
		dataScroll->SetRange(0, 10*listView->CountItems());
	}
}	

//////////////////////////////////////////////////////////////////

DataListView::DataListView(BRect r, char *nm, StabEnt *s):
	BListView(r, nm, B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL)
{
	sym = s;
}

void
DataListView::KeyDown(const char *c, int32 nb)
{
	if (nb == 1) {
		char	theChar = c[0];
		if (theChar == B_DELETE) {
//			DataObjectsView		*tv=(DataObjectsView *)Parent();
			DeleteSelection();
		} else 
			BView::KeyDown(c, nb);
	} else {
		BView::KeyDown(c, nb);
	}
}


bool
DataListView::InitiateDrag(BPoint p, int32 index, bool sel)
{
//	DataListItem	*tli = (DataListItem *)ItemAt(index);
//
//	if (tli) {
//		BMessage	dragMsg(MOVE_TAKE);
//		dragMsg.AddInt32("data item", index);
//		dragMsg.AddPointer("data list", this);
//		fprintf(stderr, "drag %d %g %g %g %g\n", index,tli->rect.left, tli->rect.top, tli->rect.right, tli->rect.bottom);
//		DragMessage(&dragMsg, tli->rect);
//		return true;
//	}
	return false;
}

void
DataListView::MouseDown(BPoint p)
{
	ulong		mods = modifiers(); // Key mods???
	
	if (mods & B_SHIFT_KEY) {
		int				index = IndexOf(p);
		DataListItem	*tli = (DataListItem *)ItemAt(index);
	
		if (tli) {
			BMessage	dragMsg(MOVE_SYMBOL);
			dragMsg.AddInt32("symbol index", index);
			dragMsg.AddPointer("symbol", tli->sym);
			dragMsg.AddPointer("data list", this);
//			fprintf(stderr, "drag %d %g %g %g %g\n", index,tli->rect.left, tli->rect.top, tli->rect.right, tli->rect.bottom);
			DragMessage(&dragMsg, tli->rect);
		}
	} else if (mods & B_COMMAND_KEY) {
		BPopUpMenu		*qMenu = new BPopUpMenu("edit", true, FALSE);
		
		BPoint			orig = p;
		ConvertToScreen(&orig);
	
		int				index = IndexOf(p);
		int				indent = 0;
		DataListItem	*tli = (DataListItem *)ItemAt(index);
		StabEnt			*ctxt;
		if (tli->sym->type == S_STRUCT) {
			ctxt = tli->sym;
			index = index+1;
			indent = tli->indent+1;
		} else {
			ctxt = tli->sym->context;		
			indent = tli->indent;
		}
		BMenu	*sub = new BMenu("Add Item");
		qMenu->AddItem(sub);
		BMenuItem *it;
		
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_FLOAT, "real value");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_BYTE, "8 bit value");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_SHORT, "16 bit value");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_INT, "32 bit value");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_LONG, "64 bit value");
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_BOOL, "boolean value");
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_STRUCT, "structure");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_NOTE, "midi note");
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_CTRL, "midi control");
		it->Message()->AddInt32("position", index);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_PROG, "midi program");
		it->Message()->AddPointer("context", ctxt);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddInt32("position", index);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_BEND, "midi bend");
		it->Message()->AddInt32("position", index);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_SYSC, "midi system common command");
		it->Message()->AddInt32("position", index);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_SYSX, "midi system exclusive command");
		it->Message()->AddInt32("position", index);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);
		it = AddTypeMenuItem(Parent()->Parent(), sub, S_MESSAGE, "message");
		it->Message()->AddInt32("position", index);
		it->Message()->AddInt32("indent", indent);
		it->Message()->AddPointer("context", ctxt);

		sub = new BMenu("Delete Item");
		qMenu->AddItem(sub);
	
		for (StabEnt *p=sym->children; p!=nullptr; p=p->sibling) {
			if (	!p->isDeleted && p->refType != REF_INSTANCE &&
					!p->isClone && p->type != S_SCOPE &&
					p->type != S_INSTANCE && p->type != S_STREAM &&
					p->type != S_STREAM_ITEM) {
				BMessage *msg = new BMessage(DELETE_SYMBOL);
				msg->AddPointer("symbol", p);
				BMenuItem *item = new BMenuItem(p->name, msg);
				item->SetTarget(this);
				sub->AddItem(item);
			}
		}
			
		qMenu->SetAsyncAutoDestruct(true);
		qMenu->Go(orig, true, false, true);
	} else if (p.x >= CTLCTLSPOT && p.x <= CTLCTLSPOT+CTLCTLLEN) {
		int				index = IndexOf(p);
		DataListItem	*tli = (DataListItem *)ItemAt(index);
	} else if (p.x >= NAMESPOT) {
		int				index = IndexOf(p);
		DataListItem	*tli = (DataListItem *)ItemAt(index);
	} else {
		BListView::MouseDown(p);
	}
}

void
DataListView::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {

	case MOVE_SYMBOL: {
		DataObjectsView		*tv=(DataObjectsView *)Parent();
		DataListView	*tlv=nullptr;
		inMsg->FindPointer("data list", (void **)&tlv);
		BPoint	where(0, 0), delta(0, 0);
		int		from = inMsg->FindInt32("symbol index");
		int		to = 0;
		if (inMsg->WasDropped()) {
			where = inMsg->DropPoint(&delta);
			ConvertFromScreen(&where);
//			where = where - delta;
		}
		
		if (tlv == this) {
			to = IndexOf(where);
			if (from != to) {
				if (MoveItem(from, to)) {
					StabEnt		*s;
					inMsg->FindPointer("symbol", (void **)&s);
					DataListItem	*i = (DataListItem *)ItemAt(to);
					DataListItem	*f = (DataListItem *)ItemAt(from);
					f->indent = i->indent;
					glob.MoveSymbol(s, i->sym->context, i->sym);
					SymEditPanel	*p = (SymEditPanel *)Parent()->Parent();
					if (Stackable *S=p->mySym->StackableValue()) {
						S->ReAllocateChildren();
					}
				}
			}
		} else {
			BListView::MessageReceived(inMsg);
		}

		break;
	}

	default: {
		BListView::MessageReceived(inMsg);
	}

	}
}

void
DataListView::SelectionChanged()
{
	int		sel=0, cnt=0;
	DataObjectsView	*tv = (DataObjectsView *)Parent();
	while ((sel=CurrentSelection(cnt++)) >= 0) {
		DataListItem	*tiv = (DataListItem *)ItemAt(sel);
//		tv->Select(tiv->sym, false);
	}
}
void
DataListView::DeleteSelection()
{
	int		sel=0, cnt=0;
	DataObjectsView	*tv = (DataObjectsView *)Parent();
	while ((sel=CurrentSelection(cnt++)) >= 0) {
		DataListItem	*tiv = (DataListItem *)ItemAt(sel);
		glob.DeleteSymbol(tiv->sym);
		for (StabEnt *q= tiv->sym->children; q!=nullptr; q=q->sibling) {
			if (!q->isDeleted && !q->isClone && !q->isHidden) {
				((DataObjectsView*)Parent())->RemoveDataItem(q);
			}
		}
//		tv->Delete(tiv->sym);
		RemoveItem(tiv);
		delete tiv;
	}
	SymEditPanel	*sp = (SymEditPanel *)Parent()->Parent();
	if (Stackable *S=sp->mySym->StackableValue()) {
		S->ReAllocateChildren();
	}
}

//////////////////////////////////////////////////////////////////

DataListItem::DataListItem(StabEnt *t, short i):
	BListItem()
{
	sym = t;
	indent = i;
	SetHeight(16);
}


void
DataListItem::DrawItem(BView *lv, BRect ir, bool drall)
{
	BRect	bnd = lv->Bounds();
	float	wid = Max(ir.right, lv->StringWidth(sym->name)+4);
	float	bot = ir.bottom;
	rect = BRect(ir.left,ir.top,ir.left+wid,ir.top+14);
	DataListView *stv = (DataListView *)lv;
	
//	if (bot > bnd.bottom || wid > bnd.right) {
//		wid = Max(wid,bnd.right);
//		bot = Max(bot,bnd.bottom);
//		lv->ResizeTo(wid, bot);
//		if (stv->Parent()) {
//			stv->Parent()->ResizeTo(wid+2, bot+14);
//		}
//	}
	
	rgb_color		col=lv->ViewColor();
	if (IsSelected()) {
		lv->SetHighColor(col=ltGray);
		lv->FillRect(ir);
	} else if (drall) {
		lv->SetHighColor(col);
		lv->FillRect(ir);
	}
	
//	if (stv->ItemAt(0) == (DataListItem *)this) {
//		lv->SetHighColor(blue);
//	} else {
//		lv->SetHighColor(black);
//	}
	lv->SetLowColor(col);
	lv->SetHighColor(black);
	extern KeyIndex	typeIndex;
	
	lv->SetFont(be_plain_font);
	lv->DrawString(typeIndex.KeyOf(sym->type), BPoint(TYPESPOT, ir.bottom-2));
	
	lv->SetFont(be_bold_font);
	lv->DrawString(sym->name, BPoint(NAMESPOT + 20*indent,ir.bottom-2));

	lv->SetFont(be_plain_font);
	lv->SetFontSize(6);
	if (sym->controlInfo) {
		LoweredBox(lv, BRect(CTLCTLSPOT,ir.top+3,CTLCTLSPOT+CTLCTLLEN,ir.bottom-3), green, true);
		lv->SetLowColor(green);
		lv->SetHighColor(black);
		lv->DrawString("CTRL", BPoint(3,ir.bottom-3));
	} else {
		RaisedBox(lv, BRect(CTLCTLSPOT,ir.top+3,CTLCTLSPOT+CTLCTLLEN,ir.bottom-3), blue, true);
		lv->SetLowColor(blue);
		lv->SetHighColor(black);
		lv->DrawString("DATA", BPoint(3,ir.bottom-3));
	}
}

void
DataListItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
	SetHeight(16);
//	fprintf(stderr, "Update\n");
}

status_t
DataListItem::Perform(perform_code d, void *arg)
{
	return Perform(d, arg);
}

