#include "qua_version.h"

#include "StdDefs.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include "PoolPlayer.h"
#include "Pool.h"
#include "Qua.h"
#include "Channel.h"

// perhaps unnecessary, but ensures that all data gets played last
// in the main loop, which may be a good thing if there are editors
// adding to things...

// i think this is a defunkt class ...

PoolPlayer::PoolPlayer(Qua *q)
{
	status = STATUS_RUNNING;
	uberQua = q;
}

PoolPlayer::~PoolPlayer()
{
	fprintf(stderr, "deleted pool player\n");
}

void
PoolPlayer::Play(Time &thisTime)
{
	schlock.lock();

	for (PoolInstance *inst: instances) {
		Stream			current_out;

//		inst->Generate(current_out, thisTime);
		if (current_out.nItems > 0) {
//			uberQua->channel[inst->channel]->OutputStream(&current_out);
			current_out.ClearStream();
		}
	}
	
	schlock.unlock();
}

void
PoolPlayer::StartInstance(PoolInstance *i)
{
	schlock.lock();
	instances.push_back(i);
	schlock.unlock();

    i->lastLoopStartTime = i->startTime;
}

void
PoolPlayer::StopInstance(PoolInstance *i)
{
	schlock.lock();
	auto ci = qut::find(instances, i);
	if (ci != instances.end()) {
		instances.erase(ci);
	}
	schlock.unlock();
}



