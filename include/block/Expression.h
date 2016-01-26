#ifndef _EXPRESSION
#define _EXPRESSION

class Block;
class Note;
class StreamItem;
class List;
class Stacker;

#include "Sym.h"

void			LValueAte(LValue &, Block *, StreamItem *, Stacker *, StabEnt *, QuasiStack *);
ResultValue		EvaluateExpression(Block  *E, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	EvaluateBuiltIn(Block *E, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue		CreateNewNote(int type, Block  *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewMesg(Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewProg(Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewCtrl(Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewBend(Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewSysX(Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue	 	CreateNewSysC(int type, Block *P, StreamItem *items=nullptr,  Stacker *instance=nullptr, StabEnt *ctxt=nullptr, QuasiStack *stack=nullptr);
ResultValue		Find(Stream *S, Block *Query);

void			PlayAtClock(struct OldPlayInfo *P, StreamItem *items,
						Stacker *instance, QuasiStack *stack,
						class TypedValueList &l);
Note			*PlayNextNote(struct OldPlayInfo *P);
Note			*WhirlNextNote(struct WhirlInfo *W);
Note			*GenNextNote(struct MarkovInfo *G);

int				StringToNote(char *buf);


#endif
