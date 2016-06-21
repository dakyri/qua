
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "SymEditPanel.h"
#include "Sym.h"
#include "Colors.h"
#include "Schedulable.h"
#include "SymEditBlockView.h"
#include "Instance.h"
#include "Schedulable.h"
#include "Block.h"
#include "Qua.h"
#include "Controller.h"
#include "Channel.h"
#include "Method.h"
#include "DataView.h"
#include "Editor.h"
#include "ScrawleBar.h"
#include "NameCtrl.h"
#include "BlockDrawBits.h"

SymEditPanel::SymEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq, char nme):
	Panel(r, maxw, "symbol")
{
	mySym = S;
	uberQua = uq;
	
	SetFontSize(12);
	SetFont(be_bold_font);

	short swid = StringWidth(mySym->UniqueName());
	BRect wrect;
	GetItemRect(MIXER_MARGIN, swid + 5, 17, 5, wrect);
	if (nme > 0) {
		vName = new NameCtrl( wrect, "name", mySym->UniqueName(), nullptr, this, B_FOLLOW_TOP|B_FOLLOW_LEFT, B_WILL_DRAW);
		((NameCtrl*)vName)->edit->target = this;
		vName->SetViewColor(mdGray);
		vName->SetFont(be_bold_font);
		vName->SetFontSize( 12 );
		AddChild(vName);
	} else if (nme < 0) {
		vName = new BStringView(wrect, "name", mySym->UniqueName(), B_FOLLOW_TOP|B_FOLLOW_LEFT, B_WILL_DRAW);
		vName->SetViewColor(mdGray);
		vName->SetFont(be_bold_font);
		vName->SetFontSize( 12 );
		AddChild(vName);
	} else {
		vName = nullptr;
	}
	
	dataChildrenView = new DataObjectsView(
								BRect(wrect.right+5, MIXER_MARGIN,
									wrect.right+145, 25),
								"Blah",
								mySym,
								(ulong) nullptr, B_WILL_DRAW|B_FRAME_EVENTS
								);

	StabEnt	*p = S->children;
	while (p) {
		if (!(p->isClone || p->isHidden || p->refType == REF_INSTANCE || p->refType == REF_POINTER)) {
			switch (p->type) {
			case S_BOOL:
			case S_BYTE:
			case S_SHORT:
			case S_INT:
			case S_LONG:
			case S_NOTE:
			case S_CTRL:
			case S_PROG:
			case S_BEND:
			case S_SYSC:
			case S_SYSX:
			case S_MESSAGE:
			case S_STRUCT:
			case S_TIME:
			case S_FLOAT: {
#ifdef DATAEDITPANEL
				SymEditPanel *SP =
					new DataEditPanel(BRect(0,0,2*MIXER_MARGIN,2*MIXER_MARGIN),
									maxw-2*MIXER_MARGIN,
									p, uberQua);
		
				AddChild(SP);
#else
				dataChildrenView->AddDataItem(p, 0, 0);
				if (dataChildrenView->Parent() == nullptr)
					AddChild(dataChildrenView);
#endif		
				break;
			}
			case S_METHOD: {
				SymEditPanel *SP =
					new ExecutableEditPanel(BRect(0,0,2*MIXER_MARGIN,2*MIXER_MARGIN),
									maxw-2*MIXER_MARGIN,
									p, uberQua);
		
				AddChild(SP);
				
				break;
			}
			case S_SCOPE: {
				SymEditPanel *SP =
					new ScopeEditPanel(BRect(0,0,2*MIXER_MARGIN,2*MIXER_MARGIN),
									maxw-2*MIXER_MARGIN,
									p, uberQua);
		
				AddChild(SP);
				
				break;
			}
			
			}
		}
		p = p->sibling;
	}
	
	int	nd = dataChildrenView->listView->CountItems();
	if (nd) {
		int		h = nd*13;
		h = Max(h,16);
		dataChildrenView->listView->ResizeTo(180, h);
		dataChildrenView->ResizeTo(
							180 + 2*MIXER_MARGIN + B_V_SCROLL_BAR_WIDTH,
							h + 2*MIXER_MARGIN);
		dataChildrenView->dataScroll->ResizeTo(B_V_SCROLL_BAR_WIDTH, h);
		dataChildrenView->dataScroll->MoveTo(180+MIXER_MARGIN+1, MIXER_MARGIN);
	}
//	fprintf(stderr, "%s arranging children\n", S->name);
	ArrangeChildren();
}

