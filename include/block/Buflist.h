#ifndef BUFLIST
#define BUFLIST

#define MAX_BUFLIST	50

class SampleTake;

struct buflist_item
{
	SampleTake	*src;
	int			val;

	bool			operator == (buflist_item &s2) { return src == s2.src && val == s2.val; }
	bool			operator != (buflist_item &s2) { return src != s2.src || val != s2.val; }
};

class Buflist
{
public:
					Buflist(short max);
	bool			Add(SampleTake *, int i);
	int				CountItems();
	buflist_item	&ItemAt(int i);
	void			MakeEmpty();
	bool			Has(SampleTake *, int i);
	char			*PrintString();
	bool			operator == (Buflist &s2);
	bool			operator != (Buflist &s2);
		
	buflist_item	items[MAX_BUFLIST];

	int				maxitem;
	int				nitems;
};

#endif