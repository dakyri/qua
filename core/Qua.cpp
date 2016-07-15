#include "qua_version.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>
#else
#include <time.h>
#endif


#include <vector>
#include <mutex>
#include <regex>
#include <string>

#include <tinyxml2.h>

#include "StdDefs.h"

#include "Qua.h"
#include "QuaEnvironment.h"
#include "Executable.h"
#include "Schedulable.h"
#include "Channel.h"
#include "Destination.h"
#include "Sym.h"
#include "QuaMidi.h"
#include "QuaAudio.h"
#include "QuaOSC.h"
#include "Block.h"
#include "Lambda.h"
#include "QuasiStack.h"
#include "Pool.h"
#include "Voice.h"
#include "Sample.h"
#include "Template.h"
//#include "include/PoolPlayer.h"
#include "Template.h"
#include "Clock.h"
#include "QuaParallel.h"

#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#ifdef QUA_V_JOYSTICK
#include "QuaJoystick.h"
#endif
#ifdef QUA_V_AUDIO
#include "Sampler.h"
#endif


QuaEnvironmentDisplay environmentDisplay;
QuaEnvironment	environment(environmentDisplay);

flag debug_schedule = 0;

/*
 * Qua.
 */
/*
Qua::Qua(std::string path, bool chan_add):
	Executable(
		DefineSymbol(
			getLeaf(path), TypedValue::S_QUA, 0,
			this, nullptr,	TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT)),
	Stacker(this),
	TimeKeeper(&Metric::std),
	bridge(this)
{
	OnCreationInit(chan_add);
	StabEnt *newObj = loadFile(path);
}
*/

QuaDisplayStub defaultDisplay;

Qua::Qua(string nm, QuaPerceptualSet &display, bool chan_add):
	Executable(
		DefineSymbol(
			(nm.size()?nm: string("New")), TypedValue::S_QUA, 0,
			this, nullptr,	TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT)),
	Stacker(this),
	TimeKeeper(&Metric::std),
	bridge(*this, display)
{
	OnCreationInit(chan_add);
}

Clip *
Qua::addClip(std::string nm, bool disp)
{
	Clip	*c = new Clip(nm, sym);
	clips.push_back(c);
	return c;
}

Clip *
Qua::addClip(std::string nm, Time&start, Time&duration, bool disp)
{
	Clip	*c = new Clip(nm, sym);
	clips.push_back(c);
	c->Set(nullptr, start, duration);
	if (disp) {
		uberQua->bridge.updateClipIndexDisplay(sym);
	}
	return c;
}

void
Qua::removeClip(Clip *c, bool disp)
{
	for (auto ci = clips.begin(); ci != clips.end(); ++ci) {
		if (*ci == c) {
			clips.erase(ci);
			break;
		}
	}

	glob.DeleteSymbol(c->sym, true);
	if (disp) {
		uberQua->bridge.updateClipIndexDisplay(sym);
	}
}