void
SymEditPanel::AttachedToWindow()
{
	Panel::AttachedToWindow();
}

void
SymEditPanel::Draw(BRect r)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}


void
SymEditPanel::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case SET_NAME: {
		if (vName) {
			char		*nm = (char *)vName->Text();
			if (strcmp(nm, mySym->name) != 0) {
				glob.Rename(mySym, nm);
				vName->ResizeTo(vName->StringWidth(nm)+5, 17);
				ArrangeChildren();
	//			fprintf(stderr, "yup %s\n", nm);
	//			if (setTitle) {
	//				sequencerWindow->SetTitle(sym->name);
	//				char	buf[2*MAX_QUA_NAME_LENGTH];
	//				sprintf(buf, "%s: mixer", sym->name);
	//				mixerWindow->SetTitle(buf);
	//			}
			}
		}
		break;
	}
	
	case MOVE_SYMBOL: {
		StabEnt		*s, *ctxt;
		inMsg->FindPointer("symbol", (void **)&s);
		inMsg->FindPointer("context", (void **)&ctxt);
		if (Stackable *S=mySym->StackableValue()) {
			S->ReAllocateChildren();
		}
		break;
	}
	
	case ADD_SYMBOL: {
		base_type_t	typ = inMsg->FindInt32("type");
		StabEnt			*ctxt=mySym;
		int				indent=0;
		int				pos=dataChildrenView->listView->CurrentSelection(0);
		
		if (inMsg->HasPointer("context"))
			inMsg->FindPointer("context", (void **) &ctxt);
		if (inMsg->HasInt32("indent"))
			indent=inMsg->FindInt32("indent");
		if (inMsg->HasInt32("position"))
			pos=inMsg->FindInt32("position");
		if (pos < 0)
			pos = 0;
		
		switch (typ) {

		case S_BOOL:
		case S_BYTE:
		case S_SHORT:
		case S_INT:
		case S_LONG:
		case S_NOTE:
		case S_CTRL:
		case S_PROG:
		case S_BEND:
		case S_SYSC:
		case S_SYSX:
		case S_JOY:
		case S_MESSAGE:
		case S_FLOAT: {
		    StabEnt *sym = DefineSymbol("item", typ, 0,
		    			(void*)AllocStack(ctxt,typ), ctxt,
		    			REF_STACK, ATTRIB_NONE, false, DISP_MODE_NOT);
			dataChildrenView->AddDataItem(sym, indent, pos);
			if (dataChildrenView->Parent() == nullptr)
				AddChild(dataChildrenView);
			break;
		}
		
		case S_STRUCT: {
		    StabEnt *sym = DefineSymbol("structure", S_STRUCT, 0,
		    			(void*)AllocStack(ctxt,S_STRUCT), ctxt,
		    			REF_STACK, ATTRIB_NONE, false, DISP_MODE_NOT);
			dataChildrenView->AddDataItem(sym, indent, pos);
			if (dataChildrenView->Parent() == nullptr)
				AddChild(dataChildrenView);
			break;
		}

		case S_METHOD: {	// assume this context!
		    Method	*F;
	
		    F = new Method("Method", mySym,
		    			uberQua, /*isLocus*/true, /*isModal*/false, /*isOncer*/false,
		    			/*isFixed*/false, /*isHeld*/false, /*isInit*/false);
			F->next = uberQua->methods;
			uberQua->methods = F;
			SymEditPanel *SP =
				new ExecutableEditPanel(BRect(0,0,2*MIXER_MARGIN,2*MIXER_MARGIN),
								maxWidth-2*MIXER_MARGIN,
								F->sym, uberQua);
	
			AddChild(SP);
			ArrangeChildren();

			break;
		}
		}
		break;
	}
	
	case DELETE_SYMBOL: {
		StabEnt		*s;
		inMsg->FindPointer("symbol", (void **)&s);
		if (s) {
			switch (s->type) {
	
			case S_BOOL:
			case S_BYTE:
			case S_SHORT:
			case S_INT:
			case S_LONG:
			case S_NOTE:
			case S_CTRL:
			case S_PROG:
			case S_BEND:
			case S_SYSC:
			case S_SYSX:
			case S_JOY:
			case S_MESSAGE:
			case S_FLOAT: {
				dataChildrenView->RemoveDataItem(s);
				glob.DeleteSymbol(s);
				if (Stackable *S=mySym->StackableValue()) {
					S->ReAllocateChildren();
				}
				break;
			}
			
			case S_STRUCT: {
				dataChildrenView->RemoveDataItem(s);
				glob.DeleteSymbol(s);
				if (Stackable *S=mySym->StackableValue()) {
					S->ReAllocateChildren();
				}
				break;
			}
	
			case S_METHOD: {	// assume this context!
				for (short i=0; i<CountChildren(); i++) {
					if (strcmp(ChildAt(i)->Name(), "symbol") == 0) {
						SymEditPanel	*SP = (SymEditPanel*)ChildAt(i);
						if (SP->mySym == s) {
							RemoveChild(SP);
						}
					}
				}
				glob.DeleteSymbol(s);
				ArrangeChildren();
			}
		}
		}
		break;
	}
	
	default:
		Panel::MessageReceived(inMsg);
	}
}

