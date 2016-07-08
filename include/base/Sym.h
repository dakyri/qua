#ifndef _SYM
#define _SYM
 
#include <stdio.h>
#include "StdDefs.h"
#include "BaseVal.h"

namespace tinyxml2 {
	class XMLElement;
}

class ResultValue: public TypedValue
{
public:
	enum {
		BLOCKED = 1,
		COMPLETE = 2,
	};

	inline					ResultValue()
	{
		type = S_UNKNOWN;
		val.Int = 0;
		refType = REF_VALUE;
		indirection = 0;
		flags = COMPLETE;
	}
	
	inline 					ResultValue(int32 t, bool block=false)
	{
		flags = block?BLOCKED:COMPLETE;
		type = t;
		val.Int = 0;
		refType = REF_VALUE;
		indirection = 0;
	}
	
	inline 					ResultValue(TypedValue &v)
	{
		flags = v.flags|COMPLETE;
		type = v.type;
		val = v.val;
		refType = v.refType;
		indirection = 0;
	}

	inline bool Complete() { return (flags & COMPLETE) != 0; }
	inline bool Blocked() { return (flags & BLOCKED) != 0; }
};

class StabEnt;
class QuasiStack;

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
	
	status_t			SaveScript(FILE *fp, short indent, bool saveControllers, bool saveInternal);

	status_t			SaveValue(FILE *fp, Stacker *i, QuasiStack *s);
	status_t			SaveSimpleType(FILE *fp, short indent, bool saveInternal);
	status_t			SaveSimpleBits(FILE *fp, short indent);

	status_t			SaveSnapshot(FILE *fp);
	status_t			SaveSimpleTypeSnapshot(FILE *fp, Stacker *stacker, QuasiStack *qs, char *additionalAttributes=nullptr);
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
	void				Dump(FILE *F, short i);
	
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
	void			DumpGlobals(FILE *fp);
	void			DumpContexts(FILE *fp);
	inline StabEnt	*operator[](long i)
		{ return stab[i]; }

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
Application			*findApplication(const string &nm, short def_cnt = -1);
QuaPort				*findQuaPort(const string &nm, short def_cnt = -1);
StabEnt				*findBuiltin(const string &nm, short def_cnt = -1);

string makeUniqueName(const string &nm);
status_t			LoadQuaSymbols(FILE *fp, StabEnt *c, Qua *u,
							bool loadQ);
extern SymTab		glob;
extern void			define_global_symbols();

#endif
