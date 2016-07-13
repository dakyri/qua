#include "qua_version.h"

#include "tinyxml2.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "Base64.h"
#include "QuasiStack.h"
#include "Sym.h"
#include "Qua.h"
#include "ControllerBridge.h"
#include "Lambda.h"
#include "Block.h"
#include "Pool.h"
#include "Pool.h"
#include "Sample.h"
#include "Instance.h"
#include "Channel.h"
#include "VstPlugin.h"
#include "Dictionary.h"

#include "QuaDisplay.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#ifdef QUA_V_AUDIO
#include "QuaAudio.h"
#endif

#ifdef QUA_V_DEBUG_CONSOLE
flag	debug_stack = 0;
#define QDBMSG_STK(X,Y,Z) if (debug_stack) fprintf(stderr, X, Y, Z);
#else
#define QDBMSG_STK(X,Y,Z)
#endif

Stackable::Stackable(StabEnt *s)
{
	sym = s;
	stackSize = 0;
}

Stackable::~Stackable()
{
	;
}

QuasiStack::QuasiStack(	class StabEnt *ctxt,
						Stacker *i,
						StabEnt *stkrSym,
						Block *block,
						Block *list,
						QuasiStack *l,
						TimeKeeper *uq,
						char *lbl)
{
#ifdef QUA_V_AUDIO
	hasAudio = AUDIO_NONE;
#endif
	context = ctxt;
	stackerSym = stkrSym;
	stacker = i;
	timeKeeper = uq;
	mulch = nullptr;
	stk.vars = nullptr;
	if (lbl) {
		label = new char[strlen(lbl)+1];
		strcpy(label, lbl);
	} else {
		label = nullptr;
	}

	stackable = ctxt->StackableValue();
	if (stackable == nullptr) {
		uq = nullptr;
	}
	
	if (stackable) {
		stackable->addStack(this);
	}

	Lambda	*So = (context->type == TypedValue::S_LAMBDA && stackable)?
					((Lambda*)stackable):nullptr;
	isLocus =	So?	So->isLocus:false;
	callingBlock = block;
	enclosingList = list;
	locusStatus = ((So && (So->isModal || So->isOncer || So->isHeld))?
						STATUS_SLEEPING:STATUS_RUNNING);
	lowerFrame = l;
	isActive = false;
	isLeaf = false;

#ifdef QUA_V_DEBUG_CONSOLE
	if (debug_stack) {
		fprintf(stderr, "QuasiStack(%s call of %s",
			isLocus?"locus":"", context->name);
		if (l) {
			fprintf(stderr, ", lower frame ");
			if (l->context)
				fprintf(stderr, "%s", l->context->name);
			else
				fprintf(stderr, "no context");
		}
		if (block || list) {
			fprintf(stderr, ", by block %x from list %x", block, list);
		} else {
			fprintf(stderr, ", irregular, block == list == nullptr");
		} fprintf(stderr, ")\n");
	}
#endif
	
	mulch = nullptr;
	CheckMulchSize();
	
//	stacker->uberQua->display.CreateStackBridge(this);

	if (callingBlock) {
		// not precisely sure if this is the right place
		// for this stuff ....
		// probably for the order of initialisations
		switch (callingBlock->type) {
			case Block::C_TUNEDSAMPLE_PLAYER: 
			case Block::C_MIDI_PLAYER: 
			case Block::C_SAMPLE_PLAYER: 
#ifdef QUA_V_AUDIO
				SetAudio(AUDIO_HAS_PLAYER);
#endif
				break;
#ifdef QUA_V_VST_HOST
			case Block::C_VST: {
				VstPlugin	*vst = callingBlock->crap.call.crap.vstplugin;
				if (vst) {
					if (vst->status != VST_PLUG_LOADED) {
						vst->Load();
					}
					if (vst->status == VST_PLUG_LOADED) {
						stk.afx = vst->AEffectInstance();
						if (stk.afx == nullptr) {
							reportError("Can't instantiate %s", vst->sym->name.c_str());
						} else {
							vst->Open(stk.afx);
							vst->MainsOn(stk.afx);
							vst->ConnectOutput(stk.afx, 0);
							vst->ConnectOutput(stk.afx, 1);
//							vst->TestDrive(afx);
							QDBMSG_STK("QuasiStack::creating vst %s -> %x\n", vst->sym->name, stk.afx);
						}
					} else {
						reportError("can't load vst plugin");
					}
				}
				SetAudio(vst->isSynth?AUDIO_HAS_PLAYER:AUDIO_HAS_FX);
				break;
			}
#endif
		}
	}
	
#ifndef INIT_CONTROL_VAR_ONLY
	for (StabEnt *p=context->children; p; p=p->sibling) {
		if (p->refType == TypedValue::REF_STACK) {
			p->Initialize(this);
		}
	}
#endif

	if (stackable) {
		long	cind=0;
		if (callingBlock) {
// ???????
//  need a thorough check which params aren't set here.
			for (	Block *p=callingBlock->crap.call.parameters; 
					p!= nullptr;
					p=p->next) {
				cind++;
			}

			for (int i=cind; i< stackable->countControllers(); i++) {
/*				QuaControllerBridge	*p = new QuaControllerBridge(
									timeKeeper,
									stacker,
									this,
									stackable->Controller(i),
									nullptr);*/
#ifdef INIT_CONTROL_VAR_ONLY
				stackable->Controller(i)->Initialize(this);
#endif
/*				controllerBridge.AddItem(p);*/
			}

		}
	
	}
}

