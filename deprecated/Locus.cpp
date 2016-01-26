#include "Locus.h"
#include "Block.h"
#include "Source.h"
#include "VoiceController.h"
#include "QuaQua.h"

Locus::Locus(class Source *S, Block *B, Block *L)
{
	Source = S;
	CallingBlock = B;
	EnclosingList = L;
	ControlPanel = nullptr;
	Status = S->Status;
	FormalParamList *fp = first_unpassed_param(this);
	if (fp)
		Params = new ControlVar(fp);
	else
		Params = nullptr;
	subLocus = nullptr;
	uberLocus = nullptr;
}

Locus::~Locus()
{
	if (Params)
		delete Params;
	// ??? delete subLocus... could cause multiple deletes: ooops.
}

Locus::Locus()
{
	Source = nullptr;
	CallingBlock = nullptr;
	EnclosingList = nullptr;
	ControlPanel = nullptr;
	Status = STATUS_UNKNOWN;
	Params = nullptr;
	subLocus = nullptr;
	uberLocus = nullptr;
}


// ???? could thios cause probs with the BControls of the contyrol
// vars
Locus::Locus(Locus *n)
{
	Source = n->Source;
	CallingBlock = n->CallingBlock;
	EnclosingList = n->EnclosingList;
	ControlPanel = n->ControlPanel;
	Status = n->Status;
	if (n->Params)
		Params = new ControlVar(n->Params);
	else
		Params = nullptr;
	subLocus = n->subLocus;		// this should & could be a
								// recursive call, but...
								// CAREFULLY!
	uberLocus = n->uberLocus;
}

bool
Locus::Trigger()
{
	if (Source->isInit) {
		if (Source->MainBlock)
			Source->MainBlock->Reset();
	}
	Status = CallingBlock->Crap.Call.Status = STATUS_RUNNING;
	if (Source->isModal) {	// turn off siblings
		LocusList	*n = uberLocus->subLocus; // must be non null
											// I exist!
		for (short i=0; i<n->Count; i++) {
			if (n->List[i] != this
					&& n->List[i]->EnclosingList == EnclosingList)
				n->List[i]->UnTrigger();
		}
	}
	if (ControlPanel)
		ControlPanel->DisplayStatus(Status);
	return TRUE;
}

bool
Locus::UnTrigger()
{
	Status = CallingBlock->Crap.Call.Status = STATUS_PAUSED;
	if (ControlPanel)
		ControlPanel->DisplayStatus(Status);
	return TRUE;
}

LocusList::LocusList()
{
	for (short i=0; i<MAX_NODES; i++)
		List[i] = nullptr;
		
	Count = 0;
}

void
LocusList::AddItem(Locus *p)
{
	List[Count++] = p;
}

void
LocusList::RemoveItem(short which)
{
	Count--;
	for (short i=which; i<Count; i++)
		List[i] = List[i+1];
}