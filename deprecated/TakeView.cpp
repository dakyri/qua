
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif


#include "Sample.h"
#include "Pool.h"
#include "TakeView.h"
#include "Colors.h"
#include "SampleEditor.h"
#include "PoolEditor.h"
#include "SampleFile.h"
#include "StdDefs.h"

////////////////////////////////////////////////////////////////////////


TakeView::TakeView(BRect r, char *nm, ulong f1, ulong f2):
	BView(r, nm, f1, f2)
{
}

void
TakeView::FrameResized(float x, float y)
{
	DataEditor *p = ((DataEditor *)Parent());
	if (p) p->ArrangeChildren();
}

void
TakeView::Draw(BRect r)
{
	SetHighColor(black);
	DrawString("Takes", BPoint(2,12));
	LoweredBox(this, listView->Frame(), ViewColor(), false);
}

//////////////////////////////////////////////////////////////////

TakeListView::TakeListView(BRect r, char *nm):
	BListView(r, nm)
{
}

void
TakeListView::KeyDown(const char *c, int32 nb)
{
	if (nb == 1) {
		char	theChar = c[0];
		if (theChar == B_DELETE) {
//			TakeView		*tv=(TakeView *)Parent();
			DeleteSelection();
		} else 
			BView::KeyDown(c, nb);
	} else {
		BView::KeyDown(c, nb);
	}
}


bool
TakeListView::InitiateDrag(BPoint p, int32 index, bool sel)
{
//	TakeListItem	*tli = (TakeListItem *)ItemAt(index);
//
//	if (tli) {
//		BMessage	dragMsg(MOVE_TAKE);
//		dragMsg.AddInt32("take item", index);
//		dragMsg.AddPointer("take list", this);
//		fprintf(stderr, "drag %d %g %g %g %g\n", index,tli->rect.left, tli->rect.top, tli->rect.right, tli->rect.bottom);
//		DragMessage(&dragMsg, tli->rect);
//		return true;
//	}
	return false;
}

void
TakeListView::MouseDown(BPoint p)
{
	ulong		mods = modifiers(); // Key mods???
	
	if (mods & B_SHIFT_KEY) {
		int				index = IndexOf(p);
		TakeListItem	*tli = (TakeListItem *)ItemAt(index);
	
		if (tli) {
			BMessage	dragMsg(MOVE_TAKE);
			dragMsg.AddInt32("take item", index);
			dragMsg.AddPointer("take list", this);
//			fprintf(stderr, "drag %d %g %g %g %g\n", index,tli->rect.left, tli->rect.top, tli->rect.right, tli->rect.bottom);
			DragMessage(&dragMsg, tli->rect);
		}
	} else {
		BListView::MouseDown(p);
	}
}