/*
QuaControllerBridge *
QuasiStack::ControllerBridgeFor(StabEnt *s)
{
	QuaControllerBridge *p;
	int i;
	for (i=0; i<NControllerBridge(); i++) {
		p=ControllerBridge(i);
		if (p->sym == s) {
			break;
		}
	}
	return p;
}*/

/**
 * allocates the temporary area for values
 * deletes any current mulch.
 */
bool
QuasiStack::CheckMulchSize()
{
	if (callingBlock && callingBlock->type == Block::C_VST) {
		return true;
	}
	if (stackable) {
		if (stackable->sym && stackable->sym->type == TypedValue::S_VST_PLUGIN) {
			return true;
		}
		QDBMSG_STK("\tCheckmulchsize: stack allocs %d stack for %x%\n", stackable->stackSize, stackable);
		if (mulch) {
			delete mulch;
		}
		mulch =  new char[stackable->stackSize+8];
		memset(mulch, 0, stackable->stackSize+8);
	 	stk.vars = (uchar *)mulch;
		short	pad = (short) (((uint32)mulch) % 8);
		if (pad) stk.vars += (8-pad);
		QDBMSG_STK("\tmulch = %x, stack = %x\n", mulch, stk.vars);
	}
	return true;
}


QuasiStack::~QuasiStack()
{
	if (callingBlock) {
		switch (callingBlock->type) {
#ifdef QUA_V_VST_HOST
			case Block::C_VST: {
				VstPlugin	*vst = callingBlock->crap.call.crap.vstplugin;
				if (vst) {
					vst->MainsOff(stk.afx);
					vst->Close(stk.afx); // which deletes the instance!
				}
				SetAudio(vst->isSynth?AUDIO_HAS_PLAYER:AUDIO_HAS_FX);
				break;
			}
#endif
		}
	}
	/*
	int i;
	for (i=0; i<NControllerBridge(); i++) {
		delete ControllerBridge(i);
	}*/
	
	if (label)
		delete label;
		
	if (stackable) {
		stackable->removeStack(this);
	}
	for (QuasiStack *q: higherFrame) {
		delete q;
	}
	if (mulch) {
		free(mulch);
//		delete mulch;
	}
}


bool
QuasiStack::Trigger()
{
	Lambda		*lambda = context->LambdaValue();
	if (lambda && lambda->isInit) {
		if (lambda->mainBlock)
			lambda->mainBlock->Reset(this);
	}
	locusStatus = STATUS_RUNNING;

	if (lambda && lambda->isModal) {	// turn off siblings
		;
		
		for (QuasiStack *n: lowerFrame->higherFrame) {
			if (n->enclosingList == enclosingList && n != this) {
				n->UnTrigger();
			}
		}
	}

	if (stacker) {
		stacker->uberQua->bridge.DisplayStatus(this);
	}
	return true;
}

bool
QuasiStack::UnTrigger()
{
	locusStatus = STATUS_SLEEPING;
	stacker->uberQua->bridge.DisplayStatus(this);
	return true;
}

bool
QuasiStack::Delete()
{
//	stacker->uberQua->display.RemoveStackBridge(this);
	;
	
	for (QuasiStack *n: higherFrame) {
		n->Delete();
	}
	delete this;
	return true;
}


