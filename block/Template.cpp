#include "qua_version.h"



#include "StdDefs.h"


#include "Template.h"
#include "Sym.h"
#include "Block.h"
#include "Schedulable.h"
#include "Method.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "QuaDisplay.h"

status_t
Template::save(FILE *fp, short indent)
{
	status_t err=B_NO_ERROR;
	const char *typenm = qut::unfind(typeIndex, (int)type).c_str();
	
	if (typenm == nullptr) {
		internalError("Invalid type for template %s: not saved");
	}
	
	tab(fp, indent);
	fprintf(fp, "template");

	fprintf(fp, " %s", typenm);
	
	if (mimeType.size())
		fprintf(fp, " \"%s\"", mimeType.c_str());

	fprintf(fp,	" %s", sym->PrintableName());

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


Template::Template(string nm, StabEnt *context,
			long typ, string mim, string pth):
	Executable(DefineSymbol(nm, TypedValue::S_TEMPLATE, 0,
						this, context,
						TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT)),
	rx("Input", sym),
	tx("Output", sym),
	wake("Wake", sym),
	sleep("Sleep", sym),
	receive("Receive", sym),
	cue("Cue", sym),
	start("Start", sym),
	stop("Stop", sym),
	record("Record", sym),
	init("Init", sym)
{
	next = nullptr;
//	interfaceBridge.SetSymbol(sym);
	type = typ;
	mimeType = nullptr;
	path = nullptr;
	
	mimeType = mim;
	path = pth;
	
#ifdef QUA_V_ARRANGER_INTERFACE
	/*
	if (path) {
		BFile		theFile(path, (ulong)B_READ_ONLY);
		BNodeInfo	nodeInfo(&theFile);
		bicon = new BBitmap(BRect(0,0,31,31), B_COLOR_8_BIT);
		sicon = new BBitmap(BRect(0,0,15,15), B_COLOR_8_BIT);
		if (nodeInfo.GetIcon(bicon, B_LARGE_ICON) != B_OK) {
			delete bicon;
			bicon = nullptr;
		}
		if (nodeInfo.GetIcon(sicon, B_MINI_ICON) != B_OK) {
			delete sicon;
			sicon = nullptr;
		}
	}
	
	if (bicon == nullptr && mimeType) {
		BMimeType	mimeT(mimeType);
		bicon = new BBitmap(BRect(0,0,31,31), B_COLOR_8_BIT);
		if (mimeT.GetIcon(bicon, B_LARGE_ICON) != B_OK) {
			delete bicon;
			bicon = nullptr;
		}
	}
	if (sicon == nullptr && mimeType) {
		BMimeType	mimeT(mimeType);
		sicon = new BBitmap(BRect(0,0,15,15), B_COLOR_8_BIT);
		if (mimeT.GetIcon(sicon, B_MINI_ICON) != B_OK) {
			delete sicon;
			sicon = nullptr;
		}
	}*/
#endif
}

Template::~Template()
{

#ifdef QUA_V_ARRANGER_INTERFACE
	/*
	if (sicon)
		delete sicon;
	if (bicon)
		delete bicon;*/
#endif
}

bool
Template::initialize()
{
	Block		*B = mainBlock;
	StabEnt		*C = sym->context;
	StabEnt		*OriginalState = glob.TopContext();

// ????????? don't need to do anything, but this could be
// dangerous.....
//
////	shit(sym, uberQua->sym);
//	glob.PushContext(sym);
//		
////fprintf(stderr, "1 %s\n", name);
//	if (B && !B->Init(this))
//		goto err_ex;
//		
////fprintf(stderr, "3 %s\n", name);
//	for (StabEnt *p=sym->children; p!=nullptr; p=p->sibling) {
//		if (p->type == S_METHOD) {
//			if (!p->MethodValue()->Init())
//				goto err_ex;
//		}
//	}
//	glob.PopContext();
	return true;

err_ex:
//	glob.PopContext();	
////	C = glob.TopContext();
////	while (C != OriginalState) {
////		glob.PopContext();
////		C = glob.TopContext();
////	}
	return false;
}

status_t
Template::instantiate(StabEnt *newContxt)
{
	status_t	err = B_NO_ERROR;

	StabEnt		*p = sym->children, *q;
	
	glob.PushContext(newContxt);
	while (p != nullptr) {

		switch (p->type) {
		case TypedValue::S_EVENT:
			break;
		default:
			q = DupSymbol(p, newContxt);
		}
		
		p = p->sibling;
	}

	for (p=newContxt->children; p!=nullptr; p=p->sibling) {
		if (p->type == TypedValue::S_METHOD) {
			if (!p->MethodValue()->Init())
				internalError("%s not instantiated", p->name);
		}
	}

	Executable		*E = newContxt->ExecutableValue();
	if (E) {
		E->stackSize = stackSize;
		if (mainBlock) {
			E->mainBlock = new Block(mainBlock, newContxt, true);
			if (!E->mainBlock->Init(E)) {
				internalError("Can't initialise main block");
			}
		}
	}

	Schedulable		*S = newContxt->SchedulableValue();
	if (S) {
		S->wake = wake;
		S->sleep = sleep;
		S->start = start;
		S->stop = stop;
		S->record = record;
		S->rx = rx;
	}

	glob.PopContext(newContxt);
	return err;
}