status_t
Qua::OnCreationInit(bool chan_add)
{
    StabEnt	*ts, *cs, *tempos;

	theStack = new QuasiStack(sym, this, sym, nullptr, nullptr, nullptr, this, nullptr);
	
	fprintf(stderr, "initting qua %s...\n", sym->name.c_str());
	fprintf(stderr, "base value %d bytes\n", sizeof(base_val_t));
	fprintf(stderr, "typed value %d bytes\n", sizeof(TypedValue));
	fprintf(stderr, "result value %d bytes\n", sizeof(ResultValue));
	fprintf(stderr, "lvalue %d bytes\n", sizeof(LValue));
    glob.PushContext(sym);

// TODO ... better seeding
	/*
			std::mt19937 eng((std::random_device())());
		std::uniform_int_distribution<> randomInt(autoReconnectMinMS,autoReconnectMaxMS);
		autoReconnectFrequency = randomInt(eng);
*/
	srand((int)0);

//	AddClip("region", false);

    iterCounter = 0;
    methods = nullptr;
	schedulees = nullptr;
    usecTime = nextTickTime = 0;
	currentLoop = nullptr;
    theTime.Set(0, metric);
//	RegionClip(0)->start.Set(0, metric);
//	RegionClip(0)->duration.Set(0, metric);
    lastUpdate = theTime - 1;
    actualTime = 0;
	loop = false;
#ifdef QUA_V_AUDIO_ASIO
//	theClock = new Clock(true);
//	timingType = QUA_TIME_ASIO;
    theClock = new Clock(false);
	timingType = QUA_TIME_INTERNAL;
	timingDevice = nullptr;
#else
    theClock = new Clock(false);
	timingType = QUA_TIME_INTERNAL;
	timingDevice = nullptr;
#endif
	ResetTicks();
    status = STATUS_SLEEPING;
	projectDirectoryPath = "."; 
	sampleDirectoryPath = "."; 
	doMetroGnome = false;
	autoCreateAmpEnv = true;
	nChannel = 0;
	for (short i=0; i<MAX_CHANNEL; i++) {
		channel[i] = nullptr;
	}
	
#if defined(QUA_V_POOLPLAYER)
    poolPlayer = new PoolPlayer(this);
#endif
	bridge.Spawn();

	if (chan_add) {	
	    for (short i=0; i<16; i++) {
	    	string nm = string("channel") + std::to_string(i+1);	
			AddChannel(nm, i, 2, 2, false, false, true,true,true,true, i+1);
			if (!channel[i]->Init()) {
		  		bridge.tragicError("cant initialize channel %d", i+1);
			}
	    }
		AddChannel("system", 16, 2, 2, false, false, true, true,true,true,0);
	} else {
//		mixerWindow->Show();
//	    sequencerWindow->Show();
	}	
    
   fprintf(stderr, "added channels\n");

    ts = DefineSymbol("actualtime", TypedValue::S_FLOAT, 0,
    					&actualTime, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    cs = DefineSymbol("ticktime", TypedValue::S_INT, 0,
    					&theTime.ticks, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    tempos = DefineSymbol("tempo", TypedValue::S_FLOAT, 0,
    					&metric->tempo, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("bar", TypedValue::S_INT, 0,
    					&currentBar, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("beat", TypedValue::S_INT, 0,
    					&currentBarBeat, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("tick", TypedValue::S_INT, 0,
    					&currentBeatTick, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("loop", TypedValue::S_BOOL, 0,
    					&loop, sym,
    					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);

    fprintf(stderr, "time quanta %g %g\n", secsPerTick, 6*secsPerTick);

    status = STATUS_SLEEPING;
	return B_NO_ERROR;
}


// this should do all the final initialisations,
// setup devices. after this we should just set the clock and press play ...
status_t
Qua::PostCreationInit()
{

	fprintf(stderr, "PostCreationInit: initializing main %x\n", (unsigned)mainBlock);
	if (mainBlock && !mainBlock->Init(this)) {
		bridge.reportError("main qua block not initialised");
	}

#ifdef INIT_BY_METHOD
	StabEnt	*is = glob.findContextSymbol("Init", sym, -1);
	Lambda	*initMethod = nullptr;
	if (is && is->type == TypedValue::S_LAMBDA) {
		initMethod = is->LambdaValue();
	}
	Block	*initBlock = nullptr;
	if (initMethod) {
		initBlock = initMethod->mainBlock;
	} else {
	}
	if (!initBlock) {
	} else {
		fprintf(stderr, "Found initialization block for Qua\n");
    	
		Stream	mainStream;

//	theStack = new QuasiStack(sym, this, sym, nullptr, nullptr, nullptr, this, nullptr);
		QuasiStack	*initStack = new QuasiStack(
							initMethod->sym,
							this, sym, nullptr, nullptr, nullptr, this, nullptr);
		if (  initBlock->StackOMatic(initStack, 1) &&
			  initBlock->Reset(initStack)) {
			initStack->lowerFrame = theStack;
			
			fprintf(stderr, "Executing Init method of %s...\n", sym->name.c_str());
			
			bool ua_complete = (UpdateActiveBlock(
		    					this,
		    					mainStream,
		    					initBlock,
		    					theTime,
		    					this,
								sym,
		    					initStack,
								1,
								true) == BLOCK_COMPLETE);
		}	
		mainStream.ClearStream();
	}
#else
    Block	*initBlock = mainBlock;
    if (initBlock) {
		fprintf(stderr, "PostCreationInit: calling main block ...\n");
		QuasiStack	*initStack = theStack;
		Stream	mainStream;
		if (  initBlock->StackOMatic(initStack, 1) &&
			  initBlock->Reset(initStack)) {
			fprintf(stderr, "PostCreationInit: executing init method of '%s'...\n", sym->name);
			
			flag ua_complete = UpdateActiveBlock(
		    					this,
		    					&mainStream,
		    					initBlock,
		    					&theTime,
		    					this,
								sym,
		    					initStack,
								1,
								true);
		}	
		mainStream.ClearStream();
	}
#endif
	fprintf(stderr, "PostCreationInit: initializing channels...\n");
// this may open and or load actual devices
	for (short i=0; i<nChannel; i++) {
		if (!channel[i]->Init()) {
		    bridge.reportError("can't init channel");
		}
	}

	fprintf(stderr, "PostCreationInit: spawning main qua thread\n");

    status = STATUS_SLEEPING;
	lastTickTime = 0;
	tickProcessEndTime = 0;
	lastUpdate.ticks = -1;
//	theTime = currentLoop?currentLoop->start:Time::zero;
	cueItem = schedule.head;
	usecTime = 0;
#ifdef QUA_V_ALARMCLOCK
    theClock->Start();
#else
	theClock->Zero();
#endif
    SetTempo(120, true);

#ifdef QUA_V_MULTIMEDIA_TIMER
	mmTimerId = timeSetEvent(
		theApp.wTimerRes,0,&MMEventProc,(DWORD_PTR)this,TIME_PERIODIC);
	if (mmTimerId == nullptr) {
		bridge.reportError("Qua::PostCreationInit(): failed to set multimedia timer event, res %dms", theApp.wTimerRes);
	}
#else
	myThread = std::thread(MainWrapper, this);

#endif
#ifdef QUA_V_AUDIO
	status_t err=environment.quaAudio.StartAudio();
	if (err != B_OK) {
		bridge.reportError("Can't start audio: %s\n", environment.quaAudio.ErrorString(err));
	}
#endif
	return B_OK;
}

// should pause and lock the channel for all of these
Channel *
Qua::AddChannel(std::string nm, short ch_id,
				short nAudioIns, short nAudioOuts,
				bool au_thru, bool midi_thru,
				bool add_dflt_au_in,
				bool add_dflt_au_out,
				bool add_dflt_str_in,
				bool add_dflt_str_out,
				short midi_ch)
{
	if (ch_id < 0 || ch_id > nChannel) {
		ch_id = nChannel;
	}
	if (channel[ch_id] != nullptr) {
		bridge.reportError("Channel with id %d already present: not added", ch_id);
		return nullptr;
	}
 	Channel *c = new Channel(nm, ch_id, au_thru, midi_thru, nAudioIns, nAudioOuts, this);
 	if (add_dflt_str_in) {
		if (environment.quaMidi.dfltInput) {
			Input *s = c->AddInput("midin", "*", environment.quaMidi.dfltInput, midi_ch, true);
		}
	}
	if (add_dflt_str_out) {
		if (environment.quaMidi.dfltOutput) {
			Output *d = c->AddOutput("midout", "*", environment.quaMidi.dfltOutput, midi_ch, true);
		}
	}
#ifdef QUA_V_AUDIO
 	if (add_dflt_au_in) {

		if (getAudioManager().dfltInput) {
			Input *s = c->AddInput("audin", "*", environment.quaAudio.dfltInput, 0, true);
			// try and make a stereo pair
			if (nAudioIns == 2) {
				if (getAudioManager().dfltInput->NInputChannels(0) == 1) {
					s->setPortInfo(getAudioManager().dfltInput, 1, 1);
				} else if (getAudioManager().dfltInput->NInputChannels(0) == 2) {
					;
				} else {
					;
				}
			}
//			Enable(s, false);
		}
	}
	if (add_dflt_au_out) {
		if (getAudioManager().dfltOutput) {
			Output *d = c->AddOutput("audout", "*", getAudioManager().dfltOutput, 0, true);
			// try and make a stereo pair
			if (nAudioOuts == 2) {
				if (getAudioManager().dfltOutput->NOutputChannels(0) == 1) {
					d->setPortInfo(getAudioManager().dfltOutput, 1, 1);
				} else if (getAudioManager().dfltOutput->NOutputChannels(0) == 2) {
					;
				} else {
					;
				}
			}
//			Enable(d, true);
		}
	}
#endif

	fprintf(stderr, "adding channel %d %x\n", nChannel, (unsigned)c);
	channel[ch_id] = c;
	if (ch_id >= nChannel) {
		nChannel++;
	}
//	display.CreateChannelBridge(c);
 	return c;
}

status_t
Qua::MoveChannel(short ch_id, short to_ch_id, bool updateDisplay)
{
	return B_NO_ERROR;
}

status_t
Qua::RemoveChannel(short ch_id, bool updateDisplay)
{
	if (ch_id >= nChannel || channel[ch_id] == nullptr) {
		bridge.reportError("Channel with id %d not present: not removed", ch_id);
		return B_ERROR;
	}
	Channel *c = channel[ch_id];
	if (c == nullptr) {
		bridge.reportError("Attemt to delete null channel %d fails", ch_id);
	}
	StreamItem	*p =nullptr;
	p = schedule.head;
	while (p != NULL) {
		if (p->type == TypedValue::S_VALUE) {
			StreamValue	*q = ((StreamValue *)p);
			Instance	*i = q->value.InstanceValue();
			if (i && i->channel == c) {
				bridge.reportError("Channel must be clear before deletion");
				return B_ERROR;
			}
		}
		p = p->next;
	}

	nChannel--;
	if (updateDisplay) {
		bridge.RemoveChannelRepresentations(c->sym);
	}
	glob.DeleteSymbol(c->sym, true);
	/* delete remove and disconnect all of this channels ins and outs */
	/* delete the channel */
	return B_NO_ERROR;
}


Qua::~Qua()
{
	fprintf(stderr, "Qua: about to delete %s\n", sym->name.c_str());
#ifdef QUA_V_AUDIO
	getAudioManager().StopAudio();
	fprintf(stderr, "Qua: stopped audio\n");
#endif
	
	status = STATUS_DEAD;
#ifdef QUA_V_MULTIMEDIA_TIMER
	if (mmTimerId != nullptr) {
		if (timeKillEvent(mmTimerId) != TIMERR_NOERROR) {
			fprintf(stderr, "failed to kill timer event for %s\n",sym->name);
		}
		mmTimerId = NULL;
	}
#else
	// XXX check thread hanging
	theClock->UnsetAlarm();
	fprintf(stderr, "Qua: waiting for main thread %s\n", sym->name.c_str());
	if (myThread.joinable()) {
		myThread.join();
	}
#endif
	/* free up blocks used */

 	fprintf(stderr, "%s::~Qua(). Halting channels ...\n", sym->name.c_str());

   for (short i=0; i<nChannel; i++) {	
		channel[i]->Stop();
		// possibly unlock these
    }

 	fprintf(stderr, "%s::~Qua(). Clearing context stack...\n", sym->name.c_str());
	glob.PopContext(sym);
	
//	sym->Dump(stderr, 0);
 	fprintf(stderr, "%s::~Qua(). Deleting symbols...\n", sym->name.c_str());
	glob.DeleteSymbol(sym, true);

	bridge.Cleanup();

	fprintf(stderr, "~Qua: deleting devices!\n");

 	fprintf(stderr, "################delete complete\n");
}

void
Qua::SetTempo(float t, bool disp)
{
	metric->tempo = t;
	secsPerTick = metric->secsPerTick();
//	fprintf(stderr, "tempo %g %g\n", metric->tempo, timeQuanta);
	if (disp) {
		bridge.DisplayTempo(metric->tempo, true);
	}
}


void
Qua::setName(const string &nm, bool setTitle)
{
	if (nm != sym->name) {
		glob.rename(sym, nm);
		if (setTitle) {
			bridge.displayArrangementTitle(sym->name.c_str());
		}
	}
}


void
Qua::UpdateRecordDisplay()
{
	if (status == STATUS_RECORDING) {
    	for (short i=0; i<nChannel; i++) {
    		channel[i]->UpdateRecordDisplay();
		}
	}
}

long
Qua::Pause()
{
	return B_OK;
}

long
Qua::Restart()
{
	return B_OK;
}


static int		new_id = 0;

Pool *
Qua::NewPool(short chan)
{
	string pool_nm = string("new") + std::to_string(new_id++);
	
    Pool *S = new Pool(pool_nm, this, sym, true);
    AddSchedulable(S);

	return S;
}

Sample *
Qua::CreateSample(std::string nm, bool andD)
{
	static int		new_sample_id = 0;

	if (nm.size() == 0) {
		nm = string("sample") + std::to_string(new_sample_id++);
	}
	Sample *S = new Sample(nm, "", this, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
    AddSchedulable(S);

	if (andD) {
		;
	}

	return S;
}

Voice *
Qua::CreateVoice(std::string nm, bool andD)
{
	static int		new_voice_id = 0;

	if (nm.size() == 0) {
		nm = string("voice")+ std::to_string(new_voice_id++);
	}

	Voice *S = new Voice(nm, this);
    AddSchedulable(S);

	if (andD) {
		;
	}
	return S;
}

Lambda *
Qua::CreateMethod(std::string nm, StabEnt *ctxt, bool andD)
{
	static int		new_method_id = 0;
	if (nm.size() == 0) {
		nm = string("lambda") + std::to_string(new_method_id++);
	}

	Lambda *S = new Lambda(nm, ctxt, this);
	S->next = methods;
	methods = S;

	if (andD) {
		;
	}
	return S;
}

long
Qua::Start()
{
	if (status != STATUS_RUNNING) {
//    	SendStart(0);
    	
    	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
    		p->QuaStart();
    	}

		bool	wasRec = status == STATUS_RECORDING;
		status = STATUS_RUNNING;
		fprintf(stderr, "starting...\n");
//		theClock->Set(actualTime);
#ifdef QUA_V_MULTIMEDIA_TIMER
		if (mmTimerId == nullptr) {
			mmTimerId = timeSetEvent(
				theApp.wTimerRes,0,&MMEventProc,(DWORD_PTR)this,TIME_PERIODIC);
			if (mmTimerId == nullptr) {
				bridge.reportError("Failed to set multimedia timer event");
			}
		}
#else
#endif	
		if (wasRec) {
			StopRecording();
		}
	}
	return B_OK;
}

status_t
Qua::StartRecording()
{
	if (status == STATUS_SLEEPING) {
		Start();
	}

   	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
   		p->QuaRecord();
   	}


	for (short i=0; i<nChannel; i++) {
		channel[i]->StartRecording();
	}
	status = STATUS_RECORDING;
	return B_OK;
}

status_t
Qua::StopRecording()
{
   	for (short i=0; i<nChannel; i++) {
   		channel[i]->StopRecording();
	}
	return B_NO_ERROR;
}

long
Qua::Stop()
{
	if (status != STATUS_SLEEPING) {
    	for (Schedulable *p = schedulees; p!=nullptr; p=p->next) {
    		fprintf(stderr, "Stopping %x %s: ", (unsigned)p, p->sym->name.c_str());
    		p->QuaStop();
    		fprintf(stderr, "done\n");
    	}
//		environment.quaAudio->StopAudio();
		bool	wasRec = status == STATUS_RECORDING;
    	status = STATUS_SLEEPING; // stops main thread
#ifdef QUA_V_MULTIMEDIA_TIMER
		if (mmTimerId != nullptr) {
			if (timeKillEvent(mmTimerId) != TIMERR_NOERROR) {
				fprintf(stderr, "failed to kill timer event for %s\n",sym->name);
			}
			mmTimerId = nullptr;
		}
#else
#endif
		if (wasRec) {
			StopRecording();
		}
//    	SendStop(0);
    }
	return true;
}

/**
 * deduce somehow the mime type of a particular file, and set it in the given BMimeType structure
 *  the mime types of inerest are mainly audio data formats.
 * internally to Qua, these are
 *		audio/x-raw			raw 16bit stereo
 *		audio/x-wav			wav file
 *		audio/x-aiff		aiff file
 *
 * anything else will be labelled as "application/octet-stream"
 *
 * returns: B_OK if the mime type is valid
 *			B_ERROR if the mime type is not set
 */
std::string
Qua::identifyFile(std::string path)
{
	int n = path.find_last_of('.');
	if (n == -1) {
		return "";
	}
	std::string p = path.substr(n + 1);
	if (p == "wav" || p == "wave") {
		return "audio/x-wav";
	} else if (p == "aif" || p == "aiff") {
		return "audio/x-aiff";
	} else if (p == "mid" || p == "midi") {
		return "audio/x-midi";
	} else if (p == "qs") {
		return "audio/x-quascript";
	} else if (p == "qua") {
		return "audio/x-quascript";
	} else {
		return "application/octet-stream";
	}
	return "";
}

// largely defunk ... used to instantiate from templates in prior incarnations
StabEnt *
Qua::loadObject(StabEnt *obj)
{
	StabEnt	*newObj = nullptr;
	switch (obj->type) {
	
	case TypedValue::S_TEMPLATE: {
		Template	*t = obj->TemplateValue();

		switch (t->type) {
		
		case TypedValue::S_SAMPLE: {
			Sample			*sam;
			sam = CreateSample(nullptr, true);
			if (t->instantiate(sam->sym) != B_OK) {
				bridge.reportError("Can't instantiate template");
			}

			break;
		}
		
		case TypedValue::S_POOL: {
			Pool			*sam;
			sam = NewPool(0);

			if (t->instantiate(sam->sym) != B_OK) {
				bridge.reportError("Can't instantiate template");
			}

			break;
		}
		
		}
		break;
	}
	
	default: {
		break;
	}
	
	}
	return newObj;
}

StabEnt *
Qua::loadFile(std::string path)
{
	StabEnt	*newObj = nullptr;
	std::string	fileMainType;
	std::string	fileSuperType;
	
	if ((fileMainType = identifyFile(path)) != "") {
		return nullptr;
	}
	fileMainType = getSupertype(fileSuperType);

// find the file's name...

	std::string nm = getBase(getLeaf(path));
	
// load an application binary
	if (fileSuperType == "application") {
		// todo xxxxx perhaps here convenience construction of an osc link
		return nullptr;

	} else 	if (fileSuperType == "audio") {
		if (	fileMainType == "audio/x-aiff" ||
				fileMainType == "audio/x-wav" ||
				fileMainType == "audio/raw") {
			Sample		*sample = new Sample(
										nm, path, this,
										MAX_BUFFERS_PER_SAMPLE,
										MAX_REQUESTS_PER_SAMPLE);
			sample->next = schedulees;
			schedulees = sample;
//			newObj = display.CreateSchedulableBridge(sample);
		} else if (fileMainType == "audio/x-midi") {
			bridge.reportError("Cannot yet import standard midi files");
		} else if (fileMainType == "audio/x-quascript") {
//			if (quapp->LoadQua(path->Path())) {
//			} else {
				FILE	*fp = fopen(path.c_str(), "r");
				if (fp != nullptr) {
					Parser	*parser = new Parser(fp, getBase(path), this);
			    	if (  !parser->ParseProgFile() ||
			    		  !ParsePass2(parser, nullptr)) {
			    	} else {
			    		;
					}
					delete parser;
					fclose(fp);
//				}
		
				return nullptr;
			}	
		} else {
			bridge.reportError("what the hell do i do with %s files?", fileMainType.c_str());
		}
	} else 	if (fileSuperType == "text") {
	
// load ascii text thingy
// should now be treated as "audio/QuaScript"		
		FILE	*fp = fopen(path.c_str(), "r");
		if (fp != nullptr) {
			Parser	*prog = new Parser(fp, getBase(path), this);
	    	if (  !prog->ParseProgFile() ||
	    		  !ParsePass2(prog, &newObj)) {
	    	} else {
	    		;
			}
			delete prog;
			fclose(fp);
		}

		return nullptr;
	} else {
		bridge.reportError("what a strange file! do you want me to sit on it?");
	}
	
	return newObj;
}


Qua *
Qua::loadScriptFile(const char *path, QuaPerceptualSet &display)
{
	std::string	thePath = path;
	FILE		*theFile = fopen(path, "r");
	Qua *q = nullptr;
	if (theFile == nullptr) {
		fprintf(stderr, "loading %s failed: not found\n", path);
	} else {
		fprintf(stderr, "loading %s\n", path);
//		char	buf[20];
//		if (fread(buf, strlen(magic), 1, theFile) != 1) {
//			fclose(theFile);
//			return false;
//		}
		string sourcename = quascript_name(getBase(thePath));
		q = new Qua(sourcename, display, false);
		Parser		*p = new Parser(theFile, sourcename, q);
		q = p->ParseQua();
		p->ShowErrors();
		if (q != nullptr && p->err_cnt == 0) {
			fprintf(stderr, "parsed a qua, proceeding with initialization %x\n", (unsigned)q->mainBlock);
			//		    	BPoint		where(0,0);
			q->ParsePass2(p, nullptr);
			fprintf(stderr, "initialised a qua, proceeding post initialization %x\n", (unsigned)q->mainBlock);
			q->PostCreationInit();
			fprintf(stderr, "fully initialized qua!\n");
		}
		fclose(theFile);
	}

	return q;
}

status_t
Qua::loadSnapshotFile(const char *path)
{
	tinyxml2::XMLDocument doc;
	FILE *xml = fopen(path, "r");
	if (xml == nullptr) {
		return B_OK;
	}
	tinyxml2::XMLError err = doc.LoadFile(xml);
	if (err != tinyxml2::XML_NO_ERROR) {
		fclose(xml);
		return B_ERROR;
	}
	tinyxml2::XMLElement* root = doc.RootElement();
	if (loadSnapshotElement(root) != B_OK) {
		fclose(xml);
		return B_ERROR;
	}
	fclose(xml);
	return B_OK;
}

status_t
Qua::loadSnapshotElement(tinyxml2::XMLElement* element)
{
	std::string	nameAttr;
	std::string scriptAttr;
	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;

	const char *scriptVal = element->Attribute("script");
	const char *nameVal = element->Attribute("name");

	if (nameVal) {
		hasNameAttr = true;
		nameAttr = nameVal;
	}


	std::string namestr = element->Value();

	if (namestr == "snapshot") {
		loadSnapshotChildren(element);
	} else if (namestr == "qua") {
		loadSnapshotChildren(element);
	} else if (namestr == "voice") {
		if (hasNameAttr) {
			Voice	*v = findVoice(nameAttr.c_str(), -1);
			if (v != nullptr) {
				v->LoadSnapshotElement(element);
			} else {
				bridge.reportError("voice '%s' not found while loading snapshot", nameAttr.c_str());
			}
		}
	} else if (namestr == "sample") {
		if (hasNameAttr) {
			Sample	*v = findSample(nameAttr.c_str(), -1);
			if (v != nullptr) {
				v->LoadSnapshotElement(element);
			} else {
				bridge.reportError("sample '%s' not found while loading snapshot", nameAttr.c_str());
			}
		}
	} else if (namestr == "pool") {
		if (hasNameAttr) {
			Pool	*v = findPool(nameAttr.c_str(), -1);
			if (v != nullptr) {
				v->LoadSnapshotElement(element);
			} else {
				bridge.reportError("pool '%s' not found while loading snapshot", nameAttr.c_str());
			}
		}
	} else if (namestr == "channel") {
		if (hasNameAttr) {
			Channel	*v = findChannel(nameAttr.c_str(), -1);
			if (v != nullptr) {
				v->LoadSnapshotElement(element);
			} else {
				bridge.reportError("channel '%s' not found while loading snapshot", nameAttr.c_str());
			}
		}
	} else if (namestr == "instance") {
	} else if (namestr == "stack") {
		loadSnapshotChildren(element);
	} else if ((namestr == "fixed") || namestr == "envelope") {
		if (hasNameAttr) {
#ifdef QUA_V_SAVE_INITASXML
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, this, sym, NULL);	
			}
#endif
		}
	} else if (namestr == "streamtake") {
	} else if (namestr == "sampletake") {
	} else {
	}

	return B_OK;
}

status_t
Qua::loadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (loadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
	}
	return B_OK;
}


bool
Qua::ParsePass2(Parser *prog, StabEnt **likelyLoad)
{
	Lambda 			*lambda;
	Schedulable 	*schedulable;
	
	fprintf(stderr, "Pass2: Initing lambda list...\n");

// reclaim dodgy entries .......
	Lambda		*SON;
    for(lambda=prog->methods; lambda!=nullptr; lambda=SON) {
    	SON = lambda->next;
		if (lambda->sym->context == sym) {
 			if (lambda->Init()) {
//		    	QO=display.CreateMethodBridge(S);
		    	lambda->next = methods;
		    	methods = lambda;
		    }
		}
    }
    
	fprintf(stderr, "Pass2: Initing schedulable list\n");

	if (likelyLoad)
		*likelyLoad = nullptr;
	Schedulable	*SN;

    for(schedulable=prog->schedulees; schedulable!=nullptr; schedulable=SN) {
    	SN=schedulable->next;
    	if (schedulable->Init()) {
/*
   		 	QO=display.CreateSchedulableBridge(SS);
			if (likelyLoad && *likelyLoad == nullptr)
				*likelyLoad = QO;
	*/
			bridge.AddSchedulableRepresentation(schedulable);
    		schedulable->next = schedulees;
    		schedulees = schedulable;
	    } else {
	    	bridge.reportError("initialization failure");
	    }
    }
    
//	glob.DumpContexts(stderr);
	fprintf(stderr, "Pass2 initializations complete\n");

	return true;
}


void
Qua::SetToRegionStart()
{
	if (currentLoop && currentLoop->start != INFINITE_TICKS) Cue(currentLoop->start);
}


void
Qua::SetToRegionEnd()
{
	if (currentLoop && currentLoop->start != INFINITE_TICKS) Cue(currentLoop->start+currentLoop->duration);
}

void
Qua::Cue(class Time &t)
{
	theTime = t;
	for (Schedulable *SO=schedulees; SO != nullptr; SO = SO->next) {
		SO->Cue(theTime);
	}
}


status_t
Qua::WaitUntilStop()
{
	return B_OK;
}

status_t
Qua::WaitUntil(Time &t)
{
	return B_OK;
}

inline status_t
Qua::Wake(Instance *inst)	{ return inst->schedulable->Wake(inst); }
inline status_t
Qua::Sleep(Instance *inst)	{ return inst->schedulable->Sleep(inst); }

bool
Qua::AddToSchedule(Instance *inst)
{
	if (!inst) return false;
	TypedValue	v;
	v.Set(inst);
	schedule.AddToStream(v, inst->startTime);
	if (inst->startTime < theTime && (inst->startTime+inst->duration) >theTime) {
		cueItem = nullptr;
	}
	return true;
}

bool
Qua::RemoveFromSchedule(Instance *inst)
{
	if (!inst) return false;
	TypedValue	v;
	v.Set(inst);
	StreamItem	*instit = schedule.FindItemAtTime(0, inst->startTime, v);
	if (instit) {
		if (instit == cueItem) {
			cueItem = nullptr;
		}
		schedule.DeleteItem(instit);
	}
	return true;
}

status_t
Qua::CheckScheduledActivations()
{
	if (cueItem == nullptr) {
		if (schedule.head == nullptr) { // nothin to do
			if (debug_schedule) fprintf(stderr, "No schedule at %s\r", theTime.StringValue());
			return B_OK;
		}
		cueItem = schedule.head;
	}
	if (debug_schedule) {
		fprintf(stderr, "Scheduling %x at %s\r", (unsigned)cueItem, theTime.StringValue());
	}
	while (cueItem && cueItem->time <= theTime) {
		if (debug_schedule) {
			fprintf(stderr, "Scheduling %x %s ", (unsigned)cueItem, cueItem->time.StringValue());
		}
		if (debug_schedule) fprintf(stderr, "at %s ", theTime.StringValue());
			switch (cueItem->type) {

			case TypedValue::S_VALUE: {
				TypedValue	v = ((StreamValue*)cueItem)->value;
				if (debug_schedule) fprintf(stderr, "value %d ", v.type);
				switch (v.type) {
					case TypedValue::S_INSTANCE: {
						Instance	*inst = v.val.instance;
						if (debug_schedule) fprintf(stderr, "instance %d ", inst->status);
						if (	inst->status != STATUS_RUNNING
								&& theTime >= inst->startTime
								&& theTime <= (inst->startTime + inst->duration)
							  ) {
							fprintf(stderr, "Waking %s at %s\n", inst->sym->name.c_str(), theTime.StringValue());
							Wake(inst);
							activeInstances.push_back(inst);
						}
						break;
					}
				}
				break;
			}

			default: {
				break;
			}
		}
		cueItem = cueItem->next;
		if (debug_schedule) fprintf(stderr, "\r");
	}
	return B_OK;
}

void
Qua::ResetTicks()
{
	theTime.Reset();
	currentBeatTick = 0;
	currentBarBeat = 0;
	currentBar = 0;
}

inline void
Qua::SequencerIteration()
{
	int	i;

	iterCounter++;
//		fprintf(stderr, "checking inputs %d %g\n", iterCounter, timeQuanta);
	for (i=0; i<nChannel; i++) {	
		channel[i]->CheckInBuffers();
	}
	switch (status) {
		case STATUS_SLEEPING: {
			nextTickTime = usecTime + (1000000.0 * secsPerTick);
			break;
		}

		case STATUS_RECORDING:
		case STATUS_RUNNING: {
			if (theTime != lastUpdate) {		
				theTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
				actualTime = usecTime/1000000.0;

#ifdef QUA_V_BETTER_SCHEDULER
// all blocks and stacks are locked.
// some objects may eventually self-modify within this loop,
// and would need write access
			
				// activeInstances.Lock();
				CheckScheduledActivations();

//					objectsBlockStack.ReadLock();
				std::vector<Instance*> sleepers;
				std::vector<Instance*> runners;
				for (auto ai = activeInstances.begin(); ai != activeInstances.end();) {
					Instance	*inst = *ai;
					if (	/* inst->status == STATUS_RUNNING && */
								(	theTime < inst->startTime
								|| theTime >= (inst->startTime + inst->duration)
							)) {
						sleepers.push_back(inst);
						ai = activeInstances.erase(ai);
					} else {
						runners.push_back(inst); 
						++ai;
					}
				}
				for (auto ai : runners) {
					ai->Run();
				}
				for (auto ai : sleepers) {
					ai->Run();
				}

//					objectsBlockStack.ReadUnlock();

				// activeInstances.Unlock();
#else
				for (SO=schedulees; SO != nullptr; SO = SO->next) {
					SO->Update(theTime);
				}
#endif
#if defined(QUA_V_POOLPLAYER)
				poolPlayer->Play(theTime);
#endif
				lastUpdate = theTime;
				
				if (doMetroGnome && status == STATUS_RECORDING && currentBeatTick == 0) {
					MetroGnomeClick();
				}

			// set up timer for next event.
				theTime.ticks++;
				fprintf(stderr, "tick %d actual time %lldus", theTime.ticks, usecTime);
				fprintf(stderr, " optimal time %lldus", nextTickTime);
				fprintf(stderr, " process time %lldus\n", tickProcessEndTime);
				if (currentLoop && currentLoop->duration.ticks > 0) {
					if (theTime >= (currentLoop->start + currentLoop->duration)) {
						if (loop) {
							theTime = currentLoop->start;
						} else {
							if (status == STATUS_RECORDING) {
								StopRecording();
							}
							status = STATUS_SLEEPING;
						}
					}
				}
				bridge.UpdateSequencerTimeDisplay(true);
				lastTickTime = nextTickTime;
				nextTickTime = nextTickTime + (bigtime_t)(1000000.0 * secsPerTick);
			}

			break;
		}
	}

// check channel outputs.
	for (i=0; i<nChannel; i++) {	
		channel[i]->CheckOutBuffers();
	}
	tickProcessEndTime = theClock->USecTime();
}


#ifdef QUA_V_MULTIMEDIA_TIMER
void
Qua::MMEventProc(
			UINT uID,      
			UINT uMsg,     
			DWORD dwUser,  
			DWORD dw1,     
			DWORD dw2      
		)
{
	Qua	*qua=(Qua *)dwUser;
	qua->usecTime = qua->theClock->USecTime();
	if (qua->usecTime >= qua->nextTickTime) {
		qua->SequencerIteration();
	}
}

#else
int32
Qua::MainWrapper(void *data)
{
	return ((Qua *)data)->Main();
}

long
Qua::Main()
{
	while (status != STATUS_DEAD) {
		SequencerIteration();

#ifdef QUA_V_ALARMCLOCK
		usecTime = theClock->WakeMeAfter(nextTickTime, 5000000); // 5 sec timeout on clock
		fprintf(stderr, "next tick = %lld\n", nextTickTime);
		fprintf(stderr, "usecs = %lld\n", usecTime);
#else
		usecTime = theClock->USecTime();
		while (usecTime < nextTickTime) {

#ifdef QUA_V_SWITCH_TO_THREAD
			// works better than sleep(1), which is jittery
			// and sleep(0) which is too greedy
			SwitchToThread();
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(0));
			usecTime = theClock->USecTime();
//			fprintf(stderr, "usecs = %lld", usecTime);
//			fprintf(stderr," nextTickTime = %lld\n", nextTickTime);
		}
//		total_err += (nextTickTime-usecTime);
#endif
	}
    return 0;
}
#endif


void
Qua::RemoveMethod(Lambda *p, bool andD, bool updateDisplay)
{
	Lambda		*q, **qp;
	
	if (p == nullptr) {
		return;
	}
	qp = &methods;
	q = methods;
	while (q != nullptr) {
		if (q == p) {
			*qp = q->next;
			break;
		}
		qp = &q->next;
		q = q->next;
	}
	if (updateDisplay) {
		bridge.RemoveMethodRepresentations(p->sym);
	}
	if (andD) {
		fprintf(stderr, "removed lambda %s\n", p->sym->name.c_str());
		glob.DeleteSymbol(p->sym, true);
	}
}


void
Qua::AddSchedulable(Schedulable *p)
{
	Schedulable		*q;
	q = schedulees;
	while (q != nullptr) {
		if (q == p) {
			return;
		}
		q = q->next;
	}
	
	p->next = schedulees;
	schedulees = p;
}

void
Qua::RemoveSchedulable(Schedulable *p, bool andDel, bool updateDisplay) 
{
	Schedulable		*q, **qp;
	
	if (p == nullptr) {
		return;
	}
	qp = &schedulees;
	q = schedulees;
	while (q != nullptr) {
		if (q == p) {
			*qp = q->next;
			break;
		}
		qp = &q->next;
		q = q->next;
	}
	short	cnt = p->countInstances();
	for (short i=0; i<cnt; i++) {
		Instance	*inst = p->instanceAt(0);
		p->removeInstance(inst, updateDisplay);
	}
	if (updateDisplay) {
		bridge.RemoveSchedulableRepresentations(p->sym);
	}
	if (andDel) {
		fprintf(stderr, "removed schedulable %s\n", p->sym->name.c_str());
		glob.DeleteSymbol(p->sym, true);
	}
}

std::string
Qua::nameFromLeaf(std::string t)
{
	std::string s = getBase(t);
	std::regex r("[.-]");
	return std::regex_replace(s, r, "");
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
Qua::DoSave(const char *fileName)
{
	projectScriptPath = getParent(fileName) + getBase(fileName) + ".qua";
#ifdef  QUA_V_SAVE_INITASXML
	projectSnapshotPath = getParent(fileName) + getBase(fileName) + ".qx";
#endif

	setName(getBase(fileName).c_str());
	FILE	*scriptfp = fopen(projectScriptPath.c_str(), "w");
	if (scriptfp == nullptr) {
		bridge.reportError("Can't open file '%s' for writing", projectScriptPath.c_str());
		return B_ERROR;
	}
	status_t	err = sym->SaveScript(scriptfp, 0, true, false);
	if (err != B_NO_ERROR) {
		bridge.reportError("can't save arrangement to %s", projectScriptPath.c_str());
	}
	fclose(scriptfp);
#ifdef  QUA_V_SAVE_INITASXML
	FILE	*snapfp = fopen(projectSnapshotPath.c_str(), "w");
	if (snapfp == nullptr) {
		bridge.reportError("Can't open file '%s' for writing", projectSnapshotPath.c_str());
		return B_ERROR;
	}
	fprintf(snapfp, "<?xml version = '1.0'?>\n");
//	fprintf(snapfp, "<!DOCTYPE snapshot [\n");
//		<!ENTITY author_codes SYSTEM "author_codes.xml">
//		<!ENTITY gender_codes SYSTEM "gender_codes.xml">
//		<!ENTITY rsc_codes SYSTEM "rsc_codes.xml">
//	fprintf(snapfp, "]>\n");
	fprintf(snapfp, "<snapshot script=\"%s\">\n", getLeaf(fileName).c_str());
	err = sym->SaveSnapshot(snapfp);
	fprintf(snapfp, "</snapshot>\n");
	if (err != B_NO_ERROR) {
		bridge.reportError("can't save arrangement to %s", projectSnapshotPath.c_str());
	}
	fclose(snapfp);
#endif
	return err;
}

status_t
Qua::Save(FILE *fp, short indent, bool clearHistory)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent); fprintf(fp, "qua %s\n", sym->printableName().c_str());
	tab(fp, indent); fprintf(fp, "{\n");
	if (clearHistory) {
		StabEnt	*is = glob.findContextSymbol("Init", sym, -1);
		if (is) {
			glob.DeleteSymbol(is,true);
		}
	}
	if (sym->children) {
		err=sym->children->SaveScript(fp, indent+1, true, false);
	}
	for (Schedulable *SO=schedulees; SO != nullptr; SO = SO->next) {
		if (SO->sym->type == TypedValue::S_PORT) {
			SO->sym->PortValue()->save(fp, indent+1);
		}
	}
#ifndef  QUA_V_SAVE_INITASXML
	SaveInit(fp, indent+1);
#endif
	tab(fp, indent); fprintf(fp, "}\n");
	return err;
}