bool
QuasiStack::Thunk()
{
	QDBMSG_STK("Thunk(%s%x, ", isLeaf?"leaf ":"",this)
	QDBMSG_STK("%d higher frames, exec %x)\n", higherFrame.CountItems(),stackable);

	if (countFrames() == 0 && !isLeaf && context->type == TypedValue::S_LAMBDA) {
		Lambda	*executable = (Lambda *)stackable;
		if (executable && executable->mainBlock) {
			if (!executable->mainBlock->StackOMatic(this, 0)) {
				return true;
			}		
		}
	} else {
		return false;
	}
	return false;
}


bool
QuasiStack::UnThunk()
{
	// no garbage collect!
	// XXX wtf !!!!!!
	return false;
}

bool
QuasiStack::GetFrameMap(frame_map_hdr *&map)
{
	short i;
	frame_map_hdr	*p;
	long	map_len;
	map_len = sizeof(frame_map_hdr) * (countFrames()+1);
	map_len += stackable->countControllers()*(sizeof(StabEnt *));
	map_len += countFrames()*(sizeof(QuasiStack *));

	char	*cmap = (char *)malloc(map_len);
	map = (frame_map_hdr *)cmap;
	p = map;
	p->context = context;
	p->frame_handle = this;
	p->n_children = countFrames();
	p->n_controller = stackable->countControllers();
	p->status = locusStatus;
	cmap += sizeof(frame_map_hdr);
	StabEnt	**q = (StabEnt **)cmap;
	for (i=0; i<stackable->countControllers(); i++) {
		*q = stackable->controller(i);
		q++;
	}
	cmap += sizeof(StabEnt *) * stackable->countControllers();
	QuasiStack **r = (QuasiStack **)cmap;
	for (i=0; i<countFrames(); i++) {
		if (frameAt(i)) {
//			fprintf(stderr, "\tname %s\n", stackable->Controller(i)->uniqueName());
		}
		*r++ = frameAt(i);
	}

	return true;
}

void
QuasiStack::Dump()
{
	if (stackable) {
		for (short i=0; i<stackable->stackSize; i++)
			fprintf(stderr, "%02x ", stk.vars[i]);
		fprintf(stderr, "\n");
	}
}

