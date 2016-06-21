#include "Block.h"
#include "SymEditBlockView.h"
#include "Colors.h"
#include "BlockDrawBits.h"
#include "QuasiStack.h"


BlockGraph::BlockGraph(BRect r, Stackable *s, Qua *uq):
	BControl(r, "", "", nullptr, B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(seablue);
	qua = uq;
	block = nullptr;
	stackable = s;
}

void
BlockGraph::SetViewColor(rgb_color c)
{
	BControl::SetViewColor(c);
}

Block *
BlockGraph::BlockValue(StabEnt *s)
{
	return block;
}

void
BlockGraph::SetValue(Block *b)
{
	block = b;
	if (block) {
		SetupDisplay(block, 2, 2, LIST_NORM, BLOCK_DISPLAY_OPEN);
		BRect r = block->displayInfo.rect;
		ResizeTo(r.right - r.left, r.bottom - r.top);
	} else {
		ResizeTo(10, 10);
	}
}


void
BlockGraph::MouseDown(BPoint p)
{
// find innermost graphic unit

	Block	*b = MouseDownBlock(block, p);

// single click
// double click
}

void
BlockGraph::Draw(BRect r)
{
	SetHighColor(black);
	DrawBlock(block, r, LIST_NORM);
}

void
BlockGraph::DrawBlock(Block *b, BRect r, short kind)
{
	if (b->displayInfo.mode == BLOCK_DISPLAY_CLOSE) {
		return;
	}
		
	if (b->next && b->next->displayInfo.rect.Intersects(r)) {
		DrawBlock(b->next, r, b->subType);
	}
	switch (b->type) {
	
// language structures
	case C_FLUX:
		break;
	case C_OUTPUT:
		break;
	case C_IF:
		break;
	case C_GUARD:
		break;
	case C_ASSIGN:
		break;
	case C_BREAK:
		break;
	case C_WAKE:
		break;
	case C_SUSPEND:
		break;
	case C_INPUT:
		break;
	case C_DIVERT:
		break;
	case C_STATE:	// state machine
		break;
	case C_WAIT:
		break;
	case C_WITH	:
		break;
	case C_FOREACH	:
		break;
	case C_REPEAT:
		break;	
// List elements
	case C_LIST	: {
			Block	*q;
			q = b->crap.list.block;
			switch(b->subType) {
			
			case LIST_SEQ: {
					BRect	rect = b->displayInfo.rect;
					
					BPoint	lp(rect.left, (rect.bottom + rect.top)/2);
					BPoint	rp;
					while (q != nullptr) {
						BRect qr = q->displayInfo.rect;
						rp.Set(qr.left, (qr.bottom + qr.top)/2);
						StrokeLine(lp, rp);
						DrawArrowHead(this, lp, rp, 0.5);
						lp.Set(qr.right, (qr.bottom + qr.top)/2);
						
						q = q->next;
					}
					rp.Set(rect.right, (rect.bottom + rect.top)/2);
					DrawArrowHead(this, lp, rp, 0.5);	
				}
				break;
				
			case LIST_FORK: {
					BRect	rect = b->displayInfo.rect;
					
					BPoint	lp(rect.left, (rect.bottom + rect.top)/2);
					BPoint	rp(rect.right, (rect.bottom + rect.top)/2);
					BPoint	lq, rq;
					while (q != nullptr) {
						BRect qr = q->displayInfo.rect;
						lq.Set(qr.left, (qr.bottom + qr.top)/2);
						rq.Set(qr.right, (qr.bottom + qr.top)/2);
						DrawArrowHead(this, lp, lq, 0.5);
						StrokeLine(lp, lq);
						DrawArrowHead(this, rq, rp, 0.5);
						StrokeLine(rq, rp);
						
						q = q->next;
					}
					rp.Set(rect.right, (rect.bottom + rect.top)/2);
				}
				break;
				
			case LIST_NORM:
			case LIST_PAR:
			default: {
					BRect	rect = b->displayInfo.rect;
					
					BPoint	lp((rect.left+rect.right)/2, rect.top);
					BPoint	rp;
					while (q != nullptr) {
						BRect qr = q->displayInfo.rect;
						rp.Set((qr.left + qr.right)/2, qr.top);
						StrokeLine(lp, rp);
						DrawArrowHead(this, lp, rp, 0.5);
						lp.Set((qr.right + qr.left)/2, qr.bottom);
						q = q->next;
					}
					rp.Set((rect.right+rect.left)/2, rect.bottom);
					StrokeLine(lp, rp);
					DrawArrowHead(this, lp, rp, 0.5);	
				}
				break;
			}
		}
		break;
		
// atomic operations
	case C_VALUE:
	case C_SYM:
	case C_ARRAY_REF:
	case C_NAME:
	case C_STRUCTURE_REF:

// operators
	case C_UNOP	:
	case C_BINOP:
	case C_TEROP:
	case C_CAST	:

	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_BUILTIN:

	case C_AUDIO_ADDON:
	case C_UNLINKED_CALL:
	case C_UNKNOWN:
	case C_CALL	:

	default: {
			if (b->displayInfo.view) {
				StrokeRect(b->displayInfo.view->Frame());
			} else {
				StrokeRect(b->displayInfo.rect);
			}
		}
		break;
	}
}


