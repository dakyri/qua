
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#include <Roster.h>
#endif

#include "StdDefs.h"
#include "Application.h"
#include "ArrangerObject.h"
#include "Time.h"
#include "SequencerWindow.h"
#include "Method.h"
#include "Sym.h"
#include "Qua.h"
#include "Block.h"
#include "Colors.h"
#include "QuaObject.h"
#include "Instance.h"
#include "ValList.h"
#include "QuasiStack.h"
#include "SchedulablePanel.h"
#include "Channel.h"

Application::Application(char *nm, BPath *path,
				Qua *Q,
				char *mimestr):
	Schedulable(
		DefineSymbol(nm, S_APPLICATION, 0,
					this, Q->sym,
					REF_VALUE, ATTRIB_NONE, false, DISP_MODE_NOT),
		Q,
		Q->metric)
{
	appFilePath.Unset();
	if (mimestr) {
		appMimeType.SetTo(mimestr);
	} else {
		appMimeType.Unset();
	}
	uberQua = Q;
	
	if (path) {
		SetAppPath(path);
	} else if (appMimeType.IsValid()) {
		entry_ref	app_ref;

		if (appMimeType.GetAppHint(&app_ref) != B_NO_ERROR) {
			BEntry		ent(&app_ref);
			BPath		p;
			ent.GetPath(&p);
			SetAppPath(&p);
		}
	}
}

bool
Application::Init()
{
	fprintf(dbfp, "Initing application %s\n", sym->name);

	if (sym == nullptr) {
	    reportError("Qua: application symbol not found\n");
	}
	glob.PushContext(sym);
	
	if (!Schedulable::Init())
		goto err_ex;
	glob.PopContext();
  	
	return TRUE;
err_ex:
	glob.PopContext();
  	
	return FALSE;
}

long
Application::SetAppPath(BPath *path)
{
 	long			argc = 1;
	char			*argv[2];
	extern char		**environ;
	char			nm[MAX_QUA_NAME_LENGTH];
	status_t		err;
	
	appFilePath = *path;

	argv[0] = sym->name;
	argv[1] = nullptr;
	
	entry_ref	app_file_ref;
	BEntry		appEnt(path->Path());
	appEnt.GetRef(&app_file_ref);
	if ((err=be_roster->Launch(
				&app_file_ref,
				(BMessage *)nullptr,
				&appTeamId)) != B_NO_ERROR) {
		reportError("can't launch: %s\n", ErrorStr(err));
	}

	if (get_team_info(appTeamId, &appInfo) != B_NO_ERROR) {
		reportError("can't get thread info\n");
	}
	
	snooze(200000);
	
	do {
		txConnect = BMessenger((const char *)nullptr, appInfo.team, &err);
	} while (err != B_NO_ERROR &&
			RetryError("error at the connect: %s: team %d\n",
						ErrorStr(err), appTeamId));
	return B_NO_ERROR;
}

Application::~Application()
{
// don't delete control blocks!!!
	mainBlock = nullptr;
	receive.block = nullptr;
	wake.block = nullptr;
	sleep.block = nullptr;

	status_t err = txConnect.SendMessage(
			new BMessage(B_QUIT_REQUESTED));
	if (err != B_NO_ERROR)
		reportError("can't get rid of %s: %s", sym->name, ErrorStr(err));
}


void
Application::Cue(Time &t)
{
	;
}


status_t
Application::Sleep(Instance *i)
{
	if (i && i->status != STATUS_SLEEPING) {
		Notification(sleep.block, i, i->sleepStack);
		Schedulable::Sleep(i);
	}
	return B_NO_ERROR;
}

status_t
Application::Wake(Instance *i)
{
	if (i && i->status != STATUS_RUNNING) {
		Schedulable::Wake(i);
		Notification(wake.block, i, i->wakeStack);
	}
	return B_NO_ERROR;
}

status_t
Application::Run(Instance *inst)
{
	inst->wakeDuration = uberQua->theTime - inst->startTime;
	inst->UpdateEnvelopes(inst->wakeDuration);
	extern flag	debug_update;
#ifdef OLD_LIST					
	TypedValueList	logItems;
	inst->logSem.Lock();
	inst->log.ItemRefsAt(
			inst->wakeDuration-1,
			inst->wakeDuration,
			logItems);
	inst->logSem.Unlock();
	for (short i=0; i<logItems.Count; i++) {
		OutMess(
			logItems.Items[i].StreamItemValue());
	}
#else
	inst->logSem.Lock();
	TypedValueList	logItems;
	logItems.Init();
	inst->log.ItemRefsAt(
			inst->wakeDuration-1,
			inst->wakeDuration,
			logItems);
	inst->logSem.Unlock();
	TypedValueListItem	*p=logItems.head;
	while (p) {
		OutMess(p->value.StreamItemValue());
		p = p->next;
	}
	logItems.Clear();
#endif // OLD_LIST					

	if (mainBlock) {
		Stream	s;
		Time	tag_time = uberQua->theTime;

		bool	uac = UpdateActiveBlock(
						uberQua,
						&s,
						mainBlock,
						1,
						&tag_time,
						TRUE, inst,
						inst->mainStack);
		OutMess(&s);
		uberQua->channel[inst->channel]->OutputStream(&s);

		s.ClearStream();
	}
	return B_NO_ERROR;
}

