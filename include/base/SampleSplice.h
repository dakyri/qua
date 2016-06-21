#ifndef _SAMPLESPLICE
#define _SAMPLESPLICE

class SampleTake;

#include "StdDefs.h"

class SampleSplice
{
public:
	SampleTake		*source;
	long			nFrames;
	long			fromFrame;
};

class SpliceList
{
public:
					SpliceList();
					~SpliceList();
					
	bool			AddSplice(SampleSplice *);
	bool			AddSplices(SampleSplice **, int);
	SampleSplice *	SpliceForFrame(long fr, long *ind);
	bool			Clear();
	
	SampleSplice	**list;
	int				nSplice;
	int				listSize;
};

class SampleSplicerator
{
public:
	void			Cut(ulong, ulong);
	void			Copy(ulong, ulong);
	void			Paste(ulong, ulong);
	void			Overwrite(ulong, ulong);
	void			Clear(ulong, ulong);
	
	SpliceList		cutBuffer;
	SpliceList		splices;
};

#endif