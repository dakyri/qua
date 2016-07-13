#include "qua_version.h"

/*
 * this is the original qua actor. previously, this was run in a seperate thread,
 * but this raises many scheduling and locking issues, especially under win$
 * now is as much of a stream data player/recorder as the pool class.
 * mayhaps pools should be kept on for a bit, in case a simple optimised
 * stream/midi player is needed.
 */


#include <tinyxml2.h>
#include "StdDefs.h"

#include "Qua.h"
#include "QuaEnvironment.h"
#include "Sym.h"
#include "Voice.h"
#include "Block.h"
#include "Lambda.h"
#include "Instance.h"
#include "QuasiStack.h"
#include "Channel.h"
#include "Clip.h"
#include "QuaAudio.h"
#include "QuaFX.h"
#include "SampleBuffer.h"
#include "QuaDisplay.h"

Voice::Voice(std::string nm, Qua *parent):
	Schedulable(
		DefineSymbol(nm, TypedValue::S_VOICE, 0,
				this, parent->sym,
				TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
				parent,
				parent->metric)
{
    Time		zero;
    Note		t;
    StreamNote	x(zero, t);
    
//    name = new char[strlen(nm)+1];
//    strcpy(name, nm);
//	
	
    DefineSymbol("self", TypedValue::S_VOICE, 0,
    			this, sym,
				TypedValue::REF_VALUE, true, false, StabEnt::DISPLAY_NOT);

#ifdef VOICE_MAINSTREAM
	streamSym = DefineSymbol("stream", TypedValue::S_STREAM, 0,
    						&mainStream, sym,
    						TypedValue::REF_VALUE, 0, StabEnt::StabEnt::ATTR_NONE, false, StabEnt::DISPLAY_NOT);
		
    DefineSymbol("streamcount", TypedValue::S_INT,
		&mainStream.nItems,
		sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("granularity", TypedValue::S_INT,
		&metric->Granularity,
		sym, -1, false, false, false, StabEnt::DISPLAY_NOT);
#endif	
	status = STATUS_SLEEPING;
}

Voice::~Voice()
{
	/* free up blocks used */
	status = STATUS_DEAD;
}


Clip *
Voice::AddClip(std::string nm, StreamTake *take, Time&start, Time&duration, bool disp)
{
	Clip	*c = new Clip(nm, sym);
	clips.push_back(c);
	c->Set(take, start, duration);
	if (disp) {
		uberQua->bridge.updateClipIndexDisplay(sym);
	}
	return c;
}


void
Voice::RemoveClip(Clip *c, bool disp)
{
	if (c) {
		auto ci = std::find(clips.begin(), clips.end(), c);
		if (ci != clips.end()) {
			clips.erase(ci);
		}
		if (disp) {
			fprintf(stderr, "updating it allll\n");
			uberQua->bridge.updateClipIndexDisplay(sym);
			fprintf(stderr, "updated it allll\n");
		}
		glob.DeleteSymbol(c->sym, true);
	}
}


// this should call up something to parse a midi file...
StreamTake	*
Voice::AddStreamTake(std::string nm, Time *t, bool disp)
{
	Time	time;
	if (t == nullptr) {
		time = Time::zero;
	} else {
		time = *t;
	}
	StreamTake	*take = new StreamTake(nm, sym, time);
	outTakes.push_back(take);
	if (disp) {
		uberQua->bridge.UpdateTakeIndexDisplay(sym);
	}
	return take;
}

// this should call up something to parse a midi file...
StreamTake	*
Voice::AddStreamTake(std::string nm, std::string fnm, bool disp)
{
	Time	time = Time::zero;
	StreamTake	*take = new StreamTake(nm, sym, time);
	outTakes.push_back(take);
	if (disp) {
		uberQua->bridge.UpdateTakeIndexDisplay(sym);
	}
	return take;
}

status_t
Voice::SelectTake(StreamTake *take, bool disp)
{
	if (take) {
		selectedTake = take;
//		mainStream = &take->stream;
//		duration = take->duration;
	}
	return B_NO_ERROR;
}

status_t
Voice::DeleteTake(StreamTake *take, bool disp)
{
	bool del = false;
//	if (outTakes.CountItems() <= 1)
//		return B_ERROR;
	if (take == selectedTake) {
		auto ci = std::find(outTakes.begin(), outTakes.end(), take);
		if (ci != outTakes.end()) {
			outTakes.erase(ci);
			del = true;
		}
		SelectTake(outTakes[0], false);
	} else {
		auto ci = std::find(outTakes.begin(), outTakes.end(), take);
		if (ci != outTakes.end()) {
			outTakes.erase(ci);
			del = true;
		}
	}
	if (del) {
		delete take;
		fprintf(stderr, "del ...\n");
		if (disp) {
			uberQua->bridge.UpdateTakeIndexDisplay(sym);
		}
	}
	return B_NO_ERROR;
}



bool
Voice::Init()
{
	StabEnt		*stream;

	fprintf(stderr, "Initing voice %s\n", sym->name.c_str());

#ifdef VOICE_MAINSTREAM
	mainStream.ClearStream();
#endif		
	if (sym == nullptr) {
		uberQua->bridge.reportError("Initialization: voice symbol '%s' not found\n" , sym->name.c_str());
	    return false;
	}
#ifdef VOICE_MAINSTREAM
	stream = glob.FindSymbol("stream", -1);
	if (stream == nullptr) {
		uberQua->bridge.reportError("Initializing voice %s: stream symbol not found\n", sym->name.c_str());
	    return false;
	}
#endif
	glob.PushContext(sym);
#ifdef VOICE_MAINSTREAM
	glob.PushContext(stream);
#endif
	if (!Schedulable::Init())
		goto err_ex;
#ifdef VOICE_MAINSTREAM
	glob.PopContext(stream);
#endif
	glob.PopContext(sym);
 	
	return true;
err_ex:
#ifdef VOICE_MAINSTREAM
	glob.PopContext(stream);
#endif
	glob.PopContext(sym);
  	
	return false;
	
}


void
Voice::Cue(Time &t)
{
	;
}

// called from:
//	* inside Qua main thread
status_t
Voice::Wake(Instance *i)
{
	if (i && i->status != STATUS_RUNNING) {
#ifdef LOTSALOX		
		stackableLock.Lock();
#endif
		if (wake.block) {
			Stream	mainStream;
			Time now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua, &mainStream,
		    	wake.block, now, i, i->sym,
		    	i->wakeStack, 1, true);
			if (i->channel)
				i->channel->OutputStream(&mainStream);
		    mainStream.ClearStream();
		}
#ifdef LOTSALOX		
	    stackableLock.Unlock();
#endif
#ifdef QUA_V_AUDIO
		if (i->mainStack->hasAudio != AUDIO_NONE) {
			getAudioManager().startInstance(i);
		}
#endif
		return Schedulable::Wake(i);
	}
	return B_NO_ERROR;
}

// called from:
//	* inside Qua main thread
status_t
Voice::Sleep(Instance *i)
{
	if (i && i->status != STATUS_SLEEPING) {
#ifdef LOTSALOX		
		stackableLock.Lock();
#endif
		if (sleep.block) {
			Stream	mainStream;
			Time Now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua,
				&mainStream,
		    	sleep.block,
				Now,
				i,
				i->sym,
		    	i->sleepStack, 
				1,
				true);
			if (i->channel)
				i->channel->OutputStream(&mainStream);
		    mainStream.ClearStream();
		}
#ifdef LOTSALOX		
	    stackableLock.Unlock();
#endif
		return Schedulable::Sleep(i);
	}
	return B_NO_ERROR;
}