void
BlockGraph::AttachedToWindow()
{
}

void
BlockGraph::SetupDisplay(Block *b, float x, float y, short listkind, short dmode)
{

	if (dmode == BLOCK_DISPLAY_TEXT) {
			if (b->displayInfo.view == nullptr) {
				BlockView *bv = new GraphBlockView(BRect(0,0,20,20), qua);
				b->displayInfo.view = bv;
				b->displayInfo.mode = BLOCK_DISPLAY_CLOSE;
				bv->SetValue(b);
				bv->ResizeToFit();
				bv->MoveTo(x, y);
				AddChild(bv);
			}
			BRect r = b->displayInfo.view->Frame();
			r.right += 3;
			r.bottom += 3;
			b->displayInfo.rect = r;
			return;
	}
	
	switch (b->type) {

// List elements
	case C_LIST	: {
			Block	*q;
			q = b->crap.list.block;
			fprintf(stderr, "a %d list %x\n", b->subType, q);
			switch(b->subType) {
			
			case LIST_SEQ: {
//					fprintf(stderr, "seq list\n");
					float minbot=y;
					float xi = x + 5;
					while (q != nullptr) {
						Block	*qn = q->next;
						q->next = nullptr;
						SetupDisplay(q, xi, 5, b->subType, dmode);
						xi = q->displayInfo.rect.right + 10;
						if  (minbot < q->displayInfo.rect.bottom)
							minbot = q->displayInfo.rect.bottom;
						q = q->next =qn;
					}
					b->displayInfo.rect.Set(x, y, xi, minbot+6);
				}
				break;
				
			case LIST_FORK: {
//					fprintf(stderr, "fork list\n");
					float rbot=y;
					float yi = y + 5;
					while (q != nullptr) {
						Block	*qn = q->next;
						q->next = nullptr;
						SetupDisplay(q, yi, 5, b->subType, dmode);
						yi = q->displayInfo.rect.bottom + 5;
						if  (rbot < q->displayInfo.rect.right)
							rbot = q->displayInfo.rect.right;
						q = q->next = qn;
					}
					b->displayInfo.rect.Set(x, y, rbot+6, yi);
				}
				break;
				
			case LIST_NORM:
			case LIST_PAR:
			default: {
//					fprintf(stderr, "normal list\n");
					float rbot=y;
					float yi = y + 5;
					while (q != nullptr) {
						Block	*qn = q->next;
						q->next = nullptr;
						SetupDisplay(q, 5, yi, b->subType, dmode);
						yi = q->displayInfo.rect.bottom + 10;
						if  (rbot < q->displayInfo.rect.right)
							rbot = q->displayInfo.rect.right;
						q = q->next = qn;
					}
					b->displayInfo.rect.Set(x, y, rbot+6, yi);
				}
				break;
			}
			b->displayInfo.mode = BLOCK_DISPLAY_OPEN;
		}
		break;
		
// language structures
	case C_FLUX:
	case C_IF:
	case C_GUARD:
	case C_ASSIGN:
	case C_BREAK:
	case C_WAKE:
	case C_SUSPEND:
	case C_INPUT:
	case C_DIVERT:
	case C_WAIT:
	case C_REPEAT:
	case C_OUTPUT:
			
	case C_FOREACH	:
	case C_WITH	:
	case C_STATE:	// state machine

// atomic operations
	case C_VALUE:
	case C_SYM:
	case C_ARRAY_REF:
	case C_NAME:
	case C_STRUCTURE_REF:

// operators
	case C_UNOP	:
	case C_BINOP:
	case C_TEROP:
	case C_CAST	:

	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_BUILTIN:

	case C_AUDIO_ADDON:
	case C_UNLINKED_CALL:
	case C_UNKNOWN:
	case C_CALL	:

	default: {
			if (b->displayInfo.view == nullptr) {
				BlockView *bv = new GraphBlockView(BRect(0,0,20,20), qua);
				b->displayInfo.view = bv;
				b->displayInfo.mode = BLOCK_DISPLAY_OPEN;
				bv->SetValue(b);
				bv->ResizeToFit();
				bv->MoveTo(x, y);
				AddChild(bv);
			}
			BRect r = b->displayInfo.view->Frame();
			r.right += 3;
			r.bottom += 3;
			b->displayInfo.rect = r;
		}
		break;
	}
}

