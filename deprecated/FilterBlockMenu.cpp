#include "QuaQua.h"
#include "Source.h"
#include "FilterBlockMenu.h"
#include "Block.h"
#include "messid.h"

FilterBlockMenu::FilterBlockMenu(Block *B, BHandler *V, QuaQua *Q):
	BMenu("Delete")
{
	uberQua = Q;
	handler = V;
	block = B;
	SetFont(be_plain_font);
}

void
FilterBlockMenu::AttachedToWindow()
{
#ifdef CHAOTIC
	int n = CountItems();
	for (int i=n-1; i>=0; i--) {
		if (BMenuItem*it = RemoveItem(i)) {
			delete it;
		}
	}

	for (Block *B=block->Crap.List.Block; B!=nullptr; B=B->Next) {
		BMessage	*msg = new BMessage(DEL_ITEM);
		msg->AddPointer("obj", B);
		msg->AddInt32("ind", B->Crap.Call.LocusNo);
		BMenuItem *item = new BMenuItem(B->Crap.Call.Crap.Call.Atom->Crap.Sym->Name, msg);
		AddItem(item);
		item->SetTarget(handler);
	}
	BMenu::AttachedToWindow();
#endif
}