#ifndef QUA_V_BETTER_SCHEDULER
status_t
Voice::Run(Instance *i)
{
	i->wakeDuration = uberQua->theTime - i->startTime;
	i->UpdateEnvelopes(i->wakeDuration);
	schlock.Lock();
	Time		Now = uberQua->theTime;
    bool ua_complete = UpdateActiveBlock(
    					uberQua,
    					&mainStream,
    					mainBlock, 1,
    					&Now,
    					TRUE, i,
    					i->mainStack);
	uberQua->channel[i->channel]->OutputStream(&mainStream);
	uberQua->channel[i->channel]->CheckOutBuffers();
    mainStream.ClearStream();
   	schlock.Unlock();
    return B_NO_ERROR;
}
#endif

// called from:
//	* inside Qua main thread
status_t
Voice::Recv(Stream &s)
{
	Time		tag_time = uberQua->theTime;

#ifdef LOTSALOX		
	stackableLock.Lock();
#endif
	for (short i=0; i<countInstances(); i++) {
		Instance	*inst = instanceAt(i);
		flag	uac = UpdateActiveBlock(
						uberQua,
						&s,
						rx.block,
						tag_time,
						inst,
						inst->sym,
						inst->rxStack,
						1,
						true);
#ifdef VOICE_MAINSTREAM
		mainStream.AppendStream(&s);
#endif
	}
#ifdef LOTSALOX		
	stackableLock.Unlock();
#endif
	s.ClearStream();
	return B_NO_ERROR;
}

