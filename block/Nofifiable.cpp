#include "qua_version.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#endif

#include "Executable.h"
#include "Qua.h"
#include "Block.h"
#include "Instance.h"
#include "QuasiStack.h"
#include "Method.h"

#ifdef QUA_V_ARRANGER_INTERFACE
#include "QuaDisplay.h"
#endif

Notifiable::Notifiable(StabEnt *S):
	Executable(S),
	start("Start", S),
	stop("Stop", S),
	record("Record", S)
{
}


Notifiable::~Notifiable()
{
	fprintf(stderr, "Notifiable::~Notifiable()\n");
}


void
Notifiable::Notification(Block *B, Stacker *i, StabEnt *stackCtxt, QuasiStack *s)
{
	if (B) {
		Time Now = i->uberQua->theTime;
		Stream	mainStream;

		flag ua_complete = UpdateActiveBlock(
	    					i->uberQua,
	    					&mainStream,
	    					B,
	    					&Now,
							i,
							stackCtxt,
	    					s,
							1,
	    					true);
	
		mainStream.ClearStream();
	}		
}


Notified::Notified(Qua *q):
	Stacker(q)
{
	startStack = nullptr;
	stopStack = nullptr;
	recordStack = nullptr;
}

Notified::~Notified()
{
}

void
Notified::SaveStackInfo(FILE *fp, short indent)
{
	bool		comma = false;

	if (startStack) {
		if (comma){ fprintf(fp, ","); } else comma=true; fprintf(fp, "\n");
		startStack->Save(fp, indent+1);
	}
	if (stopStack) {
		if (comma){ fprintf(fp, ","); } else comma=true; fprintf(fp, "\n");
		stopStack->Save(fp, indent+1);
	}
	if (recordStack) {
		if (comma){ fprintf(fp, ","); } else comma=true; fprintf(fp, "\n");
		recordStack->Save(fp, indent+1);
	}
}


bool
Notified::SetValue(Block *b)
{
	if (startStack && b) {
		startStack->SetValue(b);
		b = b->next;
	}
	if (stopStack && b) {
		stopStack->SetValue(b);
		b = b->next;
	}
	if (recordStack && b) {
		recordStack->SetValue(b);
		b = b->next;
	}
	return true;
}
bool
Notified::Init(Notifiable *n, StabEnt *ss, QuasiStack *lowerFrame)
{
	if (n->stop.block != nullptr &&	!n->stop.block->Reset(stopStack)){
			return FALSE;
	}
	if (n->start.block != nullptr && !n->start.block->Reset(startStack)){
			return FALSE;
	}
	if (n->record.block != nullptr && !n->record.block->Reset(recordStack)){
			return FALSE;
	}

	fprintf(stderr, "seq start stack...\n");
	if (startStack == nullptr && n->start.block) {
		startStack = new QuasiStack(n->start.sym,
									this, ss, nullptr, nullptr, lowerFrame, uberQua, "Start");
		if (!n->start.block->StackOMatic(startStack, 3))
			return FALSE;
	}
//	fprintf(stderr, "seq stop stack...\n");
	if (stopStack == nullptr && n->stop.block) {
		stopStack = new QuasiStack(n->stop.sym,
									this, ss, nullptr, nullptr, lowerFrame, uberQua, "Stop");
		if (!n->stop.block->StackOMatic(stopStack, 3))
			return FALSE;
	}
//	fprintf(stderr, "seq rec stack...\n");
	if (recordStack == nullptr && n->record.block) {
		recordStack = new QuasiStack(n->record.sym,
									this, ss, nullptr, nullptr, lowerFrame, uberQua, "Record");
		if (!n->record.block->StackOMatic(recordStack, 3))
			return FALSE;
	}
	return true;
}
