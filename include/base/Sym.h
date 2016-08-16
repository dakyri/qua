#ifndef _SYM
#define _SYM
 
#include <stdio.h>
#include "StdDefs.h"
#include "BaseVal.h"

namespace tinyxml2 {
	class XMLElement;
}


class StabEnt;
class QuasiStack;

/*
 * object holding all the necessary info to set or retrieve a value of a symbol in a particular context.
 */
class LValue
{
public:
	inline LValue()
	{
		addr = nullptr;
		sym = nullptr;
		indirection = 0;
		stacker = nullptr;
		stackerSym = nullptr;
		stack = nullptr;
	}
	
	inline LValue(StabEnt *s, char * adr, int8 ind, Stacker *stk, StabEnt *ssym, QuasiStack *qs)
	{
		addr = adr;
		sym = s;
		indirection = ind;
		stacker = stk;
		stackerSym = ssym;
		stack = qs;
	}
	
	void				StoreInt(int32 val);
	void				StoreLong(int64 val);
	void				StoreFloat(float val);
	void				StoreValue(TypedValue *v);
	void				StoreBlock(Block *v);
	bool				StoreData(char *d, int len);
	void				FindData(char **dp, int *lenp);
	bool				VstParamCheck(float v);
	bool				PortParamCheck(float v);
	bool				SpecialAssignment(int v);
	bool				SpecialAssignment(TypedValue *v);
	void				AssignmentExtras();
	ResultValue			CurrentValue();
	void				Initialize();
	bool				SetToString(const char *str);
	 
	char				*addr;
	StabEnt				*sym;
	int8				indirection;
	Stacker 			*stacker;
	QuasiStack			*stack;
	StabEnt				*stackerSym;
};

class StabEnt: public TypedValue
{
public:
						StabEnt(const string &nm, base_type_t typ, StabEnt *context, int8 nd, ref_type_t ref);
						~StabEnt();
	bool				RefersToStream();
	void				Initialize(QuasiStack *);
	
	const char *uniqueName();
	string preciseName();
	string printableName();
	
	status_t			SaveScript(ostream &out, short indent, bool saveControllers, bool saveInternal);

	status_t			SaveValue(FILE *fp, Stacker *i, QuasiStack *s);
	status_t			SaveSimpleType(ostream &out, short indent, bool saveInternal);
	string SaveSimpleBits();

	status_t			SaveSnapshot(ostream &out);
	status_t			SaveSimpleTypeSnapshot(ostream &out, Stacker *stacker, QuasiStack *qs, char *additionalAttributes=nullptr);
	status_t			SetAtomicSnapshot(tinyxml2::XMLElement *element, Stacker *stker, StabEnt *stackerSym, QuasiStack *stk);
	status_t			SaveInitialAssigns(FILE *fp, short indent, Stacker *stacker, QuasiStack *stack);
	status_t			SaveSimpleTypeInitialAssigns(FILE *fp, short indent, Stacker *stacker, QuasiStack *stack);

	const char* StringValue(StreamItem *items, Stacker *i, QuasiStack *stack);

	bool				SetDisplayMode(ulong displayMode);
	bool				SetEnvelopeMode(bool enable);

	bool				SetVisualParameters(Block *b);

	void				SetBounds(TypedValue min, TypedValue max);
	void				SetBounds(TypedValue min, TypedValue ini, TypedValue max);
	void				SetInit(TypedValue ini);
	void				SetBounds(int min, int max);
	void				SetHidden(bool hd=true);
	
	void				SetDefaultBounds();
	
	bool				IsControlVar();
/*
	bool				UpdateControllerBridge(Time *t, TypedValue *v, QuasiStack *);
	*/
	Qua					*TopContext();
	void				dump(ostream &os, short i);
	
	void				SetLValue(LValue &, StreamItem *items,  Stacker *instance, StabEnt *stackCtxt, QuasiStack *stack);
	int					Size(short dim=0);
	int					AllocationLength();

	StabEnt*			TypeSymbol();

