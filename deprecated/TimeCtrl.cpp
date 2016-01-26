
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "TimeCtrl.h"
#include "Metric.h"
#include "Colors.h"
#include <ctype.h>

extern BList	metrix;

MetricSelect::MetricSelect(BRect rec, char *name, Metric *m,
					BHandler *h,
					uint32 mo, uint32 f):
	BStringView(rec, name, "", mo, f)
{
	metric = nullptr;
	handler = h;
	SetValue(m);
	SetViewColor(ltGray);
}

void
MetricSelect::SetValue(Metric *m)
{
	if (metric != m) {
		metric = m;
		SetText(m?m->name:"");
	}
}

void
MetricSelect::SetFont(BFont *f)
{
	BStringView::SetFont(f);
}

void
MetricSelect::SetFontSize(float f)
{
	BStringView::SetFontSize(f);
}


MetricSelect::~MetricSelect()
{
}


void
MetricSelect::MouseDown(BPoint where)
{
	if (handler == nullptr)
		return;
		
	BMenuItem	*item;
	BMessage	*msg;
	BPoint		pt;
	Metric		*m;

	BPopUpMenu	*qMenu = new BPopUpMenu("sel", FALSE, FALSE);
	for ( short i=0;i<metrix.CountItems();i++) {
		m = (Metric *)metrix.ItemAt(i);
		msg = new BMessage(SET_METRIC);
		msg->AddPointer("metric", m);
		BMenuItem	*item = new BMenuItem(m->name, msg);
		qMenu->AddItem(item);
		item->SetTarget(handler);
	}
			
	pt.Set(0,0);	
	ConvertToScreen(&pt);
	qMenu->SetAsyncAutoDestruct(true);
	qMenu->Go(pt, true, false, true);
}


void
MetricSelect::Draw(BRect area)
{
	BStringView::Draw(area);
}

void
MetricSelect::MakeFocus(bool mf)
{
	BStringView::MakeFocus(mf);
}


MeasureView::MeasureView(BRect frame, const char *name,
		Metric	*m,
         ulong resizingMode, ulong flags):
	BTextView(frame, name, BRect(1,1,0,0), resizingMode, flags)
{
	BRect	tr = Bounds();
	tr.left = 1;
	tr.top = 1;
	tr.bottom = Bounds().bottom-1;
	tr.right = Bounds().right-1;
	SetTextRect(tr);
	controller = nullptr;
	sibling = nullptr;
	
	homeValue = upValue = downValue = 0;
	
	Time	t(0,m);
	SetValue(t);
}

void
MeasureView::SetBounds(long d, long h, long u)
{
	downValue = d;
	homeValue = h;
	upValue = u;
}


void
MeasureView::KeyDown(const char *cb, int32 ncb)
{
	long	val;
	char	*txt;
	char	achar=cb[0];

	if (!IsEditable())
		return;	
	txt = (char *)Text();
	while (*txt == ' ') txt++;
	
	switch (achar) {
	case B_DELETE:
	case B_INSERT:
	case B_END:
		break;
		
	case B_UP_ARROW:
	case B_DOWN_ARROW:
	case B_PAGE_UP:
	case B_PAGE_DOWN:
	case B_HOME: {
		char	buf[20];
		long	mods = modifiers();

		if (achar == B_UP_ARROW) {
			if (mods & B_CONTROL_KEY) {
				theTime.IncrementBar();
			} else if (mods & B_SHIFT_KEY) {
				theTime.IncrementBeat();
			} else {
				theTime.IncrementTick();
			}
		} else if (achar == B_DOWN_ARROW) { 
			if (mods & B_CONTROL_KEY) {
				theTime.DecrementBar();
			} else if (mods & B_SHIFT_KEY) {
				theTime.DecrementBeat();
			} else {
				theTime.DecrementTick();
			}
		} else if (achar == B_PAGE_UP) {
			theTime = upValue;
		} else if (achar == B_PAGE_DOWN) {
			theTime = downValue;
		} else if (achar == B_HOME) {
			theTime = homeValue;
		}
			
			
		SetValue(theTime);
		GoToLine(0);
		if (controller) {
			controller->Invoke();
		}
		break;
	}
		
	case B_ENTER:
	case B_TAB: {
		theTime.Set(txt);
		SetValue(theTime);
		MakeFocus(FALSE);
		break;
	}
		
	default: {
		BTextView::KeyDown(cb, ncb);
	}}
}

