#include "qua_version.h"

#include "StdDefs.h"



#include "Sym.h"
#include "Executable.h"
#include "Instance.h"
#include "Block.h"
#include "Lambda.h"
#include "Qua.h"
#include "ControllerBridge.h"
#include "Envelope.h"
#include "Channel.h"
#include "Schedulable.h"

#include "QuaDisplay.h"

Schedulable::Schedulable(StabEnt *s, Qua *q, class Metric *m):
	Notifiable(s),
	wake("Wake", s),
	sleep("Sleep", s),
	rx("Rx", s),
	cue("Cue", s),
	start("Start", s),
	stop("Stop", s),
	record("Record", s),
	init("Init", s)
{
	uberQua = q;
	metric = m;

//	insertableBridge.SetSymbol(s);
	mainBlock = nullptr;
	
	status = STATUS_SLEEPING;
	
	Instance	*i=0;

	chanSym = DefineSymbol("chan", TypedValue::S_CHANNEL, 0,
			(char*)&i->channel, sym, TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);
	starttSym = DefineSymbol("starttime", TypedValue::S_TIME, 0,
			(char*)&i->startTime, sym, TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);
	durSym = DefineSymbol("duration", TypedValue::S_TIME, 0,
			(char*)&i->duration, sym, TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_CTL);


    DefineSymbol("wakedurclock", TypedValue::S_INT, 0,
    		((char *)&i->wakeDuration.ticks), sym,
    		TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_NOT);
    DefineSymbol("wakeatclock", TypedValue::S_INT, 0,
    		((char*)&i->wokenAt.ticks), sym,
    		TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_NOT);

}

Schedulable::~Schedulable()
{
}

void
Schedulable::SetName(char *nm)
{
	if (strcmp(sym->name.c_str(), nm) != 0) {
		if (sym) {
			glob.rename(sym, nm);
		}
		uberQua->bridge.Rename(sym, nm);			
	}
}


bool
Schedulable::Init()
{
	for (StabEnt *p=sym->children; p!=nullptr; p=p->sibling) {
		if (p->type == TypedValue::S_LAMBDA) {
			p->LambdaValue()->Init();
		}
	}

	if (mainBlock && !mainBlock->Init(this)) {
			return false;
	}

	for (auto ci: instances) {
		if (!ci->Init()) {
			return false;
		}
	}
/*
not sure where this should be or why ... for instance creation?
but what about recalling stream data?
otherwise once for each created instance ... ????????!!!!!!!!!!
*/
/*
	Lambda		*initMethod = FindMethod("Init", -1, false);
    Block	*initBlock = initMethod->mainBlock;
    if (initBlock) {
		fprintf(stderr, "Found initializations...\n");
    	
		Stream	mainStream;

		QuasiStack	*initStack = new QuasiStack(
							initMethod->sym,
							nullptr, nullptr, nullptr, nullptr, this, nullptr);
		if (  initBlock->StackOMatic(initStack, 1) &&
			  initBlock->Reset(initStack)) {
			initStack->lowerFrame = uberQua->theStack;
			
			fprintf(stderr, "Executing Init lambda of %s...\n", sym->name);
			
			bool ua_complete = UpdateActiveBlock(
		    					uberQua,
		    					&mainStream,
		    					initBlock,1,
		    					&uberQua->theTime,
		    					true, this,
		    					initStack);
		}	
		mainStream.ClearStream();
	}
*/
	return true;
}

Instance *
Schedulable::addInstance(string nm, Time t, Time d, Channel * chan)
{
	Instance *i = new Instance(this, nm, t, d, chan);
	addInstToList(i);

	if (uberQua && i) {
//		b = b->Sibling(3);
//		i->SetValue(b);
		i->Init();
		uberQua->AddToSchedule(i);
//		uberQua->display.CreateInstanceBridge(i);
	} else {
		fprintf(stderr, "Schedulable: unexpected null while committing to schedule");
	}
	return i;
}

Instance *
Schedulable::addInstance(string nm, short chan_id, Time *t, Time *d, bool disp)
{
	Time	at_t;
	Time	dur_t;
	Channel	*c;
	if (t == nullptr) {
		return nullptr;
	}
	at_t = *t;
	if (d == nullptr) {
		dur_t = Time::infinity;
	} else {
		dur_t = *d;
	}
	if (chan_id >= uberQua->nChannel || chan_id < 0) {
		return nullptr;
	}
	c = uberQua->channel[chan_id];
	return addInstance(nm, at_t, dur_t, c);
}

void
Schedulable::removeInstance(Instance *i, bool display)
{
	removeInstFromList(i);
	if (uberQua && i) {
		uberQua->RemoveFromSchedule(i);
		if (display) {
			uberQua->bridge.RemoveInstanceRepresentations(i->sym);
		}
	}
}

bool
Schedulable::Trigger()
{
	if (uberQua->status != STATUS_RUNNING) {
		uberQua->Start();
	}
	addInstance(sym->name, uberQua->theTime, Time::infinity, uberQua->channel[0]);

	return true;
}