void
SymEditPanel::MouseDown(BPoint p)
{
	Panel::MouseDown(p);
}

void
SymEditPanel::ArrangeChildren()
{
	BRect	r;
	if (vName)
		r = vName->Frame();
	else
		r.Set(1,1,1+StringWidth(mySym->name),13);
	origH = myHeight;
	origW = myWidth;
	myHeight = r.bottom + MIXER_MARGIN;
	myWidth = r.right + MIXER_MARGIN;
	rowHeight = MIXER_MARGIN;
	thePoint.Set(myWidth,MIXER_MARGIN);

	if (dataChildrenView && dataChildrenView->Parent()) {
		BRect	r = dataChildrenView->Bounds();
		GetItemRect(MIXER_MARGIN, r.right-r.left, r.bottom-r.top, 5, r);
		dataChildrenView->MoveTo(r.left,r.top);
	}
	
	ShuffleSymEditChildren();
}

void
SymEditPanel::ReScale(float x)
{
	maxWidth = x;
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if (strcmp(V->Name(), "symbol") == 0) {
			SymEditPanel	*p = (SymEditPanel *)V;
			if (p->CountChildren() > 1)
				p->ReScale(x-2*MIXER_MARGIN);
		}
	}
	ArrangeChildren();
}

void
SymEditPanel::FrameResized(float x, float y)
{
	BView *v;
	myHeight = y;
	myWidth = x;
	if ((v=Parent()) == nullptr) {
	} else if (strcmp(v->Name(),"symbol") == 0) {
		SymEditPanel	*s = (SymEditPanel*)v;
		s->ArrangeChildren();
	} else if (strcmp(v->Name(), "back") == 0) {
//		Window()->ResizeBy(0/*myWidth-origW*/, myHeight-origH);
		Editor	*e = (Editor*)Window();
		if (e->ctrlScroll) {
			BRect	cr = Parent()->Bounds();
			float	u = myHeight - (cr.bottom-cr.top);
			u = Max(0,u);
			e->ctrlScroll->SetRange(0, u);
		}
	}
	Invalidate();
}

