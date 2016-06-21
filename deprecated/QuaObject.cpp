
#ifdef __INTEL__ 
#include <SupportDefs.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "StdDefs.h"
#include "include/Qua.h"
#include "include/QuaObject.h"
#include "include/ArrangerObject.h"
#include "include/messid.h"
#include "Colors.h"
#include "include/Application.h"
#include "include/Pool.h"
#include "include/Pool.h"
#include "include/SequencerWindow.h"
#include "include/Method.h"
#include "include/SampleEditor.h"
#include "include/ValList.h"
#include "include/GlblMessid.h"
#include "SampleFile.h"
#include "include/Sample.h"
#include "include/Sampler.h"
//#include "include/PoolPlayer.h"
#include "include/Block.h"
#include "TakeView.h"
#include "include/AQuarium.h"
#include "include/QuasiStack.h"
#include "include/Editor.h"

BRect		DefaultRect(0,0,50, 20);



QuaObject::QuaObject(StabEnt *S, BRect DefaultRect,
				BBitmap *siconData,	BBitmap *biconData,
				ObjectViewContainer *a, Qua *q, rgb_color dflt_col):
	QuaSymbolBridge(S, DefaultRect, siconData, biconData, a, dflt_col)
{
	BRect		rect;
	next = nullptr;
	uberQua = q;
	schedulable = S->SchedulableValue();

	Executable	*ex = sym->ExecutableValue();
	if (ex) {
		if (Window())
			Window()->Lock();
		ex->representation = this;
		if (ex->displayInfo) {
			ex->SetDisplayInfo(ex->displayInfo);
		} else {
			color = dflt_col;
		}
		if (Window())
			Window()->Unlock();
	}
	SetDisplayMode(OBJECT_DISPLAY_SMALL);
}


QuaObject::~QuaObject()
{
	fprintf(stderr, "deleting object %s\n", sym->name);
//	arranger->RemoveChild(this);
	if (editor) {
		editor->ctrlEdit = nullptr;
		thread_id	winThread = editor->Thread();
		editor->PostMessage(B_QUIT_REQUESTED);
		status_t	ret;
		wait_for_thread(winThread, &ret);
	}
}


void
QuaObject::Draw(BRect rect)
{
	QuaSymbolBridge::Draw(rect);
}

void
QuaObject::MouseDown(BPoint where)
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
		 	msg->AddPointer("qua_object", this);
		 	
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
QuaObject::Edit()
{
	if (editor) {
		editor->Activate();
	} else {
		editor = new Editor(
				BRect(100,100,600,500), uberQua, sym);
	}
}


void
QuaObject::MessageReceived(BMessage *inMsg)
{
	if (inMsg->what == MOVE_OBJECT) {
		if (inMsg->HasPointer("qua_object")) {
			QuaObject *model;
			inMsg->FindPointer("qua_object", (void **)&model);
			
			if (model == this) {
				ObjectView::MessageReceived(inMsg);
			} else {
				int v=OptionWin(2, "Copy %s to this?", sym->name);
			}
		}
	} else if (inMsg->what == B_CANCEL) {
		if (inMsg->HasPointer("save panel")) {
			BFilePanel		*savePanel=nullptr;
			inMsg->FindPointer("save panel", (void **)&savePanel);
			if (savePanel)
				delete savePanel;
		}
	} else if (inMsg->what == QUA_LOAD) {
		fprintf(stderr, "loading...\n");
		if (inMsg->HasPointer("save panel")) {
			BFilePanel		*savePanel=nullptr;
			inMsg->FindPointer("save panel", (void **)&savePanel);
			if (savePanel) {
				fprintf(stderr, "deleting save panel\n");
				delete savePanel;
			}
		}
		Sample	*S = sym->SampleValue();
		if (S && inMsg->HasRef("refs")) {
			fprintf(stderr, "new take...\n");
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
					sprintf(buf, "Dropped %d", S->takes.CountItems());
					SampleTake	*newTake = S->AddSampleTake(
												outFile, buf, ((char *)path.Path()));
					// safe? this is the only reader to mod this list
					if (editor) {
						editor->Lock();
						SampleEditor	*E =
							(SampleEditor*)S->representation->editor->dataEdit;
						E->takeView->AddTake(newTake);
						if (S->selectedTake == nullptr) {
							E->takeView->Select(newTake, true);
						}
						editor->Unlock();
					} else {
						if (S->selectedTake == nullptr) {
							S->SelectTake(newTake);
						}
					}
					if (cnt == 1)
						schedulable->SetName(((char *)path.Leaf()));
				} else {
					delete outFile;
				}
			}
		}
	} else if (inMsg->what == B_SAVE_REQUESTED) {
		if (inMsg->HasPointer("save panel")) {
			BFilePanel		*savePanel=nullptr;
			inMsg->FindPointer("save panel", (void **)&savePanel);
			if (savePanel)
				delete savePanel;
		}
		if (inMsg->HasString("name")) {	// do it!
			char		saveName[MAX_QUA_NAME_LENGTH];
			strcpy(saveName, inMsg->FindString("name"));
			schedulable->SetName(saveName);
			
			if (inMsg->HasRef("directory")) {
				BDirectory	theDir;
				entry_ref	dirRef;
	 			inMsg->FindRef("directory", &dirRef);
	 			theDir.SetTo(&dirRef);
	 			if (theDir.InitCheck() == B_NO_ERROR) {	
					if (schedulable->sym->type == S_SAMPLE) {
						Sample		*s = ((Sample*)schedulable);
						SampleTake	*take = s->selectedTake;
						if (take == nullptr) {
							take = (SampleTake *)s->Take(0);
						}
						if (take) {
							BEntry		ent(take->path, true);
							if (ent.InitCheck() == B_NO_ERROR) {
								ent.MoveTo(&theDir, saveName, true);
								BPath		p;
								ent.GetPath(&p);
								delete take->path;
								take->path = new char[strlen(p.Path())+1];
								strcpy(take->path, p.Path());
								s->selectedFilePathName = take->path;
							} else {
								fprintf(stderr, "can't make entry\n");
							}
						}
					}
				}
			}
		}
	} else if (inMsg->what == SET_COLOR) {
		rgb_color		*p;
		ssize_t			sz;
		
		inMsg->FindData("color", B_RAW_TYPE, (const void **)&p, &sz);
		color.red = p->red;
		color.green = p->green;
		color.blue = p->blue;
		sym->SetColor(color);
		ReDraw();
	} else {
		ObjectView::MessageReceived(inMsg);
	}
}


void
QuaObject::SetName(char *nm)
{
	SetLabel(nm);
	Window()->Lock();
	Draw(Bounds());
	Window()->Unlock();
	
	if (uberQua) {
		schedulable->schlock.Lock();
		for (ArrangerObject *p = uberQua->sequencerWindow->arrange->arrangerObjects;
							p != nullptr; p = p->next) {
			if (p->instance->schedulable == schedulable) {
				p->SetLabel(nm);
				p->Window()->Lock();
				p->Draw(p->Bounds());
				p->Window()->Unlock();
			}
		}
	    schedulable->schlock.Unlock();
	}
}
