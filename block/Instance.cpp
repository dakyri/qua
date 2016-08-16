#include "qua_version.h"

#include "tinyxml2.h"
#include "Instance.h"
#include "Qua.h"
#include "QuaTime.h"
#include "Sym.h"
#include "Block.h"
#include "Lambda.h"
#include "QuasiStack.h"
#include "Sample.h"
#include "Pool.h"
#include "Channel.h"
#include "QuaPort.h"
#include "ControllerBridge.h"

#include <iostream>

status_t
Loggable::SaveLogInfo(ostream&out, short indent)
{
	status_t	err=B_NO_ERROR;
	bool		comma = false;
	out << ", ";
	log.Save(out, nullptr, STR_FMT_TEXT);
	return err;
}

status_t
Loggable::SetLogValue(Block *b)
{
	log.SetValue(b);
	return B_NO_ERROR;
}

Instance::Instance(Schedulable &s, const std::string &nm, const Time &t, const Time &d, Channel * const c)
	: Notified(s.uberQua)
	, schedulable(s)
	, startTime(t)
	, duration(d)
{
	thruEnabled = false;
	channel = c;
	status = STATUS_SLEEPING;
	wokenAt = Time::zero;
	wakeDuration = Time::zero;

	mainStack = nullptr;
	rxStack = nullptr;
	wakeStack = nullptr;
	sleepStack = nullptr;
	startStack = nullptr;
	stopStack = nullptr;
	cueStack = nullptr;
	

	uberQua = s.uberQua;
	std::string nmbuf = glob.makeUniqueName(s.sym, nm != "" ? nm : s.sym->name, 0);
	sym = DefineSymbol(nmbuf, TypedValue::S_INSTANCE, 0,
		this, s.sym,
		TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);
	//		interfaceBridge.SetSymbol(sym);
	cerr << "instance " << sym->uniqueName() << " of " << s.sym->name<< " defined at "<< startTime.StringValue() << " on " << channel->chanId << endl;

// keep a list of  the standard controllers
//	controlVariables = new QuaControllerBridge(s->uberQua, this, nullptr, s->standardControlInfo);
}


Time 
Instance::RelativeTime(Time &t)
{
	return t - startTime;
}

status_t
Instance::Run()
{
	return B_OK;
}

size_t
Instance::Generate(float **outSig, long nFramesReqd, short nAudioChannels)
{
	return nFramesReqd;
}

bool
Instance::Init()
{
	if (sym == nullptr) {
		uberQua->bridge.tragicError("hmmm ... interesting ... instance of nullptr");
	}
	glob.PushContext(sym);
	cerr << "instance: initting children\n";
	for (StabEnt *p=sym->children; p!=nullptr; p=p->sibling) {
		if (p->type == TypedValue::S_LAMBDA) {
			if (!p->LambdaValue()->Init())
				goto err_ex;
		}
	}
	cerr << "children initialized!\n";
	mainStack = nullptr;
	rxStack = nullptr;
	wakeStack = nullptr;
	sleepStack = nullptr;
	startStack = nullptr;
	stopStack = nullptr;
	cueStack = nullptr;

	if (!SetStacks())
		goto err_ex;
	cerr <<  "Instance :: stacks setup" << endl;
		
	ResetStacks();
	
	cerr << "stacks fully reset\n";

	glob.PopContext(sym);
	return true;
err_ex:
	glob.PopContext(sym);
	return false;
}

Instance::~Instance()
{
	schedulable.removeInstance(this, false);
}

void
Instance::Reset()
{
	ResetStacks();
}

bool
Instance::ResetStacks()
{
	if (schedulable.mainBlock)
		schedulable.mainBlock->Reset(mainStack);
	if (schedulable.wake.block)
		schedulable.wake.block->Reset(wakeStack);
	if (schedulable.sleep.block)
		schedulable.sleep.block->Reset(sleepStack);
	if (schedulable.rx.block)
		schedulable.rx.block->Reset(rxStack);
	if (schedulable.start.block)
		schedulable.start.block->Reset(startStack);
	if (schedulable.stop.block)
		schedulable.stop.block->Reset(stopStack);
	if (schedulable.record.block)
		schedulable.record.block->Reset(recordStack);
	return true;
}

void
Instance::Cue(Time &t)
{
}


bool
Instance::DestroyStacks()
{
	// rebuild stack, needs a sem!
	if (mainStack) {
		mainStack->Delete();
		mainStack = nullptr;
		rxStack = nullptr;
		wakeStack = nullptr;
		sleepStack = nullptr;
		cueStack = nullptr;
		startStack = nullptr;
		stopStack = nullptr;
	}
	return true;
}