Block *
BlockGraph::MouseDownBlock(Block *b, BPoint p)
{
	Block *ret = nullptr;
	
	switch (b->type) {
	
// language structures
	case C_FLUX:
		break;
	case C_OUTPUT:
		break;
	case C_IF:
		break;
	case C_GUARD:
		break;
	case C_ASSIGN:
		break;
	case C_BREAK:
		break;
	case C_WAKE:
		break;
	case C_SUSPEND:
		break;
	case C_INPUT:
		break;
	case C_DIVERT:
		break;
	case C_STATE:	// state machine
		break;
	case C_WAIT:
		break;
	case C_FOREACH	:
		break;
	case C_WITH	:
		break;
	case C_REPEAT:
		break;	
// List elements
	case C_LIST	:
		break;
		
// atomic operations
	case C_VALUE:
	case C_SYM:
	case C_ARRAY_REF:
	case C_NAME:
	case C_STRUCTURE_REF:

// operators
	case C_UNOP	:
	case C_BINOP:
	case C_TEROP:
	case C_CAST	:

	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_BUILTIN:

	case C_AUDIO_ADDON:
	case C_UNLINKED_CALL:
	case C_UNKNOWN:
	case C_CALL	:

	default: {
			;	
		}
		break;
	}
	
	if (ret == nullptr && b->next) {
		ret = MouseDownBlock(b->next, p);
	}
	if (ret == nullptr  && b->displayInfo.rect.Contains(p)) {
		ret = b;
	}
	
	return ret;
}

void
BlockGraph::ResizeToFit()
{
	short	n  = CountChildren();
	for (short i=0; i<n; i++) {
		BView	*v = ChildAt(i);
		if (strcmp(v->Name(), "blockview") == 0) {
			BlockView	*bv = (BlockView *)v;
			bv->ResizeToFit();
		}
	}
}



void
BlockGraph::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case COMPILE_BLOCK: {

		Block		*b = nullptr;

		if (inMsg->HasPointer("graph block view")) {
			GraphBlockView	*bv;
			inMsg->FindPointer("graph block view", (void **)&bv);
			b = bv->BlockValue(stackable->sym);
		}
		break;
	}
	default:
		BControl::MessageReceived(inMsg);
	}
}


void
DrawArrowHead(BView *v, BPoint from, BPoint to, float d)
{
	BPoint	mid(from.x + d*(to.x-from.x), from.y + d*(to.y - from.y));
	
	float thetaV =  ((from.x  == to.x) ?
						((from.y>=to.y)?(M_PI/2):-(M_PI/2)):
						atan((from.y-to.y)/(from.x-to.x)));
	if (from.x >= to.x) {
		thetaV -= M_PI;
	} else {
//		thetaV += M_PI/2;
	}
	
//	fprintf(stderr, "%g\n", thetaV);
//	FillRect(BRect(mid.x-2,mid.y-2,mid.x+2,mid.y+2));
	BPoint	a1(mid.x - 7*cos(thetaV-0.5),mid.y - 7*sin(thetaV-0.5));
	BPoint	a2(mid.x - 7*cos(thetaV+0.5),mid.y - 7*sin(thetaV+0.5));
	v->FillTriangle(mid,a1,a2);
}

