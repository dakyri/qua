#ifndef _MYNODE
#define _MYNODE

class Block;
class Source;
class LocusCtlView;
class LocusList;
class FormalParamList;
class ControlVar;

#include "QuaTypes.h"

class Locus
{
public:
	bool		Trigger();
	bool		UnTrigger();
				Locus(Source *S, Block *B, Block *L);
				Locus(Locus *n);
				Locus();
				~Locus();
				
	LocusList	*subLocus;
	Locus		*uberLocus;
	qua_status	Status;
	Source		*Source;
	Block		*EnclosingList;
	Block		*CallingBlock;
	ControlVar	*Params;
	LocusCtlView	*ControlPanel;
};

#define MAX_NODES	50

class LocusList {
public:
				LocusList();
	void		AddItem(Locus *p);
	void		RemoveItem(short i);
				
	Locus		*List[MAX_NODES];
	int			Count;
};


FormalParamList *first_unpassed_param(Locus *V);

#endif