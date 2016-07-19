#include "Buflist.h"
#include <stdio.h>
#include <cstring>
#include <string>
using namespace std;

Buflist::Buflist(short max)
{
	if (max < MAX_BUFLIST)
		maxitem = max;
	else
		maxitem = MAX_BUFLIST;
}

void
Buflist::MakeEmpty()
{
	items.clear();
}

bool
Buflist::Add(SampleTake *src, int val)
{
	short i;
	if (items.size() == maxitem)
		return false;
	for (i=0; i<items.size(); i++) {
		if (items[i].src == src && items[i].val == val) {
			return true;
		} else if (items[i].val > val) {
			break;
		}
	}
	items.insert(items.begin() + i, buflist_item(src, val));
	return true;
}

int
Buflist::CountItems()
{
	return items.size();
}

buflist_item &
Buflist::ItemAt(int i)
{
	return items[i];
}

bool
Buflist::Has(SampleTake *src, int val)
{
	for (short i=0; i<items.size(); i++) {
		if (items[i].val > val)
			return false;
		else if (items[i].val == val)
			return true;
	}
	return false;
}

string
Buflist::PrintString()
{
	string buf;

	buf = "<";
	for (short i=0; i<items.size(); i++) {
		if (i > 0)
			buf += " ";
		buf += to_string(items[i].val);
	}
	buf += ">";

	return buf;
}

bool
Buflist::operator == (Buflist &x)
{
	if (items.size() != x.items.size())
		return false;
	for (short i=0; i<items.size(); i++) {
		if (items[i] != x.items[i])
			return false;
	}
	return true;
}

bool
Buflist::operator != (Buflist &x)
{
	if (items.size() != x.items.size())
		return true;
	for (short i=0; i<items.size(); i++) {
		if (items[i] != x.items[i])
			return true;
	}
	return false;
}