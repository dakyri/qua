#include "Sym.h"
#include "Colors.h"
#include "messid.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "Source.h"
#include "QuaQua.h"
#include "StdDefs.h"
#include "Block.h"
#include "SourceMenu.h"
#include "FilterBlockMenu.h"
#include "FilterBlockCtl.h"
#include "Stream.h"
#include "RWLock.h"
#include "QuasiStack.h"
#include "ControlVar.h"
#include "InstancePanel.h"

FilterBlockCtlView::FilterBlockCtlView(char *nm, StabEnt *Sc, class Block *V,
			QuaQua *Q, BRect rect,
			RWLock *lock, short mode,
			Stream *S, QuasiStack *N)
	: ControlPanel(rect, rect.right - rect.left, "filter",
				CP_TYPE_ONOFF, 
				"Bang", nm, 0)
{
	fcMode = mode;
	theBlock = V;
	theThing = Sc;
	theBlockLock = lock;
	uberQua = Q;
	theStream = S;
	theStack = N;
	BMenu		*add = new SourceMenu(this, uberQua);
	BMenu		*del = new FilterBlockMenu(theBlock, this, uberQua);
	if (theBlock) {	// assume some kind of list
		
		for (short i=0; i<theStack->higherFrame.CountItems(); i++) {
			QuasiStack	*S =
				(QuasiStack*)theStack->higherFrame.ItemAt(i);
			if (Window()) Window()->Lock();
			BRect	B1 = Bounds();
			FramePanel	*NV = new FramePanel(
						BRect(100, B1.bottom, 300, B1.bottom+25),
						200,
						S, Q);
			AddChild(NV);
			B1 = NV->Bounds();
			BRect B = NV->Bounds();
			float w = NV->Frame().right - B1.right;
			ResizeBy(w>0?w:0, B.bottom);
			if (Window()) Window()->ResizeBy(w>0?w:0, B.bottom);
			for (BView *q=NextSibling();
							q!= nullptr; q=q->NextSibling()) {
				q->MoveBy(0, B.bottom);
			}
			if (Window()) Window()->Unlock();
		}
	}
}

void
FilterBlockCtlView::MessageReceived(BMessage *msg)
{
	Block		*callBlock=nullptr;	

	if (msg->HasPointer("call")) {
		msg->FindPointer("call", &callBlock);
	}
	if (msg->what == CTRL_BANG) {
		if (callBlock) {
			Source		*S = callBlock->Crap.Call.Crap.Source;
			if (S->mainBlock)
				S->mainBlock->Reset(nullptr);
		}
		if (theStream) {		
			Time		tag_time;

			theBlockLock->RdLock();
			tag_time = zeroTime;

			bool ua = UpdateActiveBlock(
						uberQua,
						theStream,
						theBlock,
						1,
						&tag_time,
						FALSE, 0,
						theStack);
			theBlockLock->RdUnlock();
			if (fcMode = FC_MODE_CLEAR) {
				for (short i=0; i<theBlock->Crap.List.NChildren; i++) {
					DelFilterBlock(0);
				}
			}
	
		}
//	} else if (msg->what == CTRL_STOP) {
////		myApplicationBlock->UnTrigger();
	} else if (msg->what == ADD_ITEM) {
		Source *S;
		msg->FindPointer("obj", &S);
		
		AddFilterBlock(S);
	} else if (msg->what == DEL_ITEM) {
		int		which = msg->FindInt32("ind");
		
		DelFilterBlock(which);
	} else if (msg->what == PARAM_SET) {
		ControlVar *p;
		status_t err = msg->FindPointer("control var", &p);
		if (p != nullptr) {
			p->SetSymbolValue();
		}
	} else {
		BView::MessageReceived(msg);
	}
}


void	
FilterBlockCtlView::DelFilterBlock(short which)
{
#ifdef CHAOTIC
	Locus	*p = nodeList->List[which];
	LocusCtlView	*q = p->ControlPanel;
	
	theBlockLock->Lock();
	class Block	**bp = &theBlock->Crap.List.Block;
	class Block	*b;
	for (b=theBlock->Crap.List.Block; b!=nullptr; b=b->Next) {
		if (b->Crap.Call.LocusNo == which)
			break;
		bp = &b->Next;
	}
	if (b) {
		class Block	*toDel = b;
		theBlock->Crap.List.NChildren--;
		nodeList->RemoveItem(which);
		*bp = toDel->Next;
		if (fcMode == FC_MODE_COPY && toDel->Crap.Call.Crap.Call.Atom->Crap.Sym->SourceValue())
			delete toDel->Crap.Call.Crap.Call.Atom->Crap.Sym->SourceValue();
		delete toDel;
		for (b=theBlock->Crap.List.Block; b!= nullptr; b=b->Next) {
			if (b->Crap.Call.LocusNo > which)
				b->Crap.Call.LocusNo--;
		}
		Window()->Lock();
		BRect	B = q->Bounds();
		for (BView *v=q->NextSibling();
					v!= nullptr; v=v->NextSibling()) {
			v->MoveBy(0, -B.bottom);
		}
		RemoveChild(q);
		delete q;
		delete p;
		ResizeBy(0, -B.bottom);
		
		Window()->ResizeBy(0, -B.bottom);
		for (BView *v=NextSibling();
						v!= nullptr; v=v->NextSibling()) {
			v->MoveBy(0, -B.bottom);
		}
		Window()->Unlock();
	}
	theBlockLock->Unlock();
#endif
}

void
FilterBlockCtlView::AddFilterBlock(Source *S)
{
#ifdef CHAOTIC
	if (fcMode == FC_MODE_COPY) {
		StabEnt *n = DupSymbol(S->Sym, theThing);
		S = n->SourceValue();
		Glob.PushContext(theThing);
		S->Init();
		Glob.PopContext();
		S->isInit = TRUE; //????????????? slack
	}
	
	class Block	*p = new class Block(C_CALL);
	theBlockLock->Lock();
	p->Next = theBlock->Crap.List.Block;
	theBlock->Crap.List.Block = p;
	theBlock->Crap.List.NChildren++;
	if (theBlock->Crap.List.NChildren >= MAX_FILTER) {
		reportError("panic: too many filters");
	}
	p->Crap.Call.Crap.Call.Atom = new class Block( C_SYM);
	p->Crap.Call.Crap.Call.Atom->Crap.Sym = S->Sym;
	p->Crap.Call.Label = nullptr;
	p->Crap.Call.LocusNo = theBlock->Crap.List.NChildren-1;
	p->Crap.Call.Status = STATUS_RUNNING;
	p->Crap.Call.Params = nullptr;
	p->Crap.Call.Crap.Call.Block = S->mainBlock;
	theBlockLock->Unlock();
	
	Locus	*N;
	N = new Locus(S, p, theBlock);
	nodeList->AddItem(N);
	Window()->Lock();
	BRect	B1 = Bounds();
	LocusCtlView	*NV = new LocusCtlView(N,
				BRect(100, B1.bottom, 300, B1.bottom+25));
	AddChild(NV);
	BRect B = NV->Bounds();
	float w = NV->Frame().right - B1.right;
	ResizeBy(w>0?w:0, B.bottom);
	Window()->ResizeBy(w>0?w:0, B.bottom);
	for (BView *q=NextSibling();
					q!= nullptr; q=q->NextSibling()) {
		q->MoveBy(0, B.bottom);
	}
	Window()->Unlock();
#endif
}

FilterBlockCtlView::~FilterBlockCtlView()
{
}