status_t
Application::Recv(Stream &s)
{	
	Time	tag_time = uberQua->theTime;

	for (short i=0; i<instances.CountItems(); i++) {
		Instance	*inst = (Instance *)instances.ItemAt(i);
		bool	uac = UpdateActiveBlock(
						uberQua,
						&s,
						receive.block,
						1,
						&tag_time,
						TRUE, inst,
						inst->rxStack);
		OutMess(&s);
		uberQua->channel[inst->channel]
				->OutputStream(&s);
		s.ClearStream();
	}
	return B_NO_ERROR;
}

status_t
Application::QuaStart()
{
	Schedulable::QuaStart();
	activeInstances.RdLock();
	for (short i=0; i<activeInstances.CountItems(); i++) {
		Instance	*inst = (Instance *)activeInstances.ItemAt(i);
		Notification(wake.block, inst, inst->wakeStack);
	}
	activeInstances.RdUnlock();
	return B_NO_ERROR;
}

status_t
Application::QuaStop()
{
	Schedulable::QuaStop();
	activeInstances.RdLock();
	for (short i=0; i<activeInstances.CountItems(); i++) {
		Instance	*inst = (Instance *)activeInstances.ItemAt(i);
		Notification(sleep.block, inst, inst->sleepStack);
	}
	activeInstances.RdUnlock();
	return B_NO_ERROR;
}

status_t
Application::QuaRecord()
{
	Schedulable::QuaRecord();
	return B_NO_ERROR;
}


status_t
Application::Save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent);

	fprintf(fp,	"application");
	
	if (appFilePath.InitCheck() == B_OK) {
		fprintf(fp, "\n\tpath \"%s\"", appFilePath.Path());
	}
	if (appMimeType.IsValid()) {
		fprintf(fp, "\n\tmime \"%s\"", appMimeType.Type());
	}
	
	fprintf(fp,	" %s", sym->PrintableName());
	if (controlInfoList) {
		fprintf(fp, "(");
		err = controlInfoList->Save(fp, indent+2);
		fprintf(fp, ")");
	}
	if (symObject) {
		BPoint	atPoint = symObject->atPoint;
		fprintf(fp,	"\n#display {%s, {%g %g}}",
			ColorStr(symObject->color), atPoint.x, atPoint.y);
	}

	SaveMainBlock(mainBlock, fp, indent, sym, true); 
	return err;
}


bool
Application::HasStreamItems()
{
	return false;
}

void
Application::Notification(Block *B,Instance *i, QuasiStack *s)
{
	if (B) {
		Time Now = uberQua->theTime;
		Stream	mainStream;

		bool ua_complete = UpdateActiveBlock(
	    					uberQua,
	    					&mainStream,
	    					B,1,
	    					&Now,
	    					TRUE, i,
	    					s);
	
		OutMess(&mainStream);
		mainStream.ClearStream();
	}		
}


void
Application::GetStreamItems(Stream *S)
{
}

void
Application::OutMess(Stream *S)
{
	StreamItem	*p;
		  
	if (S->nItems > 0) {
// ?? could need a sem....
//
//		if (acquire_sem(mySem) != B_NO_ERROR) {
//			reportError("can't acquire sem\n");
//			return;
//		}
//		fprintf(stderr, "om\n");
	    for (p = S->head; p != nullptr; p=p->next) {
	    	OutMess(p);
	    }
	    
//	    release_sem(mySem);
	}
}

void
Application::OutMess(StreamItem *p)
{
   	switch (p->type) {
	case S_NOTE: {
	}
	
	case S_MESSAGE: {
		BMessage	*msg = ((StreamMesg *)p)->mesg;
//    	fprintf(stderr, "out %s\n", uintstr(msg->what));
		status_t err = txConnect.SendMessage(msg);
		if (err != B_NO_ERROR)
			reportError("no can do connect transmit: %s\n",
					ErrorStr(err));
			break;
	}

	case S_CTRL: {
		break;
	}

	case S_BEND: {
		break;
	}

	case S_SYSX: {
		break;
	}

	case S_SYSC: {
		break;
	}

	case S_PROG: {
		break;
	}

	case S_STREAM_ITEM: {
		break;
	}
	
	case S_VALUE: {
	}
	
	default: {
	}}
}