void
MeasureView::SetValue(Time &t)
{
	char	*txt, buf[30];
	
	theTime = t;
//	fprintf(stderr, "%s %s\n", theTime.StringValue(), theTime.metric->name);
	strcpy(buf, theTime.StringValue());
	if (strcmp(Text(), buf) != 0) {
		SetText(buf);
	}
}

void
MeasureView::SetValue(long tick)
{
	if (theTime.ticks != tick) {
		theTime.ticks = tick;
		SetValue(theTime);
	}
}

long
MeasureView::TickValue()
{
	return theTime.TickValue();			
}

float
MeasureView::ActualValue()
{
	return theTime.SecsValue();			
}

MeasureView::~MeasureView()
{
}

bool
MeasureView::AcceptsDrop(BMessage *m)
{
	return BTextView::AcceptsDrop(m);
}


void
MeasureView::SetSibling(BView *v)
{
	sibling=v;
}


void
MeasureView::MakeFocus(bool flag)
{
	if (flag)
		SelectAll();
	else if (!flag && controller) {
		if (controller->Message()) {
			if (controller->Message()->HasFloat("value"))
				controller->Message()->ReplaceFloat("value", ActualValue());
			else
				controller->Message()->AddFloat("value", ActualValue());
		}
		controller->Invoke();
	}
	BTextView::MakeFocus(flag);
}


TimeCtrl::TimeCtrl(BRect frame, const char *name,
		        char *label, 
				BMessage *msg,
				Metric *m, short mode,
				ulong f1, ulong f2):
		BControl(frame, name, label, msg, f1, f2)

{
	divider = *label?StringWidth(Label())+5:0;
	BRect	bnd = Bounds();
	textview = new MeasureView(
						BRect(	divider,bnd.top+2,
								bnd.right-22, bnd.bottom-2),
						name, m, B_FOLLOW_ALL, B_WILL_DRAW);
	textview->controller = this;
	metricSel = new MetricSelect(
						BRect(	bnd.right-22,bnd.top+2,
								bnd.right-2, bnd.bottom-2),
						 (char *)name, m, this,
						B_FOLLOW_ALL, B_WILL_DRAW);

	if (Message()) {
		Message()->AddFloat("value", 0);
	}
	
	AddChild(textview);
	AddChild(metricSel);
}

long
TimeCtrl::TickValue()
{
	return textview->TickValue();
}

float
TimeCtrl::ActualValue()
{
	return textview->ActualValue();
}

void
TimeCtrl::SetValue(long tick)
{
	textview->SetValue(tick);
}

void
TimeCtrl::SetValue(Time &t)
{
	textview->SetValue(t);
}


void
TimeCtrl::SetText(const char *txt)
{
	textview->SetText(txt);
}


void
TimeCtrl::Draw(BRect reg)
{
	BRect	r = textview->Frame()|metricSel->Frame();
	
	SetLowColor(ViewColor());
	SetHighColor(black);
	DrawString(Label(), BPoint(2,r.bottom));

	LoweredBox(this, r, ViewColor(), false);
}

const char *
TimeCtrl::Text()
{
	return textview->Text();
}


void
TimeCtrl::SetDivider(float xCoordinate)
{
	textview->ResizeBy(divider-xCoordinate, 0);
	textview->MoveTo(xCoordinate, 2);
	divider = xCoordinate;
}

void
TimeCtrl::MakeFocus(bool flag)
{
	textview->MakeFocus(flag);
}


TimeCtrl::~TimeCtrl()
{
}

void
TimeCtrl::SetSibling(BView * v)
{
	textview->SetSibling(v);
}

void
TimeCtrl::SetBounds(long h, long u, long v)
{
	textview->SetBounds(h, u, v);
}

void
TimeCtrl::MakeEditable(bool wh)
{
	textview->MakeEditable(wh);
}