void
SymEditPanel::ShuffleSymEditChildren()
{
	BRect	wrect;
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if (strcmp(V->Name(), "symbol") == 0) {
			SymEditPanel	*p = (SymEditPanel *)V;
			if (p->mySym->controlInfo) {
				GetItemRect(MIXER_MARGIN, p->myWidth, p->myHeight, 5, wrect);
				p->MoveTo(wrect.left,wrect.top);
			}
		}
	}
	
	bool	neednl = true;
	
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if (strcmp(V->Name(), "symbol") == 0) {
			SymEditPanel	*p = (SymEditPanel *)V;
			if (! p->mySym->controlInfo &&
				  p->mySym->type != S_METHOD &&
				  p->mySym->type != S_SCOPE) {
				if (neednl) {
					thePoint.x = MIXER_MARGIN;
					thePoint.y = myHeight;
					rowHeight = MIXER_MARGIN;
					myHeight = thePoint.y;
					neednl = false;
				}
				GetItemRect(MIXER_MARGIN, p->myWidth, p->myHeight, 5, wrect);
				p->MoveTo(wrect.left,wrect.top);
			}
		}
	}

	neednl = true;
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if (strcmp(V->Name(), "symbol") == 0) {
			SymEditPanel	*p = (SymEditPanel *)V;
			if (p->mySym->type == S_METHOD) {
				if (neednl) {
					thePoint.x = MIXER_MARGIN;
					thePoint.y = myHeight;
					rowHeight = MIXER_MARGIN;
					myHeight = thePoint.y;
					neednl = false;
				}
				GetItemRect(MIXER_MARGIN, p->myWidth, p->myHeight, 5, wrect);
				p->MoveTo(wrect.left,wrect.top);
			}
		}
	}

	neednl = false;
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if (strcmp(V->Name(), "symbol") == 0) {
			SymEditPanel	*p = (SymEditPanel *)V;
			if (p->mySym->type == S_SCOPE) {
				if (neednl) {
					thePoint.x = MIXER_MARGIN;
					thePoint.y = myHeight;
					rowHeight = MIXER_MARGIN;
					myHeight = thePoint.y;
					neednl = false;
				}
				GetItemRect(MIXER_MARGIN, p->myWidth, p->myHeight, 5, wrect);
				p->MoveTo(wrect.left,wrect.top);
			}
		}
	}
}

BMenuItem *
AddTypeMenuItem(BHandler *h, BMenu *sub, base_type_t type, char *lbl)
{
	BMessage	*msg = new BMessage(ADD_SYMBOL);
	msg->AddInt32("type", type);
	BMenuItem	*item = new BMenuItem(lbl, msg);
	item->SetTarget(h);
	sub->AddItem(item);
	return item;
}

void
SymEditPanel::EditMenu(BPoint p)
{
	BPopUpMenu		*qMenu = new BPopUpMenu("edit", true, FALSE);
	
	BPoint			orig = p;
	ConvertToScreen(&orig);

	BMenu	*sub = new BMenu("Add Item");
	qMenu->AddItem(sub);
	
	AddTypeMenuItem(this, sub, S_METHOD, "method");
	AddTypeMenuItem(this, sub, S_FLOAT, "real value");
	AddTypeMenuItem(this, sub, S_BYTE, "8 bit value");
	AddTypeMenuItem(this, sub, S_SHORT, "16 bit value");
	AddTypeMenuItem(this, sub, S_INT, "32 bit value");
	AddTypeMenuItem(this, sub, S_LONG, "64 bit value");
	AddTypeMenuItem(this, sub, S_BOOL, "boolean value");
	AddTypeMenuItem(this, sub, S_STRUCT, "structure");
	AddTypeMenuItem(this, sub, S_NOTE, "midi note");
	AddTypeMenuItem(this, sub, S_CTRL, "midi control");
	AddTypeMenuItem(this, sub, S_PROG, "midi program");
	AddTypeMenuItem(this, sub, S_BEND, "midi bend");
	AddTypeMenuItem(this, sub, S_SYSC, "midi system common command");
	AddTypeMenuItem(this, sub, S_SYSX, "midi system exclusive command");
	AddTypeMenuItem(this, sub, S_MESSAGE, "message");
		
	sub = new BMenu("Delete Item");
	qMenu->AddItem(sub);

	for (StabEnt *p=mySym->children; p!=nullptr; p=p->sibling) {
		if (	!p->isDeleted && p->refType != REF_INSTANCE &&
				!p->isClone && p->type != S_SCOPE && p->type != S_INSTANCE) {
			BMessage *msg = new BMessage(DELETE_SYMBOL);
			msg->AddPointer("symbol", p);
			BMenuItem *item = new BMenuItem(p->name, msg);
			item->SetTarget(this);
			sub->AddItem(item);
		}
	}
		
	qMenu->SetAsyncAutoDestruct(true);
	qMenu->Go(orig, true, false, true);
}