bool
Schedulable::UnTrigger()
{
	if (!(	status == STATUS_DEAD ||
			status == STATUS_SLEEPING)) {
//		Sleep((Instance*)instances.ItemAt(0));
		switch(sym->type) {
		case TypedValue::S_POOL:
		case TypedValue::S_SAMPLE:
		case TypedValue::S_VOICE: {
			for (Instance *p: instances) {
				if (	p->startTime < uberQua->theTime &&
						p->startTime+p->duration > uberQua->theTime) {
					Time	dur = uberQua->theTime - p->startTime;
					p->SetDuration(dur, true);
				}
			}
			break;
		}
		}
	}
	return true;
}

status_t
Schedulable::Schedule(Block *b)
{
	status_t	err = B_NO_ERROR;
   	Instance	*i=nullptr;
   	ResultValue		v0 = EvaluateExpression(b);
   	ResultValue		v1 = EvaluateExpression(b->Sibling(1));
   	ResultValue		v2 = EvaluateExpression(b->Sibling(2));
	fprintf(stderr, "scheduling %s: ", sym->name.c_str());
	fprintf(stderr, "on %s ", v0.StringValue());
	fprintf(stderr, "at %s ", v1.StringValue());
	fprintf(stderr, "for %s\n", v2.StringValue());
	Channel *c=nullptr;
	if (v0.type == TypedValue::S_CHANNEL) {
		c = (Channel *)v0.PointerValue(nullptr);
	} else {
		short chid = v0.IntValue(nullptr);
		if (chid >= 0 && chid < uberQua->nChannel) {
			c = uberQua->channel[chid];
		}
	}
	i = addInstance(sym->name, v1.type == TypedValue::S_TIME?
						*v1.TimeValue():
						Time(v1.IntValue(nullptr), uberQua->metric),
					v2.type == TypedValue::S_TIME?
						*v2.TimeValue():
						Time(v2.IntValue(nullptr), uberQua->metric),
					c);
	fprintf(stderr, "instance created and added!!!\n");
	fprintf(stderr, "scheduled!!!\n");
	return err;
}

#ifndef QUA_V_BETTER_SCHEDULER
void
Schedulable::Update(Time t)
{
	short	nWake=0;
	
	if (instances.CountItems() == 0)
		return;

	instances.Lock();
	for (short i=0; i<instances.CountItems(); i++) {
		Instance	*inst = (Instance *)instances.ItemAt(i);
		if (	t>= inst->startTime
				&& t <= inst->startTime + inst->duration
				&& inst->status != STATUS_RUNNING) {
			Wake(inst);
		} else if (	(t< inst->startTime
				|| t >= inst->startTime + inst->duration)
				&& inst->status == STATUS_RUNNING) {
			Sleep(inst);
		}
		if (inst->status == STATUS_RUNNING) {
			nWake++;
			Run(inst);
		}
	}
	
	if (nWake == 0) {
		;
	}
	instances.Unlock();
}

#endif

status_t
Schedulable::Wake(Instance *i)
{
	if (i->status != STATUS_RUNNING && i->status != STATUS_RECORDING) {
		if (i->channel)
			i->channel->AddReceivingInstance(i);		

		i->Reset();	
		i->wokenAt = uberQua->theTime;
		i->StartEnvelopes();
		i->status = STATUS_RUNNING;
		uberQua->bridge.DisplayWake(i);
	}
	return B_NO_ERROR;
}

status_t
Schedulable::Sleep(Instance *i)
{
	if (i->status != STATUS_SLEEPING) {
		fprintf(stderr, "locking inst\n");

		fprintf(stderr, "removing receiving inst\n");
		if (i->channel)
			i->channel->RemoveReceivingInstance(i);		

		i->status = STATUS_SLEEPING;
		uberQua->bridge.DisplaySleep(i);
	}
	return B_NO_ERROR;
}


status_t
Schedulable::QuaStart()
{
	if (start.block) {
		instanceLock.lock();
		for (Instance *inst: instances) {
			if (inst->status == STATUS_RUNNING) {
                Notification(start.block, inst, inst->sym, inst->stopStack);
			}
		}
		instanceLock.unlock();
	}
	return B_NO_ERROR;
}

status_t
Schedulable::QuaStop()
{
	if (stop.block) {
		instanceLock.lock();
		for (Instance *inst: instances) {
			if (inst->status == STATUS_RUNNING) {
				Notification(stop.block, inst, inst->sym, inst->stopStack);
			}
		}
		instanceLock.unlock();
	}
	return B_NO_ERROR;
}

status_t
Schedulable::QuaRecord()
{
	if (record.block) {
		instanceLock.lock();
		for (Instance *inst: instances) {
			if (inst->status == STATUS_RUNNING) {
				Notification(record.block, inst, inst->sym, inst->recordStack);
			}
		}
		instanceLock.unlock();
	}
	return B_NO_ERROR;
}

