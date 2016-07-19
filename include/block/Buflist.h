#ifndef BUFLIST
#define BUFLIST

#define MAX_BUFLIST	50

#include <vector>
#include <string>
using namespace std;

class SampleTake;

struct buflist_item
{
	buflist_item(SampleTake *_src, int _val): src(_src), val(_val) { }

	SampleTake	*src;
	int val;

	bool operator == (buflist_item &s2) { return src == s2.src && val == s2.val; }
	bool operator != (buflist_item &s2) { return src != s2.src || val != s2.val; }
};

class Buflist
{
public:
	Buflist(short max);

	bool Add(SampleTake *, int i);
	int CountItems();
	buflist_item &ItemAt(int i);
	void MakeEmpty();
	bool Has(SampleTake *, int i);
	string PrintString();

	bool operator == (Buflist &s2);
	bool operator != (Buflist &s2);
		
	vector<buflist_item> items;

	int maxitem;
};

#endif