ExecutableEditPanel::ExecutableEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq):
	SymEditPanel(r, maxw, S, uq, true)
{
	Executable	*E = S->ExecutableValue();
	if (E) {
#ifdef BLOCKGRAPH
		myBlock = new BlockGraph(BRect(0,0,20,20), E, uq);
		myBlock->SetValue(E->mainBlock);
#else
		myBlock = new SymEditBlockView(BRect(0,0,20,20), uq);
		myBlock->SetValue(E->mainBlock);
		myBlock->ResizeToFit();
#endif
		AddChild(myBlock);
		BRect	r = myBlock->Bounds();
		GetItemRect(MIXER_MARGIN, r.right-r.left, r.bottom-r.top, 5, r);
		myBlock->MoveTo(r.left,r.top);
	} else {
		myBlock = nullptr;
	}
}

void
ExecutableEditPanel::ArrangeChildren()
{
	SymEditPanel::ArrangeChildren();
	if (myBlock) {
		BRect	r = myBlock->Bounds();
		GetItemRect(MIXER_MARGIN, r.right-r.left+4, r.bottom-r.top+4, 5, r);
		myBlock->MoveTo(r.left+2,r.top+2);
	}
}

void
ExecutableEditPanel::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case COMPILE_BLOCK: {
		Block		*b = myBlock->BlockValue(mySym);
		Executable	*e = mySym->ExecutableValue();

		if (e) {
			e->ReplaceBlock(e->mainBlock, b);
		}
		break;
	}
	default:
		SymEditPanel::MessageReceived(inMsg);
	}
}


void
ExecutableEditPanel::Draw(BRect r)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);

	SetFontSize(10);
	SetFont(be_plain_font);

	SetHighColor(black);SetLowColor(ViewColor());
	
	if (myBlock != nullptr && displayMode == PANEL_DISPLAY_BIG) {
		LoweredBox(this, myBlock->Frame(), ViewColor(), false);
	}
}


void
ExecutableEditPanel::MouseDown(BPoint p)
{
	long		channel, quant;
	BRect		area = Bounds();

	ulong		mods = modifiers(); // Key mods???
	ulong		buts;
	BMessage	*msg;
	BPoint		pt;
	drawing_mode	cur_mode = DrawingMode();
	long		clicks;
	
	GetMouse(&pt, &buts);
	msg = Window()->CurrentMessage();
	
	if ((clicks=msg->FindInt32("clicks")) == 1) {
		if (buts & B_SECONDARY_MOUSE_BUTTON) {
			RunControlMenu(p);
		} else {
			EditMenu(p);
//			msg = new BMessage(MOVE_OBJECT);
//		 	msg->AddPointer("object_view", this);
		 	
			if (mods & B_SHIFT_KEY) {
//				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
//				((ObjectViewContainer *)Parent())->Select(this);
			}
		
//			DragMessage(msg, area);
		}
	} else if (clicks > 1) {	// edit object
	} else {
	}
}

ScopeEditPanel::ScopeEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq):
	SymEditPanel(r, maxw, S, uq, false)
{
	Scope	*E = S->ScopeValue();
	if (vName) {
		vName->SetFont(be_plain_font);
		vName->SetFontSize(10);
		vName->ResizeTo(vName->StringWidth(vName->Text())+5, 12);
		vName->MoveTo(1, 1);
	}
	if (E) {
#ifdef BLOCKGRAPH
		myBlock = new BlockGraph(BRect(0,0,20,20), E, uq);
#else
		myBlock = new SymEditBlockView(BRect(0,0,20,20), uq);
#endif
		myBlock->SetValue(E->block);
		myBlock->ResizeToFit();
		AddChild(myBlock);
//		BRect	r = myBlock->Bounds();
//		GetItemRect(MIXER_MARGIN, r.right-r.left, r.bottom-r.top, 5, r);
//		myBlock->MoveTo(r.left,r.top);
	} else {
		myBlock = nullptr;
	}
	displayMode = -1;
	SetDisplayMode(E->block?PANEL_DISPLAY_BIG:PANEL_DISPLAY_SMALL);
}

