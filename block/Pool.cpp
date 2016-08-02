#include "qua_version.h"

#include "tinyxml2.h"
#include "StdDefs.h"

#include "Pool.h"
#include "Sym.h"
#include "Block.h"
#include "Qua.h"
//#include "PoolPlayer.h"
#include "QuasiStack.h"
#include "Channel.h"
#include "QuaDisplay.h"

Pool::Pool(string nm, Qua *uq, bool addTake):
	Schedulable(
		DefineSymbol(nm, TypedValue::S_POOL, 0,
					this, uq->sym,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		uq,
		uq->metric)
	, selectedTakeStream(scratchStream)
{
	SelectTake(addStreamTake(string("original"), Time::zero, false));
    uberQua = uq;
	status = STATUS_RUNNING;
	
	initBlock = nullptr;
    
    PoolInstance	*i = 0;
    StabEnt			*sfs, *efs, *lcs;
    
	DefineSymbol("duration", TypedValue::S_TIME, 0, &duration, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
	DefineSymbol("mute", TypedValue::S_BOOL, 0, &i->mute, sym,
					TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);

	sfs=DefineSymbol("loopstart", TypedValue::S_TIME, 0, &i->loopStart, sym,
						TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);
	efs=DefineSymbol("loopduration", TypedValue::S_TIME, 0, &i->loopDuration, sym,
						TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);
	lcs=DefineSymbol("loopcondition", TypedValue::S_EXPRESSION, 0, &i->loopCondition, sym,
						TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);

//	sfs->SetInit(IntBaseVal(0));
//	efs->SetInit(IntBaseVal(nFrames));
	
// totally stupid hack to pass necasary context dependent stuff to parser
	lcs->SetBounds(TypedValue::Float(0.0), TypedValue::Pointer(TypedValue::S_STRANGE_POINTER, &i->stackLock));
	lcs->SetInit(TypedValue::Pointer(TypedValue::S_STRANGE_POINTER, sym));

	DefineSymbol("loopcount", TypedValue::S_INT, 0, &i->loopCount, sym,
					TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_NOT);
	recordTake = nullptr;
}


Pool::~Pool()
{
//	short cnt = outTakes.CountItems();

//	for (short i=0; i<cnt; i++) {
//		StreamTake	*s = (StreamTake *)outTakes.ItemAt(i);
//		delete s;
//	}
}

bool
Pool::SetRecordTake()
{
	if (selectedTake && selectedTake->stream.nItems == 0) {
		recordTake = selectedTake;
		return false;
	} else {
		string buf = "Record" + to_string(outTakes.size());
		recordTake = addStreamTake(buf, Time::zero, true);
	}
	return true;
}

// this should call up something to parse a midi file...
StreamTake	*
Pool::addStreamTake(std::string &nm, const Time &t, bool disp)
{
	StreamTake	*take = new StreamTake(nm, sym, t);
	outTakes.push_back(take);
	if (disp) {
		uberQua->bridge.UpdateTakeIndexDisplay(sym);
	}
	return take;
}

// this should call up something to parse a midi file...
StreamTake	*
Pool::addStreamTake(std::string &nm, const std::string &pnm, bool disp)
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
Pool::SelectTake(StreamTake *take)
{
	if (take) {
		selectedTake = take;
		selectedTakeStream = &take->stream;
		duration = take->duration;
	}
	return B_NO_ERROR;
}


status_t
Pool::DeleteTake(StreamTake *take)
{
	bool del = false;
	if (outTakes.size() <= 1)
		return B_ERROR;
	if (take == selectedTake) {
		auto ci = std::find(outTakes.begin(), outTakes.end(), take);
		if (ci != outTakes.end()) {
			outTakes.erase(ci);
			del = true;
		}

		SelectTake(outTakes[0]);
	} else {
		auto ci = std::find(outTakes.begin(), outTakes.end(), take);
		if (ci != outTakes.end()) {
			outTakes.erase(ci);
			del = true;
		}
	}
	if (del)
		delete take;
	return B_NO_ERROR;
}


bool
Pool::Init()
{
	if (sym == nullptr) {
	    uberQua->bridge.reportError("Qua: pool not found\n");
	}
	glob.PushContext(sym);
	fprintf(stderr, "Initing pool %s\n", sym->name.c_str());

	if (!Schedulable::Init())
		goto err_ex;

	if (initBlock) {
		Time		tag_time;
		
		tag_time = Time::zero;
		selectedTakeStream.ClearStream();
		QuasiStack	initStack(sym, nullptr, nullptr, nullptr, nullptr, uberQua->theStack, uberQua, nullptr);
		while (!UpdateActiveBlock(
					uberQua,
					selectedTakeStream,
					initBlock,
					tag_time,
					uberQua,
					uberQua->sym,
					&initStack,
					1,
					false)) {
			tag_time = selectedTakeStream.Duration();
		}
	}
	selectedTake->duration = duration = selectedTakeStream.Duration();
	glob.PopContext(sym);
	return true;
err_ex:
	glob.PopContext(sym);
	return false;
}

Instance *
Pool::addInstance(const std::string &nm, const Time &startt, const Time &dur, Channel *chan)
{
//	ArrangerObject *p =  new ArrangerObject(representation->label, this,
//		 chan, start,
//		 mainStream->EndTime(), tv, representation->color);
//	return p;
	PoolInstance *i = new PoolInstance(*this, nm, startt, dur, chan);
	addInstToList(i);
	if (uberQua && i) {
//		b = b->Sibling(3);
//		i->SetValue(b);
		i->Init();
		uberQua->addToSchedule(i);
//		uberQua->display.CreateInstanceBridge(i);
	} else {
		fprintf(stderr, "Schedulable: unexpected null while committing to schedule");
	}
	return i;
}

Instance *
Pool::addInstance(const string &nm, const short chan_id, const Time &t, const Time &d, const bool disp)
{
	Time	dur;
	Channel	*c;
	/*
	if (!d) {
		if (streamClip(0) == nullptr || streamClip(0)->media == nullptr) {
			dur = Time::infinity;
		}
		else {
			dur = streamClip(0)->duration;
		}
	}
	else {
		dur = d;
	}*/
	if (chan_id >= uberQua->nChannel || chan_id < 0) {
		return nullptr;
	}
	c = uberQua->channel[chan_id];
	return addInstance(nm, t, dur, c);
}

void
Pool::removeInstance(Instance *i, bool disp)
{
	PoolInstance	*p = (PoolInstance *) i;
	
#if defined(QUA_V_POOLPLAYER)
	if (p->status == STATUS_RUNNING)
		uberQua->poolPlayer->StopInstance(p);
#endif
	removeInstFromList(i);
//	uberQua->display.RemoveInstanceBridge(i);
}


void
Pool::Cue(Time &t)
{
	;
}

// called from:
//	* inside Qua main thread
status_t
Pool::Wake(Instance *i)
{
	if (i && i->status == STATUS_SLEEPING) {
		PoolInstance	*inst = (PoolInstance *)i;
#if defined(QUA_V_POOLPLAYER)
		uberQua->poolPlayer->StartInstance(inst);
#endif
#ifdef LOTSALOX
		stackableLock.Lock();
#endif
		if (wake.block) {
			Stream	mainStream;
			Time Now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua,
				mainStream,
		    	wake.block,
				Now, 
				i,
				i->sym,
		    	i->wakeStack,
				1,
				true);
			if (i->channel)
				i->channel->OutputStream(mainStream);
		    mainStream.ClearStream();
		}
#ifdef LOTSALOX
	    stackableLock.Unlock();
#endif
		return Schedulable::Wake(i);
	}
	return B_NO_ERROR;
}

// called from:
//	* inside Qua main thread
status_t
Pool::Sleep(Instance *i)
{
	if (i && i->status != STATUS_SLEEPING) {
		PoolInstance	*inst = (PoolInstance *)i;
#if defined(QUA_V_POOLPLAYER)
		uberQua->poolPlayer->StopInstance(inst);
#endif
#ifdef LOTSALOX
		stackableLock.Lock();
#endif
		if (sleep.block) {
			Stream	mainStream;
			Time Now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua,
				mainStream,
		    	sleep.block,
				Now,
				i,
				i->sym,
		    	i->sleepStack,
				1,
				true);
			if (i->channel)
				i->channel->OutputStream(mainStream);
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
Pool::Run(Instance *i)
{
	PoolInstance	*inst = (PoolInstance *)i;
	inst->wakeDuration = uberQua->theTime - inst->startTime;
	inst->UpdateEnvelopes(inst->wakeDuration);
	return B_NO_ERROR;
}
#endif

// called from:
//	* inside Qua main thread
status_t
Pool::Recv(class Stream &s)
{
	Time	tag_time = uberQua->theTime;

	for (short i=0; i<countInstances(); i++) {
		Instance	*inst = instanceAt(i);
		flag	uac = UpdateActiveBlock(
						uberQua,
						s,
						rx.block,
						tag_time,
						inst,
						inst->sym,
						inst->rxStack,
						1,
						true);
	}
	s.ClearStream();
	return B_NO_ERROR;
}


status_t
Pool::Save(ostream &out, short indent)
{
	status_t	err=B_NO_ERROR;
	out << tab(indent) << "pool";
	if (uberQua->bridge.HasDisplayParameters(sym)) {
		out << " \\display " << uberQua->bridge.DisplayParameterId() << " " << uberQua->bridge.GetDisplayParameters(sym);
	}
	out << " " << sym->printableName();
	if (countControllers() > 0) {
		out << "(";
		err = saveControllers(out, indent+2);
		out << ")";
	}
	SaveMainBlock(mainBlock, out, indent, sym->children, true, false, nullptr, nullptr); 
	return err;
}



status_t
Pool::LoadSnapshotElement(tinyxml2::XMLElement *element)
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
	} else if ((namestr == "fixed") || namestr == "envelope") {
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
Pool::LoadSnapshotChildren(tinyxml2::XMLElement *element)
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


PoolInstance::PoolInstance(Pool &p, const std::string & nm, const Time &t, const Time &d, Channel * const c):
	Instance(p,nm, t,d,c)
{
	nextItem = nullptr;
	loopStart = Time::zero;
	loopDuration = p.duration;
	loopCount = 0;
	loopCondition = new Block( Block::C_VALUE); 
	loopCondition->crap.constant.value = TypedValue::Bool(1);
	mute = false;
}

PoolInstance::~PoolInstance()
{
}

void
PoolInstance::Reset()
{
	Instance::Reset();
}

void
PoolInstance::Cue(Time &t)
{
}


status_t
PoolInstance::Run()
{
	wakeDuration = uberQua->theTime - startTime;
	UpdateEnvelopes(wakeDuration);
	
	if (schedulable.mainBlock) {
		Stream	mainStream;
		Time	tag_time = uberQua->theTime;

		////////// should be locked on mainBlock !!!
		////////// readwrite lock on stacks
		stackLock.lock();
		flag	uac = UpdateActiveBlock(
						uberQua,
						mainStream,
						schedulable.mainBlock,
						tag_time,
						this,
						sym,
						mainStack,
						1,
						true);
		stackLock.unlock();
		////////// readwrite lock on stacks

		if (channel)
			channel->OutputStream(mainStream);
		mainStream.ClearStream(); // garbage collect!!
	}
	return B_NO_ERROR;
}


/*
* TODO XXXX FIXME
* original style of the Pool had hardcoded playing of a selected pool with input loops as it's default behaviour. So
* it worked as the nearest thing to a simple midi loop. That's perhaps not a bad thing in retrospect. Currently
* these behaviours are merged into the Voice, and it would maybe be a good idea to separate these again.
* currently Pools aren't referenced in the MFC interface.
*/


#ifdef OLD_POOL_PLAY
bool
PoolInstance::LoopConditionMet(StreamItem *item)
{
	if (loopCondition == nullptr) {
		return TRUE;
	} else {
		ResultValue	v = EvaluateExpression(loopCondition, item, this, mainStack);
		return v.BoolValue(nullptr);
	}
}

void
PoolInstance::Generate(Stream &out, Time &thisTime)
{
	Time			t = thisTime-lastLoopStartTime;
	
	Pool *p = (Pool *)schedulable;
	
	// grab the tones scheduled for now and play them
	// to channel "Channel"

	if (t <= zeroTime) {
		lastLoopStartTime = thisTime;
		t = zeroTime;
		nextItem = p->IndexTo(loopStart);
	} else {
		if (t>loopDuration) {
			if (LoopConditionMet(nullptr)) {
				lastLoopStartTime = thisTime;
				t = zeroTime;
				nextItem = p->IndexTo(loopStart);
			} else {
				nextItem = nullptr;
			}
		}
	}

	if (nextItem) {
		t += loopStart;
		nextItem = p->UpForGrabs(&out, nextItem, t);
		
		if (out.nItems > 0) {
#ifdef LOTSALOX
			schedulable->stackableLock.Lock();
#endif
			if (	schedulable->mainBlock &&
					mainStack->locusStatus == STATUS_RUNNING) {
				UpdateActiveBlock(
					uberQua,
					&out,
					schedulable->mainBlock,
					1,
					&t,
					true,
					this,
					mainStack);
			}
#ifdef LOTSALOX
			schedulable->stackableLock.Unlock();
#endif
		}
	}
}

Time
Pool::UpForGrabs(class Stream *S, Time &from, Time &timeat)
{
	StreamItem *pos = mainStream->head;
	while (pos != nullptr && pos->time < from) {
		pos = pos->next;
	}
	pos =  UpForGrabs(S, pos, timeat);
	return pos? pos->time:duration;
}

StreamItem *
Pool::UpForGrabs(class Stream *S, StreamItem *pos, Time &timeat)
{
	if (status == STATUS_RUNNING)
		while (pos != nullptr && pos->time <= timeat) {
			S->AddToStream(&((StreamNote*)pos)->note, &uberQua->theTime);
			pos = pos->next;
		}
	return pos;
}

StreamItem *
Pool::IndexTo(Time &t)
{
	return mainStream->IndexTo(t);
}
#endif