status_t
QuasiStack::Save(FILE *fp, short indent)
{
	bool		comma = false;
	status_t	err=B_NO_ERROR;
	tab(fp, indent); fprintf(fp, "%s(", stackable->sym->printableName().c_str());
	QuaControllerBridge *p;
	int				i;
	/*
	for (i=0; i<NControllerBridge(); i++) {
		p = ControllerBridge(i);
		if (comma) fprintf(fp, ","); else comma = true;
		fprintf(fp, "\n");
		tab(fp, indent+1);
		err = p->Save(fp, indent);
	}*/
	bool doSave=false;
	for (QuasiStack *s: higherFrame) {
		if (s->IsInteresting())
			doSave = true;
	}
	
	if (doSave) {
		/*
		if (controllerBridge.CountItems())
			fprintf(fp, ", ");*/
		fprintf(fp, "{\n");
		comma = false;
		for (QuasiStack *s: higherFrame) {
			if (comma) fprintf(fp, ",\n"); else comma = true;
			s->Save(fp, indent+1);
		}
		fprintf(fp, "}");
	}
	fprintf(fp, ")");
	return err;
}
status_t
QuasiStack::SaveSnapshot(FILE *fp, const char *label)
{
	status_t	err=B_NO_ERROR;

	bool saveChildStacks = true;
	if (stackable) {
		bool saveBinaryValues = true;
		bool saveSymbolicValues = true;
		if (callingBlock && callingBlock->type == Block::C_VST) {
			VstPlugin	*vst = callingBlock->crap.call.crap.vstplugin;
			if (vst->programChunks) {
#ifdef QUA_V_VST_HOST
				void		*chunkPtr;
				long		chunkLen;
				chunkLen = vst->GetChunk(stk.afx, &chunkPtr);
				if (chunkLen > 0) {
					fprintf(fp,
						"<stack name=\"%s\" type=\"vst\" saveData=\"chunk\" length=\"%d\" encoding=\"base64\">\n",
						label, chunkLen);
					std::string outb = Base64::Encode((uchar *)chunkPtr, chunkLen);
					fprintf(fp, "%s\n", outb.c_str());
				} else {
					fprintf(fp,
						"<stack name=\"%s\" type=\"vst\" length=\"0\" encoding=\"base64\">\n",
						label);
				}
#endif
			} else { // programs are not chunks!
				fprintf(fp,
					"<stack name=\"%s\" type=\"vst\" length=\"0\" encoding=\"base64\">\n",
					label);
			}

		} else {
			if (stackable->stackSize > 0 && saveBinaryValues) {
				fprintf(fp, "<stack name=\"%s\" length=\"%d\" encoding=\"base64\">\n", label, stackable->stackSize);
				std::string outb = Base64::Encode((uchar *)stk.vars, stackable->stackSize);
				fprintf(fp, "%s\n", outb.c_str());
			} else {
				fprintf(fp, "<stack name=\"%s\">\n", label);
			}
		}

		if (stackable->sym) {
			StabEnt	*p = stackable->sym->children;
			while (p!= nullptr) {
				switch (p->type) {
					case TypedValue::S_VST_PROGRAM: {
						char buf[256];
						sprintf(buf, "type=\"vstprogram\"");
						err=p->SaveSimpleTypeSnapshot(fp, stacker, this, buf);
						break;
					}
#ifdef QUA_V_VST_HOST
					case TypedValue::S_VST_PARAM: {
						char buf[256];
						sprintf(buf, "type=\"vstparam\" position=\"%d\"", p->VstParamValue());
						err=p->SaveSimpleTypeSnapshot(fp, stacker, this, buf);
						break;
					}
#endif
					case TypedValue::S_BOOL:
					case TypedValue::S_SHORT:
					case TypedValue::S_BYTE:
					case TypedValue::S_INT:
					case TypedValue::S_TIME:
					case TypedValue::S_FLOAT:
					case TypedValue::S_LONG:
					case TypedValue::S_DOUBLE: {
						if (p->refType == TypedValue::REF_STACK) {
							err=p->SaveSimpleTypeSnapshot(fp, stacker, this);
						}
						break;
					}

					case TypedValue::S_CHANNEL: {
						if (p->refType == TypedValue::REF_STACK) {
							err=p->SaveSimpleTypeSnapshot(fp, stacker, this);
						}
						break;
					}
					case TypedValue::S_TAKE: {
						break;
					}
					case TypedValue::S_CLIP: {
						if (p->refType == TypedValue::REF_STACK) {
							err=p->SaveSimpleTypeSnapshot(fp, stacker, this);
						}
						break;
					}
				}
				p = p->sibling;
			}
		}
	} else {
		fprintf(fp, "<stack name=\"%s\">\n", label);
	}
	if (saveChildStacks) {
		for (short i=0; i<countFrames(); i++) {
			QuasiStack	*s = frameAt(i);
			std::string chlabel = "child";
			if (s->callingBlock) {
				switch (s->callingBlock->type) {
					case Block::C_CALL:
						if (s->callingBlock->crap.call.crap.lambda) {
							chlabel = s->callingBlock->crap.call.crap.lambda->sym->name;
						} else {
							chlabel = "call";
						}
						break;
					case Block::C_VST:
						if (s->callingBlock->crap.call.crap.vstplugin) {
							chlabel = s->callingBlock->crap.call.crap.vstplugin->sym->name;
						} else {
							chlabel = "vst";
						}
						break;

					case Block::C_GENERIC_PLAYER:
					case Block::C_MIDI_PLAYER:
					case Block::C_SAMPLE_PLAYER:
					case Block::C_TUNEDSAMPLE_PLAYER:
					case Block::C_STREAM_PLAYER:
					case Block::C_MARKOV_PLAYER:
						chlabel = findClipPlayer(s->callingBlock->type);
						break;
				}
			}
			s->SaveSnapshot(fp, chlabel.c_str());
		}
	}
	fprintf(fp, "</stack>\n");
	return err;
}


