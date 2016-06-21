#include "DestinationSelect.h"
#include "Sym.h"
#include "Qua.h"
#include "messid.h"
#include "QuaObject.h"
#include "Instance.h"
#include "Colors.h"
#include "Voice.h"
#include "Pool.h"
#include "Sample.h"
#include "Application.h"
#include "QuaMidi.h"
#include "QuaJoystick.h"

#include <ctype.h>

DestinationSelect::DestinationSelect(BRect frame, const char *name,
		char *label, 
		Destination *d, 
		BMessage *msg, Qua *q,
		ulong f1, ulong f2):
	BControl(frame, name, label, msg, f1, f2)
{
	uberQua = q;
	
	divider = 0;
	textrect = Bounds();
	textrect.Set(textrect.left+2,textrect.top+2,
				textrect.right-1,textrect.bottom-1);
	textview = new BTextView(textrect, name, textrect, nullptr, B_WILL_DRAW);
	uberThar = d;
	textview->SetText(d->Name());
	AddChild(textview);
}

void
DestinationSelect::AttachedToWindow()
{
	textview->MakeEditable(FALSE);
	textview->MakeSelectable(FALSE);
}

void DestinationSelect::Draw(BRect reg)
{
	BPoint	Pos;
	
	SetViewColor(Parent()->ViewColor());
	
	BRect wr = Bounds();
	SetHighColor(wtGray);
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

void DestinationSelect::SetDivider(float xCoordinate)
{
	BPoint	pos;
	
	divider = xCoordinate;
	textrect.Set(divider+2, textrect.top, textrect.right, textrect.bottom);
	textview->ResizeTo(textrect.right - textrect.left, textrect.bottom-textrect.top);
	pos.Set(textrect.left,textrect.top);
	textview->MoveTo(pos);
}

DestinationSelect::~DestinationSelect()
{
}


void
DestinationSelect::MouseDown(BPoint pt)
{
	if (uberQua) {
		BMenuItem	*item;
		BMessage	*msg;
		BPopUpMenu	*qMenu = new BPopUpMenu("sel", FALSE, FALSE);
		BMenu		*parent;
				
		parent = new BMenu("midi");
		qMenu->AddItem(parent);
		for (short j=0; j<MAX_MIDI_PORTS; j++) {
			char buf[5];
			sprintf(buf, "%c", j+'A');
			BMenu *p2 = new BMenu(buf);
			for ( short i=1;i<=16;i++) {
				sprintf(buf, "%d", i);
				msg = new BMessage(DESTINATION_SELECT);
				msg->AddInt32("midi", j);
				msg->AddInt32("channel", i);
				item = new BMenuItem(buf, msg);
				p2->AddItem(item);
				item->SetTarget(this);
			}
			parent->AddItem(p2);
		}
	
//		msg = new BMessage(PARAM_SET);
//		msg->AddInt32("audio", 0);
//		BMenuItem *it = new BMenuItem("audio", msg);
//		qMenu->AddItem(it);
//		
		if (uberThar->isInput) {
			parent = new BMenu("joystick");
			qMenu->AddItem(parent);
			for (short j=0; j<quaJoystick->nJoyDevices; j++) {
				char buf[5];
				sprintf(buf, "%d", j);
				msg = new BMessage(DESTINATION_SELECT);
				msg->AddInt32("joystick", j);
				item = new BMenuItem(buf, msg);
				parent->AddItem(item);
				item->SetTarget(this);
			}
		}
		
//		parent = new BMenu("midi B");
//		qMenu->AddItem(parent);
//		for ( short i=1;i<=16;i++) {
//			char buf[5];
//			sprintf(buf, "%d", i);
//			msg = new BMessage(DESTINATION_SELECT);
//			msg->AddInt32("midib", i);
//			item = new BMenuItem(buf, msg);
//			parent->AddItem(item);
//			item->SetTarget(this);
//		}
	
//		parent = new BMenu("object");
//		qMenu->AddItem(parent);
//		for (Schedulable *A=uberQua->schedulees; A!=nullptr; A=A->next) {
//			if (A->sym->Type != S_SOURCE) {
//				msg = new BMessage(DESTINATION_SELECT);
//				msg->AddPointer("schedulable", A);
//				item = new BMenuItem(A->sym->UniqueName(), msg);
//				parent->AddItem(item);
//				item->SetTarget(this);
//			}
//		}
//	
//		parent = new BMenu("instance");
//		qMenu->AddItem(parent);
//		for (Schedulable *A=uberQua->schedulees; A!=nullptr; A=A->next) {
//			if (A->sym->Type != S_SOURCE) {
//				BMenu *sm = new BMenu(A->sym->UniqueName());
//				BList	*i=nullptr;
//				switch (A->sym->Type) {
//				case S_VOICE:	i = &A->sym->VoiceValue()->instances; break;
//				case S_POOL:	i = &A->sym->PoolValue()->instances; break;
//				case S_SAMPLE:	i = &A->sym->SampleValue()->instances; break;
//				case S_APPLICATION:		i = &A->sym->ApplicationValue()->instances; break;
//				}
//				if (i)
//					for (short j=0; j<i->CountItems(); j++) {
//					}
//			}
//		}
	
		
		pt.Set(0,0);	
		ConvertToScreen(&pt);
		item = qMenu->Go(pt);
		if (item && item->Message()) {
			this->Window()->PostMessage(item->Message(), this);
		}
		delete qMenu;
	}
}

void
DestinationSelect::MessageReceived(BMessage *msg)
{
	BMessage	*tm = Message();
	switch (msg->what) {
	case DESTINATION_SELECT:
		if (msg->HasInt32("midi")) {
			int c=msg->FindInt32("midi");
			int ch=msg->FindInt32("channel");
			if (tm) {
				tm->AddInt32("midi", c);
				tm->AddInt32("channel", ch);
			}
//			if (uberThar->Set(QUA_DEV_MIDI, c, ch))
//				textview->SetText(uberThar->Name());
		} else if (msg->HasInt32("joystick")) {
			int c=msg->FindInt32("joystick");
			if (tm) {
				tm->AddInt32("joystick", c);
			}
//			if (uberThar->Set(QUA_DEV_JOYSTICK, c, 0))
//				textview->SetText(uberThar->Name());
		}
//		else if (msg->HasInt32("audio")) {
//			int c=msg->FindInt32("audio");
//			if (uberThar->Set(DST_AUDIO, c, 0))
//				textview->SetText(uberThar->Name());
//		} else if (msg->HasPointer("schedulable")) {
//			Schedulable	*A;
//			msg->FindPointer("schedulable", &A);
//			if (uberThar->Set(A))
//				textview->SetText(uberThar->Name());
//		} else if (msg->HasPointer("instance")) {
//			Instance	*A;
//			msg->FindPointer("instance", &A);
//			if (uberThar->Set(A))
//				textview->SetText(uberThar->Name());
//		}
		Invoke();	// with the BControls msg
		break;
	default:
		BControl::MessageReceived(msg);
	}
}