status_t
Voice::QuaStart()
{
	Schedulable::QuaStart();
	int	nactive = 0;
	int	i = 0;
	for (i=0; i<countInstances(); i++) {
		Instance	*inst = instanceAt(i);
		if (inst->status == STATUS_RUNNING) {
			nactive++;
		}
	}
	if (nactive) {
		status = STATUS_RUNNING;
	}
	return B_NO_ERROR;
}

status_t
Voice::QuaStop()
{
	Schedulable::QuaStop();
	status = STATUS_SLEEPING;
	return B_NO_ERROR;
}

status_t
Voice::QuaRecord()
{
	Schedulable::QuaRecord();
	int	nactive = 0;
	int	i = 0;
	for (i=0; i<countInstances(); i++) {
		Instance	*inst = instanceAt(i);
		if (inst->status == STATUS_RUNNING) {
			nactive++;
		}
	}
	if (nactive) {
		status = STATUS_RUNNING;
	}
	return B_NO_ERROR;
}


status_t
Voice::Save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent);

	fprintf(fp,	"voice");
	fprintf(fp,	" %s", sym->printableName().c_str());
	if (countControllers()>0) {
		fprintf(fp, "(");
		err = saveControllers(fp, indent+2);
		fprintf(fp, ")");
	}
	SaveMainBlock(mainBlock, fp, indent, sym, true, false, nullptr, nullptr); 
	fprintf(fp, "\n");
	return err;
}


status_t
Voice::LoadSnapshotElement(tinyxml2::XMLElement *element)
{

	const char *scriptVal = element->Attribute("script");
	const char *nameVal = element->Attribute("name");
	const char *typeVal = element->Attribute("type");
	const char *takeVal = element->Attribute("take");
	const char *startVal = element->Attribute("start");
	const char *durVal = element->Attribute("duration");

	bool	hasNameAttr = false;
	bool	hasTypeAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTakeAttr = false;
	std::string	scriptAttr;
	std::string	nameAttr;
	std::string	typeAttr;
	std::string	takeAttr;
	std::string namestr;
	Time	startt = Time::zero;
	Time	durt(12);

	if (scriptVal != nullptr) {
		hasScriptAttr = true;
		scriptAttr = scriptVal;
	}
	if (nameVal != nullptr) {
		hasNameAttr = true;
		nameAttr = nameVal;
	}
	if (typeVal != nullptr) {
		hasTypeAttr = true;
		typeAttr = typeVal;
	}
	if (takeVal != nullptr) {
		hasTakeAttr = true;
		takeAttr = takeVal;
	}
	if (startVal != nullptr) {
		startt.Set(startVal);
	}
	if (durVal != nullptr) {
		durt.Set(durVal);
	}


	const char *namep = element->Value();
	namestr = namep;

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "qua") {
		LoadSnapshotChildren(element);
	} else if (namestr == "voice") {
		LoadSnapshotChildren(element);
	} else if (namestr == "instance") {
		if (hasNameAttr) {
			Instance *i = FindInstance(sym, nameAttr);
			if (i != nullptr) {
				i->LoadSnapshotElement(element);
			}
		}
	} else if (namestr == "stack") {
		LoadSnapshotChildren(element);
	} else if (namestr == "fixed" || namestr == "envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, nullptr, nullptr, nullptr);	
			}
		}
	} else if (namestr == "take") {
		if (hasNameAttr) {
			StabEnt *takeSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_TAKE, sym);
			if (takeSym != nullptr) {
				Take	*t = takeSym->TakeValue();
				StreamTake	*strt = nullptr;
				if (t->type == Take::STREAM) {
					if (!hasTypeAttr || (typeAttr == "stream")) {
						strt = (StreamTake *)t;
						strt->LoadSnapshotElement(element);
					}
				}
			}
		}
	} else if (namestr == "clip") {
		if (hasNameAttr) {
			StabEnt *clipSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_CLIP, sym);
			if (clipSym != nullptr) {
				Clip	*c = clipSym->ClipValue(nullptr);
				if (c != nullptr && hasTakeAttr) {
					StabEnt	*takeSym = FindTypedSymbolInCtxt(takeAttr, TypedValue::S_TAKE, sym);
					Take	*take = nullptr;
					if (takeSym && (take=takeSym->TakeValue())!=nullptr) {
						c->Set(take, startt, durt);
					}
				}
			}
		}
	} else {
	}

	return B_OK;
}

