#include "qua_version.h"


#include "Envelope.h"
#include "Stacker.h"

Stacker::Stacker(Qua *q)
{
	uberQua = q;
	status = STATUS_SLEEPING;
}

Stacker::~Stacker()
{
}

Time
Stacker::RelativeTime(Time &t)
{
	return t;
}

void
Stacker::AddEnvelope(Envelope *e)
{
	envelopes.push_back(e);
}

void
Stacker::RemoveEnvelope(Envelope *e)
{
	for (auto ci = envelopes.begin(); ci != envelopes.end(); ++ci) {
		if (*ci == e) {
			envelopes.erase(ci);
			return;
		}
	}
}

Envelope*
Stacker::EnvelopeAt(int i)
{
	return i>=0 && ((unsigned)i)<envelopes.size()? envelopes[i]:nullptr;
}

int
Stacker::NEnvelopes()
{
	return envelopes.size();
}

void
Stacker::StartEnvelopes()
{
	for (auto si: envelopes) {
		si->Start(true);
	}
}

void
Stacker::LoopEnvelopes()
{
	for (auto e: envelopes) {
		if (e->rstFlags == ENV_RST_LOOP_START)
			e->Start(false);
	}
}

void
Stacker::UpdateEnvelopes(Time &t)
{
	for (auto e: envelopes) {
		e->CueAndSet(t);
	}
}