#ifndef  QUA_V_SAVE_INITASXML
status_t
Qua::SaveInit(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent); fprintf(fp, "define Init()\n");
	tab(fp, indent); fprintf(fp, "{\n");
	tab(fp, indent+1); fprintf(fp, "tempo = %g\n", metric->tempo);
	for (Schedulable *SO=schedulees; SO != nullptr; SO = SO->next) {
		for (short i=0; i<SO->countInstances(); i++) {
			Instance *inst = SO->instanceAt(i);
			inst->Save(fp, indent+1);
		}
	}
	tab(fp, indent); fprintf(fp, "}\n");
	return err;
}
#endif
status_t
Qua::WriteChunk(FILE *fp, ulong type, void *data, ulong size)
{
	status_t err=B_NO_ERROR;
	if (fwrite(&type, sizeof(type), 1, fp) != 1) {
		bridge.reportError("cant write chunk type");
		return err;
	}
	off_t		sizePos = ftell(fp);
	if (fwrite(&size, sizeof(size), 1, fp) != 1) {
		bridge.reportError("cant write chunk size");
		return err;
	}
	off_t		dataStart = ftell(fp);
	switch (type) {
	case 'NAME': {
		if (fwrite(data, size, 1, fp) != 1) {
			bridge.reportError("cant write name chunk");
			return err;
		} else {
			err = B_NO_ERROR;
		}
		break;
	}
	case 'QUAP': {
		for (Schedulable *P=schedulees; P!=nullptr; P=P->next) {
			if (P->sym->type == TypedValue::S_POOL) {
				if ((err=WriteChunk(fp, 'POOL', P, 0)) != B_NO_ERROR) {
					return err;
				}
			}
		}
		break;
	}
	case 'POOL': {
		Pool	*P = (Pool *) data;
		if ((err=WriteChunk(fp, 'NAME', (void*)P->sym->name.c_str(), P->sym->name.size())) != B_NO_ERROR) {
			return err;
		}

		int32		nTakes = P->outTakes.size();
		if (fwrite(&nTakes, sizeof(nTakes), 1, fp) != 1) {
			bridge.reportError("Qua::WriteChunk() cant write take chunk");
			return err;
		} else {
			err = B_NO_ERROR;
		}

		for (short i = 0; ((unsigned)i)<P->outTakes.size(); i++) {
			if ((err=WriteChunk(fp, 'TAKE', P->outTakes[i], 0)) != B_NO_ERROR) {
				return err;
			}
		}
		break;
	}
	case 'TAKE': {
		StreamTake	*S = (StreamTake *) data;
		if ((err = WriteChunk(fp, 'NAME', (void*)S->sym->name.c_str() , S->sym->name.size())) != B_NO_ERROR) {
			return err;
		}
		if ((err=WriteChunk(fp, 'STRM', &S->stream, 0)) != B_NO_ERROR) {
			return err;
		}
		break;
	}
	case 'STRM': {
		Stream	*S = (Stream *) data;
		err = S->Save(fp, this);
		break;
	}
	default:
		bridge.reportError("bad chunk %x", type);
		return B_ERROR;
	}
	if (size == 0) {
		off_t endPos = ftell(fp);
		size = endPos - dataStart;
		fseek(fp, sizePos, SEEK_SET);
		if (fwrite(&size, sizeof(size), 1, fp) != 1) {
			bridge.reportError("cant write chunk size");
			return err;
		} else {
			err = B_NO_ERROR;
		}
		fseek(fp, endPos, SEEK_SET);
	}
	return err;
}

