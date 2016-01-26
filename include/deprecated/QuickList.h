#ifndef _QUICKLIST
#define _QUICKLIST

class QuickList
{
public:
	inline QuickList(short maxelems)
	{
		max = maxelems;
		n = 0;
		elements = new void * [max];
	}
	short	max;
	short	n;
	void	**elements;
	
	inline void 	*Item(short i)
	{
		return (i>=0 && i<n)?elements[i]:nullptr;
	}
	
	inline void	Del(void *p)
	{
		short i;
		for (i=0; i<n; i++) {
			if (elements[i] == p) {
				break;
			}
		}
		if (i<n) {
			n--;
			for (;i<n;i++) {
				elements[i] = elements[i+1];
			}
		}
	}
	
	inline void	Add(void *p)
	{
		if (n < max) {
			elements[n++] = p;
		}
	}
	
	inline bool Has(void *p)
	{	
		for (short i=0; i<n; i++) {
			if (elements[i] == p) {
				return true;
			}
		}
		return false;
	}
		
};

#endif
