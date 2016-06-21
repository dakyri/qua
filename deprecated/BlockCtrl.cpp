
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include <ctype.h>

#include "BlockCtrl.h"
#include "Colors.h"
#include "Parse.h"
#include "Block.h"


BlockView::BlockView(BRect frame, const char *name,
         ulong resizingMode, ulong flags, Qua *q):
	BTextView(frame, name, BRect(1,1,0,0), resizingMode, flags)
{
	BRect	r = Bounds();
	r.left += EDEDGE;
	r.right -= 1;
	r.bottom -= 1;
	r.top += 1;
	SetTextRect(r);
	
	needsCompile = false;
	controller = nullptr;
	parser = new TvParser(this, name, q);
	selectedToken = false;
	SelectNextToken();
//	
//	SetWordWrap(false);
}

void
BlockView::Draw(BRect r)
{
	if (Window()) {
		BRect r = Bounds();
		r.left++; r.top ++; r.right--; r.bottom--;
		compButton.Set(r.left+1, r.top+1,r.left+4,r.top+4);
		SetHighColor(Parent()->ViewColor());
		FillRect(BRect(r.left,r.top,r.left+EDEDGE-3,r.bottom));
		DrawCompButton();
		LoweredBox(this, r, Parent()->ViewColor(), false);
	}
	BTextView::Draw(r);
}

void
BlockView::DrawCompButton()
{
	ConstrainClippingRegion(nullptr);
	if (needsCompile)
		RaisedBox(this, compButton, red, true);
	else
		RaisedBox(this, compButton, green, true);
}

BlockView::~BlockView()
{
}

bool
BlockView::AcceptsDrop(BMessage *m)
{
	return BTextView::AcceptsDrop(m);
}

void
BlockView::SelectNextToken()
{
	selectedToken = false;
	int32		start, finish;
	GetSelection(&start, &finish);
	parser->Seek(finish);
	parser->GetToken();
	if (parser->currentTokenType == TOK_EOF) {
		parser->Rewind();
		parser->GetToken();
		if (parser->currentTokenType == TOK_EOF) {
			return;
		}
	}
	selectedToken = true;
	start = parser->currentTokenPosition;
	finish = start + strlen(parser->currentToken);
	Select(start, finish);
}

void
BlockView::SelectPrevToken()
{
	selectedToken = false;
	int32		start, finish;
	GetSelection(&start, &finish);

	int		cnt = 0;
	char	c;
	start--;
	if (start < 0) {
		start = TextLength()-1;
	}
	while (start >= 0 && isspace(c = ByteAt(start))) {
		start--;
	}
	while (start >= 0 && !isspace(c = ByteAt(start))) {
		start--;
	}
	if (start<0)
		start=0;
		
	parser->Seek(start);
	parser->GetToken();
	if (parser->currentTokenType == TOK_EOF)
		return;
	selectedToken = true;

	start = parser->currentTokenPosition;
	finish = start + strlen(parser->currentToken);
	Select(start, finish);
}

void
BlockView::IncrementCurrentToken()
{
	long	mods = modifiers();
	if (parser->currentTokenType == TOK_VAL) {
		if (parser->currentTokenVal.type == S_TIME) {
			if (mods & B_CONTROL_KEY)
				parser->currentTokenVal.TimeValue()->IncrementBar();
			else if (mods & B_SHIFT_KEY)
				parser->currentTokenVal.TimeValue()->IncrementBeat();
			else
				parser->currentTokenVal.TimeValue()->IncrementTick();
		} else {
			TypedValue	v=TypedValue::Int(1);
			parser->currentTokenVal = parser->currentTokenVal + v;
		}
		int32		start, finish;
		GetSelection(&start, &finish);
		char	*nubit = parser->currentTokenVal.StringValue();
		Delete(start,finish);
		Insert(nubit);
		Select(start,start+strlen(nubit));
	}
}

void
BlockView::DecrementCurrentToken()
{
	long	mods = modifiers();
	if (parser->currentTokenType == TOK_VAL) {
		if (parser->currentTokenVal.type == S_TIME) {
			if (mods & B_CONTROL_KEY)
				parser->currentTokenVal.TimeValue()->DecrementBar();
			else if (mods & B_SHIFT_KEY)
				parser->currentTokenVal.TimeValue()->DecrementBeat();
			else
				parser->currentTokenVal.TimeValue()->DecrementTick();
		} else {
			TypedValue	v = TypedValue::Int(1);
			parser->currentTokenVal = parser->currentTokenVal - v;
		}
		int32		start, finish;
		GetSelection(&start, &finish);
		char	*nubit = parser->currentTokenVal.StringValue();
		Delete();
		Insert(nubit);
		Select(start,start+strlen(nubit));
	}
}

void
BlockView::MinimizeCurrentToken()
{
}

void
BlockView::MaximizeCurrentToken()
{
}

void
BlockView::HomeCurrentToken()
{
}