ulong
Qua::ReadChunk(FILE *fp, void **bufp, ulong *buf_lenp)
{
	ulong	type, size=0;
	status_t	err=B_OK;
	
	if (fread(&type, sizeof(type), 1, fp) != 1) {
		bridge.reportError("Qua: can't read chunk type");
		return 0;
	}
	if (fread(&type, sizeof(type), 1, fp) != 1) {
		bridge.reportError("Qua: can't read chunk size");
		return 0;
	}

	off_t	pos=ftell(fp);
	
	*buf_lenp = 0;
	switch (type) {
	case 'QUAP': {
		void	*data;
		ulong	datalen;
		while (ftell(fp) - pos < ((int)size)) {
			if (!ReadChunk(fp, &data, &datalen)) {
				return 0;
			}
			if (datalen) {
				delete data;
			}
		}
		break;
	}
	case 'NAME': {
		char *buf = new char[size+1];
		if (fread(buf, size, 1, fp) != 1) {
			bridge.reportError("Qua::ReadChunk() can't read name");
			return 0;
		} else {
			err = B_NO_ERROR;
		}
		
		buf[size] = 0;
		*bufp = buf;
		*buf_lenp = size;
		break;
	}
	case 'POOL': {
		char		*nm;
		ulong		len;
		
		if (ReadChunk(fp, (void**)&nm, &len)!='NAME') {
			bridge.reportError("Read chunk, expected name");
			if (len)	delete nm;
			break;
		}

		int32		nTakes = 0;
		
		if (fread(&nTakes, sizeof(nTakes), 1, fp) != 1) {
			bridge.reportError("Qua::ReadChunk() cant read stream takes");
			return err;
		} else {
			err = B_NO_ERROR;
		}

		Pool	*P = findPool(nm);
		if (P == nullptr) {
			bridge.reportError("Qua::ReadChunk() can't find pool %s", nm);
			break;
		}
		
// forcibly get rid of the current takes, which should only be
// the first, "Original"
		for (auto ci = P->outTakes.begin(); ci!=P->outTakes.end();) {
			StreamTake	*take = P->outTakes[0];
			ci = P->outTakes.erase(ci);
			delete take;
		}
		void	*data;
		ulong	datalen;

		if (nTakes == 0) {
			bridge.reportError("Pool with null takes! This looks dodgy");
			P->SelectTake(nullptr);
		} else {
			for (short i=0; i<nTakes; i++) {
				if (ReadChunk(fp, &data, &datalen) != 'TAKE') {
					bridge.reportError("Expected take data");
					if (datalen) delete data;
					break;
				}
				StreamTake		*rd = (StreamTake *) data;
				rd->sym->context = P->sym;
				P->outTakes.push_back(rd);
			}
			P->SelectTake(P->outTakes[0]);
		}
		break;
	}
	
	case 'TAKE': {
		char		*nm;
		ulong		len;
		
		if (ReadChunk(fp, (void**)&nm, &len)!='NAME') {
			bridge.reportError("Expected name");
			if (len)	delete nm;
			break;
		}
		StreamTake	*take = new StreamTake(nm, nullptr, Time::zero);
		*bufp = (void*)take;
		*buf_lenp = sizeof(StreamTake);

		void	*data;
		ulong	datalen;
		if (ReadChunk(fp, &data, &datalen) != 'STRM') {
			bridge.reportError("Expected take data");
			if (datalen) delete data;
			break;
		}
		Stream		*newstream = (Stream *)data;
	
		take->stream.AppendStream(*newstream);
		take->duration = take->stream.Duration();
		
		delete newstream;
		break;
	}
	
	case 'STRM': {
		Stream	*rd = new Stream();
		rd->Load(fp, this);
		*bufp = (void*)rd;
		*buf_lenp = sizeof(Stream);
		break;
	}
	default:
		bridge.reportError("mysterious quap");
		break;
	}
	fseek(fp, pos+size, SEEK_SET);
	return type;
}



