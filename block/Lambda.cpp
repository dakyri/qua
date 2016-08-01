#include "qua_version.h"

#include "StdDefs.h"

#include "StdDefs.h"
#include "Qua.h"
#include "Lambda.h"
#include "Sym.h"
#include "Block.h"
#include "StdDefs.h"
#include "QuaDisplay.h"

flag debug_method=0;

status_t
Lambda::Save(ostream &out, short indent)
{
	status_t	err=B_NO_ERROR;
	out << tab(indent);
	out << "function";
	if (isLocus)
		out << " \\node";
	if (isModal)
		out << " \\modal";
	if (isOncer)
		out << " \\oncer";
	if (isFixed)
		out << " \\fixed";
	if (isHeld)
		out << " \\held";
	if (isInit)
		out << " \\reset";
		
	out << " " << sym->name;
	if (countControllers() > 0) {
		out << "(";
		if ((err = saveControllers(out, indent+2)) != B_NO_ERROR)
			return err;;
		out << ")";
	}
//	fprintf(fp,	"\n\display {%s}", uberQua->bridge.DisplayParameterSaveString(sym));
	err = SaveMainBlock(mainBlock, out, indent, sym, false, false, nullptr, nullptr); 
	return err;
}


Lambda::Lambda(const std::string &nm, StabEnt * const context, bool f1, bool f2, bool f3, bool f4, bool f6, bool f7):
	Executable(DefineSymbol(nm, TypedValue::S_LAMBDA, 0,
						this, context,
						TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT))
{
	if (debug_method)
		fprintf(stderr, "Lambda(%s, %d%d%d%d%d%d)\n", nm.c_str(), f1, f2, f3, f4, f6, f7);

	isLocus = f1;
	isModal = f2;
	isOncer = f3;
	isFixed = f4;
	isHeld = f6;
	isInit = f7;
//	interfaceBridge.SetSymbol(sym);
	resetVal.Set(TypedValue::S_UNKNOWN,TypedValue::REF_STACK,0,0);
 }
 
Lambda::Lambda(Lambda *S, StabEnt *contxt):
	Executable(DefineSymbol(S->sym->name, TypedValue::S_LAMBDA, 0,
					this, contxt,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT))
{
 	if (debug_method)
		fprintf(stderr, "Dup Lambda()\n");

	isLocus = S->isLocus;
	isModal = S->isModal;
	isOncer = S->isOncer;
	isFixed = S->isFixed;
	isHeld = S->isHeld;
	isInit = S->isInit;
	if (S->mainBlock) {
		mainBlock = new Block(S->mainBlock, sym, true);
	} else {
		mainBlock = nullptr;
	}
//	interfaceBridge.SetSymbol(sym);
	resetVal.Set(TypedValue::S_UNKNOWN,TypedValue::REF_STACK,0,0);
}

Lambda::~Lambda()
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
Lambda::Init()
{
	Block		*B = mainBlock;
	StabEnt		*C = sym->context;
	StabEnt		*OriginalState = glob.TopContext();
	StabEnt		*p = nullptr;
	fprintf(stderr, "Init() of lambda %s in ctxt %s block %x\n", sym->name.c_str(), C?C->name.c_str() :"<glbl>, block ", (unsigned)B);

//	shit(sym, uberQua->sym);
	glob.PushContext(sym);
		
	if (B && !B->Init(this))
		goto err_ex;
		
	for (p=sym->children; p!=nullptr; p=p->sibling) {
		if (p->type == TypedValue::S_LAMBDA) {
			if (!p->LambdaValue()->Init())
				goto err_ex;
		}
	}
	fprintf(stderr, "exit Init() of lambda %s in ctxt %s block %x\n", sym->name.c_str(), C?C->name.c_str() :"<glbl>, block ", (unsigned)B);
	glob.PopContext(sym);
	return true;

err_ex:
	glob.PopContext(sym);	
	return false;
}
