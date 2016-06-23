#include "qua_version.h"

#include "StdDefs.h"

#include "Block.h"
#include "Expression.h"
#include "Qua.h"
#include "Voice.h"
#include "Pool.h"
#include "Envelope.h"
#include "VstPlugin.h"
#include "Other.h"
#include "MidiDefs.h"

#include <ctype.h>

int			debug_exp = 0;


/*
 * StringToNote:
 */
int
stringToNote(char *buf)
{
    short	pitch, rej;

    switch(*buf) {
    	case 'a':	pitch = NOTE_A; break;
    	case 'b':	pitch = NOTE_B; break;
    	case 'c':	pitch = NOTE_C; break;
    	case 'd':	pitch = NOTE_D; break;
    	case 'e':	pitch = NOTE_E; break;
    	case 'f':	pitch = NOTE_F; break;
    	case 'g':	pitch = NOTE_G; break;
		default:	return -1;
    }
    buf++;
    if (*buf == 'b') {
        pitch--;
		buf++;
    } else if (*buf == '#') {
        pitch++;
		buf++;
    }
    
    if (isdigit(*buf)) {
    	rej = *buf - '0';
    } else {
		rej = 0;
    }

    pitch = pitch + 12 * rej;
    return pitch;
}


/*
 * EvaluateExpression:
 */


