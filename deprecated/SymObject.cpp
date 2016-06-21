#include "qua_version.h"

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "include/messid.h"
#include "include/QuaDisplay.h"
#include "include/Sym.h"
#include "include/Quapp.h"
#include "Colors.h"

QuaSymbolBridge::QuaSymbolBridge(StabEnt *S, BRect r, BBitmap *siconData, BBitmap *biconData,
					ObjectViewContainer *a, rgb_color dflt_col):
	ObjectView(r, S->UniqueName(), a, "QuaSymbolBridge", nullptr, B_WILL_DRAW|B_FRAME_EVENTS)
{
	SetFontSize(10);
	if (siconData) {
		sicon = new BBitmap(
					siconData->Bounds(),
					siconData->ColorSpace());
		uchar	*p = (uchar *)sicon->Bits(),
				*q = (uchar *)siconData->Bits();
		for (int i=0; i<siconData->BitsLength(); i++) {
			p[i] = q[i];
		}
	} else {
		sicon = nullptr;
	}
	if (biconData) {
		bicon = new BBitmap(
					biconData->Bounds(),
					biconData->ColorSpace());
		uchar	*p = (uchar *)bicon->Bits(),
				*q = (uchar *)biconData->Bits();
		for (int i=0; i<biconData->BitsLength(); i++) {
			p[i] = q[i];
		}
	} else {
		bicon = nullptr;
	}
	
	sym = S;
	if (Window()) Window()->Lock();
	SetColor(dflt_col);
	if (Window()) Window()->Unlock();
	
	if (bicon)
		SetDisplayMode(OBJECT_DISPLAY_BIG);
	else if (sicon)
		SetDisplayMode(OBJECT_DISPLAY_SMALL);
	else
		SetDisplayMode(OBJECT_DISPLAY_NIKON);
}

bool
QuaSymbolBridge::SetDisplayMode(short dm)
{
	displayMode = dm;
	SetFontSize(10);
	if (displayMode == OBJECT_DISPLAY_BIG) {
		float	w = Max(45,StringWidth(label)+2*2);
		ResizeTo(w, 45);
		labelPoint.Set(3, 43);
		iconPoint.Set((w-32)/2,2);
	} else if (displayMode == OBJECT_DISPLAY_SMALL) {
		float	w = Max(18,StringWidth(label)+2*2+18);
		ResizeTo(w, 19);
		labelPoint.Set(19, 14);
		iconPoint.Set(1,2);
	} else if (displayMode == OBJECT_DISPLAY_NIKON) {
		float	w = Max(45,StringWidth(label)+2*2);
		ResizeTo(w, 45);
		labelPoint.Set(3, 43);
		iconPoint.Set((w-32)/2,2);
	}
	return true;
}

QuaSymbolBridge::~QuaSymbolBridge()
{
	if (sicon)
		delete sicon;
	if (bicon)
		delete bicon;
}

void
QuaSymbolBridge::MouseDown(BPoint where)
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
			BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
			
			BPoint			orig = where;
			ConvertToScreen(&where);
			
			BMessage	*msg = new BMessage(SET_DISPLAY_MODE);
			msg->AddInt32("display mode", OBJECT_DISPLAY_SMALL);
			BMenuItem	*item = new BMenuItem("Small", msg);
			qMenu->AddItem(item);
			item->SetTarget(this);
		
			msg = new BMessage(SET_DISPLAY_MODE);
			msg->AddInt32("display mode", OBJECT_DISPLAY_BIG);
			item = new BMenuItem("Large", msg);
			qMenu->AddItem(item);
			item->SetTarget(this);
		
			qMenu->SetAsyncAutoDestruct(true);
			qMenu->Go(where, true, false, true);
		} else {
			msg = new BMessage(MOVE_OBJECT);
		 	msg->AddPointer("sym_object", this);
		 	
			if (mods & B_SHIFT_KEY) {
				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
				((ObjectViewContainer *)Parent())->Select(this);
			}
		
			DragMessage(msg, area);
		}
	} else if (clicks > 1) {	// edit object
		Edit();
	} else {
	}
}

void
QuaSymbolBridge::Edit()
{
}


void
QuaSymbolBridge::Draw(BRect r)
{
	SetFontSize(10);
	if (displayMode == OBJECT_DISPLAY_BIG || displayMode == OBJECT_DISPLAY_SMALL) {
		BRect	bnd = Bounds();
		
		RaisedBox(this, BRect(bnd.left+1,bnd.top+1,bnd.right-1,bnd.bottom-1),
				color,true);

		SetDrawingMode(B_OP_COPY);
		
		if (displayMode == OBJECT_DISPLAY_BIG)
			DrawBitmap(bicon, iconPoint);
		else
			DrawBitmap(sicon, iconPoint);
		
		if (isSelected) {
			LoweredBox(this, 
				BRect(labelPoint.x,labelPoint.y-8,
					  labelPoint.x+StringWidth(label), labelPoint.y),
				black, true);
			SetHighColor(color);
		} else {
			SetHighColor(black);
		}
		SetDrawingMode(B_OP_OVER);
		DrawString(label, labelPoint);
	} else {
		ObjectView::Draw(r);
	}
}

status_t
StabEnt::SetColor(rgb_color c)
{
	Executable	*e = ExecutableValue();
	if (e && e->representation) {
		e->representation->SetColor(c);
	}
	return B_OK;
}
