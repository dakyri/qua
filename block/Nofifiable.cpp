#include "qua_version.h"

#include "StdDefs.h"
#include "Executable.h"
#include "Qua.h"
#include "Block.h"
#include "Instance.h"
#include "QuasiStack.h"
#include "Lambda.h"
#include "QuaDisplay.h"

#include <iostream>

Notifiable::Notifiable(StabEnt *S):
	Executable(S),
	start("Start", S),
	stop("Stop", S),
	record("Record", S)
{
	cerr << "Notifiable::Notifiable()\n";
}


Notifiable::~Notifiable()
{
	cerr << "Notifiable::~Notifiable()\n";
}


void
Notifiable::Notification(Block *B, Stacker *i, StabEnt *stackCtxt, QuasiStack *s)
{
	if (B) {
		Time now = i->uberQua->theTime;
		Stream	mainStream;

		flag ua_complete = UpdateActiveBlock( i->uberQua, mainStream, B, now,
							i, stackCtxt, s, 1, true);
	
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
Notified::SaveStackInfo(ostream&out, short indent)
{
	bool		comma = false;

	if (startStack) {
		if (comma){ out << ","; } else comma=true; out << endl;
		startStack->Save(out, indent+1);
	}
	if (stopStack) {
		if (comma){ out <<","; } else comma=true; out <<endl;
		stopStack->Save(out, indent+1);
	}
	if (recordStack) {
		if (comma){ out << ","; } else comma=true;out << endl;
		recordStack->Save(out, indent+1);
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
			return false;
	}
	if (n->start.block != nullptr && !n->start.block->Reset(startStack)){
			return false;
	}
	if (n->record.block != nullptr && !n->record.block->Reset(recordStack)){
			return false;
	}

	cerr << "seq start stack..." << endl;
	if (startStack == nullptr && n->start.block) {
		startStack = new QuasiStack(n->start.sym, this, ss, nullptr, nullptr, lowerFrame, uberQua, "Start");
		if (!n->start.block->StackOMatic(startStack, 3))
			return false;
	}

	if (stopStack == nullptr && n->stop.block) {
		stopStack = new QuasiStack(n->stop.sym, this, ss, nullptr, nullptr, lowerFrame, uberQua, "Stop");
		if (!n->stop.block->StackOMatic(stopStack, 3))
			return false;
	}

	if (recordStack == nullptr && n->record.block) {
		recordStack = new QuasiStack(n->record.sym, this, ss, nullptr, nullptr, lowerFrame, uberQua, "Record");
		if (!n->record.block->StackOMatic(recordStack, 3))
			return false;
	}
	return true;
}