void
TakeListView::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {

	case MOVE_TAKE: {
		fprintf(stderr, "move take\n");
	
		TakeView		*tv=(TakeView *)Parent();
		TakeListView	*tlv=nullptr;
		inMsg->FindPointer("take list", (void **)&tlv);
		BPoint	where(0, 0), delta(0, 0);
		int		from = inMsg->FindInt32("take item");
		int		to = 0;
		if (inMsg->WasDropped()) {
			where = inMsg->DropPoint(&delta);
			ConvertFromScreen(&where);
//			where = where - delta;
		}
		
		if (tlv == this) {
			to = IndexOf(where);
			if (from != to) {
				if (MoveItem(from, to) && (to == 0 || from == 0)) {
					tv->Select(((TakeListItem*)ItemAt(0))->myTake, true);
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
TakeListView::SelectionChanged()
{
	int		sel=0, cnt=0;
	TakeView	*tv = (TakeView *)Parent();
	while ((sel=CurrentSelection(cnt++)) >= 0) {
		TakeListItem	*tiv = (TakeListItem *)ItemAt(sel);
		tv->Select(tiv->myTake, false);
	}
}
void
TakeListView::DeleteSelection()
{
	int		sel=0, cnt=0;
	TakeView	*tv = (TakeView *)Parent();
	while ((sel=CurrentSelection(cnt++)) >= 0) {
		TakeListItem	*tiv = (TakeListItem *)ItemAt(sel);
		tv->Delete(tiv->myTake);
		RemoveItem(tiv);
		delete tiv;
	}
}

//////////////////////////////////////////////////////////////////

StreamTakeItem::StreamTakeItem(StreamTake *t):
	TakeListItem()
{
	myTake = t;
}


void
StreamTakeItem::DrawItem(BView *lv, BRect ir, bool drall)
{
	BRect	bnd = lv->Bounds();
	float	wid = Max(ir.right, lv->StringWidth(myTake->name)+4);
	float	bot = ir.bottom;
	rect = BRect(ir.left,ir.top,ir.left+wid,ir.top+14);
	TakeListView *stv = (TakeListView *)lv;
	
	if (bot > bnd.bottom || wid > bnd.right) {
		wid = Max(wid,bnd.right);
		bot = Max(bot,bnd.bottom);
		lv->ResizeTo(wid, bot);
		if (stv->Parent()) {
			stv->Parent()->ResizeTo(wid+2, bot+14);
		}
	}
	
	rgb_color		col=lv->ViewColor();
	if (IsSelected()) {
		lv->SetHighColor(col=red);
		lv->FillRect(ir);
	} else if (drall) {
		lv->SetHighColor(col);
		lv->FillRect(ir);
	}
	
	if (stv->ItemAt(0) == (TakeListItem *)this) {
		lv->SetHighColor(blue);
	} else {
		lv->SetHighColor(black);
	}
	lv->SetLowColor(col);
	lv->DrawString(myTake->name, BPoint(2,ir.bottom-1));
}

void
StreamTakeItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
//	fprintf(stderr, "Update\n");
}

status_t
StreamTakeItem::Perform(perform_code d, void *arg)
{
	return Perform(d, arg);
}

/////////////////////////////////////////////////////////////////////////

StreamTakeView::StreamTakeView(BRect r, Pool *s):
	TakeView(r, "dross", (ulong)nullptr, (ulong)B_WILL_DRAW|B_FRAME_EVENTS)
{
	BRect bnd = Bounds();
	listView = new TakeListView(
						BRect(1, 14, bnd.right-1, bnd.bottom-1),
						"Takes");
	myPool = s;
	AddChild(listView);
	for (short i=0; i<s->outTakes.CountItems(); i++) {
		AddTake((StreamTake *)s->outTakes.ItemAt(i));
	}
}

void
StreamTakeView::AddTake(StreamTake *take)
{
	TakeListItem	*ti = new StreamTakeItem(take);
	if (Window()) Window()->Lock();
	listView->AddItem(ti);
	listView->Invalidate();
	if (Window()) Window()->Unlock();
}

void
StreamTakeView::Select(Take *take, bool isMainTake)
{
	fprintf(stderr, "select... %d\n", isMainTake);
	PoolEditor	*se = (PoolEditor *)Parent();
//	mySample->flock.Lock();

	if (isMainTake) {
		myPool->SelectTake((StreamTake*)take);
	} else {
		se->streamView->SetStream(&((StreamTake*)take)->stream);
	}
//	mySample->flock.Unlock();
	fprintf(stderr, "done select\n");
	se->streamView->Invalidate();
}

void
StreamTakeView::Delete(Take *take)
{
	PoolEditor	*se = (PoolEditor *)Parent();

//	myPool->flock.Lock();
	myPool->DeleteTake((StreamTake*)take);
	if (take == myPool->selectedTake) {
		if (myPool->outTakes.CountItems() > 0) {
			StreamTake		*newT =
					((StreamTakeItem *)listView->ItemAt(0))->Take();
			
			myPool->SelectTake(newT);
		} else {
			myPool->SelectTake(nullptr);
		}
	}
//	if (se) {
//		if (myPool->outTakes.CountItems() > 0) {
//			se->editTake = myPool->selectedTake;
//			se->streamView->editTake = myPool->selectedTake;
//		} else {
//			se->editTake = nullptr;
//			se->streamView->editTake = nullptr;
//		}
//	}
//	myPool->flock.Unlock();
	se->streamView->Invalidate();
}

/////////////////////////////////////////////////////////////

SampleTakeItem::SampleTakeItem(SampleTake *t):
	TakeListItem()
{
	myTake = t;
}


void
SampleTakeItem::DrawItem(BView *lv, BRect ir, bool drall)
{
	BRect	bnd = lv->Bounds();
	float	wid = Max(ir.right, lv->StringWidth(myTake->name)+4);
	float	bot = ir.bottom;

	rect = BRect(ir.left,ir.top,ir.left+wid,ir.top+14);

	TakeListView *stv = (TakeListView *)lv;
	if (bot > bnd.bottom || wid > bnd.right) {
		wid = Max(wid,bnd.right);
		bot = Max(bot,bnd.bottom);
		lv->ResizeTo(wid, bot);
		if (stv->Parent()) {
			stv->Parent()->ResizeTo(wid+2, bot+14);
		}
	}
	
	rgb_color		col=lv->ViewColor();
	if (IsSelected()) {
		lv->SetHighColor(col=red);
		lv->FillRect(ir);
	} else if (drall) {
		lv->SetHighColor(col);
		lv->FillRect(ir);
	}
	
	if (stv->ItemAt(0) == (TakeListItem *)this) {
		lv->SetHighColor(blue);
	} else {
		lv->SetHighColor(black);
	}
	lv->SetLowColor(col);
	lv->DrawString(myTake->name, BPoint(2,ir.bottom-1)); 
}


void
SampleTakeItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
}

status_t
SampleTakeItem::Perform(perform_code d, void *arg)
{
	return Perform(d, arg);
}

//////////////////////////////////////////////////////////////////////

SampleTakeView::SampleTakeView(BRect r, Sample *s):
	TakeView(r, "dross", (ulong)nullptr, (ulong)B_WILL_DRAW|B_FRAME_EVENTS)
{
	BRect bnd = Bounds();
	listView = new TakeListView(
						BRect(1, 14, bnd.right-1, bnd.bottom-1),
						"Takes");
	mySample = s;
	AddChild(listView);
	for (short i=0; i<s->CountTakes(); i++) {
		AddTake(s->Take(i));
	}
}

void
SampleTakeView::AddTake(SampleTake *take)
{
	TakeListItem	* ti = new SampleTakeItem(take);
	if (Window()) Window()->Lock();
	listView->AddItem(ti);
	listView->Invalidate();
	if (Window()) Window()->Unlock();
}

void
SampleTakeView::Select(Take *take, bool isMainTake)
{
	fprintf(stderr, "select... %d\n", isMainTake);
	SampleEditor	*se = (SampleEditor *)Parent();
	mySample->flock.Lock();
	if (se) {
		se->editTake = (SampleTake *)take;
		se->sampleView->editTake = (SampleTake *)take;
		se->sampleView->sampleDuration = ((SampleTake *)take)->file->nFrames;
	}
	mySample->SelectTake((SampleTake*)take);
	mySample->flock.Unlock();
	se->sampleView->SetupDisplay();
	se->sampleView->Refresh();
	fprintf(stderr, "done select\n");
}


void
SampleTakeView::Delete(Take *take)
{
	SampleEditor	*se = (SampleEditor *)Parent();

	mySample->flock.Lock();
	mySample->DeleteTake((SampleTake*)take);
	if (take == mySample->selectedTake) {
		if (mySample->CountTakes() > 0) {
			SampleTake		*newT =
					((SampleTakeItem *)listView->ItemAt(0))->Take();
			
			mySample->SelectTake(newT);
		} else {
			mySample->SelectTake(nullptr);
		}
	}
	if (se) {
		if (mySample->CountTakes() > 0) {
			se->editTake = mySample->selectedTake;
			se->sampleView->editTake = mySample->selectedTake;
		} else {
			se->editTake = nullptr;
			se->sampleView->editTake = nullptr;
		}
	}
	mySample->flock.Unlock();
	se->sampleView->Refresh();
}

void
SampleTakeView::MessageReceived(BMessage *inMsg)
{
	if (inMsg->WasDropped()) {
		switch (inMsg->what) {
		case B_SIMPLE_DATA:
			if (inMsg->HasRef("refs")) {
				entry_ref		ref;
				status_t 		err;
				int				cnt = 0;
				while ((err=inMsg->FindRef("refs", cnt++, &ref)) == B_NO_ERROR) {
					SampleFile		*outFile =
									new SampleFile(&ref, B_READ_WRITE);
					if (outFile->InitCheck() == B_NO_ERROR) {
						char	buf[50];
						BEntry			ent(&ref);
						BPath			path(&ent);
						sprintf(buf, "Dropped %d", mySample->CountTakes());
						SampleTake	*newTake = mySample->AddSampleTake(
													outFile, buf, ((char *)path.Path()));
						// safe? this is the only reader to mod this list
						AddTake(newTake);
						if (mySample->selectedTake == nullptr) {
							Select(newTake, true);
						}
					} else {
						delete outFile;
					}
				}
			}
			return;
		}
	}
	TakeView::MessageReceived(inMsg);
}
