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

int32	nlpos = 0;

inline bool AddToBuf(const string &s, char *buf, long &pos, long len)
{
	if ((long)(pos+s.size())<len) {
		strcpy(&buf[pos], s.c_str());
		pos += s.size();
		return true;
	}
	return false;
}

inline bool AddNL(char *buf, long &pos, long len, short indent, short crlf)
{
	nlpos = pos;
	if (!AddToBuf("\n", buf, pos, len)) return false;
	for (short i=0; i<indent; i++) {
		if (!AddToBuf("\t", buf, pos, len)) return false;
	}
	return true;
}

inline bool CheckNL(char *buf, long &pos, long len, short indent, short crlf)
{
	if (pos - nlpos > 90) {
		AddNL(buf, pos, len, indent, crlf);
		return true;
	}
	return false;
}


bool
Esrap(Block *block, char *buf, long &pos, long len, bool do_indent, short indent, short crlf)
{
	bool	addSeperator=true;
	int		cnt=0;
	while (block) {
		cnt++;
		switch (block->type) {
	
		case Block::C_WAIT:
			if (cnt >= 2) AddNL(buf, pos, len, indent, crlf);
			if (!AddToBuf("wait(", buf, pos, len)) return false;
			if (!Esrap(block->crap.block, buf,pos,len, false, indent, crlf))
				return false;
			if (!AddToBuf(")", buf, pos, len)) return false;
			break;
	
		case Block::C_FLUX:
			if (cnt >= 2) AddNL(buf, pos, len, indent, crlf);
			if (!AddToBuf("<-", buf, pos, len)) return false;
		    if (block->crap.flux.lengthExp) {
		    	if (!AddToBuf("(", buf, pos, len)) return false;
		    	if (!Esrap(block->crap.flux.lengthExp, buf,pos,len, false, indent, crlf)) return false;
		    	if (!AddToBuf(")", buf, pos, len)) return false;
		    }
			if (!Esrap(block->crap.flux.block, buf,pos,len, do_indent, indent+1, crlf)) return false;
			if (block->crap.flux.rateExp) {
				if (!AddToBuf(" : ", buf, pos, len)) return false;
				if (!Esrap(block->crap.flux.rateExp, buf,pos,len, false, indent, crlf)) return false;
			}
			break;
	
		case Block::C_LIST:
			if (!AddToBuf(
				((char *)(
					block->subType == Block::LIST_SEQ? "[":
					block->subType == Block::LIST_FORK? "{&":
					block->subType == Block::LIST_PAR? "{|": "{")), buf, pos, len)) return false;
			if (!AddToBuf(" ", buf, pos, len)) return false;
		    if (!Esrap(block->crap.list.block, buf,pos,len, do_indent, indent+1, crlf)) return false;
			if (!AddToBuf(" ", buf, pos, len)) return false;
			if (!AddToBuf(((char *)(block->subType == Block::LIST_SEQ?"]":"}")), buf, pos, len)) return false;
			addSeperator = !do_indent;
		    break;
		
		case Block::C_DIVERT:
			if (!Esrap(block->crap.divert.block, buf,pos,len, do_indent, indent, crlf)) return false;
			if (!AddToBuf(" @ ", buf, pos, len)) return false;
			if (!Esrap(block->crap.divert.clockExp, buf,pos,len, false, indent, crlf)) return false;
			break;
			
		case Block::C_BUILTIN: {
			std::string s;
			if (block->subType == Block::BUILTIN_CREATE) {
				AddNL(buf, pos, len, indent, crlf);
				int typ = block->crap.call.crap.createType;
				s = qut::unfind(builtinCommandIndex, typ);
			} else {
				int typ = block->subType;
				s=qut::unfind(builtinCommandIndex, typ);
			}
			if (s.size() == 0) {
				if (!AddToBuf("undefined", buf, pos, len)) return false;
			} else {
				if (!AddToBuf(s.c_str(), buf, pos, len)) return false;
			}
			Block		*params=nullptr;
			
			params=block->crap.call.parameters;
	
			if (params) {
				if (!AddToBuf("(", buf, pos, len)) return false;
				if(!Esrap(params, buf,pos,len, false, indent, crlf)) return false;
				if (!AddToBuf(")", buf, pos, len)) return false;
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
				if (!AddToBuf(s.c_str(), buf, pos, len)) return false;
				Block		*params=nullptr;
			
				params=block->crap.call.parameters;
	
				if (params) {
					if (!AddToBuf("(", buf, pos, len)) return false;
					if(!Esrap(params, buf,pos,len, false, indent, crlf)) return false;
					if (!AddToBuf(")", buf, pos, len)) return false;
					addSeperator = false;
	 			} else {
					addSeperator = true;
				}
			}
			break;
		}
		
		case Block::C_OUTPUT: {
			char	b2[20];
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
			sprintf(b2, ">> %d", block->crap.channel->chanId);
			addSeperator = false;
			if (!AddToBuf(b2, buf, pos, len)) return false;
			break;
		}
			 
		case Block::C_INPUT: {
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
			char	b2[20];
			sprintf(b2, "<< %d", block->crap.channel->chanId);
			addSeperator = false;
			if (!AddToBuf(b2, buf, pos, len)) return false;
			break;
		}
	
		case Block::C_UNLINKED_CALL:
		    if (block->crap.call.crap.name) {
				if (!AddToBuf(block->crap.call.crap.name, buf, pos, len)) return false;
			    if (block->crap.call.parameters) {
			    	if (!AddToBuf("(", buf, pos, len)) return false;
			    	if (!Esrap(block->crap.call.parameters, buf,pos,len, false, indent, crlf)) return false;
			    	if (!AddToBuf(")", buf, pos, len)) return false;
			    }
				addSeperator = false;
	 	    } else {
				addSeperator = true;
			}
			break;
	
		case Block::C_CALL:
		    if (block->crap.call.crap.lambda) {
				if (!AddToBuf(block->crap.call.crap.lambda->sym->name, buf, pos, len)) return false;
			    if (block->crap.call.parameters) {
			    	if (!AddToBuf("(", buf, pos, len)) return false;
			    	if (!Esrap(block->crap.call.parameters, buf,pos,len, false, indent, crlf)) return false;
			    	if (!AddToBuf(")", buf, pos, len)) return false;
			    }
				addSeperator = false;
	 	    } else
				addSeperator = true;
			break;

//		case Block::C_SAMPLE_PLAYER: // 38,	// play samples at pitch, no frills
//		case Block::C_STREAM_PLAYER: // 39,
//		case Block::C_GENERIC_PLAYER: // 40,

		case Block::C_VST: // 41,
			AddNL(buf, pos, len, indent, crlf);
			if (block->crap.call.crap.vstplugin) {
				if (!AddToBuf(block->crap.call.crap.vstplugin->sym->name, buf, pos, len)) return false;
			    if (block->crap.call.parameters) {
			    	if (!AddToBuf("(", buf, pos, len)) return false;
			    	if (!Esrap(block->crap.call.parameters, buf,pos,len, false, indent, crlf)) return false;
			    	if (!AddToBuf(")", buf, pos, len)) return false;
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
			if (!AddToBuf("break", buf, pos, len)) return false;
			addSeperator = false;
			break;
	
		case Block::C_WAKE:
		case Block::C_SUSPEND:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
		    if (block->crap.call.crap.sym) {
				if (!AddToBuf(block->crap.call.crap.sym->name, buf, pos, len)) return false;
			    if (block->crap.call.parameters) {
			    	if (!AddToBuf("(", buf, pos, len)) return false;
			    	if (!Esrap(block->crap.call.parameters, buf,pos,len, false, indent, crlf)) return false;
			    	if (!AddToBuf(")", buf, pos, len)) return false;
			    }
				addSeperator = false;
	 	    } else
				addSeperator = true;
		    break;
	
		case Block::C_ASSIGN:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
		    if (!Esrap(block->crap.assign.atom, buf,pos,len, false, indent, crlf)) return false;
			if (!AddToBuf(" = ", buf, pos, len)) return false;
		    if (!Esrap(block->crap.assign.exp, buf,pos,len, false, indent, crlf)) return false;
		    break;
	
		case Block::C_GUARD:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
		    if (!Esrap(block->crap.guard.condition, buf,pos,len, false, indent, crlf)) return false;
			if (!AddToBuf(" :: ", buf, pos, len)) return false;
		    if (!Esrap(block->crap.guard.block, buf,pos,len, do_indent, indent, crlf)) return false;
			addSeperator = false;
		    break;
	
		case Block::C_REPEAT:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
			if (!AddToBuf("repeat(", buf, pos, len)) return false;
		    if (!Esrap(block->crap.repeat.Exp, buf,pos,len, false, indent, crlf)) return false;
			if (!AddToBuf(")", buf, pos, len)) return false;
		    if (!Esrap(block->crap.repeat.block, buf,pos,len, do_indent, indent, crlf)) return false;
			addSeperator = false;
		    break;
	
		case Block::C_IF:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
			if (!AddToBuf("if (", buf, pos, len)) return false;
		    if (!Esrap(block->crap.iff.condition, buf,pos,len, false, indent, crlf)) return false;
			if (!AddToBuf(")", buf, pos, len)) return false;
			AddNL(buf, pos, len, indent+1, crlf);
		    if (!Esrap(block->crap.iff.ifBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
		    if (block->crap.iff.elseBlock) {
				AddNL(buf, pos, len, indent, crlf);
		    	if (!AddToBuf("else", buf, pos, len)) return false;
				AddNL(buf, pos, len, indent+1, crlf);
				if (!Esrap(block->crap.iff.elseBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_FOREACH:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
			if (!AddToBuf("foreach ", buf, pos, len)) return false;
			if (block->crap.foreach.condition) {
				if (!AddToBuf("having (", buf, pos, len)) return false;
			    if (!Esrap(block->crap.foreach.condition, buf,pos,len, false, indent, crlf)) return false;
				if (!AddToBuf(")", buf, pos, len)) return false;
				AddNL(buf, pos, len, indent+1, crlf);
			    if (!Esrap(block->crap.foreach.ifBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
			    if (block->crap.foreach.elseBlock) {
					AddNL(buf, pos, len, indent, crlf);
			    	if (!AddToBuf("else", buf, pos, len)) return false;
					AddNL(buf, pos, len, indent+1, crlf);
					if (!Esrap(block->crap.foreach.elseBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
				}
			} else {
			    if (!Esrap(block->crap.foreach.ifBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_WITH:
			/* if (cnt >= 2) */AddNL(buf, pos, len, indent, crlf);
		    if (block->crap.with.object) {
				if (!AddToBuf("with ", buf, pos, len)) return false;
			    if (!Esrap(block->crap.with.object, buf,pos,len, false, indent, crlf)) return false;
			}
		    if (block->crap.with.condition) {
				if (!AddToBuf(" having (", buf, pos, len)) return false;
			    if (!Esrap(block->crap.with.condition, buf,pos,len, false, indent, crlf)) return false;
				if (!AddToBuf(")", buf, pos, len)) return false;
			}
			AddNL(buf, pos, len, indent+1, crlf);
		    if (!Esrap(block->crap.with.withBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
		    if (block->crap.with.withoutBlock) {
				AddNL(buf, pos, len, indent, crlf);
		    	if (!AddToBuf("else", buf, pos, len)) return false;
				AddNL(buf, pos, len, indent+1, crlf);
				if (!Esrap(block->crap.with.withoutBlock, buf,pos,len, do_indent, indent+1, crlf)) return false;
			}
			addSeperator = false;
		    break;
	
		case Block::C_UNOP:
		    if (block->crap.op.l) {
		    	switch(block->subType) {
		    	case Block::OP_UMINUS:	if (!AddToBuf("-", buf, pos, len)) return false; break;
		    	case Block::OP_NOT:	if (!AddToBuf("!", buf, pos, len)) return false; break;
		    	case Block::OP_BNOT:	if (!AddToBuf("~", buf, pos, len)) return false; break;
		    	default:
					internalError("Unimplimented Esrap: unop");
		    	}
		    	if ((block->crap.op.l->isOperator())) if (!AddToBuf("(", buf, pos, len)) return false;
		    	if (!Esrap(block->crap.op.l, buf,pos,len, do_indent, indent, crlf))
		    		return false;
		    	if ((block->crap.op.l->isOperator()))
					if (!AddToBuf(")", buf, pos, len)) return false;
		    }
		    break;
		    
		case Block::C_BINOP:
			if (block->crap.op.l && block->crap.op.r) {
		    	if ((block->crap.op.l->isOperator()))
					if (!AddToBuf("(", buf, pos, len)) return false;
		    	if (!Esrap(block->crap.op.l, buf,pos,len, do_indent, indent, crlf)) return false;
		    	if ((block->crap.op.l->isOperator()))
					if (!AddToBuf(")", buf, pos, len)) return false;
		    	switch(block->subType) {
				case Block::OP_LT:		if (!AddToBuf("<", buf, pos, len)) return false; break;
				case Block::OP_GT:		if (!AddToBuf(">", buf, pos, len)) return false; break;
				case Block::OP_EQ:		if (!AddToBuf("==", buf, pos, len)) return false; break;
				case Block::OP_NEQ:	if (!AddToBuf("!=", buf, pos, len)) return false; break;
				case Block::OP_LE:		if (!AddToBuf("<=", buf, pos, len)) return false; break;
				case Block::OP_GE:		if (!AddToBuf(">=", buf, pos, len)) return false; break;
				case Block::OP_MUL:	if (!AddToBuf("*", buf, pos, len)) return false; break;
				case Block::OP_DIV:	if (!AddToBuf("/", buf, pos, len)) return false; break;
				case Block::OP_ADD:	if (!AddToBuf("+", buf, pos, len)) return false; break;
				case Block::OP_SUB:	if (!AddToBuf("-", buf, pos, len)) return false; break;
				case Block::OP_MOD:	if (!AddToBuf("%", buf, pos, len)) return false; break;
				case Block::OP_AND:	if (!AddToBuf("&&", buf, pos, len)) return false; break;
				case Block::OP_OR:		if (!AddToBuf("||", buf, pos, len)) return false; break;
				case Block::OP_BAND:	if (!AddToBuf("&", buf, pos, len)) return false; break;
				case Block::OP_BOR:	if (!AddToBuf("|", buf, pos, len)) return false; break;
		    	default:
					internalError("Unimplimented Esrap: unop");
		    	}
		
		    	if ((block->crap.op.r->isOperator()))
					if (!AddToBuf("(", buf, pos, len)) return false;
		    	if (!Esrap(block->crap.op.r, buf,pos,len, do_indent, indent, crlf)) return false;
		    	if ((block->crap.op.r->isOperator()))
					if (!AddToBuf(")", buf, pos, len)) return false;
			}
		    break;
		    
		case Block::C_IFOP:
		    if (block->crap.iff.condition && !Esrap(block->crap.iff.condition, buf,pos,len, do_indent, indent, crlf)) return false;
			if (!AddToBuf("?", buf, pos, len)) return false;
		    if (block->crap.iff.ifBlock && !Esrap(block->crap.iff.ifBlock, buf,pos,len, do_indent, indent, crlf)) return false;
			if (!AddToBuf(":", buf, pos, len)) return false;
		    if (block->crap.iff.elseBlock && !Esrap(block->crap.iff.elseBlock, buf,pos,len, do_indent, indent, crlf)) return false;
		    break;
		    
		case Block::C_VALUE:
			if (block->crap.constant.value.type == TypedValue::S_STRING) {
				if (!AddToBuf("\"", buf, pos, len)) return false;
				if (!AddToBuf(block->crap.constant.value.StringValue(), buf, pos, len)) return false;
				if (!AddToBuf("\"", buf, pos, len)) return false;
			} else if (block->crap.constant.stringValue) {
				if (!AddToBuf(block->crap.constant.stringValue, buf, pos, len)) return false;
			} else {
				if (!AddToBuf(block->crap.constant.value.StringValue(), buf, pos, len)) return false;
			}
		    break;
		    
		case Block::C_SYM:
			if (!AddToBuf(block->crap.sym->name, buf, pos, len)) return false;
			break;
	
		case Block::C_NAME:
			if (!AddToBuf(block->crap.name, buf, pos, len)) return false;
		    break;
		    
		case Block::C_STRUCTURE_REF:
			if (!Esrap(block->crap.structureRef.base, buf,pos,len, do_indent, indent, crlf)) return false;
			if (!AddToBuf(".", buf, pos, len)) return false;
			if (block->crap.structureRef.member) {
				if (!AddToBuf(block->crap.structureRef.member->name, buf, pos, len)) return false;
			} else if (block->crap.structureRef.unresolved) {
				if (!Esrap(block->crap.structureRef.unresolved, buf,pos,len, do_indent, indent, crlf)) return false;
			}
			break;
			
		case Block::C_ARRAY_REF:
			if (block->crap.arrayRef.base && block->crap.arrayRef.index) {
				if (!Esrap(block->crap.arrayRef.base, buf,pos,len, do_indent, indent, crlf)) return false;
				if (!AddToBuf("[", buf, pos, len)) return false;
				if (!Esrap(block->crap.arrayRef.index, buf,pos,len, do_indent, indent, crlf)) return false;
				if (!AddToBuf("]", buf, pos, len)) return false;
			}
			break;
	
		case Block::C_CAST: {
			std::string s=findTypeName(block->crap.cast.type);
			if (!AddToBuf("(#", buf, pos, len)) return false;
			if (!AddToBuf(s.c_str(), buf, pos, len)) return false;
			if (!AddToBuf(" ", buf, pos, len)) return false;
			if (!Esrap(block->crap.cast.block,buf,pos,len, do_indent, indent, crlf)) return false;
			if (!AddToBuf(")", buf, pos, len)) return false;
			break;
		}
		
		default:
		    internalError("Esrap: Bad block element %d", block->type);
		    return false;
		}
		
		block = block->next;
		if (block) {
			if (addSeperator)
				if (!AddToBuf(",", buf, pos, len)) return false;
			if (!do_indent || !CheckNL(buf, pos, len, indent, crlf)) {
				if (!AddToBuf(" ", buf, pos, len)) return false;
			}
		}
	}	
	return true;
}


