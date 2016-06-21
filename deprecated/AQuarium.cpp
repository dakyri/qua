
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include <stdio.h>

#include "StdDefs.h"
#include "include/SequencerWindow.h"
#include "include/ArrangerObject.h"
#include "include/QuaObject.h"
#include "Colors.h"
#include "include/messid.h"
#include "include/Qua.h"
#include "include/Application.h"
#include "include/AQuarium.h"
#include "include/Template.h"
#include "include/QuaPort.h"
#include "include/Toolbox.h"

AQuarium::AQuarium(BRect area, char *name, Qua *q) :
	RosterView(area, name)
{
	SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	uberQua = q;
	
	palette = new ColorSelectObjectView(BRect(5, 10, 5, 10));
	AddChild(palette);

	templateView = new TemplateObjectView(BRect(5, 10, 5, 10));
	AddChild(templateView);

	libraryView = new LibraryObjectView(BRect(5, 10, 5, 10));
	AddChild(libraryView);
	
	audiofxView = new AudioFXObjectView(BRect(5, 10, 5, 10));
	AddChild(audiofxView);
}


AQuarium::~AQuarium()
{
	fprintf(stderr, "deleted aquarium\n");
}

void
AQuarium::Draw(BRect area)
{
	ObjectViewContainer::Draw(area);
}

void
AQuarium::MouseDown(BPoint p)
{
	ObjectViewContainer::MouseDown(p);
}

bool
AQuarium::MessageDropped(BMessage *message,BPoint where,
								BPoint delta)
{
	if (message->what == MOVE_OBJECT) {
		if (message->HasPointer("qua_object")) {		
			QuaObject		*obj;
			message->FindPointer("qua_object", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			obj->MoveTo(where);
			return TRUE;
		} else if (message->HasPointer("sym_object")) {
			QuaSymbolBridge		*obj;
			message->FindPointer("sym_object", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			QuaObject *newO = uberQua->LoadObject(obj->sym, where);
			obj->MoveTo(where);
			return TRUE;
		} else if (message->HasPointer("port_object")) {
			PortObject		*obj;
			message->FindPointer("port_object", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			obj->MoveTo(where);
			return TRUE;
		}
	} else if (message->what == ADD_SYMBOL) {
		if (message->HasPointer("symbol")) {
			StabEnt		*obj;
			message->FindPointer("symbol", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			QuaObject *newO = uberQua->LoadObject(obj, where);
//			obj->MoveTo(where);
			return TRUE;
		}
	} else if (message->what == B_SIMPLE_DATA) {
		if (message->HasRef("refs")) {
			entry_ref		ref;
			status_t err = message->FindRef("refs", &ref);
			where = where - delta;
			BPath	path;
			BEntry	ent(&ref);
			ent.GetPath(&path);
			QuaObject *newO = uberQua->LoadFile(&path, where);
			
		} else
			return FALSE;
	} else
		return FALSE;

	return ObjectViewContainer::MessageDropped(message, where, delta);
}


void
AQuarium::KeyDown(const char *c, int32 nb)
{
	if (nb == 1) {
		char	theChar = c[0];
		if (theChar == B_DELETE) {
			BList	L(selectedObjects);
			DeSelectAll();
			for (short i=0; i<L.CountItems(); i++) {
				QuaObject *p = (QuaObject *)L.ItemAt(i);
				RemoveChild(p);
				uberQua->DeleteQuaObject(p);
			}
		} else 
			BView::KeyDown(c, nb);
	} else {
		BView::KeyDown(c, nb);
	}
}

void
AQuarium::MessageReceived(BMessage *message)
{
	if (message->WasDropped()) {
		BPoint delta(0, 0);
		BPoint where = message->DropPoint(&delta);
		ConvertFromScreen(&where);
		if (!MessageDropped(message, where, delta)) {
			RosterView::MessageReceived(message);
		}
		return;
	} else {
		RosterView::MessageReceived(message);
	}
}
