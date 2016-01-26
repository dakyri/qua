
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "PoolSelect.h"
#include "Pool.h"
#include "Sym.h"
#include "Qua.h"
#include "messid.h"
#include "Colors.h"

#include <ctype.h>

PoolSelect::PoolSelect(BRect frame, const char *name,
		          char *label, 
		          Pool *p,
					BMessage *msg, Qua *q,
					ulong f1, ulong f2):
	BControl(frame, name, label, msg, f1, f2)

{
	uberQua = q;
	selected = p;
	
	divider = 0;
	textrect = Bounds();
	textrect.Set(textrect.left+2,textrect.top+2,
				textrect.right-1,textrect.bottom-1);
	textview = new BTextView(textrect, name, textrect, f1, f2);
	if (p)
		textview->SetText(p->sym->name);
	AddChild(textview);
}

void
PoolSelect::AttachedToWindow()
{
	textview->MakeEditable(FALSE);
	textview->MakeSelectable(FALSE);
}

void PoolSelect::Draw(BRect reg)
{
	BPoint	Pos;
	
	if (Parent())SetViewColor(Parent()->ViewColor());
	
	BRect wr = Bounds();
	SetHighColor(ViewColor());
	FillRect(wr);
	
	SetLowColor(ViewColor());
	SetHighColor(black);SetPenSize(1.0);
	Pos.Set(wr.left,wr.bottom);	MovePenTo(Pos);
	SetHighColor(128,128,128);
	Pos.Set(wr.left,wr.top);	StrokeLine(Pos);
	Pos.Set(wr.right,wr.top);	StrokeLine(Pos);
	SetHighColor(230,230,230);
	Pos.Set(wr.right,wr.bottom);StrokeLine(Pos);
	Pos.Set(wr.left,wr.bottom);	StrokeLine(Pos);
	
	SetHighColor(black);
	Pos.Set(3,textrect.bottom-3);DrawString(Label(), Pos);
}

void PoolSelect::SetDivider(float xCoordinate)
{
	BPoint	pos;
	
	divider = xCoordinate;
	textrect.Set(divider+2, textrect.top, textrect.right, textrect.bottom);
	textview->ResizeTo(textrect.right - textrect.left, textrect.bottom-textrect.top);
	pos.Set(textrect.left,textrect.top);
	textview->MoveTo(pos);
}

PoolSelect::~PoolSelect()
{
}


void
PoolSelect::MouseDown(BPoint pt)
{
	if (uberQua) {
		BMenuItem	*item;
		BMessage	*msg;
		BPopUpMenu	*qMenu = new BPopUpMenu("sel", FALSE, FALSE);
		
		for (Schedulable	*p = uberQua->schedulees; p != nullptr; p = p->next) {
			if (p->sym->type == S_POOL) {
				msg = new BMessage(PARAM_SET);
				msg->AddPointer("obj", p);
			
				item = new BMenuItem(p->sym->name, msg);
				qMenu->AddItem(item);
				item->SetTarget(this);
			}
		}
	
		qMenu->AddSeparatorItem();
		
		msg = new BMessage(PARAM_SET);
		msg->AddPointer("obj", nullptr);
		
		item = new BMenuItem("New", msg);
		qMenu->AddItem(item);
		item->SetTarget(this);

		pt.Set(0,0);	
		ConvertToScreen(&pt);
		qMenu->SetAsyncAutoDestruct(true);
		qMenu->Go(pt, true, false, true);
	}
}

void
PoolSelect::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case PARAM_SET:
		msg->FindPointer("obj", (void **)&selected);
		if (!selected)  {
			selected = uberQua->NewPool(0);
		}
		if (selected)
			textview->SetText(selected->sym->name);
		Invoke();	// with the BControls msg
		break;
	default:
		BControl::MessageReceived(msg);
	}
}