char *
TypedValue::StringValue()
{
	static char buf[1024];
	switch (type) {
	case S_BOOL:
		if (refType != REF_VALUE)
			return "0";
		else
			return ((char *)(val.Bool?"true":"false"));
	case S_BYTE:
		if (refType != REF_VALUE)
			return "0";
		else
			sprintf(buf, "%d", val.byte);
		return buf;
	case S_SHORT:
		if (refType != REF_VALUE)
			return "0";
		else
			sprintf(buf, "%d", val.Short);
		return buf;
	case S_INT:
		if (refType != REF_VALUE)
			return "0";
		else
			sprintf(buf, "%d", val.Int);
		return buf;
	case S_LONG:
		if (refType != REF_VALUE)
			return "0";
		else
			sprintf(buf, "%lld", val.Long);
		return buf;
	case S_FLOAT:
		if (refType != REF_VALUE)
			return "0";
		else
			sprintf(buf, "%g", val.Float);
		return buf;
	case S_STRING:
		return val.string?val.string:"";
	case S_QUA:
		return (val.qua != nullptr && val.qua->sym != nullptr)?
			val.qua->sym->name.c_str():"_null_qua";
	case S_TEMPLATE:
		return (val.qTemplate != nullptr && val.qTemplate->sym != nullptr)?
			val.qTemplate->sym->name.c_str() :"null_template";
	case S_VOICE:
		return (val.voice != nullptr && val.voice->sym != nullptr)?
			val.voice->sym->name.c_str() :"_null_voice";
	case S_SAMPLE:
		return (val.sample != nullptr && val.sample->sym != nullptr)?
			val.sample->sym->name.c_str() :"_null_sample";
	case S_CHANNEL:
		return (val.channel != nullptr && val.channel->sym != nullptr)?
			val.channel->sym->name.c_str() :"_null_channel";
	case S_CLIP:
		return (val.clip != nullptr && val.clip->sym != nullptr)?
			val.clip->sym->name.c_str() :"_null_clip";
	case S_POOL:
		return (val.pool != nullptr && val.pool->sym != nullptr)?
			val.pool->sym->name.c_str() :"_null_pool";
	case S_LAMBDA:
		return (val.lambda != nullptr && val.lambda->sym != nullptr)?
			val.lambda->sym->name.c_str() :"_null_method";
	case S_INSTANCE:
		return (val.instance != nullptr && val.instance->sym != nullptr)?
			val.instance->sym->name.c_str() :"_null_instance";
	case S_EXPRESSION:
	case S_BLOCK: {
		static char	txt[10*1024];
		long	len = 0;
		Block	*block = refType>0?*val.blockP:val.block;
		
		if (!block) {
			return "";
		} else if (!(Esrap(block, txt, len, 10*1024, false, 0, false))) {
			internalError("block size too large...");
			return "";
		} else {
			return txt;
		}
		break;
	}
	case S_TIME:
		if (refType == REF_VALUE) {
			return ((Time *)&val.time)->StringValue();
//			return ((Time*)val.timeBuf)->StringValue();
		} else if (refType == REF_POINTER) {
			return val.timeP->StringValue();
		}
	default:
		internalError("TypedValue::StringValue of unexpected type %d", type);
		sprintf(buf, "%d", type);
		return buf;
	}
}

