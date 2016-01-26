// junk

class ACListView : public BListView
{
public:
					ACListView(BRect wrect, char *nm,
							 ulong f1, ulong f2, AppCtl *A);
	virtual void	DrawItem(BRect r, long i);
	virtual void	Draw(BRect r);
	virtual void	Select(long i);
	virtual void	MessageReceived(BMessage *msg);

	NumCtrl			*rtct;
	AppCtl			*AC;
};

ACListView::ACListView(BRect wrect, char *nm, ulong f1, ulong f2, AppCtl *A):
	BListView(wrect, nm, f1, f2)
{
//	SetFontName( "Courier New Bold" );
//	SetFontSize(8);
	rtct = nullptr;
	AC = A;
}

void
ACListView::Select(long ind)
{
	BPoint		pt;
	ulong		bt;
	BRect		wrect;
	BMessage	*msg;

	if (rtct != nullptr) {
		RemoveChild(rtct);
//		DrawItem(ItemFrame(CurrentSelection()), CurrentSelection());
		pt.x = RATE_COL;
		pt.y = ItemFrame(CurrentSelection()).bottom - 2;
		DrawString(rtct->Text(), pt);
		delete rtct;
	}
	
	GetMouse(&pt, &bt);

	msg = new BMessage(SET_RATE);
	wrect.Set( RATE_COL, ItemHeight()*ind, RATE_COL+60, ItemHeight()*(ind+1) );
	rtct = new NumCtrl( wrect, "",
		"", sstr(((AppCtlBlock *)ItemAt(ind))->Rate), msg, 0, B_WILL_DRAW );
	rtct->SetRanges(-4,12,11111111);
	rtct->SetDivider(0);
	rtct->SetFontName( "Emily" );
	rtct->SetFontSize( 12 );
	AddChild(rtct);
	rtct->SetTarget(this);
			
	BListView::Select(ind);
}

void
ACListView::MessageReceived(BMessage *msg)
{
	AppCtlBlock	*A;
	int			rate = ACR_NOT;
	
	switch(msg->what) {
	case SET_RATE:
		A = (AppCtlBlock *)ItemAt(CurrentSelection());
		rate = sval(rtct->Text());
		rtct->SetText(sstr(rate));
		A->Rate = rate;
		break;
	}
	BListView::MessageReceived(msg);
}

void
ACListView::Draw(BRect wrect)
{
	BRect	Box=Bounds();
	BPoint	Pos(0,Box.bottom);
	
	Pos.Set(Box.left,Box.bottom);	MovePenTo(Pos);
	SetHighColor(128,128,128);SetPenSize(1.0);
	Pos.Set(Box.left,Box.top);	StrokeLine(Pos);
	Pos.Set(Box.right,Box.top);	StrokeLine(Pos);
	SetHighColor(238,238,238);SetPenSize(1.0);
	Pos.Set(Box.right,Box.bottom);StrokeLine(Pos);
	Pos.Set(Box.left,Box.bottom);	StrokeLine(Pos);
	BListView::Draw(wrect);
}

void
ACListView::DrawItem(BRect wrect, long ind)
{
	char buf[20];
	BRect fr = ItemFrame(ind);	
	BPoint	where(fr.left+5,fr.bottom-2);
	AppCtlBlock	*A = ((AppCtlBlock *)ItemAt(ind));

	DrawString(A->Source->Name, where);
	where.x = RATE_COL;
	DrawString(sstr(A->Rate), where);
}

