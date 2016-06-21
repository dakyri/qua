#include "RateCtrl.h"
#include "Application.h"
#include <ctype.h>


KeyVal	rates[] = {
	{"not",	ACR_NOT},
	{"sleep",	ACR_SLEEP},
	{"wake",	ACR_WAKE},
	{"death",	ACR_SHUTDOWN},
	{"stream",	ACR_STREAM},
	{"startup",	ACR_STARTUP}};

RateCtrl::RateCtrl(BRect frame, const char *name,
		         char *label, 
		        const char *text,
					BMessage *msg,
					ulong f1, ulong f2):
	NumCtrl(frame, name, label, "not", msg, f1, f2)

{
	SetKeyVal(rates, 6);
	SetRanges(-5,0,1024);
	SetInc(1);
}

void
RateCtrl::SetValue(long v)
{
	if (v < 0)
		MakeEditable(FALSE);
	else
		MakeEditable(TRUE);
	NumCtrl::SetValue(v);
}

RateCtrl::~RateCtrl()
{
}