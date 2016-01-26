#include "Buflist.h"
#include <stdio.h>
#include <cstring>

Buflist::Buflist(short max)
{
	if (max < MAX_BUFLIST)
		maxitem = max;
	else
		maxitem = MAX_BUFLIST;
	nitems = 0;
}

void
Buflist::MakeEmpty()
{
	nitems = 0;
}

bool
Buflist::Add(SampleTake *src, int val)
{
	short i;
	if (nitems == maxitem)
		return false;
	for (i=0; i<nitems; i++) {
		if (items[i].src == src && items[i].val == val) {
			return true;
		} else if (items[i].val > val) {
			break;
		}
	}
	for (short j=nitems; j>=i; j--) {
		items[j+1] = items[j];
	}

	items[i].val = val;
	items[i].src = src;
	
	nitems++;
	return true;
}

int
Buflist::CountItems()
{
	return nitems;
}

buflist_item &
Buflist::ItemAt(int i)
{
	return items[i];
}

bool
Buflist::Has(SampleTake *src, int val)
{
	for (short i=0; i<nitems; i++) {
		if (items[i].val > val)
			return false;
		else if (items[i].val == val)
			return true;
	}
	return false;
}

char *
Buflist::PrintString()
{
	static char		buf[1024];
	char			*p = buf;

	strcpy(p,"<");
	p++;
	for (short i=0; i<nitems; i++) {
		if (i > 0)
			*p++ = ' ';
		p += sprintf(p, "%d", items[i].val);
	}
	strcpy(p,">");

	return buf;
}

bool
Buflist::operator == (Buflist &x)
{
	if (nitems != x.nitems)
		return false;
	for (short i=0; i<nitems; i++) {
		if (items[i] != x.items[i])
			return false;
	}
	return true;
}

bool
Buflist::operator != (Buflist &x)
{
	if (nitems != x.nitems)
		return true;
	for (short i=0; i<nitems; i++) {
		if (items[i] != x.items[i])
			return true;
	}
	return false;
}