bool
Instance::SetStacks()
{
	if (schedulable.uberQua == nullptr)
		return false;
		
	QuasiStack	*mainMainStack = mainStack;
	cerr <<  "main stack for " << sym->uniqueName() << endl;
	if (mainStack == nullptr) {
		mainStack = new QuasiStack(schedulable.sym, this, sym, nullptr, nullptr, schedulable.uberQua->theStack, schedulable.uberQua, nullptr);

		if (schedulable.mainBlock &&
				!schedulable.mainBlock->StackOMatic(mainStack, 3))
			return false;
			
		mainMainStack = mainStack;
	}
	
	cerr <<  "wake stack for " << sym->uniqueName() << endl;
	if (wakeStack == nullptr && schedulable.wake.block) {
		wakeStack = new QuasiStack(
							schedulable.wake.sym,
							this, sym, nullptr, nullptr, mainStack, schedulable.uberQua, "Wake");
		if (!schedulable.wake.block->StackOMatic(wakeStack, 3))
			return false;
		wakeStack->lowerFrame = mainMainStack;
	}
	
	cerr <<  "sleep stack for " << sym->uniqueName() << endl;
	if (sleepStack == nullptr && schedulable.sleep.block) {
		sleepStack = new QuasiStack(
							schedulable.sleep.sym,
							this, sym, nullptr, nullptr, mainStack, schedulable.uberQua, "Sleep");
		if (!schedulable.sleep.block->StackOMatic(sleepStack, 3))
			return false;
		sleepStack->lowerFrame = mainMainStack;
	}
	
	if (rxStack == nullptr && schedulable.rx.block) {
		rxStack = new QuasiStack(
							schedulable.rx.sym,
							this, sym, nullptr, nullptr, mainStack, schedulable.uberQua, "Receive");
		if (!schedulable.rx.block->StackOMatic(rxStack, 3))
			return false;
		rxStack->lowerFrame = mainMainStack;
	}
	return true;
}


void
Instance::Recv(Stream &s)
{
	Time	tag_time = uberQua->theTime;
	if (s.nItems > 0) {
		flag	uac = UpdateActiveBlock(
					uberQua,
					s,
					schedulable.rx.block,
					tag_time,
					this,
					sym,
					rxStack,
					1, 
					true);
	}
}

bool
Instance::SetValue(Block *b)
{
	if (b == nullptr)
		return true;
	if (b->type == Block::C_LIST) {
		Block *p=b->crap.list.block;
		if (mainStack && p) {
			mainStack->SetValue(p);
			p = p->next;
		}
		if (wakeStack && p) {
			wakeStack->SetValue(p);
			p = p->next;
		}
		if (sleepStack && p) {
			sleepStack->SetValue(p);
			p = p->next;
		}
		if (rxStack && p) {
			rxStack->SetValue(p);
			p = p->next;
		}
	} else {
		mainStack->SetValue(b);
	}
	return true;
}

status_t
Instance::SaveSnapshot(ostream &out)
{
	out << "<instance name=\""<< sym->name <<"\">" << endl;
	if (sym && sym->context) {
		StabEnt	*p = sym->context->children;
		while (p!=nullptr) {
			if (p->refType == TypedValue::REF_INSTANCE) {
				p->SaveSimpleTypeSnapshot(out, this, nullptr);
			}
			p = p->sibling;
		}
		p = sym->children;
		while (p!=nullptr) {
			if (p->refType == TypedValue::REF_INSTANCE || p->refType == TypedValue::REF_POINTER || p->refType == TypedValue::REF_INSTANCE) {
				p->SaveSimpleTypeSnapshot(out, this, nullptr);
			}
			p = p->sibling;
		}
	}
	if (mainStack) {
		mainStack->SaveSnapshot(out, sym->name.c_str());
	}
	if (rxStack) {
		rxStack->SaveSnapshot(out, "rx");
	}
	if (sleepStack) {
		sleepStack->SaveSnapshot(out, "sleep");
	}
	if (wakeStack) {
		wakeStack->SaveSnapshot(out, "wake");
	}
	if (recordStack) {
		recordStack->SaveSnapshot(out, "rec");
	}
	if (cueStack) {
		cueStack->SaveSnapshot(out, "cue");
	}
	if (startStack) {
		startStack->SaveSnapshot(out, "start");
	}
	if (stopStack) {
		stopStack->SaveSnapshot(out, "stop");
	}
	out << "</instance>" << endl;

	return B_OK;
}

