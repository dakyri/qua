#include "qua_version.h"


#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include "StdDefs.h"

#include "Qua.h"
#include "Quapp.h"
#include "Channel.h"
#include "Destination.h"
#include "Parse.h"
#include "Sym.h"
#include "QuaMidi.h"
#include "messid.h"
#include "Block.h"
//#include "Source.h"
#include "QuasiStack.h"
#include "Pool.h"
#include "Pool.h"
#include "Sample.h"
#include "Sampler.h"
//#include "include/PoolPlayer.h"

#if defined(QUA_V_ARRANGER_INTERFACE) 
#if defined(WIN32)
#endif
#endif

#if defined(QUA_V_APP_HANDLER) 
#include "Application.h"
#endif
//time_t		time();

extern StabEnt			*quaSym;

/*
 * Qua.
 */
#ifdef _BEOS
Qua::Qua(B &ref, bool chan_add)
{
	char	nm[MAX_QUA_NAME_LENGTH];
	BEntry	theFile(&ref);

	metric = &StdMetric;
	theFile.GetName(nm);
	Init(nm, chan_add);
	QuaObject *newObj = Load(ref, BPoint(0, 0));
}
#endif

Qua::Qua(char *nm, bool chan_add)
{
	metric = &StdMetric;
	if (nm) {
		Init(nm, chan_add);
	} else {
		Init("New", chan_add);
	}
}

