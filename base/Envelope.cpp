#include "qua_version.h"

#include "StdDefs.h"


#include "Envelope.h"
#include "Sym.h"
#include "ControllerBridge.h"
#include "Instance.h"
#include "Block.h"
#include "Expression.h"
#include "Qua.h"

#include "QuaDisplay.h"


Envelope::Envelope(StabEnt *symbol,
					Metric *m,
					bool add_seg,
					float v,
					Stacker *envel,
					StabEnt *stkCtxt,
					QuasiStack *st)
{
	nSeg = 0;
	stacker = envel;
	stack = st;
	rstFlags = ENV_RST_NOT;
	segment = new EnvelopeSegment[DEFAULT_SEG];
	segSize = DEFAULT_SEG;
	sym = symbol;
	stackCtxt = stkCtxt;
	
	startTime = Time::zero;
	cueTime = Time::zero;
	cueValue = 0;
	cueSegment = -1;
	valueMetric = m;
	
	if (add_seg)
		AddSegment(Time::zero, v);

	if (stacker) {
		stacker->AddEnvelope(this);
	}
	Cue(Time::zero);
}

Envelope::~Envelope()
{
	delete [] segment;
	if (stacker) {
		stacker->RemoveEnvelope(this);
	}
}

EnvelopeSegment	*
Envelope::Segment(int i)
{
	return &segment[i];
}

void
Envelope::Start(bool fully)
{
	if (fully) {
		startTime = Time::zero;
	} else {
		startTime = cueTime;
	}
	CueAndSet(startTime, true);
}

int
Envelope::Segment(Time &t)
{
	if (nSeg==0 || segment[0].time > t)
		return -1;
	int sk;
	for (sk=0; sk < nSeg-1 && segment[sk+1].time < t; sk++) {
	}
	int		fnd=-1;
	for (short i=sk; i<nSeg; i++) {
		if (t < segment[i].time) {
			break;
		}
		fnd = i;
	}
	return fnd;
}

void
Envelope::Print(FILE *fp)
{
	for (short sk=0; sk < nSeg; sk++) {
		fprintf(fp, "%d: (%d, %g, %d)\n", sk, segment[sk].time.ticks,
							segment[sk].val, segment[sk].type);
	}
}

bool
Envelope::Cue(Time &t)
{
	segLock.lock();
	
	cueTime = t;
	Time	envTime = t - startTime;

	if (rstFlags == ENV_RST_IMMEDIATE && envTime > segment[nSeg-1].time) {
		Start(false);
		envTime = Time::zero;
	}
	
	if (  cueSegment < 0 ||
		  cueSegment > nSeg-1) {
		cueSegment = Segment(envTime);
//		fprintf(stderr, "cue absolute seg %d...\n", cueSegment); 
	} else if (segment[cueSegment].time <= envTime) {	// cue forward, maybe
		while (cueSegment < nSeg-1) {
//			fprintf(stderr, "cue forward seg %d time %d %d\n", cueSegment, segment[cueSegment+1].time.ticks, envTime.ticks); 
			if (segment[cueSegment+1].time > envTime) {
				break;
			}
			cueSegment++;
		}
	} else {									// cue backward
		while (cueSegment >= 0) {
//			fprintf(stderr, "cue backward seg %d...\n", cueSegment); 
			if (segment[cueSegment].time <= envTime) {
				break;
			}
			cueSegment--;
		}
	}
//	fprintf(stderr, "cued t %d seg %d %g...\n", cueTime.ticks, cueSegment, cueValue); 
	
	float	nueValue = cueValue;

	if (cueSegment < 0) {
		segLock.unlock();
		cueValue = nSeg?segment[0].val:0;
		return cueValue != nueValue;
	}
		
	if (cueSegment >= nSeg-1) {
		segLock.unlock();
		cueValue = segment[nSeg-1].val;
//		fprintf(stderr, "lastseg cuev %g\n", cueValue);
		return cueValue != nueValue;
	}
	
	Time	segTime = envTime - segment[cueSegment].time;
	
	switch(segment[cueSegment].type) {
	
	case ENV_SEG_TYPE_FIXED:
		cueValue = segment[cueSegment].val;
		break;

	case ENV_SEG_TYPE_LINEAR:
	default: {
		float rate = (segment[cueSegment+1].val - segment[cueSegment].val)/
				(segment[cueSegment+1].time.ticks - segment[cueSegment].time.ticks);
		cueValue = segment[cueSegment].val + rate * segTime.ticks;
		
		break;
	}
	}
//	fprintf(stderr, "cued... %g\n", cueValue);

	segLock.unlock();
	return cueValue != nueValue;
}