void
ScopeEditPanel::ArrangeChildren()
{
	BRect	r;
	if (vName)
		r = vName->Frame();
	else
		r.Set(1,1,1+StringWidth(mySym->name), 13);
	if (displayMode == PANEL_DISPLAY_SMALL) {
		BPoint p(r.right,r.bottom);
		myWidth = p.x+3; myHeight = p.y + 1;
	} else {
		origH = myHeight;
		origW = myWidth;
		rowHeight = MIXER_MARGIN;
		thePoint.Set(0, r.bottom);
		if (myBlock) {
			r = myBlock->Bounds();
			GetItemRect(MIXER_MARGIN, r.right-r.left+4, r.bottom-r.top+4, 5, r);
			myBlock->MoveTo(r.left+2,r.top+2);
		}
		myHeight = r.bottom+1;
		myWidth = r.right+1;
	}
	ResizeTo(myWidth, myHeight);
}

void
ScopeEditPanel::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case COMPILE_BLOCK: {
			// let's just presume the simplest, this kind of
			// structure only appears as a hack just within
			// a useful context
			
		glob.PushContext(mySym->context);
		Block		*b = myBlock->BlockValue(mySym);
		Scope		*e = mySym->ScopeValue();
		glob.PopContext();
		
		if (e) {
			e->ReplaceBlock(e->block, b);
		}
		break;
	}
	default:
		SymEditPanel::MessageReceived(inMsg);
	}
}

void
ScopeEditPanel::SetDisplayMode(short dm)
{
	short	odm = displayMode;
	displayMode = dm;
	if (odm != dm) {
		if (displayMode == PANEL_DISPLAY_SMALL) {
			RemoveChild(myBlock);
			ArrangeChildren();
			Invalidate();
		} else {
			if (myBlock->Parent() == nullptr)
				AddChild(myBlock);
			ArrangeChildren();
			Invalidate();
		}
	}
}

void
ScopeEditPanel::Draw(BRect r)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	char	buf[30];
	sprintf(buf, "%s", mySym->name);
	SetFont(be_plain_font);
	SetFontSize(10);
	SetHighColor(black);
	SetLowColor(ViewColor());
	DrawString(buf, BPoint(3, 10));
	RaisedBox(this, b, ViewColor(), false);
	if (myBlock != nullptr && displayMode == PANEL_DISPLAY_BIG) {
		LoweredBox(this, myBlock->Frame(), ViewColor(), false);
	}
}


void
ScopeEditPanel::MouseDown(BPoint p)
{
	SymEditPanel::MouseDown(p);
}


SchedulableEditPanel::SchedulableEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq):
	SymEditPanel(r, maxw, S, uq, true)
{
	Schedulable	*E = S->SchedulableValue();
	if (E) {
#ifdef BLOCKGRAPH
		myBlock = new BlockGraph(BRect(0,0,20,20), E, uq);
#else
		myBlock = new SymEditBlockView(BRect(0,0,20,20), uq);
#endif
		myBlock->SetValue(E->mainBlock);
		myBlock->ResizeToFit();
		AddChild(myBlock);
		
	} else {
		myBlock = nullptr;
	}
	ArrangeChildren();
}


void
SchedulableEditPanel::Draw(BRect r)
{
//	BRect	b(1,1,myWidth-1, myHeight-1);
//	RaisedBox(this, b, ViewColor(), false);

	SetFontSize(10);
	SetFont(be_plain_font);

	SetHighColor(black);SetLowColor(ViewColor());
	
	if (myBlock != nullptr && displayMode == PANEL_DISPLAY_BIG) {
		LoweredBox(this, myBlock->Frame(), ViewColor(), false);
	}
}