long
Qua::Init(char *nm, bool chan_add)
{
    StabEnt	*ts, *cs, *tempos;
	Voice			*V;
	Pool			*P;
	ArrangerObject	*SO;
	QuaObject		*QO;
	short			chan;
	BRect			windowRect;

	fprintf(stderr, "initting %s\n", nm);
	Sym = DefineSymbol(nm, S_QUAQUA, this, nullptr, 0,
					false, false, false, StabEnt::DISPLAY_NOT);
    Glob.PushContext(Sym);
    
	srand((int)time(nullptr));
	strcpy(Name, nm);
    iteration = 0;
    SourceList = nullptr;
	schedulees = nullptr;
    mSecs = 0;
    iteration = 0;
    sequencerTime = zeroTime;
    LastUpdate = sequencerTime - 1;
    
	currentBeatTick = 0;
	currentBarBeat = 0;
	currentBar = 0;
	
    sampler = new Sampler();
    poolPlayer = new PoolPlayer(this);
	fprintf(stderr, "created sampler and pool player\n", nm);
    
	BEntry	ent;
	ent.SetTo(quapp->ProjPath.Path());
	ent.GetRef(&quaSaveDir);
	ent.SetTo(quapp->SamplePath.Path());
	ent.GetRef(&sampleSaveDir);
	quaSaveFile = nullptr;
	quapSaveFile = nullptr;
	savePanel = nullptr;
	
	DoMetroGnome = FALSE;
	AutoCreateAmpEnv = TRUE;
	
    fprintf(stderr, "time quanta %g %g\n", TimeQuanta, 6*TimeQuanta);
    
    Status = STATUS_SLEEPING;
    if ((mySem = create_sem(0, "blah")) < B_NO_ERROR) {
	   	reportError("Can't create sem");
	   	return B_ERROR;
	}

	NChannel = 0
	;
//	windowRect.Set( 200, 200, 800, 200 + 70 + NChannel*TRACK_HEIGHT +
//							B_H_SCROLL_BAR_HEIGHT );
	windowRect.Set( 200, 200, 800, 200 + 70 + 8*TRACK_HEIGHT +
							B_H_SCROLL_BAR_HEIGHT );
	sequencerWindow = new SequencerWindow( windowRect, Name, B_WILL_ACCEPT_FIRST_CLICK, this );
	sequencerWindow->SetTarget(this);
	sequencerWindow->SetMetric(metric);

	mixerWindow = new MixerWindow("Qua: control",
						B_WILL_ACCEPT_FIRST_CLICK|B_NOT_CLOSABLE|B_NOT_ZOOMABLE|B_NOT_RESIZABLE,
						this);

	// set a funky background image
	if (quapp->backgroundPattern == nullptr) {
		reportError("can't set background");
	} else {
		sequencerWindow->Lock();
		sequencerWindow->backView->SetTilingPattern(*quapp->backgroundPattern);
		sequencerWindow->backView->Invalidate();
		sequencerWindow->Unlock();
		mixerWindow->Lock();
		mixerWindow->mixerView->SetTilingPattern(*quapp->backgroundPattern);
		mixerWindow->mixerView->Invalidate();
		mixerWindow->Unlock();
	}


	if (chan_add) {	

	    for (short i=0; i<16; i++) {
	    	char		nm[30];	
	    	char		tx[30];	
	    	Destination	dst(false, nullptr);
	    	dst.Set(DST_MIDI, 0, i+1);
			sprintf(nm, "channel%d", i+1);
			strcpy(tx, dst.LongName());
//			fprintf(stderr, "creating channel %d\n", i+1);
			AddChannel(nm, tx, tx, false);
//			fprintf(stderr, "initing channel %d\n", i+1);
		   	if (!OutChannel[i]->Init())
		  		TragicError("cant initialize channel %d", i+1);
	    }
	}
	
	mixerWindow->Show();
    sequencerWindow->Show();
    
    fprintf(stderr, "added channels\n");

    ts = DefineSymbol("actualtime", TypedValue::S_FLOAT, &ActualTime, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    cs = DefineSymbol("ticktime", TypedValue::S_INT, &sequencerTime.Ticks, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    tempos = DefineSymbol("tempo", TypedValue::S_FLOAT, &metric->Tempo, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("bar", TypedValue::S_INT, &currentBar, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("beat", TypedValue::S_INT, &currentBarBeat, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("tick", TypedValue::S_INT, &currentBeatTick, Sym, -1, false, false, false, StabEnt::DISPLAY_NOT);

	theStack = new QuasiStack(Sym, nullptr, nullptr, nullptr, nullptr);
	
    SetTempo(120);
	
    myThread = spawn_thread(MainWrapper, Name,
						B_URGENT_PRIORITY, this);
						
	return B_NO_ERROR;
}

status_t
Qua::AddChannel(char *nm, char *rx, char *tx, bool thru)
{
 	Channel *c = new Channel(nm, NChannel, rx, tx, false, this);
	OutChannel[NChannel] = c;
	fprintf(stderr, "adding channel %d %x\n", NChannel, c);
	NChannel++;
 	sequencerWindow->AddChannel(c);
 	mixerWindow->AddChannel(c);
 	return B_NO_ERROR;
}

Qua::~Qua()
{
	long			crap;
	Voice			*V, *N;
	ArrangerObject	*S;
	status_t 		err;
	fprintf(stderr, "about to delete %s\n", Name);
	Status = STATUS_DEAD;
	resume_thread(myThread); // in case stopped: else we hang
	if ((err=wait_for_thread(myThread, &crap)) != B_NO_ERROR) {
		reportError("waiting for death fails: %s\n", ErrorStr(err));
	}
	fprintf(stderr, "thread exitted\n");

	if (release_sem(mySem) < B_NO_ERROR)	// all clear for window to fuck off
		reportError("can't release sem");
	fprintf(stderr, "released sem\n");
	/* free up blocks used */

    for (short i=0; i<NChannel; i++) {	
		OutChannel[i]->Stop();
    }

	fprintf(stderr, "voices stopped in %s\n", Name);

	Glob.PopContext();
//	Sym->Dump(stderr, 0);
	Glob.DeleteSymbol(Sym);

	mixerWindow->PostMessage(B_QUIT_REQUESTED);
	fprintf(stderr, "symbols deleted: about to zap lalaland\n");
	delete sampler;
	delete poolPlayer;

	if (delete_sem(mySem) < B_NO_ERROR)
		reportError("can't delete_sem");
 	fprintf(stderr, "################delete complete\n");
}

void
Qua::SetTempo(float t)
{
	metric->Tempo = t;
	TimeQuanta = 1 / (metric->Granularity * (metric->Tempo / 60.0));
	sequencerWindow->SetTempo(metric->Tempo);
}


void
Qua::SetName(char *nm, bool setTitle)
{
	if (strcmp(nm, Name) != 0) {
		Glob.Rename(Sym, nm);
		strcpy(Name, nm);
		if (setTitle) sequencerWindow->SetTitle(Name);
	}
}


void
Qua::Cue(class Time &t)
{
	for (Schedulable *SO=schedulees; SO != nullptr; SO = SO->next) {
		SO->Cue(t);
	}
}

void
Qua::MessageReceived(BMessage* inMsg)
{
	switch(inMsg->what) {
	case QUA_TIME: {
		BMessage		*Forward;
		int				val;
							
		if   (inMsg->HasFloat("tempo")) {
			SetTempo(inMsg->FindFloat("tempo"));
		}
		if (inMsg->HasInt32("ticks")) {
			sequencerTime.Ticks = inMsg->FindInt32("ticks");
			sequencerTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
		} else if (inMsg->HasInt32("msticks")) {
			sequencerTime.SetToSecs(inMsg->FindInt32("ticks")/1000.0);
			sequencerTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
		}
		if (inMsg->HasFloat("actual")) {
			ActualTime = inMsg->FindFloat("actual");
			Clock.Set(ActualTime);
		}
		break;
	}
	
	case QUA_SAVE: {
		DoSave(inMsg->HasInt32("terminally"));
		break;
	}
	
	case QUA_SAVE_AS: {
		DoSaveAs(inMsg->HasInt32("terminally"));
		break;
	}
	
	case B_CANCEL: {
		if (inMsg->HasInt32("terminally")) {
			quapp->PostMessage(QUA_CLOSE, this);
		}
		break;
	}
	
	case B_SAVE_REQUESTED: {
		if (inMsg->HasString("name")) {	// do it!
			entry_ref	file_ref;
			BDirectory	*the_dir = new BDirectory();
			int			i;				
			char		saveName[MAX_QUA_NAME_LENGTH];
			
			if (savePanel) {
				delete savePanel;
				savePanel = nullptr;
			}
			strcpy(saveName, inMsg->FindString("name"));
			fprintf(stderr, "save \"%s\" \n", saveName);
			SetName(saveName, !inMsg->HasInt32("terminally"));
 			inMsg->FindRef("directory", &quaSaveDir);
 			
			if (the_dir->SetTo(&quaSaveDir) != B_NO_ERROR) {
				reportError("Save: Can't set directory\n");
				break;
			}
			if (quaSaveFile == nullptr) {
				quaSaveFile = new BFile();
			}
			if (quapSaveFile == nullptr) {
				quapSaveFile = new BFile();
			}

			if (the_dir->CreateFile(saveName, quaSaveFile)
						!= B_NO_ERROR) {
				reportError("cannot %s add to dir\n", saveName);
				break;
			}

			strcat(saveName, ".crp");
			if (the_dir->CreateFile(saveName, quapSaveFile)
						!= B_NO_ERROR) {
				reportError("cannot %s add to dir\n", saveName);
				break;
			}
			
			DoSave(inMsg->HasInt32("terminally"));

		} else {
			DoSaveAs(inMsg->HasInt32("terminally"));
		}
		break;
	} 
	
	case QUA_GO:
		Start();
		break;
	
	case QUA_PAUSE:
		Stop();
		break;
	
	case QUA_RECORD:
		StartRecording();
		break;
		
	default:
		BHandler::MessageReceived(inMsg);
	}
}

status_t
Qua::DoSaveAs(bool andQuit)
{
	if (savePanel) {
		delete savePanel;
		savePanel = nullptr;
	}

	BEntry	save_dir(&quaSaveDir);
	entry_ref	*p;
	p = (save_dir.InitCheck() == B_NO_ERROR?
							&quaSaveDir:nullptr);
	savePanel = new BFilePanel(B_SAVE_PANEL, &RightHere,
					p, FALSE, FALSE);
				
	BMessage		*m = new BMessage(B_SAVE_REQUESTED);
	if (andQuit)
		m->AddInt32("terminally", 0);
	savePanel->SetMessage(m);
	savePanel->SetButtonLabel(B_CANCEL_BUTTON, "no way");
	savePanel->SetSaveText(Name);
	savePanel->Window()->SetTitle("qua: save shgite information");
	savePanel->Refresh();
	savePanel->Window()->Show();
 	return B_NO_ERROR;
}

status_t
Qua::DoSave(bool andQuit)
{
	status_t		err=B_NO_ERROR;
	
	if (quaSaveFile == nullptr) {
		DoSaveAs(andQuit);
	} else {
		quaSaveFile->Seek(0, SEEK_SET);
		quapSaveFile->Seek(0, SEEK_SET);

		status_t err = Sym->Save(quaSaveFile, 0);
		quaSaveFile->SetSize(quaSaveFile->Position());
		if (err != B_NO_ERROR)
			reportError("can't save file");

		err = WriteChunk(quapSaveFile, 'QUAP', 0, 0);
		if (err < B_NO_ERROR)
			reportError("can't save .crp file");
		if (andQuit) {
			quapp->PostMessage(QUA_CLOSE, this);
		}
	}
	return err;
}



void Qua::DisplayTimer()
{
	sequencerWindow->Lock();
	sequencerWindow->SetTime(sequencerTime);
	sequencerWindow->arrange->
			current_time_cursor->SetTime(sequencerTime);
	sequencerWindow->Unlock();
}

void
Qua::UpdateRecSeqObj()
{
	if (Status == STATUS_RECORDING) {
    	for (short i=0; i<NChannel; i++) {
    		Channel	*sel = OutChannel[i];
			if (sel->Status == STATUS_RECORDING) {
				if (  sel->streamRecordInstance &&
					  sel->streamRecordInstance->arrangerObject) {
					class Time dur = sequencerTime - sel->streamRecordStarted;
					sel->streamRecordInstance->arrangerObject->SetDuration(dur);
					sel->streamRecordInstance->arrangerObject->Invalidate();
				}
				if (  sel->audioRecordInstance &&
					  sel->audioRecordInstance->arrangerObject) {
					class Time dur = sequencerTime - sel->audioRecordStarted;
					sel->audioRecordInstance->arrangerObject->SetDuration(dur);
					sel->audioRecordInstance->arrangerObject->Invalidate();
				}
			}
		}
	}
}

long
Qua::Pause()
{
	return TRUE;
}

long
Qua::Restart()
{
	return TRUE;
}


static int		new_id = 0;

Pool *
Qua::NewPool(short chan)
{
	char			pool_nm[30];
	sprintf(pool_nm, "new%d", new_id++);
	
    Pool *S = new Pool(pool_nm, this, Sym);
	S->next = schedulees;
	schedulees = S;
    
	QuaObject * QP = AddQuaObject(S->Sym, quaIcon, BPoint(10*new_id,50*chan));
	return S;
}

Sample *
Qua::NewSample(short chan)
{
	char			samp_nm[30];
	sprintf(samp_nm, "new%d", new_id++);
	
    Sample *S = new Sample(samp_nm, nullptr, this);
	S->next = schedulees;
	schedulees = S;
    
	QuaObject * QP = AddQuaObject(S->Sym,
								 quaIcon,
								 BPoint(10*new_id,50*chan));
	return S;
}

long
Qua::Start()
{
	if (Status != STATUS_RUNNING) {
    	SendStart(0);
    	sampler->Start(0);
    	
    	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
    		p->QuaStart();
    	}

		bool	wasRec = Status == STATUS_RECORDING;
		Status = STATUS_RUNNING;
		fprintf(stderr, "starting...\n");
		Clock.Set(ActualTime);
		if (resume_thread(myThread) != B_NO_ERROR) {
			reportError("can't resume\n");
			return FALSE;
		}
		
		if (wasRec) {
			StopRecording();
		}
	}
	return TRUE;
}

long
Qua::StartRecording()
{
	if (sequencerWindow->channelView->CountSelected() > 0) {
		if (Status != STATUS_RECORDING) {
			FlushInput();
		}
		if (Status == STATUS_SLEEPING) {
			Start();
		}

    	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
    		p->QuaRecord();
    	}

		short cnt = sequencerWindow->channelView->CountSelected();
		for (short i=0; i<cnt; i++) {
			Channel		*sel =
				((Channel *)sequencerWindow->channelView->Selected(i));
			if (sel->Status != STATUS_RECORDING) {
				sel->StartRecording();
			}
		
		}
		Status = STATUS_RECORDING;
	}
	return TRUE;
}

status_t
Qua::StopRecording()
{
   	for (short i=0; i<NChannel; i++) {
   		OutChannel[i]->StopRecording();
	}
	return B_NO_ERROR;
}

long
Qua::Stop()
{
	if (Status != STATUS_SLEEPING) {
		sampler->Stop(0);
    	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
    		p->QuaStop();
    	}

		bool	wasRec = Status == STATUS_RECORDING;
    	Status = STATUS_SLEEPING; // stops main thread
		if (wasRec) {
			StopRecording();
		}
    	SendStop(0);
    }
	return TRUE;
}

status_t
Qua::IdentifyFile(BFile &theFile,
			BMimeType &mainType,
			BBitmap *&iconData)
{
	type_code	typeFound;
	int32		idFound;
	size_t		lengthFound;
	char		*nameFound;
	long		res_ind=0, ap_res = -1, icon_res = -1;
	BNodeInfo	nodeInfo(&theFile);
	
	BResources	theResources(&theFile);
	while (theResources.GetResourceInfo(res_ind,
				&typeFound, &idFound,
				&nameFound, &lengthFound)) {
		char *buf = (char *) &typeFound;
		fprintf(stderr, "type %c%c%c%c\n", buf[0],buf[1],buf[2],buf[3]);
		if (typeFound == 'APPI')
			ap_res = res_ind;
		else if (typeFound == 'ICON') {
//			uchar data[1024];
//			icon_res = res_ind;
//			if (theResources.ReadResource(
//						typeFound,
//						idFound,
//						data,
//			        	0,
//			        	1024) != B_NO_ERROR) {
//				//reportError("can't read icon\n");
//			}
//			for (short i=0; i<32; i++) {
//				for (short j=0; j<32; j++) {
//					fprintf(stderr, "0x%2x", data[32*i+j]);
//				}
//				fprintf(stderr, "\n");
//			}
		}
		res_ind++;
	}
	
//	
//	if (ap_res >= 0) {
//		isApplication = true;
//		return B_NO_ERROR;
//	} else {
//		isApplication = false;
//		iconData = nullptr;
//	}
	char	mimeStr[100];
	if (nodeInfo.GetType(mimeStr) != B_NO_ERROR) {
		reportError("Can't get mime type");
		return B_ERROR;
	}
	if (nodeInfo.GetTrackerIcon(iconData, B_LARGE_ICON) != B_NO_ERROR) {
		fprintf(stderr, "can't find icon\n");
	}
	if (strcmp(mimeStr, "application/x-vnd.Be-RAWM") == 0) {
		strcpy(mimeStr, "audio/raw");
	} else if (strcmp(mimeStr, "audio/x-raw") == 0) {
		strcpy(mimeStr, "audio/raw");
	} else if (strcmp(mimeStr, "text/plain") == 0) {
		strcpy(mimeStr, "audio/QuaScript");
	}
	mainType.SetTo(mimeStr);
	if (mainType.InitCheck() == B_ERROR) {
	}
	
	return B_NO_ERROR;
}
	


QuaObject *
Qua::Load(entry_ref &ref, BPoint where)
{
	long		obj_type;
	QuaHead		qua_header;
	BFile		theFile(&ref, B_READ_ONLY);
	short		chan = -1;
	QuaObject	*QO, *newObj = nullptr;
	BRect		rec;
	status_t	err;
	BMimeType	fileMainType;
	BMimeType	fileSuperType;
	bool		isApplication;
	
	BBitmap		*iconData = new BBitmap(BRect(0,0,31,31), B_CMAP8);
	
	err = IdentifyFile(theFile, fileMainType, iconData);
	fileMainType.GetSupertype(&fileSuperType);

// find the file's name...
	BEntry				ent(&ref);
	char				nm[MAX_QUA_NAME_LENGTH];
	ent.GetName(nm);
	
// load an application binary
	if (strcmp(fileSuperType.Type(), "application") == 0) {
		Application			*api;
		api = new Application(nm, &ref, iconData, this);			
		api->next = schedulees;
		schedulees = api;
		newObj = AddQuaObject(api->Sym, iconData, where);

		return newObj;

	} else 	if (strcmp(fileSuperType.Type(), "audio") == 0) {
		if (	strcmp(fileMainType.Type(), "audio/aiff") == 0 ||
				strcmp(fileMainType.Type(), "audio/x-aiff") == 0 ||
				strcmp(fileMainType.Type(), "audio/wav") == 0 ||
				strcmp(fileMainType.Type(), "audio/wave") == 0 ||
				strcmp(fileMainType.Type(), "audio/raw") == 0 ) {
			Sample		*sample = new Sample(nm, &ref, this);
			sample->next = schedulees;
			schedulees = sample;
			newObj = AddQuaObject(sample->Sym, iconData, where);
		} else if (	strcmp(fileMainType.Type(), "audio/QuaScript") == 0) {
			if (quapp->LoadFile(ref)) {
			} else {
				if ((err=theFile.Seek(0, SEEK_SET)) != B_NO_ERROR) {
					reportError(ErrorStr(err));
					return nullptr;
				}
				fprintf(stderr, "prog file\n");

				iconData->SetBits(quaIcon->Bits(), quaIcon->BitsLength(), 0, B_CMAP8);
				Parser	*Prog = new Parser(&theFile, this);
		    	if (  !Prog->ParseProgFile() ||
		    		  !ParsePass2(Prog, iconData, where)) {
		    	} else {
		    		;
				}
		
				theFile.Unset();
		
				return nullptr;
			}	
		} else {
			reportError("what the hell do i do with %s files?", fileMainType.Type());
		}
	} else 	if (strcmp(fileSuperType.Type(), "text") == 0) {
	
// load ascii text thingy
// should now be treated as "audio/Qua"		
		if ((err=theFile.Seek(0, SEEK_SET)) != B_NO_ERROR) {
			reportError(ErrorStr(err));
			return nullptr;
		}
		Parser	*Prog = new Parser(&theFile, this);
		iconData->SetBits(quaIcon->Bits(), quaIcon->BitsLength(), 0, B_CMAP8);
    	if (  !Prog->ParseProgFile() ||
    		  !ParsePass2(Prog, iconData, where)) {
    	} else {
    		;
		}
		
		theFile.Unset();

		return nullptr;
	} else {
		if (iconData) delete iconData;
		reportError("what a strange file! do you want me to sit on it?");
	}
	
	return newObj;
}


bool
Qua::ParsePass2(Parser *Prog, BBitmap *iconData, BPoint where)
{
	Source 			*S, **SLP;
	Schedulable 	*SS, **SSLP;
	QuaObject		*QO;
	
	fprintf(stderr, "Pass2: Initing source list...\n");

// reclaim dodgy entries .......
    Source		*initSource = FindSource("Init");
	Source		*SON;
    for(S=Prog->SourceList; S!=nullptr; S=SON) {
    	SON = S->Next;
		if (S->Sym->Context == Sym) {
 			if (S->Init() && S != initSource) {
		    	QO=AddQuaObject(S->Sym, iconData, where);
		    	S->Next = SourceList;
		    	SourceList = S;
		    }
		}
    }
    
	fprintf(stderr, "Pass2: Initing schedulable list\n");

	Schedulable	*SN;
    for(SS=Prog->schedulees; SS!=nullptr; SS=SN) {
    	SN=SS->next;
    	if (SS->Init()) {
    		BBitmap		*iD = SS->FindIconData();
   		 	QO=AddQuaObject(SS->Sym, iD?iD:iconData, where);
    		SS->next = schedulees;
    		schedulees = SS;
	    } else {
	    	reportError("initialization failure");
	    }
    }
    
	fprintf(stderr, "Setting up Init...\n");

    // execute inits!
    Block	*initBlock = initSource?initSource->mainBlock:nullptr;
    if (initBlock) {
		Stream	MainStream;

		QuasiStack	*initStack = new QuasiStack(
							initSource->Sym,
							nullptr, nullptr, nullptr, nullptr);
		if (  initBlock->StackOMatic(initStack, 1) &&
			  initBlock->Reset(initStack)) {
			initStack->lowerFrame = theStack;
			
			fprintf(stderr, "Executing Init...\n");
			
			bool ua_complete = UpdateActiveBlock(
		    					this,
		    					&MainStream,
		    					initBlock,1,
		    					&sequencerTime,
		    					TRUE, nullptr,
		    					initStack);
		}	
		MainStream.ClearStream();
	}
	
	if (initSource) {	// get rid of faked init routine
		Glob.DeleteSymbol(initSource->Sym);
	}
	
	fprintf(stderr, "completed pass 2\n");
	return true;
}

long
Qua::MainWrapper(void *data)
{
	return ((Qua *)data)->Main();
}

void
Qua::IncrementTicks()
{
	sequencerTime.Ticks++;
	sequencerTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
}

void
Qua::ResetTicks()
{
	sequencerTime.Reset();
	currentBeatTick = 0;
	currentBarBeat = 0;
	currentBar = 0;
}


long
Qua::Main()
{
   int		i, j;
    float	lastSync;
    double	lastSyncMSecs;
    char	buf[20];
    Voice	*V;
    Block	*B;
    float	lastMSecs,
    		mSecsThisClock;
    
	Schedulable	*SO;
	
	lastMSecs = mSecsThisClock = 0;

    mSecs = 0;
    LastSync = 0;
    LastSyncMSecs = 0;
    
    Clock.Zero();
    Clock.Set(ActualTime);

    /* voice specific inits */

	while (Status != STATUS_DEAD) {
		iteration++;
		for (short i=0; i<NChannel; i++) {	
			OutChannel[i]->CheckInBuffers();
		}
		if (Status == STATUS_SLEEPING) {
//			if (suspend_thread(myThread) != B_NO_ERROR) {
//				reportError("heap o' shite\n");
//				exit(1);
//			}
			if (Status == STATUS_DEAD)
				break;
		} else {
			lastMSecs = mSecs;    
			mSecs = Clock.GetRunningTime();
			ActualTime = ((float)mSecs) / 1000.0;
			
			if ((mSecsThisClock += (mSecs - LastMSecs)) < 0)
				mSecsThisClock = 0;
			
			if (mSecsThisClock > 1000.0 * TimeQuanta) {
				IncrementTicks();
				mSecsThisClock = 0;
				
				if (DoMetroGnome && Status == STATUS_RECORDING && currentBeatTick == 0)
					beep();
			}
			
			if (sequencerTime != LastUpdate) {		
				for (SO=schedulees; SO != nullptr; SO = SO->next) {
					SO->Update(sequencerTime);
				}
				
				poolPlayer->Play(sequencerTime);
				LastUpdate = sequencerTime;
			}
		}
		for (short i=0; i<NChannel; i++) {	
//			OutChannel[i]->CheckInBuffers();
			OutChannel[i]->CheckOutBuffers();
		}
		snooze(500.0);
	}
	
    return 0;
}

void
Qua::DeleteQuaObject(QuaObject *Q)
{
	if (Q->Sym->Type == S_SOURCE) {
		DeleteSource(Q->Sym->SourceValue());
		return;
	}
	Schedulable		*S = Q->schedulable;
	sequencerWindow->arrange->DeleteArrangerObject(Q);
	if (S && S->controlPanel) {
		mixerWindow->DeleteSchedulablePanel(S->controlPanel);
	}
	Schedulable		*q, **qp;
	
	qp = &schedulees;
	q = schedulees;
	while (q != nullptr) {
		if (q == S) {
			*qp = q->next;
			DeleteSchedulable(q);
			break;
		}
		qp = &q->next;
		q = q->next;
	}

	Q->aQuarium->RemoveChild(Q);
	delete Q;
}

QuaObject *
Qua::AddQuaObject(StabEnt *S, BBitmap *iconData, BPoint &where)
{
	QuaObject				*QO = nullptr;
	SchedulablePanel		*SC = nullptr;
	
	BRect		Qrect(where.x, where.y, 50+where.x, where.y+20);
	BRect		CRect(0,0,FULL_MIXER_WIDTH,2*MIXER_MARGIN);
	
	fprintf(stderr, "add qua_obj @ %g %g in %x\n", where.x, where.y, sequencerWindow->objectView);
	
	switch (S->Type) {
	case S_POOL: {
    	QO = new QuaObject(S, Qrect, iconData, sequencerWindow->objectView, this, red);
    	SC = new SchedulablePanel(CRect, S->PoolValue());
		break;
	}	
	case S_SAMPLE: {
    	QO = new QuaObject(S, Qrect, iconData, sequencerWindow->objectView, this, yellow);
    	SC = new SchedulablePanel(CRect, S->SampleValue());
		break;
	}	
	case S_VOICE: {
		QO = new QuaObject(S, Qrect, iconData, sequencerWindow->objectView, this, green);
    	SC = new SchedulablePanel(CRect, S->VoiceValue());
		break;
	}
	case S_SOURCE: {
		if (S->Context == Sym) {
   		 	QO = new QuaObject(S, Qrect, iconData, sequencerWindow->objectView, this, ltGray);
		}
		break;
	}
 		
	case S_APPLICATION: {
    	QO = new QuaObject(S, Qrect, iconData, sequencerWindow->objectView, this, blue);
    	SC = new SchedulablePanel(CRect, S->ApplicationValue());
		break;
	}}
	
	if (QO) {
		where.y += 50;
	}
	if (SC) {
		mixerWindow->AddSchedulablePanel(SC);
	}
	return QO;
}

QuaObject *
Qua::FindQuaObject(StabEnt *S)
{
	Executable *p = S->ExecutableValue();
	if (p)
		return p->quaObject;
	return nullptr;
}


void
Qua::DeleteSource(Source *p)
{
	Source		*q, **qp;
	
	qp = &SourceList;
	q = SourceList;
	while (q != nullptr) {
		if (q == p) {
			*qp = q->Next;
			Glob.DeleteSymbol(p->Sym);
			break;
		}
		qp = &q->Next;
		q = q->Next;
	}
}


void
Qua::DeleteSchedulable(Schedulable *p)
{
	Schedulable		*q, **qp;
	
	qp = &schedulees;
	q = schedulees;
	while (q != nullptr) {
		if (q == p) {
			*qp = q->next;
			Glob.DeleteSymbol(p->Sym);
			break;
		}
		qp = &q->next;
		q = q->next;
	}
}


char *
NameStr(short s)
{
	switch (s) {
	case STATUS_UNKNOWN:
		return "unknown";
	case STATUS_RUNNING:
		return "running";
	case STATUS_DEAD:
		return "dead";
	case STATUS_SLEEPING:
		return "sleeping";
	case STATUS_RECORDING:
		return "recording";
	case STATUS_CLEANING_UP:
		return "cleaning up";
	default:
		return "wierd";
	}
}

status_t
Qua::Save(BFile *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent); bprintf(fp, "define #quaqua %s\n", Sym->PrintableName());
	tab(fp, indent); bprintf(fp, "{\n");
	if (Sym->Children) {
		err=Sym->Children->Save(fp, indent+1);
	}
	SaveInit(fp, indent+1);
	tab(fp, indent); bprintf(fp, "}\n");
	return err;
}

status_t
Qua::SaveInit(BFile *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent); bprintf(fp, "define Init()\n");
	tab(fp, indent); bprintf(fp, "{\n");
	tab(fp, indent+1); bprintf(fp, "tempo = %g\n", metric->Tempo);
	for (Schedulable *SO=schedulees; SO != nullptr; SO = SO->next) {
		for (short i=0; i<SO->instances.CountItems(); i++) {
			Instance *inst = (Instance*)SO->instances.ItemAt(i);
			inst->Save(fp, indent+1);
		}
	}
	tab(fp, indent); bprintf(fp, "}\n");
	return err;
}