status_t
QuasiStack::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *valAttrVal = element->Attribute("value");
	const char *positionVal = element->Attribute("position");
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *encodingAttrVal = element->Attribute("encoding");

	std::string valAttr;
	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;
	int		position = 0;

	if (valAttrVal != nullptr) {
		valAttr = valAttrVal;
	}
	if (positionVal != nullptr) {
		position = atoi(valAttrVal);
	}
	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}
	if (encodingAttrVal != nullptr) {
		if (std::string(encodingAttrVal) == "base64") {
			encoding = 1;
		}
	}

	std::string namestr = element->Value();
	long		childCount = 0;
	if (namestr == "snapshot") {
		std::vector<std::string> textFragments;
		LoadSnapshotChildren(element, textFragments);
	} else if (namestr == "stack") {
		std::vector<std::string> textFragments;
		LoadSnapshotChildren(element, textFragments);
	} else if ((namestr == "fixed") || namestr == "envelope") {
//		reportError("%s nm %s", nameAttr, context?context->name.c_str():"nul");
		if (hasNameAttr && context!=nullptr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, context);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, stacker, stackerSym, this);	
			}
		}
	} else {
		;
	}

	return B_OK;
}

status_t
QuasiStack::LoadSnapshotChildren(tinyxml2::XMLElement *element, std::vector<std::string> textFragments)
{
	tinyxml2::XMLNode*childNode = element->FirstChildElement();
	int childStackCount = 0;
	while (childNode != nullptr) {
		tinyxml2::XMLText *textNode;
		tinyxml2::XMLElement *elementNode;
		if ((textNode = childNode->ToText()) != nullptr) {
			textFragments.push_back(textNode->Value());
		}
		else if ((elementNode = childNode->ToElement()) != nullptr) {
			const char *namestr = elementNode->Value();
			if (namestr == "stack") {
				QuasiStack	*child = nullptr;
				if (childStackCount < countFrames()) {
					child = frameAt(childStackCount);
				}
				if (child) {
					child->LoadSnapshotElement(elementNode);
				}
				childStackCount++;
			} else {
				LoadSnapshotElement(elementNode);
			}
		}

		childNode = childNode->NextSibling();
	}

	return B_OK;
}

bool
QuasiStack::SetValue(Block *b)
{
	QDBMSG_STK("stack %s set value %x\n", stackable->sym->name, b);
	if (!b)
		return true;
	if (b->type == Block::C_CALL) {
		if (b->crap.call.crap.lambda->sym != stackable->sym) {
			return false;
		}
	} else if (b->type == Block::C_WAKE) {
		if (b->crap.call.crap.sym != stackable->sym) {
			return false;
		}
	} else if (b->type == Block::C_UNLINKED_CALL) {
		if (strcmp(b->crap.call.crap.name, stackable->sym->name.c_str()) != 0)
			return false;
	} else {
		return false;
	}
	
	Block *par = b->crap.call.parameters;
	int		i;
	for (i=0; i<stackable->countControllers() && par!=nullptr; i++, par=par->next) {
/*		ControllerBridge(i)->SetValue(par);*/
		StabEnt	*sym = stackable->controller(i);
		ResultValue v = EvaluateExpression(par);
		LValue	lval;
		sym->SetLValue(lval, 0, stacker, stackerSym, this);
		lval.StoreValue(&v);
	}
	
	if (par && par->type == Block::C_LIST) {
		Block	*stk = par->crap.list.block;
		for (short i=0; i<higherFrame.size() && stk!=nullptr; i++, stk=stk->next) {
			higherFrame[i]->SetValue(stk);
		}
	}
	return true;
}

bool
QuasiStack::IsInteresting()
{
	if (callingBlock == nullptr) {
		return true;
	}
	long npassed = 0;
	for (	Block *p=callingBlock->crap.call.parameters; 
			p!= nullptr;
			p=p->next) {
		npassed++;
	}

	if (stackable->countControllers() != npassed)
		return true;
/*	if (controllerBridge.CountItems() > 0)
		return true;*/
	for (QuasiStack *q: higherFrame) {
		if (q && q->IsInteresting())
			return true;
	}
	return false;
}

bool
QuasiStack::RemoveHigherFrames()
{
	auto ci = higherFrame.begin();
	while (higherFrame.size() > 0) {
		QuasiStack	*q = *ci;
		ci = higherFrame.erase(ci);
		if (q) {
			q->RemoveHigherFrames();
			delete q;
		}
	}
	return true;
}

bool
QuasiStack::RemoveControlVariables()
{
	if (stacker && stacker->uberQua) {
		stacker->uberQua->bridge.RemoveControlVariables(this);
		return true;
	}
	return false;
}

