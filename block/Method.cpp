#include "qua_version.h"

#include "StdDefs.h"

#if defined(WIN32)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(_BEOS)

#ifdef __INTEL__ 
#endif

#endif

#include "StdDefs.h"
#include "Qua.h"
#include "Method.h"
#include "Sym.h"
#include "Block.h"
#include "StdDefs.h"
#if defined(QUA_V_ARRANGER_INTERFACE)
#if defined(WIN32)
#include "QuaDisplay.h"
#elif defined(_BEOS)
#include "QuaObject.h"
#endif
#endif

flag debug_method=0;

status_t
Method::Save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent);
	fprintf(fp, "define");
	if (isLocus)
		fprintf(fp, " #node");
	if (isModal)
		fprintf(fp, " #modal");
	if (isOncer)
		fprintf(fp, " #oncer");
	if (isFixed)
		fprintf(fp, " #fixed");
	if (isHeld)
		fprintf(fp, " #held");
	if (isInit)
		fprintf(fp, " #reset");
		
	fprintf(fp, " %s", sym->name);
	if (countControllers() > 0) {
		fprintf(fp, "(");
		if ((err = saveControllers(fp, indent+2)) != B_NO_ERROR)
			return err;;
		fprintf(fp, ")");
	}
#ifdef QUA_V_ARRANGER_INTERFACE
//	fprintf(fp,	"\n#display {%s}", uberQua->bridge.DisplayParameterSaveString(sym));
#endif
	err = SaveMainBlock(mainBlock, fp, indent, sym, false, false, nullptr, nullptr); 
	return err;
}


Method::Method(std::string nm, StabEnt *context, Qua *q, bool f1,
			bool f2, bool f3, bool f4,
			bool f6, bool f7):
	Executable(DefineSymbol(nm, TypedValue::S_METHOD, 0,
						this, context,
						TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT))
{
	if (debug_method)
		fprintf(stderr, "Method(%s, %d%d%d%d%d%d)\n", nm, f1, f2, f3, f4, f6, f7);

	isLocus = f1;
	isModal = f2;
	isOncer = f3;
	isFixed = f4;
	isHeld = f6;
	isInit = f7;
//	interfaceBridge.SetSymbol(sym);
	resetVal.Set(TypedValue::S_UNKNOWN,TypedValue::REF_STACK,0,0);
 }
 
Method::Method(Method *S, StabEnt *contxt):
	Executable(DefineSymbol(S->sym->name, TypedValue::S_METHOD, 0,
					this, contxt,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT))
{
 	if (debug_method)
		fprintf(stderr, "Dup Method()\n");

	isLocus = S->isLocus;
	isModal = S->isModal;
	isOncer = S->isOncer;
	isFixed = S->isFixed;
	isHeld = S->isHeld;
	isInit = S->isInit;
	if (S->mainBlock) {
		mainBlock = new Block(S->mainBlock, sym, TRUE);
	} else {
		mainBlock = nullptr;
	}
//	interfaceBridge.SetSymbol(sym);
	resetVal.Set(TypedValue::S_UNKNOWN,TypedValue::REF_STACK,0,0);
}

Method::~Method()
{
}

void shit(StabEnt *S, StabEnt *US);
void shit(StabEnt *S, StabEnt *US)
{
	if (S != US) {
		shit(S->context, US);
		glob.PushContext(S);
	}
}

bool
Method::Init()
{
	Block		*B = mainBlock;
	StabEnt		*C = sym->context;
	StabEnt		*OriginalState = glob.TopContext();
	StabEnt		*p = nullptr;
	fprintf(stderr, "Init() of method %s in ctxt %s block %x\n", sym->name,
				C?C->name:"<glbl>, block ", B);

//	shit(sym, uberQua->sym);
	glob.PushContext(sym);
		
	if (B && !B->Init(this))
		goto err_ex;
		
	for (p=sym->children; p!=nullptr; p=p->sibling) {
		if (p->type == TypedValue::S_METHOD) {
			if (!p->MethodValue()->Init())
				goto err_ex;
		}
	}
	fprintf(stderr, "exit Init() of method %s in ctxt %s block %x\n", sym->name,
				C?C->name:"<glbl>, block ", B);
	glob.PopContext(sym);
	return true;

err_ex:
	glob.PopContext(sym);	
	return false;
}