status_t
Qua::WriteChunk(BFile *fp, ulong type, void *data, ulong size)
{
	fprintf(stderr, "chunk %x\n", type);
	status_t err=B_NO_ERROR;
	if ((err=fp->Write(&type, sizeof(type))) < B_NO_ERROR) {
		reportError("cant write chunk: %s", ErrorStr(err));
		return err;
	}
	off_t		sizePos = fp->Position();
	if ((err=fp->Write(&size, sizeof(size))) < B_NO_ERROR) {
		reportError("cant write chunk: %s", ErrorStr(err));
		return err;
	}
	off_t		dataStart = fp->Position();
	switch (type) {
	case 'NAME': {
		if ((err=fp->Write(data, size)) < B_NO_ERROR) {
			reportError("cant write chunk: %s", ErrorStr(err));
			return err;
		}
		break;
	}
	case 'QUAP': {
		for (Schedulable *P=schedulees; P!=nullptr; P=P->next) {
			if (P->Sym->Type == S_POOL)
				if ((err=WriteChunk(fp, 'POOL', P, 0)) != B_NO_ERROR) {
					return err;
				}
		}
		break;
	}
	case 'POOL': {
		Pool	*P = (Pool *) data;
		if ((err=WriteChunk(fp, 'NAME', P->Sym->Name, strlen(P->Sym->Name))) != B_NO_ERROR) {
			return err;
		}
		if ((err=WriteChunk(fp, 'STRM', &P->Stream, 0)) != B_NO_ERROR) {
			return err;
		}
		break;
	}
	case 'STRM': {
		Stream	*S = (Stream *) data;
		err = S->Save(fp);
		break;
	}
	default:
		reportError("bad chunk %x", type);
		return B_ERROR;
	}
	if (size == 0) {
		off_t endpos = fp->Position();
		size = endpos - dataStart;
		fp->Seek(sizePos, SEEK_SET);
		if ((err=fp->Write(&size, sizeof(size))) < B_NO_ERROR) {
			reportError("cant write chunk: %s", ErrorStr(err));
			return err;
		}
		fp->Seek(endpos, SEEK_SET);
	}
	return err;
}