bool
QuasiStack::AddControlVariables()
{
	if (stacker && stacker->uberQua) {
		stacker->uberQua->bridge.AddControlVariables(this);
		return true;
	}
	return false;
}

bool
QuasiStack::RemoveHigherFrameRepresentations()
{
	if (stacker && stacker->uberQua) {
		stacker->uberQua->bridge.RemoveHigherFrameRepresentations(this);
		return true;
	}
	return false;
}

bool
QuasiStack::PopHigherFrameRepresentations()
{
	if (stacker && stacker->uberQua) {
		stacker->uberQua->bridge.PopHigherFrameRepresentations(this);
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////////
// Stackable
///////////////////////////////////////////////////////////////////////////////
status_t
Stackable::saveControllers(FILE *fp, int indent)
{
	for (short i=0; i<countControllers(); i++) {
		StabEnt	*ctlsym = controller(i);
		if (  ctlsym->refType == TypedValue::REF_POINTER ||
			ctlsym->refType == TypedValue::REF_INSTANCE) {
		} else {
			ctlsym->SaveSimpleBits(fp,0);
			if (i < countControllers()-1) {
				fprintf(fp, ",\n");
				tab(fp, indent);
			}
		}
	}
	return B_NO_ERROR;
}


void
Stackable::AllocateDataStacks(StabEnt *s)
{
	if (s->isDeleted || s->refType == TypedValue::REF_INSTANCE)
		return;

	if (s->refType == TypedValue::REF_STACK) {
		long	nobj=1;
		for (short i=1; i<=sym->indirection; i++) {
			nobj *= sym->size[i];
		}
		
		switch (s->type) {
			case TypedValue::S_BOOL:
					
			case TypedValue::S_BYTE:
			case TypedValue::S_SHORT:
			case TypedValue::S_INT:
			case TypedValue::S_LONG:
			case TypedValue::S_NOTE:
			case TypedValue::S_CTRL:
			case TypedValue::S_PROG:
			case TypedValue::S_BEND:
			case TypedValue::S_SYSC:
			case TypedValue::S_SYSX:
			case TypedValue::S_MESSAGE:
			case TypedValue::S_JOY:
			case TypedValue::S_FLOAT:
				s->SetStackReference(sym, AllocStack(s->context, s->type, nobj));
				break;
			
			case TypedValue::S_STRUCT: {
				s->size[0] = 0;
				s->SetStackReference(sym, AllocStack(s->context, TypedValue::S_STRUCT, nobj));
				for (StabEnt *p=s->children; p!=nullptr; p=p->sibling) {
					AllocateDataStacks(p);
				}
				EndStructureAllocation(sym);
				break;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
//   currently only called by data recalculation due to modifying symbols
// called:
//    * on object modification by interface thread
///////////////////////////////////////////////////////////////////////
void
Stackable::ReAllocateChildren()
{
	int		i;
	for (i=0; i<stacksO.size(); i++) {
		QuasiStack	*qs = stacksO.at(i);
		qs->RemoveControlVariables();
//		q->bridge.RemoveControlVariables(qs);
	}
#ifdef LOTSALOX
	stackableLock.Lock();
#endif
	stackSize = 0;
	AllocateDataStacks(sym);
	if (Executable *E=sym->ExecutableValue()) {
		if (E->mainBlock) {	// this allocates space allocated to execution states
			E->mainBlock->AllocateExecStack(this);
		}
	}
	
	for (QuasiStack *qs: stacksO) {
		qs->CheckMulchSize();
		for (StabEnt *p=sym->children; p!=nullptr; p=p->sibling) {
			p->Initialize(qs);
		}
	}

#ifdef LOTSALOX
	stackableLock.Unlock();
#else
#endif	
	for (i=0; i<stacksO.size(); i++) {
		QuasiStack	*qs = (QuasiStack*)stacksO.at(i);
		qs->AddControlVariables();
//		q->bridge.AddControlVariables(qs);
	}
}

// called:
//    * on object modification by interface thread
//	  * by as yet unimplimented block creation deletion inside main qua thread
// - needs full lock for write access
// - might already have a read level lock. ??????
void
Stackable::ReplaceBlock(Block *&blockv, Block*b)
{
	int		i;

	for (i=0; i<stacksO.size(); i++) {
		QuasiStack	*qs = stacksO.at(i);
		if (qs) {
			qs->RemoveHigherFrameRepresentations();
		}
	}
	
#ifdef LOTSALOX
	stackableLock.Lock();
#else
//	uberQua->objectsBlockStack.Lock();
#endif
	for (QuasiStack *qs: stacksO) {
		qs->CheckMulchSize();		// Parse() will have reallocated system vars.
		if (qs->RemoveHigherFrames() && b) {
			b->StackOMatic(qs,1);
			b->Reset(qs);
		}
	}
	Block	*oldBlock = blockv;
	blockv = b;
#ifdef LOTSALOX
	stackableLock.Unlock();
#else
//	uberQua->objectsBlockStack.Unlock();
#endif	
	if (oldBlock)
		oldBlock->DeleteAll();

	for (i=0; i<stacksO.size(); i++) {
		QuasiStack	*qs = stacksO.at(i);
		if (qs) {
			qs->PopHigherFrameRepresentations();
		}
	}
}


long
AllocStack(StabEnt *contxt, base_type_t typ, int nelem)
{
	stack_size_t	size = 1;
	stack_size_t	align = 1;
	long			off = 0;
		
	size = GetTypeSize(typ);
	align = GetTypeAlign(typ);

	if (contxt) {
		if (size > 0) {
			short	pad = 0;
			if (contxt->size[0]) {
				pad = ((short)contxt->size[0]) % align;
			}
			contxt->size += pad;
			off = contxt->size[0];
			Stackable	*e = contxt->StackableValue();
			contxt->size[0] += nelem*size;
			if (e) {
				e->stackSize = (stack_size_t)contxt->size[0];
			}
#ifdef QUA_V_DEBUG_CONSOLE
			if (debug_stack) {
				fprintf(stderr, "AllocStack: %d * %d bytes, type %d in %s size %d\n",
						nelem, size, typ, contxt->name, contxt->size[0]);
			}
#endif
		}
		return off;
	}
	return 0;
}

/*
 * get the size for a particular type. only needed for types that
 * could be declared on the stack.
 */
stack_size_t
GetTypeSize(base_type_t typ)
{
	stack_size_t	size = 0;
	switch (typ) {
	case TypedValue::S_DOUBLE:	size = 8;break;
	case TypedValue::S_FLOAT:	size = 4;break;
	case TypedValue::S_BOOL:
	case TypedValue::S_BYTE:	size = 1; break;
	case TypedValue::S_SHORT:	size = 2; break;
	case TypedValue::S_INT:		size = 4; break;
	case TypedValue::S_LONG:	size = 8; break;
	case TypedValue::S_STRANGE_POINTER:	size = 4; break;
	case TypedValue::S_STRUCT:	size = 0; break;
	case TypedValue::S_NOTE:	size = sizeof(Note); break;
	case TypedValue::S_CTRL:	size = sizeof(Ctrl); break;
	case TypedValue::S_PROG:	size = sizeof(Prog); break;
	case TypedValue::S_SYSC:	size = sizeof(SysC); break;
	case TypedValue::S_JOY:		size = sizeof(QuaJoy); break;
	case TypedValue::S_SYSX:	size = sizeof(SysX); break;
	case TypedValue::S_TIME:	size = sizeof(qua_time); break;
	case TypedValue::S_CLIP:	size = sizeof(Clip); break;
	}
	return size;
}

stack_size_t
GetTypeAlign(base_type_t typ)
{
	stack_size_t align = 1;
	switch (typ) {
	case TypedValue::S_FLOAT:
	case TypedValue::S_TIME:
	case TypedValue::S_CLIP:
	case TypedValue::S_STRANGE_POINTER: return 4;
	case TypedValue::S_BOOL:
	case TypedValue::S_BYTE:
	case TypedValue::S_SHORT:	
	case TypedValue::S_INT: 
	case TypedValue::S_LONG:	return 1;
	case TypedValue::S_STRUCT:	return 1; // maybe this should be 4 ... allow for floats????!!!?????
	}
	return align;
}

void
EndStructureAllocation(StabEnt *sym)
{
	long size = sym->AllocationLength();
	if (sym) {
		if (size > 0) {
			sym->context->size[0] += size;
			Stackable	*e = sym->context->StackableValue();
			if (e) {
				e->stackSize = (stack_size_t) sym->context->size[0];
			}
		}
	}
}