void
BlockView::KeyDown(const char *cb, int32 ncb)
{
	char	achar=cb[0];

	if (!IsEditable())
		return;	

	long	mods = modifiers();
	bool	csrd = false;

	BRect		r = Bounds();
	r.left = EDEDGE;
	BRegion	cr;
	cr.Include(r);
	ConstrainClippingRegion(&cr);

	switch (achar) {

	case B_LEFT_ARROW:
		if (mods) {
			SelectPrevToken();
		} else {
			selectedToken = false;
			BTextView::KeyDown(cb, ncb);
		}
		break;
		
	case B_RIGHT_ARROW:
		if (mods) {
			SelectNextToken();
		} else {
			selectedToken = false;
			BTextView::KeyDown(cb, ncb);
		}
		break;
			
	case B_UP_ARROW: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		if (!selectedToken)
			SelectNextToken();
		if (selectedToken)
			IncrementCurrentToken();
		break;
	}
	case B_DOWN_ARROW: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		if (!selectedToken)
			SelectNextToken();
		if (selectedToken)
			DecrementCurrentToken();
		break;
	}
	case B_PAGE_UP: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		if (!selectedToken)
			SelectNextToken();
		if (selectedToken)
			MaximizeCurrentToken();
		break;
	}
	case B_PAGE_DOWN: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		if (!selectedToken)
			SelectNextToken();
		if (selectedToken)
			MinimizeCurrentToken();
		break;
	}
	case B_HOME: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		if (!selectedToken)
			SelectNextToken();
		if (selectedToken)
			HomeCurrentToken();
		break;
	}
		
	case B_INSERT:
		break;
		
	case B_END:
	case B_ENTER:
	case B_TAB: {
		if (achar == B_TAB && controller && controller->NextSibling()) {
			controller->NextSibling()->MakeFocus(TRUE);
		} else {
			MakeFocus(FALSE);
		}
		break;
	}
		
	default: {
		if (!needsCompile) {
			csrd = true;
			needsCompile = true;
		}
		selectedToken = false;
		BTextView::KeyDown(cb, ncb);
	}}
	
	if (csrd) {
		DrawCompButton();
	}
}

bool
BlockView::CanEndLine(int32 off)
{
	return false;//ByteAt(off) == '\n';
}

void
BlockView::SetValue(Block * val)
{
	if (val) {
		char	txt[10*1024];
		long	len = 0;

		if (!Esrap(val, txt, len, 10*1024, true, -1)) {
			reportError("block size too large...");
		} else {
			SetText(txt);
			needsCompile = false;
			if (Window()) {
				DrawCompButton();
			}
		}
	}
}

void
BlockView::MakeFocus(bool flag)
{
	if (flag)
		SelectAll();
	else if (!flag && controller) {
		controller->Invoke();
	}
	BTextView::MakeFocus(flag);
}

Block *
BlockView::ExpressionValue(StabEnt *v)
{
	parser->Rewind();
	parser->GetToken();
	Block	*b = parser->ParseExpression(v);
	if (parser->err_cnt == 0) {
		glob.PushContext(v);
		if (b && !b->Init(nullptr)) {
			b->DeleteAll();
			b = nullptr;
		} else {
			needsCompile = false;
			if (Window())
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

Block *
BlockView::BlockValue(StabEnt *v)
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
			if (Window())
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

void
BlockView::ResizeToFit()
{
	float	maxwid = 30;
	int		nl = CountLines();
	for (short i=0; i<nl; i++) {
		float	w = LineWidth(i);
		if (w > maxwid) maxwid = w;	
//		fprintf(stderr, "bv linew maxw: %g %g\n", w, maxwid);
	}
	float	lh;
	BPoint	p = PointAt(OffsetAt(nl-1),&lh);
	BRect r = TextRect();
	ResizeTo(maxwid+r.left+1, r.top+p.y+lh+1);
	r.right = r.left+maxwid;
	r.bottom = r.top+p.y+lh;
	SetTextRect(r);
//	fprintf(stderr,
//			"%d lines, offset %d text rect in rsz %g %g %g %g p %g %g lh %g\n",
//			nl, OffsetAt(nl-1),
//			r.left, r.top, r.right, r.bottom,
//			p.x, p.y,
//			lh);
//	fprintf(stderr, "%s\n", Text());
	ScrollTo(0,0);
	Invalidate();
}

BlockCtrl::BlockCtrl(BRect frame, const char *name,
		        char *label, 
				const char *text,
				BMessage *msg, ulong typ,
				ulong f1, ulong f2, Qua *q):
	BControl(frame, name, label, msg, f1, f2)

{
	divider = 0;
	blockType = typ;
	textrect = Bounds();
	textrect.Set(textrect.left+2,textrect.top+2,
				textrect.right-1,textrect.bottom-1);
	textview = new BlockView(textrect, name, f1, f2, q);
	textview->controller = this;
	if (text)
		textview->SetText(text);
	AddChild(textview);
}


void BlockCtrl::SetText(const char *txt)
{
	textview->SetText(txt);
}

void
BlockCtrl::Draw(BRect reg)
{
	BPoint		Pos(1,14);
	SetLowColor(ViewColor());
	SetHighColor(black);
	DrawString(Label(), Pos);
}

char *
BlockCtrl::Text()
{
	return (char *)textview->Text();
}

Block *
BlockCtrl::ExpressionValue(StabEnt *S)
{
	return textview->ExpressionValue(S);
}

Block *
BlockCtrl::BlockValue(StabEnt *S)
{
	return textview->BlockValue(S);
}

void
BlockCtrl::SetDivider(float xCoordinate)
{
	BPoint	pos;
	
	divider = xCoordinate;
	textrect.Set(divider+2, textrect.top, textrect.right, textrect.bottom);
	textview->ResizeTo(textrect.right - textrect.left, textrect.bottom-textrect.top);
	pos.Set(textrect.left,textrect.top);
	textview->MoveTo(pos);
}

void BlockCtrl::MakeFocus(bool flag)
{
	textview->MakeFocus(flag);
}

BlockCtrl::~BlockCtrl()
{
}

void
BlockCtrl::SetValue(Block *b)
{
	textview->SetValue(b);
}

void
BlockCtrl::MakeEditable(bool wh)
{
	textview->MakeEditable(wh);
}