ulong
Qua::ReadChunk(BFile *fp, void **bufp, ulong *buf_lenp)
{
	ulong	type, size;
	status_t	err;
	
	if ((err=fp->Read(&type, sizeof(type))) < B_NO_ERROR) {
		reportError("cant read chunk: %s", ErrorStr(err));
		return 0;
	}
	if ((err=fp->Read(&size, sizeof(size))) < B_NO_ERROR) {
		reportError("cant read chunk: %s", ErrorStr(err));
		return 0;
	}

	off_t	pos=fp->Position();
	switch (type) {
	case 'QUAP': {
		void	*data;
		ulong	datalen;
		while (fp->Position()-pos < size) {
			if (!ReadChunk(fp, &data, &datalen)) {
				return 0;
			}
			if (datalen)
				delete data;
		}
		break;
	}
	case 'NAME': {
		char *buf = new char[size+1];
		if ((err=fp->Read(buf, size)) != B_NO_ERROR) {
			reportError("can't read data");
			return 0;
		}
		buf[size] = 0;
		*bufp = buf;
		*buf_lenp = size;
		break;
	}
	case 'POOL': {
		char		*nm;
		ulong		len;
		
		if (ReadChunk(fp, &nm, &len)!='NAME') {
			reportError("Expected name");
			if (len)	delete nm;
			break;
		}
		Pool	*P = FindPool(nm);
		if (P == nullptr) {
			reportError("can't find %s", nm);
			break;
		}
		void	*data;
		ulong	datalen;
		if (ReadChunk(fp, &data, &datalen) != 'STRM') {
			reportError("Expected stream");
			if (datalen) delete data;
			break;
		}
		Stream		*rd = (Stream *) data;
		P->Stream.AppendStream(rd);
		delete data;
		break;
	}
	
	case 'STRM': {
		Stream	*rd = new Stream();
		rd->Load(fp);
		*bufp = (void*)rd;
		*buf_lenp = sizeof(Stream);
		break;
	}
	default:
		reportError("mysterious quap");
		break;
	}
	fp->Seek(pos+size, SEEK_SET);
	return type;
}