status_t
Instance::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *valAttrVal = element->Attribute("value");
	const char *positionVal = element->Attribute("position");
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *encodingAttrVal = element->Attribute("encoding");

	std::string valAttr;
	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;
	int		position = 0;

	if (valAttrVal != nullptr) {
		valAttr = valAttrVal;
	}
	if (positionVal != nullptr) {
		position = atoi(valAttrVal);
	}
	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}
	if (encodingAttrVal != nullptr) {
		if (std::string(encodingAttrVal) == "base64") {
			encoding = 1;
		}
	}

	std::string namestr = element->Value();

	if (namestr =="snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr =="instance") {
		LoadSnapshotChildren(element);
	} else if (namestr =="stack") {
		if (hasNameAttr) {
			if (nameAttr ==sym->name) {
				if (mainStack) {
					mainStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="rx") {
				if (rxStack) {
					rxStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="sleep") {
				if (sleepStack) {
					sleepStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="wake") {
				if (wakeStack) {
					wakeStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="rec") {
				if (recordStack) {
					recordStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="cue") {
				if (cueStack) {
					cueStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="start") {
				if (startStack) {
					startStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr =="stop") {
				if (stopStack) {
					stopStack->LoadSnapshotElement(element);
				}
			} else {
				;
			}
		}
	} else if ((namestr =="fixed") || namestr =="envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, this, sym, nullptr);	
			}
		}
	} else {
	}

	return B_OK;
}

status_t
Instance::LoadSnapshotChildren(tinyxml2::XMLElement *element)
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

status_t
Instance::Save(ostream &out, short indent)
{
	status_t	err=B_NO_ERROR;
	out << tab(indent) << schedulable.sym->printableName();
	out << "(";
	if (channel && channel->sym){
		out << channel->sym->name;
	}
	out << ", "<< startTime.StringValue();
	out << ", " << duration.StringValue();
	SaveStackInfo(out, indent);
	SaveLogInfo(out, indent);
	out << ")" << endl;
	return err;
}

status_t
Instance::SaveStackInfo(ostream &out, short indent)
{
	status_t	err=B_NO_ERROR;
	bool		comma = false;
	out << ", {";
	if (mainStack) {
		comma=true;
		out << endl;
		mainStack->Save(out, indent+1);
	}
	if (wakeStack && wakeStack->IsInteresting()) {
		if (comma){ out << ","; } else comma=true; out << endl;
		wakeStack->Save(out, indent+1);
	}
	if (sleepStack && sleepStack->IsInteresting()) {
		if (comma){ out << ","; } else comma=true;out << endl;
		sleepStack->Save(out, indent+1);
	}
	if (rxStack && rxStack->IsInteresting()) {
		if (comma){ out << ","; } else comma=true; out << endl;
		rxStack->Save(out, indent+1);
	}
	out << "}";
	return err;
}


void
Instance::StartRecording()
{
	schedulable.status = STATUS_RECORDING;
	status = STATUS_RECORDING;
	uberQua->bridge.DisplayStatus(this, status);
}

void
Instance::StopRecording()
{
//	Stream.Coalesce();
	switch (schedulable.sym->type) {
	
	case TypedValue::S_SAMPLE: {
		Sample *sample = (Sample *) &schedulable;
		break;
	} 
	
	case TypedValue::S_POOL: {
		Pool *pool = (Pool *) &schedulable;
		((PoolInstance*)this)->loopDuration = 
				pool->recordTake->duration =
				pool->duration = pool->selectedTakeStream.EndTime();
		break;
	} 
	
	default: {
		uberQua->bridge.reportError("what a strange thing to be recording");
	}}
	uberQua->bridge.DisplayDuration(this);
//	if (controlPanel)
//		controlPanel->DisplayStatus(STATUS_RUNNING);
	status = STATUS_SLEEPING;
	schedulable.status = STATUS_RUNNING;
}

// careful some big changes may have to be made when changing channels
// or ... sometimes not
// pool data not.
// running voice not.
// sample pulled out of one channels audio list
// app stopped and started.... maybe. this could be good or bad.
status_t
Instance::SetChannel(Channel * const newchan, const bool display)
{
	if (status == STATUS_RUNNING) {
		if (channel)
			channel->RemoveReceivingInstance(this);
	}
	channel = newchan;
	if (status == STATUS_RUNNING) {
		if (channel)
			channel->AddReceivingInstance(this);
	}

	if (display) {
		uberQua->bridge.DisplayChannel(this);
	}
	return B_NO_ERROR;
}

status_t
Instance::SetDuration(const Time &t, const bool display)
{
	duration = t;
	if (display) {
		uberQua->bridge.DisplayDuration(this);
	}
	return B_NO_ERROR;
}

status_t
Instance::SetStartTime(const Time &t, const bool display)
{
	startTime = t;
	uberQua->updateInstanceSchedule(this);
	if (display) {
		uberQua->bridge.DisplayStartTime(this);
	}
	return B_NO_ERROR;
}


bool
Instance::Move(short chan_id, const Time &t, const Time &d, bool display)
{
	if (startTime != t) {
		SetStartTime(t, display);
	}
	if (duration != d) {
		SetDuration(d, display);
	}
	if (chan_id >= 0 && chan_id < uberQua->nChannel) {
		Channel	*c = uberQua->channel[chan_id];
		SetChannel(c, display);
	}
	return true;
}

