
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "SymEditBlockView.h"
#include "Parse.h"
#include "Panel.h"
#include <ctype.h>
#include "Colors.h"
#include "Sym.h"
#include "Block.h"
#include "Colors.h"

SymEditBlockView::SymEditBlockView(BRect r, Qua *q):
	BlockView(r, "block", (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW|B_FRAME_EVENTS, q)
{
	SetWordWrap(false);
	SetViewColor(seablue);
}

void
SymEditBlockView::Draw(BRect r)
{
	BlockView::Draw(r);
}

void
SymEditBlockView::MouseDown(BPoint p)
{
	BlockView::MouseDown(p);
}

void
SymEditBlockView::KeyDown(const char *ca, int32 nb)
{
	long		start, fin;
	GetSelection(&start, &fin);
	BPoint ip = PointAt(start);
	bool	bsrd = false;
	
	BRect		r = Bounds();
	ulong		mods = modifiers();

	char		c = ca[0];
	switch (c) {
	
	case B_UP_ARROW:
	case B_DOWN_ARROW:
	case B_LEFT_ARROW:
	case B_RIGHT_ARROW: {
		if (mods) {
			BlockView::KeyDown(ca, nb);
		} else {
			BTextView::KeyDown(ca, nb);
		}
		break;
	}

	case B_ENTER: {
		BTextView::KeyDown(ca,nb);
		ResizeToFit();
		break;
	}
	case B_TAB: {
		BTextView::KeyDown(ca,nb);
		break;
	}
	
	case B_BACKSPACE:
	case B_DELETE: {
		bsrd = ip.x < EDEDGE+7;
		BlockView::KeyDown(ca,nb);
		break;
	}
	
	case B_INSERT:
		break;
		
	case B_END: {		// let's see if we can make sense of this garbage...
		if (Parent()) {
			BMessage	msg(COMPILE_BLOCK);
			msg.AddPointer("block view", this);
			Window()->PostMessage(&msg, Parent());
		}
		break;
	}
	
	default: {
		bsrd = ip.x > r.right - 7;
		BlockView::KeyDown(ca,nb);
		break;
	}
	}
	if (bsrd) {
		ResizeToFit();
	}
	//	ScrollTo(PointAt(start));
	ConstrainClippingRegion(nullptr);
}


void
SymEditBlockView::FrameResized(float x, float y)
{
//	fprintf(stderr, "block editor resized\n");
	BView	*V = Parent();
	if (V) {
		((Panel *)V)->ArrangeChildren();
	}
}



Block *
SymEditBlockView::BlockValue(StabEnt *v)
{
	parser->Rewind();
	parser->GetToken();
	Block	*b, **bp=&b;
	short nc = 0;
	for (;;) {
		*bp = parser->ParseBlockInfo(v);
		if (*bp == nullptr)
			break;
		nc++;
		bp = &(*bp)->next;
	}
	
	if (b == nullptr)
		return nullptr;
		
	if (b->next) {
		Block *l = new Block(C_LIST, LIST_NORM);
		l->crap.list.nChildren = nc;
		l->crap.list.block = b;
		l->crap.list.stuffVar.Set(
			S_BYTE,
			REF_STACK,
			v,
			(long)AllocStack(
				v,
				S_BYTE,
				l->crap.list.nChildren));
		b = l;
	}


	if (parser->err_cnt == 0) {
		glob.PushContext(v);
		if (b && !b->Init(nullptr)) {
			b->DeleteAll();
			b = nullptr;
		} else {
			needsCompile = false;
			DrawCompButton();
		}
		glob.PopContext();
	} else {
		if (b)
			b->DeleteAll();
		b = nullptr;
	}
	return b;
}


GraphBlockView::GraphBlockView(BRect r, Qua *q):
	SymEditBlockView(r, q)
{
	SetWordWrap(false);
}

void
GraphBlockView::Draw(BRect r)
{
	SymEditBlockView::Draw(r);
}

void
GraphBlockView::MouseDown(BPoint p)
{
	SymEditBlockView::MouseDown(p);
}

void
GraphBlockView::KeyDown(const char *ca, int32 nb)
{
	
	long		start, fin;
	GetSelection(&start, &fin);
	BPoint ip = PointAt(start);
	bool	bsrd = false;
	
	BRect		r = Bounds();
	ulong		mods = modifiers();

	char		c = ca[0];
	switch (c) {
	
	case B_END: {		// let's see if we can make sense of this garbage...
		if (Parent()) {
			BMessage	msg(COMPILE_BLOCK);
			msg.AddPointer("graph block view", this);
			Window()->PostMessage(&msg, Parent());
		}
		break;
	}
	
	default: {
		bsrd = ip.x > r.right - 7;
		SymEditBlockView::KeyDown(ca,nb);
		break;
	}
	}
	if (bsrd) {
		ResizeToFit();
	}
	//	ScrollTo(PointAt(start));
	ConstrainClippingRegion(nullptr);
}


void
GraphBlockView::FrameResized(float x, float y)
{
//	fprintf(stderr, "block editor resized\n");
	BView	*V = Parent();
	if (V) {
//		((Panel *)V)->ArrangeChildren();
	}
}



Block *
GraphBlockView::BlockValue(StabEnt *v)
{
	return SymEditBlockView::BlockValue(v);
}