void
Qua::MetroGnomeClick()
{
	;
}

long
Qua::Rewind()
{
	return B_OK;
}

long
Qua::FastForward()
{
	return B_OK;
}

long
Qua::ToPreviousMarker()
{
	bool	cued = false;
	Time	cute;
	if (nClip() == 0) {
		cute = Time::zero;
		cued = true;
	} else {
		Clip	*gotClip = nullptr;
		fprintf(stderr, "to next marker %d\n", nClip());
		for (short i=0; i<nClip(); i++) {
			Clip	*c = regionClip(i);
			fprintf(stderr, "to clip %x\n", (unsigned)c);
			if (c != NULL) {
				if (c->start < theTime && (gotClip == nullptr || c->start > gotClip->start)) {
					gotClip = c;
				}
			}
		}
		if (gotClip != NULL) {
			cued = true;
			cute = gotClip->start;
		}
	}
	if (cued) {
		Cue(cute);
		bridge.UpdateSequencerTimeDisplay(false);
	}
	return B_OK;
}

long
Qua::ToNextMarker()
{
	if (nClip() == 0) {
		return B_OK;
	}
	Clip	*gotClip = nullptr;
	fprintf(stderr, "to next marker %d\n", nClip());
	for (short i=0; i<nClip(); i++) {
		Clip	*c = regionClip(i);
		fprintf(stderr, "to clip %x\n", (unsigned)c);
		if (c != nullptr) {
			if (c->start > theTime && (gotClip == nullptr || c->start < gotClip->start)) {
				gotClip = c;
			}
		}
	}
	if (gotClip != nullptr) {
		Cue(gotClip->start);
		bridge.UpdateSequencerTimeDisplay(false);
	}
	return B_OK;
}