bool
Application::ExtraneousMessage(class ArrangerObject *a, BMessage *inMsg)
{
	if (inMsg->HasRef("refs")) {
		if (inMsg->WasDropped()) {
			
			BPoint	dropPoint, offsetPoint;
			dropPoint = inMsg->DropPoint(&offsetPoint);
	// get pixels in arranger co-ords for time conversion
			a->Arranger()->ConvertFromScreen(&dropPoint);
			
			BMessage	*refMsg = new BMessage(*inMsg);
			refMsg->what = B_REFS_RECEIVED;
			
			Time	dropTime = a->Arranger()->PixelToTime(dropPoint.x);
	// convert time relative to start of this instance...
			dropTime -= a->instance->startTime;
			
			entry_ref	ref;
			inMsg->FindRef("refs", &ref);
			BEntry		refEnt(&ref);
			char		refName[MAX_QUA_NAME_LENGTH];
			float		pt = a->Arranger()->TimeToPixel(dropTime);
			refEnt.GetName(refName);
			refMsg->AddString("_Qua_refname", refName);
			refMsg->AddRect("_Qua_refrect",
					BRect(pt, 30, pt+Max(50,a->StringWidth(refName)+4), 48));
			
			a->instance->logSem.Lock();
			a->instance->log.AddToStream(refMsg, &dropTime);
			a->instance->logSem.Unlock();
//			a->instance->Log.PrintStream(stderr);
			a->Invalidate();	// redraw crap!
			
			return true;
		} else {
			return false;
		}
	} else if (Schedulable::ExtraneousMessage(a, inMsg)) {
		return true;
	}
	return false;
}

void
Application::DrawExtraneous(class ArrangerObject *a, BRect r)
{
// r is in a's co-ord system!
	
	Schedulable::DrawExtraneous(a, r);
	
	a->ConvertToScreen(&r);
	a->Arranger()->ConvertFromScreen(&r);
	Time startExtDraw = a->Arranger()->PixelToTime(r.left-250) - a->instance->startTime;
	Time endExtDraw = a->Arranger()->PixelToTime(r.right+250) - a->instance->startTime;

	TypedValueList	logItems;
#ifndef OLD_LIST
	logItems.Init();
#endif
	a->instance->logSem.Lock();
	a->instance->log.ItemRefsBetween(startExtDraw, endExtDraw, logItems);
	a->instance->logSem.Unlock();

#ifdef OLD_LIST
	for (short i=0; i<logItems.Count; i++) {
		StreamItem	*p = logItems.Items[i].StreamItemValue();
		
		DrawLogItem(a, p);
	}
#else
	TypedValueListItem	*p = logItems.head;
	while (p) {
		StreamItem	*q = p->value.StreamItemValue();
		p = p->next;
		DrawLogItem(a, q);
	}
	logItems.Clear();
#endif
}

void
Application::DrawLogItem(ArrangerObject *a, StreamItem *p)
{
	switch (p->type) {
	case S_MESSAGE: {
		StreamMesg	*q = (StreamMesg*)p;
		BMessage	*m = q->mesg;
		char		buf[MAX_QUA_NAME_LENGTH];
		char		*nmp;
		BRect		drect;

		m->FindString("_Qua_refname", ((const char **)&nmp));
		m->FindRect("_Qua_refrect", &drect);
		
		a->SetHighColor(Inverse(a->color));
		a->FillRect(drect);
		a->SetHighColor(white);
		a->SetLowColor(Inverse(a->color));
		a->DrawString(nmp, BPoint(drect.left+2, drect.top+8));
		
		a->DrawString(p->time.StringValue(), BPoint(drect.left+2, drect.top+16));

		a->SetHighColor(black);
		a->StrokeRect(drect);
		break;
	}
	}
}

bool
Application::ExtraneousMouse(ArrangerObject *a, BPoint where, ulong buts, ulong mods)
{
	BPoint		orig = where;
	
	a->ConvertToScreen(&where);
	a->Arranger()->ConvertFromScreen(&where);
	
	Time startExtDraw = a->Arranger()->PixelToTime(where.x-150) - a->instance->startTime;
	Time endExtDraw = a->Arranger()->PixelToTime(where.x+150) - a->instance->startTime;

	TypedValueList	logItems;
#ifndef OLD_LIST
	logItems.Init();
#endif
	a->instance->logSem.Lock();
	a->instance->log.ItemRefsBetween(startExtDraw, endExtDraw, logItems);
	a->instance->logSem.Unlock();

#ifdef OLD_LIST
	for (short i=0; i<logItems.Count; i++) {
		StreamItem	*p = logItems.Items[i].StreamItemValue();
		if (MouseDownLogItem(a, orig, p))
			return true;
	}
#else
	TypedValueListItem	*p = logItems.head;
	while (p) {
		StreamItem	*q = p->value.StreamItemValue();
		p = p->next;
		if (MouseDownLogItem(a, orig, q)) {
			logItems.Clear();
			return true;
		}
	}
	logItems.Clear();
#endif
	return false;
}

bool
Application::MouseDownLogItem(ArrangerObject *a, BPoint where, StreamItem *p)
{
	BPoint		orig = where;
	switch (p->type) {
		case S_MESSAGE: {
			StreamMesg	*q = (StreamMesg*)p;
			char		*nmp;
	
			a->track_msg = q->mesg;
			a->track_msg->FindString("_Qua_refname", ((const char **)&nmp));
			a->track_msg->FindRect("_Qua_refrect", &a->track_rect);
			if (a->track_rect.Contains(where))
				return false;
			
			a->track_lastp = where;
			a->track_deltax = where.x - a->track_rect.left;
			a->track_streamitem = p;
			
			a->tracking = MOUSE_TRACK_MOVE_LOGENTRY;
			a->SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

			return true;
			break;
		}
	}
	return false;
}