	enum {
		DISPLAY_NOT = 0,	// static variable
		DISPLAY_CTL = 1,	// an edittable controller
		DISPLAY_DISP = 2,		// displayed value only
		DISPLAY_ENV = 3		// displayed value only
	};
protected:
	void				BaseAddress(LValue &, StreamItem *items,  Stacker *instance, QuasiStack *stack);

public:
    string name;
#ifdef DEFINE_COUNT
    short				defineCount;
#endif
    q_size_t			*size;
    StabEnt				*context;
    StabEnt				*sibling;
    StabEnt				*children;

	uchar				controlMode;
	bool				isEnveloped;
	bool				isClone;
    bool				isDeleted;
    bool				hasBounds;
    bool				hasInit;
    bool				isHidden;
    TypedValue			minVal;
    TypedValue			iniVal;
    TypedValue			maxVal;
};

inline int
StabEnt::Size(short nd)
{
	return (size && nd >= 0 && nd <= indirection)? size[nd]:0;
}

// a very slackly thrown together symbol table
#include <string>
using namespace std;

class SymTab
{
public:
					SymTab(int n);
					~SymTab();

	static bool		ValidSymbolName(const string &nm);
	static string	MakeValidSymbolName(const string &nm);
	static string	MakeValidSymbolName(const char *p);

	ulong hash(const string nm, StabEnt *context);
	long findFreeInd(const string &nm, StabEnt *context, short &dcp);
	StabEnt *addSymbol(const string &nm, base_type_t typ, StabEnt *context, int8 nd, ref_type_t refc, short defcnt);
	void			PushContext(StabEnt *sym);
	StabEnt			*PopContext(StabEnt *expect=nullptr);
	StabEnt			*TopContext();
	StabEnt *findSymbol(const string &nm, short def_cnt = -1);
	StabEnt *findContextSymbol(const string &nm, StabEnt *S, short def_cnt = -1);
	void			DeleteSymbol(StabEnt *S, bool cleanup);
	bool rename(StabEnt *S, const string &nm);
	string makeUniqueName(StabEnt *ctxt, const string basenm, long startind);
	void			MoveSymbol(StabEnt *S, StabEnt *ctxt, StabEnt *np);

	inline StabEnt	*operator[](long i)
		{ return stab[i]; }

	void			dumpGlobals(ostream &os);
	void			dumpContexts(ostream &os);

private:
	long findSymbolInd(StabEnt *S, short def_cnt = -1);
	long findContextSymbolInd(const string &nm, StabEnt *S, short def_cnt = -1);

	StabEnt			*contextStack[MAX_CONTEXT_STACK];
	int				contextCount;
	StabEnt			**stab;
	long			stabLen;
};

StabEnt *			FindSymbolInCtxt(const string nm, StabEnt *contxt);
StabEnt *			FindTypedSymbolInCtxt(const string nm, base_type_t type, StabEnt *contxt);
Instance *			FindInstance(StabEnt *contxt, string nm);
StabEnt				*DupSymbol(StabEnt *S, StabEnt *C);
StabEnt				*DefineSymbol(const string nm, base_type_t typ, int8 ndim, void *val,
						StabEnt *context, ref_type_t reft,
						bool isClone, bool isEnv, short disp_mode);
Pool				*findPool(const string &nm, short def_cnt = -1);
Sample				*findSample(const string &nm, short def_cnt = -1);
Voice				*findVoice(const string &nm, short def_cnt = -1);
Channel				*findChannel(const string &nm, short def_cnt = -1);
Lambda				*findMethod(const string &nm, short def_cnt = -1);
Block				*findMethodMain(const string &nm, short def_cnt = -1);
StabEnt				*findBuiltin(const string &nm, short def_cnt = -1);

QuaPort *findQuaPort(const int type, const string &nm, int direction=0, int nports=-1);

string makeUniqueName(const string &nm);
status_t LoadQuaSymbols(FILE *fp, StabEnt *c, Qua *u,
							bool loadQ);
extern SymTab		glob;
extern void			define_global_symbols();

#endif