void
SchedulableEditPanel::ArrangeChildren()
{
	SymEditPanel::ArrangeChildren();

	if (myBlock) {
		BRect	r = myBlock->Bounds();
		GetItemRect(MIXER_MARGIN, r.right-r.left+4, r.bottom-r.top+7, 5, r);
		myBlock->MoveTo(r.left+2,r.top+2);
	}
	
	myWidth=maxWidth;
	ResizeTo(maxWidth,myHeight);
}

void
SchedulableEditPanel::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case COMPILE_BLOCK: {

		Schedulable	*e = mySym->SchedulableValue();
		Block		*b = nullptr;

		if (inMsg->HasPointer("block view")) {
			BlockView	*bv;
			inMsg->FindPointer("block view", (void **)&bv);
			b = bv->BlockValue(mySym);
		} else if (inMsg->HasPointer("block graph")) {
			BlockGraph	*bv;
			inMsg->FindPointer("block graph", (void **)&bv);
			b = bv->BlockValue(mySym);
		} else if (inMsg->HasPointer("graph block view")) {
			GraphBlockView	*bv;
			inMsg->FindPointer("graph block view", (void **)&bv);
			b = bv->BlockValue(mySym);
		} else {
			b = myBlock->BlockValue(mySym);;
		}
		if (e) {
			e->ReplaceBlock(e->mainBlock, b);
		}
		break;
	}
	default:
		SymEditPanel::MessageReceived(inMsg);
	}
}

void
SchedulableEditPanel::MouseDown(BPoint p)
{
	BRect		area = Bounds();

	ulong		mods = modifiers(); // Key mods???
	ulong		buts;
	BMessage	*msg;
	BPoint		pt;
	drawing_mode	cur_mode = DrawingMode();
	long		clicks;
	
	GetMouse(&pt, &buts);
	msg = Window()->CurrentMessage();
	
	if ((clicks=msg->FindInt32("clicks")) == 1) {
		if (buts & B_SECONDARY_MOUSE_BUTTON) {
			RunControlMenu(p);
		} else {
			EditMenu(p);
//			msg = new BMessage(MOVE_OBJECT);
//		 	msg->AddPointer("object_view", this);
		 	
			if (mods & B_SHIFT_KEY) {
//				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
//				((ObjectViewContainer *)Parent())->Select(this);
			}
		
//			DragMessage(msg, area);
		}
	} else if (clicks > 1) {	// edit object
	} else {
	}
}


ChannelEditPanel::ChannelEditPanel(BRect r, float maxw, StabEnt *S, Qua *uq):
	SymEditPanel(r, maxw, S, uq, true)
{
}

void
ChannelEditPanel::ArrangeChildren()
{
	SymEditPanel::ArrangeChildren();
}

void
ChannelEditPanel::Draw(BRect r)
{
//	if (myBlock != nullptr && displayMode == PANEL_DISPLAY_BIG) {
//		RaisedBox(this, myBlock->Frame(), ViewColor(), false);
//	}
}


void
ChannelEditPanel::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case COMPILE_BLOCK: {
//		Block		*b = myBlock->BlockValue(mySym);
//		Channel		*e = mySym->ChannelValue();
//
//		if (e) {
//			e->ReplaceBlock(e->mainBlock, b);
//		}
		break;
	}
	default:
		SymEditPanel::MessageReceived(inMsg);
	}
}


void
ChannelEditPanel::MouseDown(BPoint p)
{
	EditMenu(p);
}


DataEditPanel::DataEditPanel(BRect r, float maxw, StabEnt *S, Qua *q):
	SymEditPanel(r, maxw, S, q, true)
{
}

void
DataEditPanel::MessageReceived(BMessage *inMsg)
{
	SymEditPanel::MessageReceived(inMsg);
}

void
DataEditPanel::ArrangeChildren()
{
	BRect	r = vName->Frame();
	myWidth = r.right-r.left+2*MIXER_MARGIN;
	myHeight = r.bottom - r.top+2*MIXER_MARGIN;
	ResizeTo(myWidth, myHeight);
}

void
DataEditPanel::MouseDown(BPoint p)
{
}


