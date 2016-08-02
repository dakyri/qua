#include "qua_version.h"

#include "StdDefs.h"

#include "Block.h"
#include "Sym.h"
#include "Stackable.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "QuaFX.h"
#include "Channel.h"
#include "Lambda.h"

#include "Dictionary.h"

int32 lineLength = 0;

inline void put(const string &s, ostream &out)
{
	out << s;
	lineLength += s.size();
}

inline bool putNL(ostream &out, short indent, short crlf)
{
	lineLength = 0;
	out << endl << tab(indent);
	return true;
}

inline bool checkNL(ostream &out, short indent, short crlf)
{
	if (lineLength > 90) {
		putNL(out, indent, crlf);
		return true;
	}
	return false;
}


bool
Esrap(Block *block, ostream &out, bool do_indent, short indent, short crlf)
{
	bool	addSeperator=true;
	int		cnt=0;
	while (block) {
		cnt++;
		switch (block->type) {
	
		case Block::C_WAIT:
			if (cnt >= 2) putNL(out, indent, crlf);
			put("wait(", out);
			if (!Esrap(block->crap.block, out, false, indent, crlf))
				return false;
			put(")", out);
			break;
	
		case Block::C_FLUX:
			if (cnt >= 2) putNL(out, indent, crlf);
			put("<-", out);
		    if (block->crap.flux.lengthExp) {
		    	put("(", out);
		    	if (!Esrap(block->crap.flux.lengthExp, out, false, indent, crlf)) return false;
		    	put(")", out);
		    }
			if (!Esrap(block->crap.flux.block, out, do_indent, indent+1, crlf)) return false;
			if (block->crap.flux.rateExp) {
				put(" : ", out);
				if (!Esrap(block->crap.flux.rateExp, out, false, indent, crlf)) return false;
			}
			break;
	
		case Block::C_LIST:
			put((
					block->subType == Block::LIST_SEQ? "[":
					block->subType == Block::LIST_FORK? "{&":
					block->subType == Block::LIST_PAR? "{|": "{"), out);
			put(" ", out);
		    if (!Esrap(block->crap.list.block, out, do_indent, indent+1, crlf)) return false;
			put(" ", out);
			put(((char *)(block->subType == Block::LIST_SEQ?"]":"}")), out);
			addSeperator = !do_indent;
		    break;
		
		case Block::C_DIVERT:
			if (!Esrap(block->crap.divert.block, out, do_indent, indent, crlf)) return false;
			put(" @ ", out);
			if (!Esrap(block->crap.divert.clockExp, out, false, indent, crlf)) return false;
			break;
			
		case Block::C_BUILTIN: {
			std::string s;
			if (block->subType == Block::BUILTIN_CREATE) {
				putNL(out, indent, crlf);
				int typ = block->crap.call.crap.createType;
				s = qut::unfind(builtinCommandIndex, typ);
			} else {
				int typ = block->subType;
				s=qut::unfind(builtinCommandIndex, typ);
			}
			if (s.size() == 0) {
				put("undefined", out);
			} else {
				put(s, out);
			}
			Block		*params=nullptr;
			
			params=block->crap.call.parameters;
	
			if (params) {
				put("(", out);
				if(!Esrap(params, out, false, indent, crlf)) return false;
				put(")", out);
				addSeperator = false;
	 	    } else {
				addSeperator = true;
			}
			break;
		}
		
		case Block::C_TUNEDSAMPLE_PLAYER:
		case Block::C_MIDI_PLAYER:
		case Block::C_STREAM_PLAYER:
		case Block::C_GENERIC_PLAYER:
		case Block::C_SAMPLE_PLAYER: {
			std::string s=findClipPlayer(block->type);
			if (s.size() == 0) {
				fprintf(stderr, "not recognised player");
			} else {
				put(s, out);
				Block		*params=nullptr;
			
				params=block->crap.call.parameters;
	
				if (params) {
					put("(", out);
					if(!Esrap(params, out, false, indent, crlf)) return false;
					put(")", out);
					addSeperator = false;
	 			} else {
					addSeperator = true;
				}
			}
			break;
		}
		
		case Block::C_OUTPUT: {
			string b2;
			/* if (cnt >= 2) */putNL(out, indent, crlf);
			b2 = ">> " + to_string(block->crap.channel->chanId);
			addSeperator = false;
			put(b2, out);
			break;
		}
			 
		case Block::C_INPUT: {
			/* if (cnt >= 2) */putNL(out, indent, crlf);
			string b2;
			b2 = "<< " + to_string(block->crap.channel->chanId);
			addSeperator = false;
			put(b2, out);
			break;
		}
	
		case Block::C_UNLINKED_CALL:
		    if (block->name.size()) {
				put(block->name, out);
			    if (block->crap.call.parameters) {
			    	put("(", out);
			    	if (!Esrap(block->crap.call.parameters, out, false, indent, crlf)) return false;
			    	put(")", out);
			    }
				addSeperator = false;
	 	    } else {
				addSeperator = true;
			}
			break;
	
		case Block::C_CALL:
		    if (block->crap.call.crap.lambda) {
				put(block->crap.call.crap.lambda->sym->name, out);
			    if (block->crap.call.parameters) {
			    	put("(", out);
			    	if (!Esrap(block->crap.call.parameters, out, false, indent, crlf)) return false;
			    	put(")", out);
			    }
				addSeperator = false;
	 	    } else
				addSeperator = true;
			break;

//		case Block::C_SAMPLE_PLAYER: // 38,	// play samples at pitch, no frills
//		case Block::C_STREAM_PLAYER: // 39,
//		case Block::C_GENERIC_PLAYER: // 40,

		case Block::C_VST: // 41,
			putNL(out, indent, crlf);
			if (block->crap.call.crap.vstplugin) {
				put(block->crap.call.crap.vstplugin->sym->name, out);
			    if (block->crap.call.parameters) {
			    	put("(", out);
			    	if (!Esrap(block->crap.call.parameters, out, false, indent, crlf)) return false;
			    	put(")", out);
			    }
				addSeperator = false;
			} else {
				addSeperator = true;
			}
			break;
	
			break;

		case Block::C_SCHEDULE: // 42,
		case Block::C_MARKOV_PLAYER: // 44,
//		case Block::C_MIDI_PLAYER: // 45,		// play samples with a midi trigger and pitch
//		case Block::C_TUNEDSAMPLE_PLAYER: // 46,		// play samples with a midi trigger and pitch

		case Block::C_OSCILLATOR: // 47,
		case Block::C_FILTER: // 48
			break;
	
		case Block::C_BREAK:
			put("break", out);
			addSeperator = false;
			break;
	
		case Block::C_WAKE:
		case Block::C_SUSPEND:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
		    if (block->crap.call.crap.sym) {
				put(block->crap.call.crap.sym->name, out);
			    if (block->crap.call.parameters) {
			    	put("(", out);
			    	if (!Esrap(block->crap.call.parameters, out, false, indent, crlf)) return false;
			    	put(")", out);
			    }
				addSeperator = false;
	 	    } else
				addSeperator = true;
		    break;
	
		case Block::C_ASSIGN:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
		    if (!Esrap(block->crap.assign.atom, out, false, indent, crlf)) return false;
			put(" = ", out);
		    if (!Esrap(block->crap.assign.exp, out, false, indent, crlf)) return false;
		    break;
	
		case Block::C_GUARD:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
		    if (!Esrap(block->crap.guard.condition, out, false, indent, crlf)) return false;
			put(" :: ", out);
		    if (!Esrap(block->crap.guard.block, out, do_indent, indent, crlf)) return false;
			addSeperator = false;
		    break;
	
		case Block::C_REPEAT:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
			put("repeat(", out);
		    if (!Esrap(block->crap.repeat.Exp, out, false, indent, crlf)) return false;
			put(")", out);
		    if (!Esrap(block->crap.repeat.block, out, do_indent, indent, crlf)) return false;
			addSeperator = false;
		    break;
	
		case Block::C_IF:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
			put("if (", out);
		    if (!Esrap(block->crap.iff.condition, out, false, indent, crlf)) return false;
			put(")", out);
			putNL(out, indent+1, crlf);
		    if (!Esrap(block->crap.iff.ifBlock, out, do_indent, indent+1, crlf)) return false;
		    if (block->crap.iff.elseBlock) {
				putNL(out, indent, crlf);
		    	put("else", out);
				putNL(out, indent+1, crlf);
				if (!Esrap(block->crap.iff.elseBlock, out, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_FOREACH:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
			put("foreach ", out);
			if (block->crap.foreach.condition) {
				put("having (", out);
			    if (!Esrap(block->crap.foreach.condition, out, false, indent, crlf)) return false;
				put(")", out);
				putNL(out, indent+1, crlf);
			    if (!Esrap(block->crap.foreach.ifBlock, out, do_indent, indent+1, crlf)) return false;
			    if (block->crap.foreach.elseBlock) {
					putNL(out, indent, crlf);
			    	put("else", out);
					putNL(out, indent+1, crlf);
					if (!Esrap(block->crap.foreach.elseBlock, out, do_indent, indent+1, crlf)) return false;
				}
			} else {
			    if (!Esrap(block->crap.foreach.ifBlock, out, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_WITH:
			/* if (cnt >= 2) */putNL(out, indent, crlf);
		    if (block->crap.with.object) {
				put("with ", out);
			    if (!Esrap(block->crap.with.object, out, false, indent, crlf)) return false;
			}
		    if (block->crap.with.condition) {
				put(" having (", out);
			    if (!Esrap(block->crap.with.condition, out, false, indent, crlf)) return false;
				put(")", out);
			}
			putNL(out, indent+1, crlf);
		    if (!Esrap(block->crap.with.withBlock, out, do_indent, indent+1, crlf)) return false;
		    if (block->crap.with.withoutBlock) {
				putNL(out, indent, crlf);
		    	put("else", out);
				putNL(out, indent+1, crlf);
				if (!Esrap(block->crap.with.withoutBlock, out, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_UNOP:
		    if (block->crap.op.l) {
		    	switch(block->subType) {
		    	case Block::OP_UMINUS:	put("-", out); break;
		    	case Block::OP_NOT:	put("!", out); break;
		    	case Block::OP_BNOT:	put("~", out); break;
		    	default:
					internalError("Unimplimented Esrap: unop");
		    	}
		    	if ((block->crap.op.l->isOperator())) put("(", out);
		    	if (!Esrap(block->crap.op.l, out, do_indent, indent, crlf))
		    		return false;
		    	if ((block->crap.op.l->isOperator()))
					put(")", out);
		    }
		    break;
		    
		case Block::C_BINOP:
			if (block->crap.op.l && block->crap.op.r) {
		    	if ((block->crap.op.l->isOperator()))
					put("(", out);
		    	if (!Esrap(block->crap.op.l, out, do_indent, indent, crlf)) return false;
		    	if ((block->crap.op.l->isOperator()))
					put(")", out);
		    	switch(block->subType) {
				case Block::OP_LT:		put("<", out); break;
				case Block::OP_GT:		put(">", out); break;
				case Block::OP_EQ:		put("==", out); break;
				case Block::OP_NEQ:	put("!=", out); break;
				case Block::OP_LE:		put("<=", out); break;
				case Block::OP_GE:		put(">=", out); break;
				case Block::OP_MUL:	put("*", out); break;
				case Block::OP_DIV:	put("/", out); break;
				case Block::OP_ADD:	put("+", out); break;
				case Block::OP_SUB:	put("-", out); break;
				case Block::OP_MOD:	put("%", out); break;
				case Block::OP_AND:	put("&&", out); break;
				case Block::OP_OR:		put("||", out); break;
				case Block::OP_BAND:	put("&", out); break;
				case Block::OP_BOR:	put("|", out); break;
		    	default:
					internalError("Unimplimented Esrap: unop");
		    	}
		
		    	if ((block->crap.op.r->isOperator()))
					put("(", out);
		    	if (!Esrap(block->crap.op.r, out, do_indent, indent, crlf)) return false;
		    	if ((block->crap.op.r->isOperator()))
					put(")", out);
			}
		    break;
		    
		case Block::C_IFOP:
		    if (block->crap.iff.condition && !Esrap(block->crap.iff.condition, out, do_indent, indent, crlf)) return false;
			put("?", out);
		    if (block->crap.iff.ifBlock && !Esrap(block->crap.iff.ifBlock, out, do_indent, indent, crlf)) return false;
			put(":", out);
		    if (block->crap.iff.elseBlock && !Esrap(block->crap.iff.elseBlock, out, do_indent, indent, crlf)) return false;
		    break;
		    
		case Block::C_VALUE:
			if (block->crap.constant.value.type == TypedValue::S_STRING) {
				put("\"", out);
				put(block->crap.constant.value.StringValue(), out);
				put("\"", out);
			} else if (block->name.size()) {
				put(block->name, out);
			} else {
				put(block->crap.constant.value.StringValue(), out);
			}
		    break;
		    
		case Block::C_SYM:
			put(block->crap.sym->name, out);
			break;
	
		case Block::C_NAME:
			put(block->name, out);
		    break;
		    
		case Block::C_STRUCTURE_REF:
			if (!Esrap(block->crap.structureRef.base, out, do_indent, indent, crlf)) return false;
			put(".", out);
			if (block->crap.structureRef.member) {
				put(block->crap.structureRef.member->name, out);
			} else if (block->crap.structureRef.unresolved) {
				if (!Esrap(block->crap.structureRef.unresolved, out, do_indent, indent, crlf)) return false;
			}
			break;
			
		case Block::C_ARRAY_REF:
			if (block->crap.arrayRef.base && block->crap.arrayRef.index) {
				if (!Esrap(block->crap.arrayRef.base, out, do_indent, indent, crlf)) return false;
				put("[", out);
				if (!Esrap(block->crap.arrayRef.index, out, do_indent, indent, crlf)) return false;
				put("]", out);
			}
			break;
	
		case Block::C_CAST: {
			std::string s=findTypeName(block->crap.cast.type);
			put("(#", out);
			put(s.c_str(), out);
			put(" ", out);
			if (!Esrap(block->crap.cast.block,out, do_indent, indent, crlf)) return false;
			put(")", out);
			break;
		}
		
		default:
		    internalError("Esrap: Bad block element %d", block->type);
		    return false;
		}
		
		block = block->next;
		if (block) {
			if (addSeperator)
				put(",", out);
			if (!do_indent || !checkNL(out, indent, crlf)) {
				put(" ", out);
			}
		}
	}	
	return true;
}