status_t
Voice::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}



Instance *
Voice::addInstance(std::string nm, Time t, Time d, Channel * chan)
{
	VoiceInstance *i = new VoiceInstance(this, nm, t, d, chan);
	addInstToList(i);

	if (uberQua && i) {
//		b = b->Sibling(3);
//		i->SetValue(b);
		i->Init();
		uberQua->AddToSchedule(i);
//		uberQua->display.CreateInstanceBridge(i);
	} else {
		fprintf(stderr, "Voice: unexpected null while committing to schedule");
	}
	return i;
}


Instance *
Voice::addInstance(std::string nm, short chan_id, Time *t, Time *d, bool disp)
{
	Time	at_t;
	Time	dur_t;
	Channel	*c;
	if (t == nullptr) {
		return nullptr;
	}
	at_t = *t;
	if (d == nullptr) {
		if (streamClip(0) == nullptr || streamClip(0)->media == nullptr) {
			dur_t = Time::infinity;
		} else {
			dur_t = streamClip(0)->duration;
		}
	} else {
		dur_t = *d;
	}
	if (chan_id >= uberQua->nChannel || chan_id < 0) {
		return nullptr;
	}
	c = uberQua->channel[chan_id];
	return addInstance(nm, at_t, dur_t, c);
}


VoiceInstance::VoiceInstance(Voice *s, std::string nm, Time t, Time d, Channel * c):
	Instance(s, nm, t, d, c)
{
}

VoiceInstance::~VoiceInstance()
{
}

#ifdef QUA_V_BETTER_SCHEDULER
status_t
VoiceInstance::Run()
{
//	fprintf(stderr, "run voice\n");
	wakeDuration = uberQua->theTime - startTime;
	UpdateEnvelopes(wakeDuration);
	if (schedulable->mainBlock) {
		Stream	mainStream;

		////////// should be locked on mainBlock !!!
		////////// readwrite lock on stacks
		stackLock.lock();

		flag ua_complete = UpdateActiveBlock(
    						uberQua,
    						&mainStream,
    						schedulable->mainBlock,
    						uberQua->theTime,
							this,
							sym,
    						mainStack, 1,
    						true);

		stackLock.unlock();
		////////// readwrite lock on stacks

		if (channel) {
			channel->OutputStream(&mainStream);
			channel->CheckOutBuffers();
		}
		mainStream.ClearStream();
	}
    return B_NO_ERROR;
}
#endif

size_t
VoiceInstance::Generate(float **outSig, long nFramesReqd, short nAudioChannels)
{
	sample_buf_zero(outSig, nAudioChannels, nFramesReqd);
	
#ifdef LOTSALOX
	schedulable->stackableLock.Lock();
#endif
	if (schedulable->mainBlock && mainStack->locusStatus == STATUS_RUNNING) {
		ApplyQuaFX(mainStack, schedulable->mainBlock, outSig, nFramesReqd, nAudioChannels);
	}
#ifdef LOTSALOX
	schedulable->stackableLock.Unlock();
#endif

//	fprintf(stderr, "@os[0] %g oframe %d %d-%d\n", outSig[0], outFrame, startFrame.ticks, endFrame.ticks);
	
// now in stereo

	return nFramesReqd;
}
