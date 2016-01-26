
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "RenderStatusView.h"
#include "Colors.h"

RenderStatusView::RenderStatusView(BRect r, char *nm):
	BView(r, nm, (ulong)nullptr, (ulong)B_WILL_DRAW)
{
	SetStatusValue(101);
}

void
RenderStatusView::SetStatusValue(short val)
{
	if (Window()) Window()->Lock();
	if (val <= 100) {
		SetViewColor(red);
	} else if (val >= 101) {
		SetViewColor(green);
	}
	percentValue = val;
	Invalidate();
	if (Window()) Window()->Unlock();
}

void
RenderStatusView::Draw(BRect r)
{
	SetHighColor(ViewColor());
	BRect bnd = Bounds();
	FillRect(bnd);
	if (percentValue < 101) {
		SetHighColor(black);
		SetLowColor(ViewColor());
		char buf[20];
		sprintf(buf, "%d%%", percentValue);
		DrawString(buf, BPoint(2, bnd.bottom-2));
	}
}