ResultValue
EvaluateBuiltIn(Block *block, StreamItem *items,  Stacker *stacker, StabEnt *stackCtxt, QuasiStack *stack)
{

	ResultValue		ret_val;
	
    switch (block->subType) {
#ifdef QUA_V_OLD_BUILTIN
    case Block::BUILTIN_NOTE: {
		ret_val = CreateNewNote(block->type,
				block->crap.call.parameters, items, stacker, stackCtxt, stack);	 
	 	break;
    }
    
#ifdef QUA_V_APP_HANDLER
    case  Block::BUILTIN_MESG: {
		ret_val = CreateNewMesg(block->crap.call.parameters,
				items, stacker, stack);
		break;	 
    }
#endif
    
    case  Block::BUILTIN_SYSX: {
		ret_val = CreateNewSysX(block->crap.call.parameters,
				items, stacker, stackCtxt, stack);
		break;	 
    }
    
    case  Block::BUILTIN_SYSC: {
		ret_val = CreateNewSysC(block->crap.call.parameters->crap.constant.value.IntValue(nullptr),
								block->crap.call.parameters->next,
								items, stacker, stackCtxt, stack);
		break;	 
    }
    
    case  Block::BUILTIN_CTRL: {
		ret_val = CreateNewCtrl(block->crap.call.parameters,
				items, stacker, stackCtxt, stack);
		break;	 
    }
    
    case  Block::BUILTIN_BEND: {
		ret_val = CreateNewBend(block->crap.call.parameters,
				items, stacker, stackCtxt, stack);
		break;	 
    }
    
    case  Block::BUILTIN_PROG: {
		ret_val = CreateNewProg(block->crap.call.parameters,
				items, stacker, stackCtxt, stack);
		break;	 
    }
#else
    case Block::BUILTIN_CREATE: {
		switch (block->crap.call.crap.createType) {
			case TypedValue::S_NOTE: {
				ret_val = CreateNewNote(block->type,
						block->crap.call.parameters, items, stacker, stackCtxt, stack);	 
	 			break;
			}
		    
#ifdef QUA_V_APP_HANDLER
			case TypedValue::TypedValue::S_MESG: {
				ret_val = CreateNewMesg(block->crap.call.parameters,
						items, stacker, stack);
				break;	 
			}
#endif
    
			case  TypedValue::TypedValue::S_SYSX: {
				ret_val = CreateNewSysX(block->crap.call.parameters,
						items, stacker, stackCtxt, stack);
				break;	 
			}
		    
			case  TypedValue::TypedValue::S_SYSC: {
				ret_val = CreateNewSysC(block->crap.call.parameters->crap.constant.value.IntValue(nullptr),
										block->crap.call.parameters->next,
										items, stacker, stackCtxt, stack);
				break;	 
			}
		    
			case  TypedValue::S_CTRL: {
				ret_val = CreateNewCtrl(block->crap.call.parameters,
						items, stacker, stackCtxt, stack);
				break;	 
			}
		    
			case  TypedValue::S_BEND: {
				ret_val = CreateNewBend(block->crap.call.parameters,
						items, stacker, stackCtxt, stack);
				break;	 
			}
		    
			case  TypedValue::S_PROG: {
				ret_val = CreateNewProg(block->crap.call.parameters,
						items, stacker, stackCtxt, stack);
				break;	 
			}
		}
    	break;
    }
    
#endif
    
    case Block::BUILTIN_RAMP: {
    	break;
    }
    
    case Block::BUILTIN_SELECT: {
    	break;
    }
    
    case Block::BUILTIN_ROLL: {
    	ResultValue	range = EvaluateExpression(block->crap.call.parameters, items, stacker, stackCtxt, stack);
		ret_val.type = TypedValue::S_INT;
		if (range.type == TypedValue::S_FLOAT) {
			ret_val.SetValue((int32)roll(range.FloatValue(stack)));
		} else if (range.type == TypedValue::S_INT) {
			ret_val.SetValue((range.IntValue(nullptr)-1) * frandom());
		}
		break;
    }
    
    case Block::BUILTIN_FIND: {
    	Pool	*P = block->crap.call.parameters->crap.sym->PoolValue();
    	Block	*Q = block->crap.call.parameters->next;
		if (P != nullptr) {
			ret_val = Find(P->mainStream, Q);
		} else {
			; // TypedValue::S_UNKNOWN
			ret_val = ResultValue((int32)0,true); //  blocked
		}
		break;
    }
    
    case Block::BUILTIN_DELETE: {
    	Pool	*P = block->crap.call.parameters->crap.sym->PoolValue();
    	long	deletind = EvaluateExpression(block->crap.call.parameters->next,
    						items, stacker, stackCtxt, stack).IntValue(stack);
    	if (block->crap.call.parameters->next) {
    	} else {
	    	ret_val.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
	    	ret_val.SetValue(
	    		(int32)P->mainStream->DeleteRefItems(items));
	    }
	    break;
    }
    
    case Block::BUILTIN_INSERT: {
    	Pool	*P = block->crap.call.parameters->crap.sym->PoolValue();
    	long	clockat = EvaluateExpression(
    						block->crap.call.parameters->next,
    						items, stacker, stackCtxt, stack).IntValue(stack);
    	ResultValue	item = EvaluateExpression(
    						block->crap.call.parameters->next->next,
    						items, stacker, stackCtxt, stack);
    	Time	time(clockat, P->uberQua->metric);
    	ret_val.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
    	ret_val.SetValue((int32)P->mainStream->InsertItem(&time, &item));
    	break;
	}
	
	case Block::BUILTIN_SIN: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)sin(p1));
		break;
	}
	
	case Block::BUILTIN_COS: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)cos(p1));
		break;
	}
	
	case Block::BUILTIN_TAN: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)tan(p1));
		break;
	}
	
	case Block::BUILTIN_ASIN: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)asin(p1));
		break;
	}
	
	case Block::BUILTIN_ACOS: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)acos(p1));
		break;
	}
	
	case Block::BUILTIN_ATAN: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)atan(p1));
		break;
	}
	
	case Block::BUILTIN_SINH: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)sinh(p1));
		break;
	}
	
	case Block::BUILTIN_COSH: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)cosh(p1));
		break;
	}
	
	case Block::BUILTIN_TANH: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)tanh(p1));
		break;
	}
	
	case Block::BUILTIN_EXP: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)exp(p1));
		break;
	}
	
	case Block::BUILTIN_LOG: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)log(p1));
		break;
	}
	
	case Block::BUILTIN_POW: {
    	float	p1 = EvaluateExpression(
    						block->crap.call.parameters,
    						items, stacker, stackCtxt, stack).FloatValue(stack);
    	float	p2 = block->crap.call.parameters?
    						EvaluateExpression(
    						block->crap.call.parameters->next,
    						items, stacker, stackCtxt, stack).FloatValue(stack):0;
		ret_val.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
		ret_val.SetValue((float)pow(p1,p2));
		break;
	}
	}
	
	return ret_val;
}

	
ResultValue
EvaluateExpression(Block *block, StreamItem *items, Stacker *stacker, StabEnt *stackCtxt, QuasiStack *stack)
{
	ResultValue	ret_val;
	
	if (debug_exp>=2 && block)
		fprintf(stderr, "expression type %d\n", block->type);
    if (block == nullptr) {
    	ret_val.flags = ResultValue::COMPLETE;
		return ret_val;
	}
	
	switch (block->type) {

#ifndef STATEMENT_ASSIGNEMTS
	case Block::C_ASSIGN: {
//		int				i;
		StreamItem	*p;
//		StabEnt	*sym;
	
		if (debug_exp >= 2)
		    fprintf(stderr, "assign %x %x str\n", (unsigned)block, (unsigned)block->next);
	
		p = items;
		
		LValue		lval;
		LValueAte(lval,
			block->crap.assign.atom,
			p,
			stacker, 
			stackCtxt,
			stack);
		if (lval.sym && lval.sym->RefersToStream()) {
			while (p != nullptr) {
			    ret_val = EvaluateExpression(
			    			block->crap.assign.exp,
			    			p, stacker, stackCtxt, stack);
				if (debug_exp)
					fprintf(stderr, "EvaluateExpression: stream item assign %x %x lval %x-> %s\n", (unsigned)block, (unsigned)block->next, (unsigned)lval.addr, ret_val.StringValue());
			    if (!ret_val.Blocked()) {
			    	lval.StoreValue(&ret_val);
			    }
				p = p->next;
				if (p) {
					LValueAte(
						lval,
						block->crap.assign.atom,
						p,
						stacker,
						stackCtxt,
						stack);
				}
			}
		} else {
			ret_val = EvaluateExpression(block->crap.assign.exp, items, stacker, stackCtxt, stack);
			if (debug_exp)
				fprintf(stderr, "EvaluateExpression: assign %x %x lval %x -> %s flags %d\n", (unsigned)block, (unsigned)block->next, (unsigned)lval.addr, ret_val.StringValue(), ret_val.flags);
			if (!ret_val.Blocked()) {
			    lval.StoreValue(&ret_val);
				fprintf(stderr, "EvaluateExpression: assign %x %x lval %x -> %s flags %d\n", (unsigned)block, (unsigned)block->next, (unsigned)lval.addr, ret_val.StringValue(), ret_val.flags);
//				lval.sym->UpdateControllerBridge(tag_time, &val, stack);
//?????????????????????????????????????????
				// and if this is a display var, display it
			}
		}
	
		break;
	}
#endif

	case Block::C_ARRAY_REF:
	case Block::C_STRUCTURE_REF:
    case Block::C_SYM: {
		if (debug_exp >= 2) {
		    fprintf(stderr, "eval (*lval) %x\n", (unsigned)block);
		}
		LValue		lval;
		LValueAte(lval,
			block,
			items, 
			stacker,
			stackCtxt,
			stack);
//		fprintf(stderr, "lv = %s\n", lval.CurrentValue().StringValue());
		return lval.CurrentValue();
	}

	case Block::C_IFOP: {
		if (debug_exp >= 2)
			fprintf(stderr, "if: cond %d doeval %d\n",
				block->crap.iff.condVar.IntValue(stack),
				block->crap.iff.doEvalVar.IntValue(stack));
	
	/* maybe the simplest is the best !!! */
		if (block->crap.iff.doEvalVar.BoolValue(stack)) {
			ResultValue v	 = EvaluateExpression(block->crap.iff.condition,
					 items, stacker, stackCtxt, stack);
			if (v.Blocked()) {
//			    blocked->crap.iff.condVar.SetValue((int32)true,stack);
				ret_val.flags = ResultValue::BLOCKED;
				return ret_val;
			}
			block->crap.iff.condVar.SetValue(v.IntValue(stack),stack);
			block->crap.iff.doEvalVar.SetValue((int32)false,stack);
		}
		
		if (block->crap.iff.condVar.IntValue(stack)) {
		    ret_val = EvaluateExpression(
			    	 block->crap.iff.ifBlock,
					 items,
					 stacker,
					 stackCtxt,
					 stack);
		} else {
			ret_val = EvaluateExpression(
						 block->crap.iff.elseBlock,
						 items,
						 stacker,
						 stackCtxt,
						 stack);
		}
		if (ret_val.Complete()) {
			block->crap.iff.doEvalVar.SetValue((int32)true,stack);
		}
		break;
	}

	case Block::C_LIST: {
		if (block->subType == Block::LIST_SEQ) {
	
			Block	*active = (Block*)block->crap.list.stuffVar.IntValue(stack);
			if (active == nullptr)
				active = block->crap.list.block;
			ret_val = EvaluateExpression(active, items, stacker, stackCtxt, stack);
			if ((active = active->next)	== nullptr) {
				ret_val.flags = ResultValue::COMPLETE;
			}
			*((int32*)(block->crap.list.stuffVar.StackAddressValue(stack)))
					 = (int32)active;
			break;
		} else if (block->subType == Block::LIST_NORM) {	// an TypedValue::S_LIST
#ifdef OLD_LIST
			TypedValueList	*L = new TypedValueList();
			ret_val.Set(TypedValue::S_LIST, TypedValue::REF_POINTER);
			ret_val.SetValue(L);
			for (Block *B = block->crap.list.block; B != nullptr;
						B = B->next) {
						
				ResultValue expv = EvaluateExpression(B, items, stacker, stackCtxt, stack);
				if (expv.Blocked()) {
					// should block? leave for now...
					// BIG ????????. ALSO for Block::C_SEQ_LIST
				}
				ret_val.ListValue()->AddItem(expv);
			}
#else
			ret_val = TypedValue::List();
			for (Block *B = block->crap.list.block; B != nullptr;
						B = B->next) {
						
				ResultValue expv = EvaluateExpression(B, items, stacker, stackCtxt, stack);
				if (expv.Blocked()) {
					// should block? leave for now...
					// BIG ????????. ALSO for Block::C_SEQ_LIST
				}
				ret_val.ListValue()->AddItem(&expv);
			}
#endif
			ret_val.flags = ResultValue::COMPLETE;
			break;
		}
		break;
	}
		
	case Block::C_CAST: {
		ResultValue	exp_val = EvaluateExpression(block->crap.cast.block, items, stacker, stackCtxt, stack);
		ret_val.Set(block->crap.cast.type, TypedValue::REF_VALUE);
		ret_val.SetValue(&exp_val);
		ret_val.flags = ResultValue::COMPLETE;
		break;
    }
    
    case Block::C_VALUE: {
		if (debug_exp >= 2)
		    fprintf(stderr, "eval const %x %s\n", (unsigned)block, block->crap.constant.value.StringValue());
		ret_val.Set(&block->crap.constant.value);
		ret_val.flags = ResultValue::COMPLETE;
		break;
    }
    
	case Block::C_BINOP: {
		ResultValue		lv, rv;
	
		if (debug_exp >= 2)
		    fprintf(stderr, "doowop:eval op %x %d\n", (unsigned)block, block->type);
		lv = EvaluateExpression(block->crap.op.l, items, stacker, stackCtxt, stack);
		if (lv.Blocked()) return lv;
	
		rv = EvaluateExpression(block->crap.op.r, items, stacker, stackCtxt, stack);
		if (rv.Blocked()) return rv;
	
		if (debug_exp >= 2)
		    fprintf(stderr, "eval op %x %d\n", (unsigned)block, block->subType);
        switch (block->subType) {
        case Block::OP_GT:	ret_val = (lv > rv); break;
        case Block::OP_LT:	ret_val = lv < rv; break;
        case Block::OP_LE:	ret_val = lv <= rv; break;
        case Block::OP_GE:	ret_val = lv >= rv; break;
        case Block::OP_EQ:	ret_val = lv == rv; break;
        case Block::OP_NEQ:	ret_val = lv != rv; break;

        case Block::OP_AND:	ret_val = lv && rv; break;
        case Block::OP_OR:	ret_val = lv || rv; break;
	
		case Block::OP_ADD:	ret_val = lv + rv; break;
		case Block::OP_SUB:	ret_val = lv - rv; break;
		case Block::OP_MUL: ret_val = (lv * rv); break;
		case Block::OP_DIV: ret_val = lv / rv; break;
		case Block::OP_MOD:ret_val = lv % rv; break;
        default:
			internalError("Unknown binary operator %d\n", block->subType);
        }
        if (debug_exp >= 2) {
        	fprintf(stderr, "ret: %s", lv.StringValue());
        	fprintf(stderr, " op %s", rv.StringValue());
        	fprintf(stderr, " -> %s\n", ret_val.StringValue());
        }
        break;
	}
	
	case Block::C_UNOP: {
		if (debug_exp >= 2)
		    fprintf(stderr, "unoop:eval unop %x %d\n", (unsigned)block, block->subType);
		ResultValue lv = EvaluateExpression(block->crap.op.l, items, stacker, stackCtxt, stack);
		switch (block->subType) {
		case Block::OP_NOT: ret_val = ! lv; break;
		case Block::OP_BNOT: ret_val = ~ lv; break;
		case Block::OP_UMINUS: ret_val = - lv; break;
		}
		break;
	}
	
	default: {
		if (block->type == Block::C_BUILTIN) {
			ret_val = EvaluateBuiltIn(block, items, stacker, stackCtxt, stack);
			if (ret_val.type != TypedValue::S_UNKNOWN) {
				if (debug_exp) {
					fprintf(stderr, "eval builtin %d.\n", ret_val.type);
				}
			} else {
				fprintf(stderr, "unimplimented builtin %d", block->type);
			}
		} else {
			fprintf(stderr,"Unknown expression block %d\n", block->type);
	    }
		break;
	}}
    return ret_val;
}