void
Qua::GotoStartOfClip(StabEnt *clipSym)
{
	if (clipSym == nullptr) {
		ToNextMarker();
	}
	Clip	*c = clipSym->ClipValue(nullptr);
	if (c) {
		Cue(c->start);
		bridge.UpdateSequencerTimeDisplay(false);
	}
}

void
Qua::GotoEndOfClip(StabEnt *clipSym)
{
	if (clipSym == nullptr) {
		ToNextMarker();
	}
	Clip	*c = clipSym->ClipValue(nullptr);
	if (c) {
		Time t = c->start + c->duration;
		Cue(t);
		bridge.UpdateSequencerTimeDisplay(false);
	}
}

void
Qua::SelectRegion(StabEnt *clipSym)
{
}

long
Qua::CountEnvelopeSegments(StabEnt *, StabEnt *)
{
	return 0;
}

void
Qua::FetchEnvelopeSegments(StabEnt *, StabEnt *, long, EnvelopeSegment *)
{
	;
}

string
Qua::getVersionString()
{
	return "0.94, universal";
}

string
Qua::getCapabilityString()
{
	string caps;
#ifdef QUA_V_AUDIO
#ifdef QUA_V_AUDIO_ASIO
	caps += "asio ";
#endif
#endif
#ifdef QUA_V_DIRECT_MIDI
	caps += "dxmidi ";
#endif
#ifdef QUA_V_JOYSTICK
#ifdef QUA_V_JOYSTICK_DX
	caps += "dxjoy ";
#endif
#ifdef QUA_V_JOYSTICK_MMC
	caps += "mmcjoy ";
#endif
#endif
#ifdef QUA_V_VST
#if defined(QUA_V_VST_HOST_GUI)
	caps += "vstgui "; 
#elif defined(QUA_V_VST_HOST)
	caps += "vsthost ";
#else
#endif
#endif
#ifdef QUA_V_RAM_LOCKED_BUFFERS
#endif
#ifdef QUA_V_GDI_PLUS
#endif

	// different structural versions ...
#ifdef QUA_V_BETTER_SCHEDULER
#endif
#ifdef QUA_V_HAS_POOLPLAYER
#endif
#ifdef QUA_V_SAVE_INITASXML
#endif

	return caps;
}



QuaPort *
findQuaPort(int deviceType, const string &nm, int direction, int nports)
{
	QuaPort *p = nullptr;
	switch (deviceType) {
	case Attribute::DEVICE_AUDIO:
		p = getAudioManager().findPortByName(nm, direction, nports);
		break;
	case Attribute::DEVICE_MIDI:
		p = environment.quaMidi.findPortByName(nm, direction, nports);
		break;
	case Attribute::DEVICE_JOYSTICK:
#ifdef QUA_V_JOYSTICK
		p = environment.quaJoystick->findPortByName(nm, direction, nports);
#endif
		break;
	case Attribute::DEVICE_PARALLEL:
		p = environment.quaParallel.findPortByName(nm, direction, nports);
		break;
	case Attribute::DEVICE_OSC:
		break;
	case Attribute::DEVICE_SENSOR:
		break;
	}
	return p;
}