int	
Envelope::AddSegment(Time &t, float v, uchar type, bool draw)
{
	segLock.lock();
	if (nSeg == segSize) {
		segSize += DEFAULT_SEG;
		EnvelopeSegment		*e = new EnvelopeSegment[segSize];
		for (short i=0; i<nSeg; i++) {
			e[i] = segment[i];
		}
		delete [] segment;
		
		segment = e;
		
	}
	int 	ins_at;
	
	if (nSeg==0) {
		ins_at = 0;
	} else {

		ins_at = Segment(t) + 1;
		
		if (t == segment[ins_at].time || (ins_at > 0 && t == segment[ins_at-1].time)) {
			segLock.unlock();
			return -1;
		}

		for (short i=nSeg-1; i>=ins_at; i--) {
			segment[i+1] = segment[i];
		}
	}
	segment[ins_at].val = v;
	segment[ins_at].time = t;
	segment[ins_at].type = type;

	nSeg++;
	segLock.unlock();
	
	if (ins_at <= cueSegment) {
// cued segment may have changed, though not necessarily value.
		CueAndSet(cueTime);
	}

	/*
	if (interfaceBridge && draw) {
		interfaceBridge->DrawNewEnvSegment(this, ins_at);
	}*/

	return ins_at;
}


bool
Envelope::DelSegment(int s, bool draw)
{
	if (nSeg == 1)		// or maybe s == nSeg-1
		return false;
	segLock.lock();
	for (short i=s; i<nSeg; i++) {
		segment[i] = segment[i+1];
	}
	nSeg--;	
	segLock.unlock();
	
	if (s <= cueSegment) {
// cued segment may have changed, though not necessarily value.
		CueAndSet(cueTime);
	}

	return false;
}


bool
Envelope::MoveSegment(int i, Time &t, float v, bool draw)
{
	if (!MoveableSegment(i,t,v))
		return false;
	if (segment[i].time != t || segment[i].val != v) {	
		/*
		if (interfaceBridge && draw) {
			interfaceBridge->DrawNewEnvSegPosition(this, i, true);
		}*/	
		segment[i].time = t;
		segment[i].val = v;
		
		if (i == cueSegment || i == cueSegment+1) {
// cued segment may have changed, though not necessarily value.
			CueAndSet(cueTime);
		}

		/*
		if (interfaceBridge && draw) {
			interfaceBridge->DrawNewEnvSegPosition(this, i, false);
		}*/
	}
	return true;
}

void
Envelope::SetName(const char *nm)
{
	if (strcmp(sym->name.c_str(), nm) != 0) {
		if (sym)
			glob.rename(sym, ((char *)nm));
	}
}
bool
Envelope::MoveableSegment(int i, Time &t, float v)
{
	return !  (		i<0
				|| (i<nSeg-1 && segment[i+1].time <= t)
				|| (i>0 && segment[i-1].time>=t)
				|| (v < sym->minVal.FloatValue(nullptr))
				|| (v > sym->maxVal.FloatValue(nullptr)));
}	

status_t
Envelope::Save(FILE *fp)
{
	status_t	err=B_NO_ERROR;
	TypedValue		val=TypedValue::Int(rstFlags);
	fprintf(fp, "{");
	fprintf(fp, val.StringValue());
	for (short i=0; i<nSeg; i++) {
		TypedValue		v2=TypedValue::Float(segment[i].val);
		TypedValue		v3=TypedValue::Int(segment[i].type);
		fprintf(fp, ", {%s", segment[i].time.StringValue());
		fprintf(fp, ", %s", v2.StringValue());
		fprintf(fp, ", %s}", v3.StringValue());
	}
	fprintf(fp, "}");
	return err;
}

bool
Envelope::SetValue(Block *b)
{
	int oldnseg = nSeg;
	nSeg = 0;
	if (b->type == Block::C_LIST) {
		Block	*lst = b->crap.list.block;
		if (lst) {
			for (Block *envSeg = lst->next; envSeg != nullptr; envSeg = envSeg->next) {
				if (envSeg->type == Block::C_LIST) {
					Block	*els = envSeg->crap.list.block;
					if (els && els->Sibling(1) && els->Sibling(2)) {
						class ResultValue v0 = EvaluateExpression(els);
						class ResultValue v1 = EvaluateExpression(els->Sibling(1));
						class ResultValue v2 = EvaluateExpression(els->Sibling(2));
						Time	t(v0.IntValue(0));
						AddSegment(t,v1.FloatValue(0),(uchar)v2.IntValue(0));
					}
				} else {
					nSeg = oldnseg;
					return false;
				}
			}
		}
	}
	return true;
}

bool
Envelope::CueAndSet(Time &t, bool force)
{
	if (stack && stacker) {
		if (t != cueTime || force) {
			if (Cue(t) || force) {
//				fprintf(stderr, "%s %g\n", controlVar->sym->name, cueValue);
				LValue		lval;
				sym->SetLValue(lval, nullptr, stacker, stackCtxt, stack);
				lval.StoreFloat(cueValue);
			}
		}
		return true;
	}
	return false;
}
