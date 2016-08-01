#include "qua_version.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "StdDefs.h"

#include "Qua.h"
#include "Voice.h"
#include "Sample.h"
#include "Pool.h"
#include "Lambda.h"
#include "Sym.h"
#include "Block.h"
#include "Template.h"
#include "QuaFX.h"
#include "Envelope.h"
#include "Executable.h"
#include "Channel.h"
#include "QuasiStack.h"
#include "QuaPort.h"
#include "QuaJoy.h"
#include "Markov.h"
#include "BaseVal.h"
#include "VstPlugin.h"
#include "Parse.h"
#include "MidiDefs.h"
#include "Dictionary.h"

#include <iostream>

int				debug_parse=1;


// routine used to convert external to internal names
string quascript_name(const string &nm)
{
	string s;
	auto it = nm.begin();
	while (it != nm.end() && !isalpha(*it) && !(*it == '_')) {
		++it;
	}
	while (it != nm.end()) {
		if (isalnum(*it) || *it == '_') {
			s.push_back(*it);
		}
		++it;
	}
	return s;
}



// keep this comment for historical reasons.

/*
 * ReadProgFile:
 *    input the song info and do some initialization
 *  format:
 *    note	duration
 */
 
/**
 * class Parser, class QSParser
 * this is slowly being replaced by a bison grammar which will use stream output, stdio will do here for now
 */

Parser::Parser(FILE *fp, std::string fnm, ErrorHandler &_display, Qua *q):
	QSParser(q, _display, getBase(fnm))
{
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	text_buf = nullptr;
	if (size > 0) {
		text_buf = new char[size+1];
		text_buf_ind = 0;
		long nr = 0;
		if (((off_t)(nr=fread(text_buf, 1, size, fp))) != size) {
			size = nr;
//			AbortError("can't read the shite in %s. got %d trying to read %d", fnm, nr, size);
//			err_cnt = 1;
		}
		text_buf[size] = 0;
	}
}

Parser::~Parser()
{
	delete text_buf;
}

bool
Parser::atEof()
{
	return	text_buf_ind > size || text_buf == nullptr;
}

void
Parser::unGetChar()
{
   	if (text_buf_ind > 0) text_buf_ind--;
}

char
Parser::getChar()
{
	if (text_buf_ind < size && text_buf)
		return text_buf[text_buf_ind++];
	else {
		if (text_buf_ind == size)
			text_buf_ind++;
		return '\0';
	}
}


off_t
Parser::position()
{
	return text_buf_ind;
}

void
Parser::rewind()
{
	display.parseErrorViewClear();
	err_cnt = 0;
	text_buf_ind = 0;
}

void
Parser::seek(off_t ind)
{
	text_buf_ind = ind;
}

/*
 * TODO FIXME XXXX assuming here, q != nullptr ... is that going to be ok?
 */
TxtParser::TxtParser(char *txt, long len, const char *srcnm, Qua *q):
	QSParser(q, q->bridge, srcnm)
{
	if (len >= 0) {
		size = len;
	} else {
		size = strlen(txt);
	}
	fprintf(stderr, "created parser for:\n%s\n", txt);
	text_buf = txt;
	text_buf_ind = 0;
}

TxtParser::~TxtParser()
{
}


bool
TxtParser::atEof()
{
	return	text_buf_ind > size || text_buf == nullptr;
}

void
TxtParser::unGetChar()
{
   	if (text_buf_ind > 0) text_buf_ind--;
}

char
TxtParser::getChar()
{
	if (text_buf_ind < size) {
		return text_buf[text_buf_ind++];
	} else {
		if (text_buf_ind == size)
			text_buf_ind++;
		return '\0';
	}
}

void
TxtParser::rewind()
{
	display.parseErrorViewClear();
	err_cnt = 0;
	text_buf_ind = 0;
}


off_t
TxtParser::position()
{
	return text_buf_ind;
}

void
TxtParser::seek(off_t ind)
{
	text_buf_ind = ind;
}



QSParser::QSParser(Qua *q, ErrorHandler &_display, const std::string srcnm)
	: display(_display)
{
	uberQua = q;
	methods = nullptr;
	templates = nullptr;
	schedulees = nullptr;
	currentTokenType = TOK_NOT;
	lineno = 1;
	display.parseErrorViewClear();
	err_cnt = 0;

	comment = "";
	cout << srcnm.size() << " got string " << endl;
	if (srcnm.size() > 0) {
		txtSrcName = srcnm;
	} 
}

QSParser::~QSParser()
{
	ClearComment();

	auto bi = blocks.begin();
	while (blocks.size() > 0) {
		Block *b = blocks[0];
		bi = blocks.erase(bi);
		b->DeleteAll();
	}

}

void
QSParser::AddCharToComment(char c)
{
	comment += c;
}

void
QSParser::ClearComment()
{
	comment = nullptr;
}

#include <stdarg.h>
void
QSParser::ParseError(int severity, char *msg,  ...)
{
	char		buf1[512];
	char		buf[512];
	
	va_list		args;
	va_start(args, msg);
	vsprintf(buf1, msg, args);
	if (txtSrcName.size() > 0) {
	    sprintf(buf, "Parse error in %s, line %d: %s", txtSrcName.c_str(), lineno, buf1);
	} else {
	    sprintf(buf, "Parse error, line %d: %s", lineno, buf1);
	}
	std::string errMsg = buf;
	display.parseErrorViewAddLine(errMsg, severity);
	err_cnt++;
}

void
QSParser::ShowErrors()
{
	if (err_cnt > 0) {
		display.parseErrorViewShow();
	}
}

/*
 * GetToken:
 * v.dodgy lexical analyser.
 *  xxxx todo strings are a can of worms ownership wise for the moment they will generate an error unless
 * given hasWorms=true in which case the tokval isn't initialized, it is still kept in the currentToken, a
 * static memmber
 * FIXME XXXX TODO WTF
 */
int
QSParser::GetToken(bool hasWorms)
{
//    int		v, l;
    uchar	c;
    char	*p;
	c = ' ';
	while (! atEof()) {
		c = getChar();
    
		if (c == '\n') {
			lineno++;
		} else if (c == '/') {
			c = getChar();
		    if (c == '/') {
		    	do {
					c = getChar();
					AddCharToComment(c);
		    	} while (c != '\n' && ! atEof());
		    	lineno++;
		    } else {
				unGetChar();
			    c = '/';
		    	break;
		    }
		} else if (!isspace(c)) {
			break;
		}
 	}
 	
	if (debug_parse >= 3) {
		fprintf(stderr, "non space c = '%c'\n", c);
	}
	currentTokenPosition = position()-1;
	if (atEof()) { // must be at eof!
		if (debug_parse)
	    	fprintf(stderr, "got an EOF after token %s\n", currentToken);
		currentToken[0] = 0;
		currentTokenType = TOK_EOF;
		return 0;
	}

check_token:	
	currentToken[0] = c;
	switch(c) {
	    case ',':
	    case '(':
	    case ')':
	    case '}':
	    case '[':
	    case ']':
	    case '.':
	    case '~':
	    case '@':
	    case '+':
	    case '*':
	    case '/':
	    case '%':
	    case ';':
		case '?':
			currentTokenType = TOK_OP;
			currentToken[1] = '\0';
			goto return_token;
	    case '{':
			c = getChar();
			if (c == '&') {
			    currentToken[1] = '&';
			    currentToken[2] = '\0';
			} else if (c == '|') {
			    currentToken[1] = '|';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '-':
			c = getChar();
			if (c == '>') {
			    currentToken[1] = '>';
			    currentToken[2] = '\0';
//			} else if (isdigit(c)) {
//				currentToken[1] = c;
//				p = currentToken+2;
//				goto read_constant;
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case ':':
			c = getChar();
			if (c == ':') {
			    currentToken[1] = ':';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '<':
			c = getChar();
			if (c == '-') {
			    currentToken[1] = '-';
			    currentToken[2] = '\0';
			} else if (c == '<') {
			    currentToken[1] = '<';
			    currentToken[2] = '\0';
			} else if (c == '=') {
			    currentToken[1] = '=';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '>':
			c = getChar();
			if (c == '=') {
			    currentToken[1] = '=';
			    currentToken[2] = '\0';
			} else if (c == '>') {
			    currentToken[1] = '>';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '|':
			c = getChar();
			if (c == '|') {
			    currentToken[1] = '|';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '&':
			c = getChar();
			if (c == '&') {
			    currentToken[1] = '&';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
	    case '=':
			c = getChar();
			if (c == '=') {
			    currentToken[1] = '=';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;

 	    case '!':
			c = getChar();
			if (c == '=') {
			    currentToken[1] = '=';
			    currentToken[2] = '\0';
			} else {
			    currentToken[1] = '\0';
				unGetChar();
			}
			currentTokenType = TOK_OP;
			goto return_token;
			break;
			
		case '"': {		// a string. drops the quotes. maybe we will need escape codes in here
		    p = currentToken;
		    c = getChar();
		    while (c != '"') {
				*p++ = c;
				c = getChar();
		    }
		    *p = '\0';
		    currentTokenType = TOK_VAL;
			if (hasWorms) {
				cout << "accepting input " << currentToken << endl;
			} else {
				cout << "not accepting input " << currentToken << endl;
				ParseError(2, "assignment to string... skipping ... worm can bypassing");
			}
//		    char *buf = new char[strlen(currentToken)+1];
//		    strcpy(buf, currentToken);
//			currentTokenStringLiteral.assign(currentToken + 1, strlen(currentToken) - 2);
		    currentTokenVal.Set(TypedValue::S_STRING, TypedValue::REF_POINTER);
// this stuff is problematic to say the least
//		    currentTokenVal.SetValue(buf);
			goto return_token;
			break;
		}
		case '\'': {
			unsigned int	val=0;
			short	i=0;
		    p = currentToken+1;
			char	*vp = (char *)&val;
		    c = getChar();
		    while (c != '\'') {
		    	if (i == 4) {
		    		ParseError(ERROR_ERR, "shonky constant, near '%s'", currentToken);
				} else {
					*p++ = c;
					*vp++ = c;
					i++;
				}
				c = getChar();
		    }
			*p++ = '\'';
			*p = '\0';
		    currentTokenType = TOK_VAL;
		    currentTokenVal.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
		    currentTokenVal.SetValue((int32)val);
			goto return_token;
		}
		
		case '\\': {
		    p = currentToken;
		    c = getChar();
		    while (isalnum(c) || c == '_') {
				*p++ = c;
				c = getChar();
		    }
			unGetChar();
			*p = '\0';
			if (findTypeAttribute(currentToken) != TypedValue::S_UNKNOWN) {
				currentTokenType = TOK_TYPE;
			} else {
				currentTokenType = TOK_TYPE;
			}
			goto return_token;
		}
			
	    default:
			if (isdigit(c)) {
				bool isa_float;
			    p = currentToken+1;
	    read_constant:	// ha fucking ha
				isa_float = false;
			    c = getChar();
			    if (c == 'x') {
					*p++ = c;
					bool	at_eof=false;
			    	ulong val = 0;
			    	while (isdigit(c=getChar()) ||
			    			(c >= 'a' && c<='f')) {
						at_eof = atEof();
						*p++ = c;
			    		val = 16*val + (isdigit(c)?c-'0':c-'a'+10);
			    		if (at_eof) break;
			    	}
					*p = '\0';
			    	currentTokenVal.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
			    	currentTokenVal.SetValue((int32)val, nullptr);
				    currentTokenType = TOK_VAL;
			    } else {
					bool	at_eof=false;
				    while (isdigit(c) || c == '.') {
				    	if (c == '.') isa_float = true;
						*p++ = c;
			    		if (at_eof) break;
						c = getChar();
						at_eof = atEof();
				    }
					*p = '\0';
				    currentTokenType = TOK_VAL;
				    if (c == ':') {
					    Metric	*met = &Metric::std;
				    	long	bars = atoi(currentToken);
				    	long	beats = 0;
				    	long	ticks = 0;
				    	if (isdigit(c = getChar())) {
				    		*p++ = ':';
				    		char	*btstr = p;
						    do {
								*p++ = c;
					    		if (at_eof) break;
								c = getChar();
								at_eof = atEof();
						    } while (isdigit(c));
						    *p = '\0';
				    		beats = atoi(btstr);
					    	if (c == '.' || c==':') {
						    	if (isdigit(c = getChar())) {
						    		*p++ = '.';
						    		char	*btstr = p;
								    do {
										*p++ = c;
							    		if (at_eof) break;
										c = getChar();
										at_eof = atEof();
								    } while (isdigit(c));
								    *p = '\0';
					    			ticks = atoi(btstr);
							    }
							}
					    	if (c == '\'') {
					    		*p++ = '\'';
					    		char	*tfstr = p;
							    while (isalnum(c=getChar())) {
									*p++ = c;
						    		if (at_eof) break;
									at_eof = atEof();
							    }
							    *p = '\0';
							    met=findMetric(tfstr);
							}
					    } else {
					    	unGetChar();
					    }
					    Time	t;
					    t.Set(bars, beats, ticks, met);
					    currentTokenVal.Set(t.ticks,t.metric);
				    } else {
					    if (isa_float) {
						    currentTokenVal.Set(TypedValue::S_FLOAT, TypedValue::REF_VALUE);
						    currentTokenVal.SetValue((float)atof(currentToken));
					    } else {
							currentTokenVal.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
							currentTokenVal.SetValue((int32)atoi(currentToken));
						}
					}
				}
			} else if (isalpha(c) || c == '_') {
				bool	at_eof=false;
			    p = currentToken+1;
			    c = getChar();
			    while (isalnum(c) || c == '#' || c == '_' || c=='\\'){
			    	if (c=='\\') {
			    		switch(c=getChar()) {
			    		case 't': c = '\t'; break;
			    		default:
			    			;
			    		}			    		
			    	}
					*p++ = c;
					if (at_eof)
						break;
					c = getChar();
					at_eof = atEof();
			    }
				while (isspace(c) && !atEof()) {
					if (c == '\n') {
						lineno++;
					}
					c = getChar();
				}
				// gains a bit of lookahead, and helps differentiate
				// things like label definitions ...
				*p = '\0';
			    currentTokenType = TOK_WORD;
				long v;
				if ((v = findType(currentToken)) != TypedValue::S_UNKNOWN) {
				    currentTokenType = TOK_TYPE;
				} else if ((v = findDfltEventType(currentToken)) != TypedValue::S_UNKNOWN) {
//				    currentTokenType = TOK_TYPE;
				} else if ((v = findConstant(currentToken)) != INT_MIN) {
				    currentTokenType = TOK_VAL;
					if (v == INFINITE_TICKS) {
						currentTokenVal.Set(&Time::infinity);
					} else {
						currentTokenVal.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
						currentTokenVal.SetValue((int32)v);
					}
				}
			} else {
//			    ParseError(ERROR_ERR, "bad character 0x%x\n", c);
				c = getChar();
				if (c == '\n') {
					lineno++;
				}
				goto check_token;
			}
			lookaheadChar = c;
			if (!atEof())
				unGetChar();
    }
return_token:
	if (debug_parse >= 1) {
		if (currentTokenType == TOK_VAL) {
			fprintf(stderr, "token value(%s)\n", currentTokenVal.StringValue());
		} else {
	    	fprintf(stderr, "token \"%s\" type=%d\n", currentToken, currentTokenType);
		}
	}
    return 1;
}  



Block *
QSParser::ParseBlockList(StabEnt *context, StabEnt *schedSym, char *type)
{
    Block	*block, **lastpp;
    char	*Term = ((char *)(
    				  strcmp(type, "{")==0?"}":
    				  strcmp(type, "[")==0?"]":
    				  strcmp(type, "{|")==0?"}":
    				  strcmp(type, "{&")==0?"}":
    					"...."));

	Block	*children=nullptr;
	int		nchildren = 0;
	lastpp = &children;
	short subtype = strcmp(type, "{")==0? Block::LIST_NORM:
   						strcmp(type, "[")==0? Block::LIST_SEQ:
   						strcmp(type, "{|")==0? Block::LIST_PAR:
   						strcmp(type, "{&")==0? Block::LIST_FORK:0;
	GetToken();
   	while (!atEof()) {
		if (strcmp(currentToken, Term) == 0) {
			break;
		} else {
			Block *p;
			if (debug_parse >= 2) fprintf(stderr, "block el %s\n", currentToken);
			if (strcmp(currentToken, ",") == 0)
				GetToken();
			p = ParseBlockInfo(context, schedSym);
			if (debug_parse >= 2) {
				fprintf(stderr, "block el end %s %x\n", currentToken, p);
			}
			if (p != nullptr) {
				nchildren++;
				*lastpp = p;
				lastpp = &p->next;
			}
    	}
	}
	if (debug_parse >= 2) fprintf(stderr, "end of list...\n");

	if (strcmp(currentToken, Term) == 0) {
    	GetToken();
	} else {
    	ParseError(ERROR_ERR, ((char *) (
    			subtype == Block::LIST_NORM?"Block list expected } at %s":
    			subtype == Block::LIST_SEQ? "Block list expected ] at %s":
    			subtype == Block::LIST_PAR? "Block list expected } at %s":
    			subtype == Block::LIST_FORK? "Block list expected } at %s":
   					"Strange internal error reading block at '%s'")), currentToken);
	}

	if (nchildren == 0) {
		return nullptr;
	} else if (nchildren == 1) {
		return children;
	}

   	block = new Block(Block::C_LIST, subtype);
	block->crap.list.block = children;
	block->crap.list.nChildren = nchildren;

	if (block->subType == Block::LIST_NORM) {
		block->crap.list.stuffVar.Set(
			TypedValue::S_BYTE,
			TypedValue::REF_STACK,
			context,
			(long)AllocStack(
				context,
				TypedValue::S_BYTE,
				block->crap.list.nChildren));
	} else if (block->subType == Block::LIST_SEQ) {
		block->crap.list.stuffVar.Set(
			TypedValue::S_INT,
			TypedValue::REF_STACK,
			context,
			(long)AllocStack(context,TypedValue::S_INT));
	}
	
    return block;
}

/*
process a single attribute ... either \word or "string value" ... if this has a match
in the property idex we'll store that value along with it ... otherwise the name and -1
we may still be interested in non-system attributes
eventually
that was always the plan anyway

currently the system attributes are for 'cellstart' and 'phraseend' used as attributes for
notes by the markov generator
*/
bool
QSParser::parseAttribute(AttributeList * al, StabEnt *context, StabEnt *schedSym, bool resolve) {
	if (currentTokenType == TOK_TYPE) {
		int v = findProperty(currentToken);
		al->add(currentToken, v); // v could be -1 but it could still be interesting
		GetToken(true);
		return true;
	} else if (currentTokenType == TOK_VAL && currentTokenVal.type == TypedValue::S_STRING) {
		string attrname(currentToken);
		int v = findProperty(currentToken);
		al->add(currentToken, v);
		GetToken(true);
		return true;
	}
	return false;
}

/*
 * '(' attr ',' attr ',' ... ')' or just a single attr
 * attributes here are properties of a stream item, and this attribute list is set by the 'set' operator
 * these are for principally for tagging notes with metadata, and some builtin functions rely on this
 * also planning on using similalar attribute structure for sub-type attributes.
 * maybe. i think this is clear and avoids doubling up on similar types. maybe TODO XXXX FIXME
 */
AttributeList *
QSParser::parseAttributeList(StabEnt *context, StabEnt *schedSym, bool resolve) {
	AttributeList *al = new AttributeList();

	if (strcmp(currentToken, "(") == 0) {
		GetToken(true);
	} else {
		parseAttribute(al, context, schedSym, resolve);
		return al;
	}

	while (strcmp(currentToken, ")") == 0) {
		if (!parseAttribute(al, context, schedSym, resolve)) {
			;
		}
		if (strcmp(currentToken, ",") == 0) {
			GetToken(true);
		}
	}

	GetToken();

	return al;
}

Block *
QSParser::ParseActualsList(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*p, *q, **pp;

    p = nullptr;
    pp = &p;

    if (strcmp(currentToken, "(") != 0) {
		ParseError(ERROR_ERR, "Actuals list, expected ( at %s", currentToken);
    }
    GetToken();
    while (strcmp(currentToken, ")") != 0 && !atEof()) {
    	if (debug_parse)
    		fprintf(stderr, "actuals list member %s...\n", currentToken);
    	if ( strcmp(currentToken, ",") == 0
		  || (q = ParseExpression(context, schedSym, resolveNames)) == nullptr) {
			q = new Block(Block::C_UNKNOWN);
		}
		*pp = q;
		pp = &q->next;
    	if (strcmp(currentToken, ",") == 0) {
	    	GetToken();
    	}
    }
    if (debug_parse)
    	fprintf(stderr, "end of asctuals list.. %s\n", currentToken);
    if (strcmp(currentToken, ")") != 0) {
		ParseError(ERROR_ERR, "Actuals list, expected ) near '%s'", currentToken);
    }
    return p;
}

void
QSParser::ParseFormalsList(StabEnt *context, StabEnt *schedSym, bool doDefine)
{
    if (strcmp(currentToken, "(") == 0) {
	    GetToken();
	    while (strcmp(currentToken, ")") != 0) {
	    
			TypedValue		v1, v2;
			bool			hasRange = false, hasInit = false;
			
    		int32	type;
    		short	dispMode=StabEnt::DISPLAY_NOT;
			int		vstIndex = 0;
			bool	isEnveloped = false;
    		if (currentTokenType == TOK_TYPE) {
		    	switch (type = findType(currentToken)) {
		    	case TypedValue::S_INT:
		    	case TypedValue::S_BYTE:
		    	case TypedValue::S_BOOL:
		    	case TypedValue::S_SHORT:
		    	case TypedValue::S_LONG:
		    	case TypedValue::S_FLOAT:
		    		dispMode = StabEnt::DISPLAY_CTL;
					isEnveloped = true;
		    		break;
		    	case TypedValue::S_TIME:
		    		dispMode = StabEnt::DISPLAY_CTL;
		    		break;
		    	case TypedValue::S_EXPRESSION:
		    	case TypedValue::S_POOL:
		    		break;
				case TypedValue::S_VST_PARAM:
		    		dispMode = StabEnt::DISPLAY_CTL;
					GetToken();
					if (currentTokenType == TOK_VAL) {
						vstIndex = currentTokenVal.IntValue(nullptr);
					} else {
		    			ParseError(ERROR_ERR, "expected vstParam index '%s'", currentToken);
					}
					break;
		    	default:
		    		type = TypedValue::S_UNKNOWN;
		    		ParseError(ERROR_ERR, "invalid type '%s'", currentToken);
		    	}
		    	GetToken();
		    } else {
		    	ParseError(ERROR_ERR, "expected type at '%s'", currentToken);
		    }
		    
	   		while (currentTokenType == TOK_TYPE) {
		    	switch (findTypeAttribute(currentToken)) {
				case Attribute::RANGE: {
					bool	neg;
					GetToken();
					if (strcmp(currentToken, "-") == 0) {
						neg = true;
						GetToken();
					} else
						neg = false;
					if (currentTokenType != TOK_VAL) {
						ParseError(ERROR_ERR, "expected constant at '%s'", currentToken);
						break;
					}
					v1 = currentTokenVal;
					if (neg) v1 = - v1;
					
					GetToken();
					if (strcmp(currentToken, "-") == 0) {
						neg = true;
						GetToken();
					} else
						neg = false;
					if (currentTokenType != TOK_VAL) {
						ParseError(ERROR_ERR, "expected constant at '%s'", currentToken);
						break;
					}
					v2 = currentTokenVal;
					if (neg) v2 = - v2;
					hasRange = true;
					break;
				}
		    	default:
		    		ParseError(ERROR_ERR, "unexpected subtype '%s'", currentToken);
		    	}
			   	GetToken();
		    }
		    if (currentTokenType != TOK_WORD) {
		    	ParseError(ERROR_ERR, "malformed declaration (expects name) at '%s'", currentToken);
		    	GetToken();
		    } else {
		    	StabEnt		*sym=nullptr;
				if (dispMode == StabEnt::DISPLAY_NOT) {
					dispMode = StabEnt::DISPLAY_CTL;
				}
				if (type == TypedValue::S_VST_PARAM ) {
					if (doDefine)
						sym = DefineSymbol(currentToken, type, 0,
							(void*)vstIndex,
							context,
							(int16)TypedValue::REF_VALUE,
							false, isEnveloped,
							(int16)dispMode);
				} else {
		    		if (type != TypedValue::S_UNKNOWN) { // only ever want to skip unused vst params
						sym = DefineSymbol(currentToken, type, 0,
							(void*)AllocStack(context,(base_type_t)type),
							context,
							(int16)TypedValue::REF_STACK,
							false, isEnveloped,
							(int16)dispMode);
						if (hasRange) {
							sym->SetBounds(v1, v2);
						}
					}
				}
				GetToken();
				if (strcmp(currentToken, "=") == 0) {
					bool	negate = false;
					GetToken();
					if (strcmp(currentToken, "-") == 0) {
						GetToken();
						negate = true;
					}
					if (currentTokenType == TOK_VAL && sym) {
						if (negate) {
							sym->SetInit(-currentTokenVal);
						} else {
							sym->SetInit(currentTokenVal);
						}
					} else {
						ParseError(ERROR_ERR, "bad initiator '%s'", currentToken);
					}
					GetToken();
				}
		    }
		    
	    	if (strcmp(currentToken, ",") == 0) {
		    	GetToken();
	    	}
	    }
	    if (debug_parse >= 2)
	    	fprintf(stderr, "end of formals list.. %s\n", currentToken);
	    if (strcmp(currentToken, ")") != 0) {
			ParseError(ERROR_ERR, "Formals list, expected ) at %s", currentToken);
	    }
	    GetToken();
	}
}

// ??? what kinds allowed?
Block *
QSParser::ParseExpressionList(StabEnt *context, StabEnt *schedSym, char *type, bool resolveNames)
{
    Block	*p, *q, **pp;
    char	*Term = ((char *) (
    				  strcmp(type, "{")==0?"}":
    				  strcmp(type, "[")==0?"]":
    				  strcmp(type, "{|")==0?"|}":
    					"...."));
   	Block	*block;
   	
   	if (debug_parse)
   		fprintf(stderr, "parsing expression list, curtok %s\n", currentToken);
   	block = new Block(Block::C_LIST,
   						strcmp(type, "{")==0? Block::LIST_NORM:
   						strcmp(type, "[")==0? Block::LIST_SEQ:Block::LIST_PAR);
    p = nullptr;
    pp = &p;

	block->crap.list.nChildren = 0;
    GetToken();
    while (strcmp(currentToken, Term) != 0) {
		block->crap.list.nChildren++;
    	if (debug_parse)
    		fprintf(stderr, "list member %s...\n", currentToken);
		if ((q = ParseExpression(context, schedSym, resolveNames)) == nullptr) {
			q = new Block(Block::C_UNKNOWN);
		}
		*pp = q;
		pp = &q->next;

    	if (strcmp(currentToken, ",") == 0) {
	    	GetToken();
    	}
    }
	block->crap.list.block = p;
	if (block->subType == Block::LIST_NORM) {
		block->crap.list.stuffVar.Set(
			TypedValue::S_BYTE,
			TypedValue::REF_STACK,
			context,
			(long)AllocStack(
				context,
				TypedValue::S_BYTE,
				block->crap.list.nChildren));
	} else if (block->subType == Block::LIST_SEQ) {
		block->crap.list.stuffVar.Set(
			TypedValue::S_INT,
			TypedValue::REF_STACK,
			context,
			(long)AllocStack(context,TypedValue::S_INT));
	}

    if (strcmp(currentToken, Term) != 0) {
		ParseError(ERROR_ERR, ((char *) (
						type[0] == '['?
							"Expression list, expected ']' at %s":
							"Expression list, expected '}' at %s")), currentToken);
    } else {
    	GetToken();
    }

    return block;
}


Block *
QSParser::ParseBuiltin(StabEnt *context, StabEnt *schedSym)
{
    Block	*p, *varp;
//	Block	**lastpp;
    long	curFunk=0, curSubFunk=0;
    string name;
    
	curFunk = Block::C_UNKNOWN;
	StabEnt		*builtinSym = findBuiltin(currentToken);
	if (builtinSym == nullptr) {
		fprintf(stderr, "<%s> not recognised as builtin ...\n", currentToken);
		return nullptr;
	}
	fprintf(stderr, "parsing builtin <%s>...\n", currentToken);
	name = currentToken;	 
	curFunk = builtinSym->BuiltinValue()->type;
	curSubFunk = builtinSym->BuiltinValue()->subType;

	if (curFunk == Block::C_GENERIC_PLAYER && schedSym) {
		if (schedSym->type == TypedValue::S_SAMPLE) {
			builtinSym = findBuiltin("sampleplay");
			if (builtinSym == nullptr) {
				display.reportError("Sample player not found... oops");
				return nullptr;
			}
			curFunk = builtinSym->BuiltinValue()->type;
		} else if (schedSym->type == TypedValue::S_VOICE || schedSym->type == TypedValue::S_POOL) {
			builtinSym = findBuiltin("streamplay");
			if (builtinSym == nullptr) {
				display.reportError("Stream player not found... oops");
				return nullptr;
			}
			curFunk = builtinSym->BuiltinValue()->type;
		}
	}
  
	GetToken();

	Block	*par=nullptr;
	if (strcmp(currentToken, "(") == 0) {
       	glob.PushContext(builtinSym);
		par = ParseActualsList(context, schedSym, true);
//		glob.DumpContexts(stderr);
		glob.PopContext(builtinSym);
		GetToken();
	}

	p = new Block(curFunk, curSubFunk);

	p->crap.call.parameters = par;
	
    switch (curFunk) {
	    case Block::C_BUILTIN: {
	     	switch(curSubFunk) {
#ifdef QUA_V_OLD_BUILTIN
			    case Block::BUILTIN_NOTE: {
					if (par == nullptr || par->next == nullptr || par->next->next == nullptr) {
						ParseError(ERROR_ERR, "parameter mismatch at '%s'", currentToken);
					}
					
					p->crap.call.parameters = par;
					varp = p;
					break;
					
			    }
			    
			    case Block::BUILTIN_SYSC: {
					long subsubt = systemCommonCodeIndex.ValueOf(name);
					Block *p0 = new Block( Block::C_VALUE);
					p0->crap.constant.value.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
					p0->crap.constant.value.SetValue((int32)subsubt); 
					p0->next = p->crap.call.parameters;
					p->crap.call.parameters = p0;
					varp = p;
					break;
				}
		
				case Block::BUILTIN_CTRL:
			    case Block::BUILTIN_MESG:
				case Block::BUILTIN_BEND:
				case Block::BUILTIN_SYSX:
				case Block::BUILTIN_PROG: {
		// ????????????? some param checks.		
					varp = p;
					break;
				}
#else
				case Block::BUILTIN_CREATE: {
		// ????????????? some param checks.		
					varp = p;
					break;
				}
#endif
			    case Block::BUILTIN_IS: {
			    	p->crap.constant.value.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
			    	p->crap.constant.value.SetValue((int32)-1);
			    	varp = p;
			    	break;
			    }
			    
			
				case Block::BUILTIN_SIN:
				case Block::BUILTIN_COS:
				case Block::BUILTIN_TAN:
				case Block::BUILTIN_ASIN:
				case Block::BUILTIN_ACOS:
				case Block::BUILTIN_ATAN:
				case Block::BUILTIN_EXP:
				case Block::BUILTIN_POW:
				case Block::BUILTIN_LOG:
				case Block::BUILTIN_SINH:
				case Block::BUILTIN_COSH:
				case Block::BUILTIN_TANH:
				case Block::BUILTIN_BARS:
				case Block::BUILTIN_BEATS:
				case Block::BUILTIN_FIND:
				case Block::BUILTIN_DELETE:
				case Block::BUILTIN_FLUSH:
				case Block::BUILTIN_INSERT:
				case Block::BUILTIN_COALESCE:
			    case Block::BUILTIN_ROLL:
			    case Block::BUILTIN_SELECT: {
//					int		i, j;
		// ????????????? some param checks.		
					varp = p;
					break;
				}
				default: {
					ParseError(ERROR_ERR, "Unknown builtin function ...");
					varp = p;
					break;
				}
			}
			break;
		}
		
		case Block::C_GENERIC_PLAYER:  {
			if (debug_parse)
				fprintf(stderr, "parse: builtin generic player\n");
			varp = p;
			p->crap.call.crap.player->playerSym = builtinSym;
			break;
		}
		case Block::C_STREAM_PLAYER:  {
			if (debug_parse)
				fprintf(stderr, "parse: builtin stream player\n");
			varp = p;
			p->crap.call.crap.mplayer->playerSym = builtinSym;
			break;
		}
		case Block::C_MARKOV_PLAYER:  {
			if (debug_parse)
				fprintf(stderr, "parse: builtin sample player\n");
			varp = p;
			p->crap.call.crap.markov->playerSym = builtinSym;
			break;
		}
		case Block::C_MIDI_PLAYER:  {
			if (debug_parse)
				fprintf(stderr, "parse: builtin sample player\n");
			varp = p;
			p->crap.call.crap.flayer->playerSym = builtinSym;
			break;
		}
		case Block::C_SAMPLE_PLAYER: {
			if (debug_parse)
				fprintf(stderr, "parse: builtin sample player\n");
			varp = p;
			p->crap.call.crap.splayer->playerSym = builtinSym;
			break;
		}
		
		case Block::C_TUNEDSAMPLE_PLAYER: {
			if (debug_parse)
				fprintf(stderr, "parse: builtin sample player\n");
			varp = p;
			p->crap.call.crap.tplayer->playerSym = builtinSym;
			break;
		}
		
		default: {
			ParseError(ERROR_ERR, "Unknown builtin function ...");
			varp = p;
		}
	}
	return varp;
}

Block *
QSParser::ParseAtom(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*p, *varp;
	StabEnt	*labelSym = nullptr;
	char	l2;
//	Block **lastpp;
//    char		buf[20];

	if (atEof() && currentTokenType == TOK_EOF) {
		if (debug_parse)
			fprintf(stderr, "found us an EOF, token %s ...\n", currentToken);
		return nullptr;
	}

	// wierd but ... a label can reference any block, and references from object players could return a value so ... 
	if (currentTokenType == TOK_WORD && lookaheadChar == ':') {
		l2 = getChar();
		unGetChar();
		if (l2 != ':') { // a label!
			labelSym = DefineSymbol(currentToken, TypedValue::S_LABEL,
								0, 0, context,TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);
			labelSym->LabelValue()->Set(nullptr, 0);
			GetToken(); // the ':'
		}
	}
    if (currentTokenType == TOK_VAL) {
		varp = p = new Block( Block::C_VALUE);
		p->crap.constant.value = currentTokenVal;
		if (*currentToken) {
			p->crap.constant.stringValue = new char [strlen(currentToken)+1];
			strcpy(p->crap.constant.stringValue, currentToken);
		}
        if (debug_parse)
        	fprintf(stderr, "\tconst %s\n", currentToken);

		GetToken();
    } else if (strcmp(currentToken, "-") == 0) {
		GetToken();
		if ((p = ParseExpression(context, schedSym, resolveNames)) == nullptr)
			return nullptr;
		if (p->type == Block::C_VALUE) {
			varp = p;
			varp->crap.constant.value = -varp->crap.constant.value;
		} else {
			varp = new Block( Block::C_UNOP, Block::OP_UMINUS);
			varp->crap.op.l = p;
		}
    } else if (strcmp(currentToken, "~") == 0) {
		GetToken();
		if ((p = ParseExpression(context, schedSym, resolveNames)) == nullptr)
			return nullptr;
		if (p->type == Block::C_VALUE) {
			varp = p;
			varp->crap.constant.value = ~varp->crap.constant.value;
		} else {
			varp = new Block( Block::C_UNOP, Block::OP_BNOT);
			varp->crap.op.l = p;
		}
    } else if (strcmp(currentToken, "!") == 0) {
		GetToken();
		if ((p = ParseExpression(context, schedSym, resolveNames)) == nullptr)
			return nullptr;
		if (p->type == Block::C_VALUE) {
			varp = p;
			varp->crap.constant.value = !varp->crap.constant.value;
		} else {
			varp = new Block( Block::C_UNOP, Block::OP_NOT);
			varp->crap.op.l = p;
		}
    } else if (strcmp(currentToken, "+") == 0) {
		GetToken();
		if ((varp = ParseExpression(context, schedSym, resolveNames)) == nullptr)
			return nullptr;
    } else if (strcmp(currentToken, "(") == 0) {
		GetToken();

		if (currentTokenType == TOK_TYPE ) {
			int32	type = findType(currentToken);
			if (type == TypedValue::S_UNKNOWN) {
				ParseError(ERROR_ERR, "%s should be base type", currentToken);
				GetToken();
				p = ParseExpression(context, schedSym, resolveNames);
			}  else {
				GetToken();
				p = new Block(Block::C_CAST);
				p->crap.cast.type = (base_type_t)type;
				p->crap.cast.block = ParseExpression(context, schedSym, resolveNames);
			}
		} else {
			p = ParseExpression(context, schedSym, resolveNames);
		}
		if (strcmp(currentToken, ")") != 0) {
	    	ParseError(ERROR_ERR, "expected ) at %s", currentToken);
		}
		GetToken();
		varp = p;
	} else if (	strcmp(currentToken, "[") == 0 ||
				strcmp(currentToken, "{") == 0) { 
		varp = ParseExpressionList(context, schedSym, currentToken, resolveNames);
    } else {
    	if ((varp=ParseBuiltin(context, schedSym)) == nullptr) {
    		StabEnt *sym = glob.findSymbol(currentToken);
			if (debug_parse >= 2) {
				fprintf(stderr, "name not builtin, sym = %x/%s/%d\n", sym, sym?sym->name.c_str() :"unknown", sym?sym->type:-1);
			}

    		if (sym != nullptr) {
				// create a block appropriate for this sym,
				// either Block::C_CALL, Block::C_VST, Block::C_SYM
				varp = new Block(sym);
    			GetToken();
    		} else {
		        if (debug_parse) fprintf(stderr, "\tname %s\n", currentToken);
		        if (currentTokenType != TOK_WORD) {
		        	ParseError(ERROR_ERR, "expected name at %s, current token type %d", currentToken, currentTokenType);
		        	GetToken();
		        	return nullptr;
		        } else {
					p = new Block( Block::C_NAME);
					varp = p;
					p->crap.name = new char[strlen(currentToken) + 1];
					strcpy(p->crap.name, currentToken);
			   		GetToken();
			   	}
		   	}

		   	if (strcmp(currentToken, "(") == 0) {
	        	if (sym) glob.PushContext(sym);
		   		Block *p = ParseActualsList(context, schedSym, sym!=nullptr);
				if (sym) glob.PopContext(sym);

		   		GetToken();
		   		switch (varp->type) {
		   		case Block::C_CALL:
				case Block::C_VST:
				case Block::C_TUNEDSAMPLE_PLAYER:
				case Block::C_SAMPLE_PLAYER:
				case Block::C_MIDI_PLAYER:
				case Block::C_MARKOV_PLAYER:
				case Block::C_STREAM_PLAYER:
				case Block::C_GENERIC_PLAYER:
		   			varp->crap.call.parameters = p;
		   			break;
		   			
		   		case Block::C_SYM: {
		   			StabEnt	*sym = varp->crap.sym;
		   			switch (sym->type) {
		   			case TypedValue::S_LAMBDA:
			   			varp->type = Block::C_CALL;
			   			varp->crap.call.crap.lambda = sym->LambdaValue();
			   			break;
			   		case TypedValue::S_VOICE:
			   		case TypedValue::S_POOL:
			   		case TypedValue::S_SAMPLE:
			   		case TypedValue::S_PORT:
			   			varp->type = Block::C_SCHEDULE;
			   			varp->crap.call.crap.sym = sym;
			   			break;
			   		default:
			   			break;
			   		}
		   			varp->crap.call.parameters = p;
		   			break;
		   		}
		   		
		   		case Block::C_NAME: {
		   			char *nm = varp->crap.name;
					long	v;
					if ((v = findMidiNote(nm)) != INT_MIN) {
						TypedValue	val;
						val.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
						val.SetValue((int32)v);
						varp->Set(val, nm);
						varp->next = p;
						p = varp;
#ifdef QUA_V_OLD_BUILTIN
						varp = new Block(Block::C_BUILTIN, Block::BUILTIN_NOTE);
#else
						varp = new Block(Block::C_BUILTIN, Block::BUILTIN_CREATE);
						varp->crap.call.crap.createType = TypedValue::S_NOTE;
#endif
						varp->crap.call.parameters = p;
					} else {
			   			varp->type = Block::C_UNLINKED_CALL;
			   			varp->crap.call.crap.name = nm;
			   			varp->crap.call.parameters = p;
					}
		   			break;
		   		}
		   		default: {
		   			ParseError(ERROR_ERR, "unexpected parameter list at '%s'", currentToken);
		   			p->DeleteAll();
		   		}}
			} else { // a name, with no call
//				if ((v = midiNoteIndex.ValueOf(varp->crap.name)) != KEYVAL_SHITE) {
//					TypedValue	val;
//					val.Set(TypedValue::S_INT, TypedValue::REF_VALUE);
//					val.SetValue((int32)v);
//					varp->Set(val, varp->crap.name);	// looks dangerous but ok
//				}

			}

//	   		if (varp && varp->type == Block::C_CALL) {
//	   			if (currentTokenType == TOK_TYPE) {
//	   				int32 typ = findSubType(currentToken);
//	   				if (typ == Attribute::_LABEL) {
//	   					GetToken();
//	   					if (  currentTokenType == TOK_VAL &&
//	   						  currentTokenVal.type == TypedValue::S_STRING)
//	   						varp->crap.call.label = currentTokenVal.StringValue();
//						GetToken();
//	   				}
//	   			}
//	   		}
		} else {
			if (debug_parse)
				fprintf(stderr, "got a builtin\n");
		}

    }
	if (debug_parse) fprintf(stderr, "\tparsed atom, var block %x %d\n", varp, varp?varp->type:-1);
	if (varp) {
		if (labelSym) {
			labelSym->LabelValue()->block = varp;
			varp->labelSym = labelSym;
		}
	}
    return varp;
}

   
Block 	*
QSParser::ParseDExp(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block *varp = ParseAtom(context, schedSym, resolveNames);
    while (		strcmp(currentToken, ".") == 0
   		     ||	strcmp(currentToken, "[") == 0) {
	    if (strcmp(currentToken, "[") == 0) {
	    	if (debug_parse)
	    		fprintf(stderr, "aref\n");
	        GetToken();
	        Block *p = new Block( Block::C_ARRAY_REF);
	        p->crap.arrayRef.index = ParseExpression(context, schedSym, resolveNames);
	        p->crap.arrayRef.base = varp;
	        if (strcmp(currentToken, "]") != 0)
	        	ParseError(ERROR_ERR, "expected ']' near '%s'", currentToken);
	        GetToken();
	        varp = p;
	    } else if (strcmp(currentToken, ".") == 0) {
	    	Block		*p;
	    	
	    	if (debug_parse)
	    		fprintf(stderr, "sref\n");
	        GetToken();
	        
	        p = new Block(Block::C_STRUCTURE_REF);
	        p->crap.structureRef.base = varp;
	        StabEnt	*s = nullptr;
			if (varp) {
	        	s=varp->TypeSymbol();
				if (debug_parse) {
					fprintf(stderr, "ParseDExp: struct ref type %d %s\n", varp->type, s?s->name.c_str() :"no type sym");
				}
			}
			string name = currentToken;
			if (s) {
	        	glob.PushContext(s);
			}
	        p->crap.structureRef.member = nullptr;
	        p->crap.structureRef.unresolved = ParseAtom(context, schedSym, resolveNames);
			if (s) {
	        	glob.PopContext(s);
			}
			if (p->crap.structureRef.unresolved) {
				switch (p->crap.structureRef.unresolved->type) {
				case Block::C_SYM: {
	        		p->crap.structureRef.member = 
	        			p->crap.structureRef.unresolved->crap.sym;
	        		p->crap.structureRef.unresolved->DeleteAll();
					p->crap.structureRef.unresolved = nullptr;
	        		break;
				}
				default:
	        		ParseError(ERROR_ERR, "member expected, near '%s'", name.c_str());
				}
			}
			varp = p;
		}
   }
   return varp;
}

Block *
QSParser::ParseCExp(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*l, *r, *p;
    int		type;

    if (debug_parse) fprintf(stderr, "\tBlock::C_Exp\n");

/*    GetToken(); */
    p = l = ParseDExp(context, schedSym, resolveNames);

    type = Block::C_UNKNOWN;
    if (strcmp(currentToken, "*") == 0) 
		type = Block::OP_MUL;
    else if (strcmp(currentToken, "/") == 0)
		type = Block::OP_DIV;
    else if (strcmp(currentToken, "%") == 0)
		type = Block::OP_MOD;

    if (type != Block::C_UNKNOWN) {
		p = new Block( Block::C_BINOP, type);
		GetToken();
        r = ParseDExp(context, schedSym, resolveNames);
		p->crap.op.l = l;
		p->crap.op.r = r;
		if (r == nullptr) {
			ParseError(ERROR_ERR, "incomplete expression, near '%s'", currentToken);
		}
    }
    return p;
}

Block *
QSParser::ParseBExp(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*l, *r, *p;
    int		type;

    if (debug_parse) fprintf(stderr, "\tB_Exp\n");

    p = l = ParseCExp(context, schedSym, resolveNames);

    type = Block::C_UNKNOWN;
    if (strcmp(currentToken, "+") == 0) 
		type = Block::OP_ADD;
    else if (strcmp(currentToken, "-") == 0)
		type = Block::OP_SUB;

    if (type != Block::C_UNKNOWN) {
		p = new Block( Block::C_BINOP, type);
		GetToken();
        r = ParseBExp(context, schedSym, resolveNames);
		p->crap.op.l = l;
		p->crap.op.r = r;
		if (r == nullptr) {
			ParseError(ERROR_ERR, "incomplete expression, near '%s'", currentToken);
		}
    }
    return p;
}

Block *
QSParser::ParseAExp(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*l, *r, *p;
    int		type;

    if (debug_parse) fprintf(stderr, "\tA_Exp %s\n", currentToken);

    p = l = ParseBExp(context, schedSym, resolveNames);

    type = Block::C_UNKNOWN;
    if (strcmp(currentToken, ">") == 0)		type = Block::OP_GT;
    else if (strcmp(currentToken, "<") == 0)	type = Block::OP_LT;
    else if (strcmp(currentToken, "==") == 0)	type = Block::OP_EQ;
    else if (strcmp(currentToken, "!=") == 0)	type = Block::OP_NEQ;
    else if (strcmp(currentToken, "<=") == 0)	type = Block::OP_LE;
    else if (strcmp(currentToken, ">=") == 0)	type = Block::OP_GE;
 
    if (debug_parse) fprintf(stderr, "\tA_Exp: operator %d\n", type);

    if (type != Block::C_UNKNOWN) {
		p = new Block( Block::C_BINOP, type);
		GetToken();
	    r = ParseBExp(context, schedSym, resolveNames);
		p->crap.op.l = l;
		p->crap.op.r = r;
		if (r == nullptr) {
			ParseError(ERROR_ERR, "incomplete expression, near '%s'", currentToken);
		}
    }
    return p;
}

Block *
QSParser::ParseExpression(StabEnt *context, StabEnt *schedSym, bool resolveNames)
{
    Block	*l, *r, *p;
    int		type;

    if (debug_parse) fprintf(stderr, "\tExpr\n");
    type = Block::C_UNKNOWN;

    p = l = ParseAExp(context, schedSym, resolveNames);

    type = Block::C_UNKNOWN;
	if (strcmp(currentToken, "=") == 0) {
		if (debug_parse) {
			fprintf(stderr, "assign...\n");
		}
		p = new Block( Block::C_ASSIGN);
		GetToken();
		r = ParseExpression(context, schedSym, resolveNames);
		p->crap.assign.atom = l;
		p->crap.assign.exp = r;
		return p;
	} else if (strcmp(currentToken, "&&") == 0) {
		type = Block::OP_AND;
	} else if (strcmp(currentToken, "||") == 0) {
		type = Block::OP_OR;
	}

	if (strcmp(currentToken, "?") == 0) {
		Block	*ifb = new Block(Block::C_IFOP);
		ifb->crap.iff.condition = p;
		ifb->crap.iff.ifBlock = nullptr;
		ifb->crap.iff.elseBlock = nullptr;

		GetToken();
		ifb->crap.iff.ifBlock = ParseExpression(context, schedSym, resolveNames);
		if (strcmp(currentToken, ":") == 0) {
			GetToken();
			ifb->crap.iff.elseBlock = ParseExpression(context, schedSym, resolveNames);
		} else {
			ParseError(ERROR_ERR, "Expected ':' near '%s'", currentToken);
		}
		ifb->crap.iff.condVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_BYTE));
		ifb->crap.iff.doEvalVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_BYTE));
		return ifb;
	} else if (type == Block::C_UNKNOWN) {
		return p;
    } else {
		p = new Block( Block::C_BINOP, type);
		GetToken();
	    r = ParseExpression(context, schedSym, resolveNames);
		p->crap.op.l = l;
		p->crap.op.r = r;
    }

    return p;
}

Block *
QSParser::ParseBlockInfo(StabEnt *context, StabEnt *schedSym)
{
    Block		*block, *p;
//	Block		**lastpp;
//    int			i;

	if (debug_parse >= 2) {
		fprintf(stderr, "doing block, ctxt %s\n", context?context->name.c_str() :"global");
	}
	block = nullptr;
	if (	strcmp(currentToken, "[") == 0 ||
			strcmp(currentToken, "{|") == 0 ||
			strcmp(currentToken, "{&") == 0 ||
			strcmp(currentToken, "{") == 0) {
		block = ParseBlockList(context, schedSym, currentToken);
	} else if (	strcmp(currentToken, "]") == 0 ||
			strcmp(currentToken, "}") == 0) {
		block = nullptr;
    } else if (strcmp(currentToken, "define") == 0 || currentTokenType == TOK_TYPE) {
		if (debug_parse) {
			cout << "processing a define at " << currentToken << " type " << currentTokenType << endl;
		}
       	Block *b =ParseDefine(	context, schedSym);
      	block = b;
    } else if (strcmp(currentToken, "<<") == 0) {
	
		GetToken();
		p = new Block( Block::C_INPUT);
		short chan = atoi(currentToken);
		if (uberQua)
			p->crap.channel = uberQua->channel[chan-1];
		else {
			ParseError(ERROR_ERR, "midi io command must be defined in sequencer, near '%s'", currentToken);
			p->crap.channel = nullptr;
		}
		GetToken();
		block = p;
	
    } else if (strcmp(currentToken, "<-") == 0) {
//		int		i, j;
		Block	*cons, *rate;
	
		if (debug_parse) fprintf(stderr, "\tflux\n");
	
		p = new Block( Block::C_FLUX);
	
		GetToken();
		
		if (strcmp(currentToken, "(") == 0) {
		    p->crap.flux.lengthExp = ParseExpression(context, schedSym, true);
		} else
			p->crap.flux.lengthExp = nullptr;
		
		p->crap.flux.timeVar.Set(TypedValue::S_TIME, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_BYTE,sizeof(Time)));

		if (debug_parse)
			fprintf(stderr, "bout to do block\n");	
		cons = ParseBlockInfo(context, schedSym);
		
		if (strcmp(currentToken, ":") == 0) {
			GetToken();
			rate = ParseExpression(context, schedSym, true);
		} else
			rate = nullptr;
			
	
		p->crap.flux.block = cons;
		p->crap.flux.rateExp = rate;

		block = p;
    } else if (strcmp(currentToken, ">>") == 0) {
		int		chan;
	
		GetToken();
		chan = atoi(currentToken);
		if (debug_parse) fprintf(stderr, "\tout %d %x\n", chan, (unsigned)context);
	
		p = new Block( Block::C_OUTPUT);
		if (uberQua)
			p->crap.channel = uberQua->channel[chan-1];
		else {
			ParseError(ERROR_ERR, "midi command must be defined within a particular sequencer, near '%s'", currentToken);
			p->crap.channel = nullptr;
		}
		block = p;
		GetToken();
	} else if (	strcmp(currentToken, "repeat") == 0) {
		if (debug_parse) fprintf(stderr, "\trepeat\n");
		p = new Block( Block::C_REPEAT);
	
		GetToken();
		if (strcmp(currentToken, "(") != 0) {
		    ParseError(ERROR_ERR, "expected ( at '%s'", currentToken);
		}
		GetToken();
	    p->crap.repeat.Exp = ParseExpression(context, schedSym, true);
		if (strcmp(currentToken, ")") != 0) {
		    ParseError(ERROR_ERR, "expected ) at '%s'", currentToken);
		}
		GetToken();
		p->crap.repeat.block = ParseBlockInfo(context, schedSym);
		p->crap.repeat.countVar.Set(TypedValue::S_INT, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_INT));
		p->crap.repeat.boundVar.Set(TypedValue::S_INT, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_INT));

	
    } else if (strcmp(currentToken, "if") == 0) {
		if (debug_parse) fprintf(stderr, "\tif\n");
	
		p = new Block( Block::C_IF);
	
		GetToken();
		if (strcmp(currentToken, "(") != 0) {
		    ParseError(ERROR_ERR, "expected ( at '%s'", currentToken);
		}
		GetToken();
		p->crap.iff.condition = ParseExpression(context, schedSym, true);
		if (strcmp(currentToken, ")") != 0) {
		    ParseError(ERROR_ERR, "expected ) at '%s'", currentToken);
		}
		GetToken();
		p->crap.iff.ifBlock = ParseBlockInfo(context, schedSym);
	
		if (strcmp(currentToken, "else") == 0) {
		    if (debug_parse) fprintf(stderr, "\telse\n");
		    GetToken();
		    p->crap.iff.elseBlock = ParseBlockInfo(context, schedSym);
		} else {
		    p->crap.iff.elseBlock = nullptr;
		}
		block = p;

		p->crap.iff.condVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_BYTE));
		p->crap.iff.doEvalVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	context,
			(long)AllocStack(context,TypedValue::S_BYTE));

    } else if (strcmp(currentToken, "with") == 0) {
		if (debug_parse) fprintf(stderr, "\twith\n");
	
		p = new Block( Block::C_WITH);
	
		GetToken();
	    p->crap.with.object = ParseAtom(context, schedSym, true);

		if (p->crap.with.object && p->crap.with.object->type == Block::C_SYM) {
			p->crap.with.objectType = p->crap.with.object->crap.sym;
			glob.PushContext(p->crap.with.object->crap.sym);
		} else {
			p->crap.with.objectType = nullptr;
		}
		if (strcmp(currentToken, "having") == 0) {
			GetToken();
		
			if (strcmp(currentToken, "(") != 0) {
			    ParseError(ERROR_ERR, "expected ( at '%s'", currentToken);
			}
			GetToken();
		    p->crap.with.condition = ParseExpression(context, schedSym, true);
		    
			if (strcmp(currentToken, ")") != 0) {
			    ParseError(ERROR_ERR, "expected ) at '%s'", currentToken);
			}
			GetToken();
			
		} else {
		    p->crap.with.condition = nullptr;
		}
		p->crap.with.withBlock = ParseBlockInfo(context, schedSym);

		if (p->crap.with.object && p->crap.with.object->type == Block::C_SYM) {
			glob.PopContext(nullptr);
		}

		if (strcmp(currentToken, "else") == 0) {
		    if (debug_parse) fprintf(stderr, "\twithout\n");
		    GetToken();
		    p->crap.with.withoutBlock = ParseBlockInfo(context, schedSym);
		} else {
		    p->crap.with.withoutBlock = nullptr;
		}
	
		block = p;

    } else if (strcmp(currentToken, "foreach") == 0) {
		if (debug_parse) fprintf(stderr, "\twith\n");
	
		p = new Block( Block::C_FOREACH);
	
		GetToken();
		
		if (strcmp(currentToken, "having") == 0) {
			GetToken();
		
			if (strcmp(currentToken, "(") != 0) {
			    ParseError(ERROR_ERR, "expected ( at '%s'", currentToken);
			}
			GetToken();
		    p->crap.foreach.condition = ParseExpression(context, schedSym, true);
		    
			if (strcmp(currentToken, ")") != 0) {
			    ParseError(ERROR_ERR, "expected ) at '%s'", currentToken);
			}
			GetToken();
			
			p->crap.foreach.ifBlock = ParseBlockInfo(context, schedSym);
			if (strcmp(currentToken, "else") == 0) {
			    if (debug_parse) fprintf(stderr, "\tforeach else\n");
			    GetToken();
			    p->crap.foreach.elseBlock = ParseBlockInfo(context, schedSym);
			} else {
			    p->crap.foreach.elseBlock = nullptr;
			}
		} else {
		    p->crap.foreach.condition = nullptr;
			p->crap.foreach.ifBlock = ParseBlockInfo(context, schedSym);
			p->crap.foreach.elseBlock = nullptr;
		}

	
		block = p;

	} else if (strcmp(currentToken, "wait") == 0) {
		if (debug_parse) fprintf(stderr, "wait\n");
	 
		p = new Block(Block::C_WAIT);
	
		GetToken();
		p->crap.block = ParseExpression(context, schedSym, true);
	
		block = p;
	
    } else if (strcmp(currentToken, "suspend") == 0) {
		if (debug_parse) fprintf(stderr, "\tsuspend\n");
	
		GetToken();
		
		p = ParseAtom(context, schedSym, true);
		if (p->type == Block::C_SYM) {
			StabEnt		*sym = p->crap.sym;
			p->crap.call.crap.sym = sym;
		} else if (p->type == Block::C_NAME) {
			char		*nm = p->crap.name;
			p->crap.call.crap.name = nm;
			ParseError(ERROR_ERR, "suspend linkage error at '%s'", currentToken);
		}
		p->type = Block::C_SUSPEND;
	
		block = p;
	
    } else if (strcmp(currentToken, "wake") == 0) {
		if (debug_parse) fprintf(stderr, "\twake\n");
	 
		GetToken();
		
		p = ParseAtom(context, schedSym, true);
		if (p->type == Block::C_SYM) {
			StabEnt		*sym = p->crap.sym;
			p->crap.call.crap.sym = sym;
		} else if (p->type == Block::C_NAME) {
			char		*nm = p->crap.name;
			p->crap.call.crap.name = nm;
			ParseError(ERROR_ERR, "wake linkage error, at '%s'", currentToken);
		}
		p->type = Block::C_WAKE;
	
		block = p;
	
    } else if (strcmp(currentToken, "break") == 0) {
		if (debug_parse) fprintf(stderr, "break\n");
	 
		p = new Block( Block::C_BREAK);
		GetToken();
		block = p;
	
    } else {
		Block	*atom;
// should be expr....
		atom = ParseExpression(context, schedSym, true);
		if (atom == nullptr) {
			fprintf(stderr, "got a null atom\n");
			return nullptr;
		}
		if (debug_parse) {
			fprintf(stderr, "parsed an atom expression: type %d\n", atom->type);
		}

		switch (atom->type) {
			case Block::C_UNLINKED_CALL:
			case Block::C_WAKE:
			case Block::C_SCHEDULE:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_CALL:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_VST:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_BUILTIN:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_GENERIC_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_STREAM_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_TUNEDSAMPLE_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_SAMPLE_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_MIDI_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			case Block::C_MARKOV_PLAYER:
				block = atom;
				if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				break;
			default:
				if (strcmp(currentToken, "::") == 0) {
					Block	*b;
			
					if (debug_parse) fprintf(stderr, "\tguard\n");
				
		//		} else if (atom->type == Block::C_NAME && strcmp(currentToken, ":") == 0) {
		//		    b = ParseBlockInfo(context);
					p = new Block( Block::C_GUARD);
					GetToken();
					b = ParseBlockInfo(context, schedSym);
					p->crap.guard.condition = atom;
					p->crap.guard.block = b;
					block = p;
					if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
					p->crap.guard.condVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK, context,
						(long)AllocStack(context,TypedValue::S_BYTE));
					p->crap.guard.doEvalVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK, context,
						(long)AllocStack(context,TypedValue::S_BYTE));
				} else {
					p = atom;
					block = p;
					if (block->labelSym) block->labelSym->SetLabelValue(block, nullptr);
				}
				break;
		}

	}
  
    if (strcmp(currentToken, "@") == 0) {
    	p = block;
    	block = new Block( Block::C_DIVERT);
		GetToken();
		block->crap.divert.block = p;
		block->crap.divert.clockExp = ParseExpression(context, schedSym, true);
	} else if (strcmp(currentToken, "set") == 0) {
		p = block;
		block = new Block(Block::C_SET_ATTRIB);
		GetToken(true);
		block->crap.setAttrib.block = p;
		block->crap.setAttrib.attributes = parseAttributeList(context, schedSym, true);
	}
    return block;
}

void
QSParser::ParseSchedulable(Schedulable *S)
{
	Lambda	*So;
	
	if (debug_parse)
		fprintf(stderr, "reading schedulable %x %x\n", (unsigned)S, (unsigned)S->sym);
    ParseFormalsList(S->sym, S->sym, true);
    S->mainBlock = ParseBlockInfo(S->sym, S->sym);
	if (S->sym->type == TypedValue::S_POOL && (So=findMethod("Init", -1))) {
		((Pool *)S)->initBlock = So->mainBlock;
	}
}

Block *
QSParser::ParseDefine(StabEnt *context, StabEnt *schedSym)
{
    Stream		y;
    bool		isLocus = false,
    			isModal = false,
    			isOncer = false,
    			isPool = false,
    			isFixed = false,
    			isToggled = false,
    			isException= false,
    			isInit = false,
    			isHeld = false;
    long		type = TypedValue::S_UNKNOWN;
    long		templateType = TypedValue::S_UNKNOWN;
    long		subType = 0;
	long		chDstIndex = -1;
    long		eventType = TypedValue::S_UNKNOWN;
    StabEnt		*sym=nullptr;
	Schedulable	*schedulable = nullptr;

	string dataPath;
	string mime;

	bool		foundDisplayParameters=false;

	TypedValue	resetVal;
	resetVal.Set(TypedValue::S_UNKNOWN,TypedValue::REF_STACK,0,0);


    if (strcmp(currentToken, "define") == 0) {
    	GetToken();
    }
    if (debug_parse >= 2)
    	fprintf(stderr, "defining a type '%s' in %s %d\n", currentToken, context?context->name.c_str() :"global", context?context->type:-1);
    	
    if (currentTokenType == TOK_TYPE) {
		if (currentToken[0] == '\\') { // a type qualifier
			type = TypedValue::S_LAMBDA;
		} else {
			type=findType(currentToken);
			if (type == TypedValue::S_UNKNOWN) {
				type = findDfltEventType(currentToken);
				switch(type) {
					case Attribute::EVENT_RX:
					case Attribute::EVENT_TX:
					case Attribute::EVENT_RECEIVE:
					case Attribute::EVENT_WAKE:
					case Attribute::EVENT_SLEEP:
					case Attribute::EVENT_START:
					case Attribute::EVENT_STOP:
					case Attribute::EVENT_RECORD:
					case Attribute::EVENT_CUE:
					case Attribute::EVENT_INIT: {
						eventType = type;
						type = TypedValue::S_EVENT;
						break;
					}
				}
			}
			
			GetToken();	// we have the type, so ... 
			if (strcmp(currentToken, "(") == 0) { // constructor
				Block	*pb = ParseActualsList(context, schedSym, true /*resolveNames*/);
				Block	*cb = new Block(Block::C_BUILTIN, Block::BUILTIN_CREATE);
				cb->crap.call.crap.createType = type;
				cb->crap.call.parameters = pb;
				GetToken();
				return cb;
			}
			switch (type) {
				case TypedValue::S_CHANNEL:
				case TypedValue::S_INPUT:
				case TypedValue::S_OUTPUT: {
					if (currentTokenType == TOK_VAL) {
						chDstIndex = (uint32) currentTokenVal.IntValue(nullptr);
						GetToken();
					}
					break;
				}
			}
    	}
	} else if (strcmp(currentToken, "<") == 0) {
		type = TypedValue::S_TEMPLATE;
		if (context && context != nullptr)
			ParseError(ERROR_ERR, "template must be defined in application context, at '%s'", currentToken);
		GetToken();
		if (currentTokenType == TOK_TYPE) {
			templateType=findType(currentToken);
			switch (templateType) {
			case TypedValue::S_SAMPLE:
				GetToken();
				break;
				
			case TypedValue::S_VOICE:
				GetToken();
				break;
			case TypedValue::S_POOL:
				GetToken();
				break;
			case TypedValue::S_LAMBDA:
				GetToken();
				break;
				
			case TypedValue::S_INPUT:
			case TypedValue::S_OUTPUT:
				GetToken();
				break;
				
			case TypedValue::S_FLOAT:
			case TypedValue::S_BYTE:
			case TypedValue::S_LONG:
			case TypedValue::S_SHORT:
			case TypedValue::S_INT:
			case TypedValue::S_BOOL:
			case TypedValue::S_STRUCT:
				GetToken();
				break;
			default:
	    		ParseError(ERROR_ERR, "type '%s' cannot be used in templates", currentToken);
	    		GetToken();
	    	}
	    } else {
	    	ParseError(ERROR_ERR, "expected type for template, near '%s'", currentToken);
	    	GetToken();
	    }
    } else {
    	type = TypedValue::S_LAMBDA;
	}
	int	ndimensions=0;
	int	dimensions[50];
	while (strcmp(currentToken, "[") == 0) {
		GetToken();
		Block *b = ParseExpression(context, schedSym, true);
		ResultValue	v = EvaluateExpression(b, nullptr, nullptr, nullptr);
		if (v.Blocked()) {
			ParseError(ERROR_ERR, "cannot evaluate dimension, near '%s'", currentToken);
			dimensions[ndimensions] = 1;
		} else {
			dimensions[ndimensions] = v.IntValue(nullptr);
		}
		ndimensions ++;
		if (strcmp(currentToken, "]") == 0) {
			GetToken();
		} else {
			ParseError(ERROR_ERR, "']' expected for dimension, near '%s'", currentToken);
		}
	}
	
	bool	first=true;

	bool doLoadPlugin = false;
	bool isSynthPlugin = false;
	bool isEnabled = true;
	bool mapVstParams = false;
	bool isStream = false;
	bool isSample = false;
	uint32 id32 = 'qua6';
	int32 nIns = 2;
	int32 nOuts = 2;
	int32 nParam = 0;
	int32 nProgram = 0;
	bool audioThru = false;
	bool midiThru = false;
	int deviceKind = QuaPort::Device::MIDI;
	while (currentTokenType == TOK_TYPE) {
		subType = findTypeAttribute(currentToken);
		if (debug_parse) {
			cout << "subtype " <<  subType << " at " << currentToken << endl;
		}
    	switch (subType) {
    	
   		case Attribute::LOAD:
			GetToken();
    		doLoadPlugin = true;
    		break;
   		case Attribute::NOLOAD:
			GetToken();
    		doLoadPlugin = false;
    		break;
   		case Attribute::SYNTH:
			GetToken();
    		isSynthPlugin = true;
    		break;
   		case Attribute::ENABLE:
			GetToken();
    		isEnabled = true;
    		break;
   		case Attribute::DISABLE:
			GetToken();
    		isEnabled = false;
    		break;
   		case Attribute::MAP_VSTPARAM:
			GetToken();
    		mapVstParams = true;
    		break;
   		case Attribute::AUDIOTHRU:
			GetToken();
    		audioThru = true;
    		break;
   		case Attribute::MIDITHRU:
			GetToken();
    		midiThru = true;
    		break;
   		case Attribute::NO_AUDIOTHRU:
			GetToken();
    		audioThru = false;
    		break;
   		case Attribute::NO_MIDITHRU:
			GetToken();
    		midiThru = false;
    		break;
   		case Attribute::ID:
			GetToken();
			if (currentTokenType == TOK_VAL) {
				id32 = (uint32) currentTokenVal.IntValue(nullptr);
			} else {
				ParseError(ERROR_ERR, "Expected a uint constant for id near '%s'", currentToken);
			}
			GetToken();
    		break;

		case Attribute::DEVICE_AUDIO: {
			GetToken();
			deviceKind = QuaPort::Device::AUDIO;
			break;
		}

		case Attribute::DEVICE_MIDI: {
			GetToken();
			deviceKind = QuaPort::Device::MIDI;
			break;
		}

		case Attribute::DEVICE_JOYSTICK: {
			GetToken();
			deviceKind = QuaPort::Device::JOYSTICK;
			break;
		}

		case Attribute::DEVICE_PARALLEL: {
			GetToken();
			deviceKind = QuaPort::Device::PARALLEL;
			break;
		}
		case Attribute::DEVICE_SENSOR: {
			GetToken();
			deviceKind = QuaPort::Device::SENSOR;
			break;
		}

		case Attribute::DEVICE_OSC: {
			GetToken();
			deviceKind = QuaPort::Device::OSC;
			break;
		}

		case Attribute::INS:
			GetToken();
			if (currentTokenType == TOK_VAL) {
				nIns = currentTokenVal.IntValue(nullptr);
			} else {
				ParseError(ERROR_ERR, "Expected an int constant for id near '%s'", currentToken);
			}
			GetToken();
    		break;
   		case Attribute::OUTS:
			GetToken();
			if (currentTokenType == TOK_VAL) {
				nOuts = (uint32) currentTokenVal.IntValue(nullptr);
			} else {
				ParseError(ERROR_ERR, "Expected an int constant for id near '%s'", currentToken);
			}
			GetToken();
    		break;
   		case Attribute::NPARAM:
			GetToken();
			if (currentTokenType == TOK_VAL) {
				nParam = currentTokenVal.IntValue(nullptr);
			} else {
				ParseError(ERROR_ERR, "Expected an int constant for id near '%s'", currentToken);
			}
			GetToken();
    		break;
   		case Attribute::NPROGRAM:
			GetToken();
			if (currentTokenType == TOK_VAL) {
				nProgram = currentTokenVal.IntValue(nullptr);
			} else {
				ParseError(ERROR_ERR, "Expected an int constant for id near '%s'", currentToken);
			}
			GetToken();
    		break;
   		case Attribute::MODAL:
			GetToken();
    		isModal = true;
    		break;
    	case Attribute::NODE:
			GetToken();
    		isLocus = true;
    		break;
    	case Attribute::ONCER:
			GetToken();
    		isOncer = true;
    		break;
    	case Attribute::FIXED:
			GetToken();
    		isFixed = true;
    		break;
		case Attribute::RESET:
			GetToken();
			isInit = true;
			break;
		case Attribute::MIME: {
    		GetToken(true);
			mime = currentToken;
			GetToken();
			break;
		}
		
		case Attribute::STREAM: {
			isStream = true;
			GetToken();
			break;
		}

		case Attribute::SAMPLE: {
			isSample=true;
    		GetToken(true);
			dataPath = currentToken;
			GetToken();
			break;
		}

		case Attribute::PATH: {
    		GetToken(true);
			dataPath = currentToken;
			GetToken();
			break;
		}
		
		case Attribute::TOGGLE:
		   	GetToken();
			isToggled = true;
			break;
		case Attribute::HELD:
		   	GetToken();
			isHeld = true;
			break;
		case Attribute::RESETTO:
		   	GetToken();
		   	if (currentTokenType != TOK_VAL) {
		   		ParseError(ERROR_ERR, "expected constant reset value, near '%s'", currentToken);
		   	} else {
		   		resetVal = currentTokenVal;
		   	}
		   	GetToken();
			break;
    	default:
    		ParseError(ERROR_ERR, "unknown type qualifier '%s'", currentToken);
		   	GetToken();
    	}
    	first = false;
    }

   	if (type == TypedValue::S_TEMPLATE) {
   		if (strcmp(currentToken, ">") == 0) {
    		GetToken();
    	} else {
     		ParseError(ERROR_ERR, "template: '>' expected near '%s'");
   		}
    }
	if (type != TypedValue::S_EVENT && currentTokenType != TOK_WORD) {
		ParseError(ERROR_ERR, "Expected identifier in definition near '%s'", currentToken);
		if (debug_parse) {
			cout << "expect ident: type " << type << " subtype " << subType << " at " << currentToken << " (" << currentTokenType << ")" << endl;
		}
	}
//	if (type != TypedValue::S_VST_PLUGIN) reportError("type %d %s", type, currentToken);
	switch (type) {

	case TypedValue::S_EVENT: {
		Schedulable	*sch;
		Template	*tem;
		Channel		*cha;
		Event		*ev = nullptr;
		QuasiStack	*qs = nullptr;
		if (context) {
			if (context->type == TypedValue::S_TEMPLATE) {
				ParseError(ERROR_ERR, "fix up template events you nong, near '%s'", currentToken);
				Block		*b = ParseBlockInfo(context, context);
				b->DeleteAll();
			} else if ((cha=context->ChannelValue())!=nullptr) {
				switch (eventType) {
					case Attribute::EVENT_TX: ev = &cha->tx; qs = cha->txStack; break;
					case Attribute::EVENT_RX: ev = &cha->rx; qs = cha->rxStack; break;
				}
				if (ev && qs) {
					*ev = ParseBlockInfo(cha->rx.sym, schedSym);
					qs->CheckMulchSize();
				} else {
					ParseError(ERROR_ERR, "event not valid in channel, near '%s'", currentToken);
					Block		*b = ParseBlockInfo(context, context);
					b->DeleteAll();
				}
			} else if ((sch=context->SchedulableValue()) != nullptr) {
				switch (eventType) {
					case Attribute::EVENT_WAKE: ev = &sch->wake; break;
					case Attribute::EVENT_SLEEP: ev = &sch->sleep; break;
					case Attribute::EVENT_RX: ev = &sch->rx; break;
					case Attribute::EVENT_CUE: ev = &sch->cue; break;
					case Attribute::EVENT_START: ev = &sch->start; break;
					case Attribute::EVENT_STOP: ev = &sch->stop; break;
					case Attribute::EVENT_RECORD: ev = &sch->record; break;
					case Attribute::EVENT_INIT:  ev = &sch->init; break;
						break;
				}
				if (ev) {
					*ev = ParseBlockInfo(sch->rx.sym, schedSym);
				} else {
					ParseError(ERROR_ERR, "event not valid in schedulable, near '%s'", currentToken);
					Block		*b = ParseBlockInfo(context, context);
					b->DeleteAll();
				}

			} else {
				ParseError(ERROR_ERR, "lousy non context for handler, near '%s'", currentToken);
				Block		*b = ParseBlockInfo(context, context);
				b->DeleteAll();
			}
	
		} else {
			ParseError(ERROR_ERR, "lousy non context for handler, near '%s'", currentToken);
			Block		*b = ParseBlockInfo(context, context);
			b->DeleteAll();
		}
		break;
	}

	case TypedValue::S_CLIP: {
		if (!context) {
			ParseError(ERROR_ERR, "Clips must be defined within a context");
		}
		char	nmbuf[256];
		strcpy(nmbuf, currentToken);
		GetToken();
		Block *acts = ParseActualsList(context, context, true);
		GetToken();
		if (context && uberQua) {
			Time	at_t;
			Time	dur_t;
			at_t.Set("0:0.0");
			dur_t.Set("1:0.0");
			Take *take=nullptr;

			if (acts) {
	   			ResultValue		v0 = EvaluateExpression(acts);
				if (v0.type == TypedValue::S_TAKE) {
					take = (Take *)v0.PointerValue(nullptr);
				}
				acts = acts->next;
				if (acts) {
		   			ResultValue		v1 = EvaluateExpression(acts);
					if (v1.type == TypedValue::S_TIME) {
						at_t = *v1.TimeValue();
					} else {
						at_t = Time(v1.IntValue(nullptr), uberQua->metric);
					}
					acts = acts->next;
					if (acts) {
	   					ResultValue		v2 = EvaluateExpression(acts);
						if (v2.type == TypedValue::S_TIME) {
							dur_t = *v2.TimeValue();
						} else {
							dur_t = Time(v2.IntValue(nullptr), uberQua->metric);
						}
					}
				}
			}
			if (context->type == TypedValue::S_VOICE) {
				Voice *v = context->VoiceValue();
				if (take->sym->context == context && take->type == Take::STREAM) {
					v->AddClip(nmbuf, (StreamTake *)take, at_t, dur_t, false);
				}
			} else if (context->type == TypedValue::S_SAMPLE) {
				Sample *s = context->SampleValue();
				if (take->sym->context == context && take->type == Take::SAMPLE) {
					s->AddClip(nmbuf, (SampleTake*)take, at_t, dur_t, false);
				}
			} else if (context->type == TypedValue::S_POOL) {
				Pool *p = context->PoolValue();
			} else if (context->type == TypedValue::S_QUA) {
				Qua *q = context->QuaValue();
			}
		} else {
			display.reportError("expected non null context at clip");
		}

		break;
	}

	case TypedValue::S_TAKE: {
		string nm;
		if (currentTokenType == TOK_WORD) {
			nm = currentToken;
			GetToken(true); // we're ready for worms
		} else {
			nm = "takedata";
		}
		if (schedSym) {
			Block *initBlock = nullptr;
			if (currentTokenType == TOK_VAL && currentTokenVal.type == TypedValue::S_STRING) {
				dataPath = currentToken;
				GetToken();
			} else if (strcmp(currentToken, "[")==0 || strcmp(currentToken, "{") == 0) { 
				// TODO XXX FIXME messy syntax is getting messy lots of odd cases could happen
				initBlock = ParseBlockInfo(context, schedSym);
			}

			if (schedSym->type == TypedValue::S_SAMPLE) {
				Sample	*samp = schedSym->SampleValue();
				samp->addSampleTake(nm, dataPath, false);
			} else if (schedSym->type == TypedValue::S_VOICE) {
				Voice	*v = schedSym->VoiceValue();
				Time	dur = Time::zero;
				if (dataPath.size()) {
					v->addStreamTake(nm, dataPath, false);
				} else {
					v->addStreamTake(nm, dur, false);
				}
			} else if (schedSym->type == TypedValue::S_POOL) {
				Pool	*p = schedSym->PoolValue();
				Time	dur = Time::zero;
				if (dataPath.size()) {
					p->addStreamTake(nm, dataPath, false);
				} else {
					p->addStreamTake(nm, dur, false);
				}
			} else {
				ParseError(ERROR_ERR, "inappropriate context for take data, near '%s'", currentToken);
			}
			if (initBlock != nullptr) {
				initBlock->DeleteAll();
			}
		} else {
			ParseError(ERROR_ERR, "inappropriate context for take data, near '%s'", currentToken);
			Block		*b = ParseBlockInfo(context, schedSym);
			b->DeleteAll();
		}
		break;
	}
	
	case TypedValue::S_INPUT: {
		string nm;
		cout << "defining an input" << endl;
		if (currentTokenType == TOK_WORD) {
			nm = currentToken;
			GetToken(true);
		} else {
			nm = "linein";
		}
		vector<int> chans;
		string portDeviceName = currentToken;
		QuaPort *port = nullptr;
		StabEnt *sym = nullptr;
		bool found = parsePort(deviceKind, portDeviceName, port, sym, QUA_PORT_IN, chans);
		if (context && context->type == TypedValue::S_CHANNEL) {
 			Channel		*cha=context->ChannelValue();

			Input *s = cha->AddInput(nm, PortSpec(deviceKind, portDeviceName, chans), true);
			// todo this is a a strage attempt to do initialization of parameters like port gain: XXXX do this differently, it sucks
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
		} else if (context && context->type == TypedValue::S_QUA) {
			Qua		*cha=context->QuaValue();
//			Output *s = cha->AddOutput(nm, trxport[0], sch[0], true);
//			for (short i=1; i<nport; i++) {
//				s->SetPortInfo(trxport[i], sch[i], i);
//			}
//			glob.PushContext(s->sym);
//			s->initBlock = ParseBlockInfo(s->sym, s->sym);
//			glob.PopContext();
			// xxxx as aboce
			if (strcmp(currentToken, "{") == 0) {
				Block		*b = ParseBlockInfo(context, schedSym);
				if (b) {
					b->DeleteAll();
				}
			}
		} else {
			ParseError(ERROR_ERR, "lousy context for input, %s", context->name);
			if (strcmp(currentToken, "{") == 0) {
				Block		*b = ParseBlockInfo(context, schedSym);
				if (b) {
					b->DeleteAll();
				}
			}
			break;
		}
		cout << "found an input and done, currently at " << currentToken << endl;
		break;
	}
	
	case TypedValue::S_OUTPUT: {
		string nm;
		cout << "defining an oputput" << endl;

		if (currentTokenType == TOK_WORD) {
			nm = currentToken;
			GetToken(true);
		} else {
			nm = "lineout";
		}
		vector<int> chans;
		string portDeviceName = currentToken;
		QuaPort *port = nullptr;
		StabEnt *sym = nullptr;
		bool found = parsePort(deviceKind, portDeviceName, port, sym, QUA_PORT_OUT, chans);
		if (context && context->type == TypedValue::S_CHANNEL) {
			Channel		*cha=context->ChannelValue();
			Output *s = cha->AddOutput(nm, PortSpec(deviceKind, portDeviceName, chans), true);
			// xxxx as aboce
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
		} else if (context && context->type == TypedValue::S_QUA) {
			Qua		*cha=context->QuaValue();
//			Output *s = cha->AddOutput(nm, trxport[0], sch[0], true);
//			for (short i=1; i<nport; i++) {
//				s->SetPortInfo(trxport[i], sch[i], i);
//			}
//			glob.PushContext(s->sym);
//			s->initBlock = ParseBlockInfo(s->sym, s->sym);
//			glob.PopContext();
			// xxxx as above
			if (strcmp(currentToken, "{") == 0) {
				Block		*b = ParseBlockInfo(context, schedSym);
				if (b) {
					b->DeleteAll();
				}
			}
		} else {
			ParseError(ERROR_ERR, "lousy context for input, %s", context->name);
			if (strcmp(currentToken, "{") == 0) {
				Block		*b = ParseBlockInfo(context, schedSym);
				if (b) {
					b->DeleteAll();
				}
			}
			break;
		}
		cout << "found an toutput and done, currently at " << currentToken << endl;
		break;
	}
	
		
		
	case TypedValue::S_TEMPLATE: {
 	    Template	*F = new Template(
	    						currentToken,
	    						context,
	    						templateType,
	    						mime,
	    						dataPath);
	    GetToken();
	    ParseFormalsList(F->sym, F->sym, true);
	    glob.PushContext(F->sym);
	    F->mainBlock = ParseBlockInfo(F->sym, F->sym);
	    sym = F->sym;
	    glob.PopContext(F->sym);
	    
	    F->next = templates;
	    templates = F;
	    
		break;
	}
	
	case TypedValue::S_INSTANCE: {
		if (!context) {
			ParseError(ERROR_ERR, "Instances must be defined within a schedulable's context");
		}
		Schedulable	*sch = context->SchedulableValue();
		if (!sch) {
			ParseError(ERROR_ERR, "Instances must be defined within a schedulable's context");
		}
		char	nmbuf[256];
		strcpy(nmbuf, currentToken);
		GetToken();
		Block *acts = ParseActualsList(context, context, true);
		GetToken();
		if (sch && uberQua) {
			Time	at_t;
			Time	dur_t;
			short	chan_id;
			chan_id = 0;
			at_t.Set("0:0.0");
			dur_t.Set("1:0.0");

			if (acts) {
	   			ResultValue		v0 = EvaluateExpression(acts);
				Channel *c=nullptr;
				if (v0.type == TypedValue::S_CHANNEL) {
					c = (Channel *)v0.PointerValue(nullptr);
					chan_id = c->chanId;
				} else {
					short chid = v0.IntValue(nullptr);
					if (chid >= 0 && chid < uberQua->nChannel) {
						chan_id = chid;
					}
				}
				acts = acts->next;
				if (acts) {
		   			ResultValue		v1 = EvaluateExpression(acts);
					if (v1.type == TypedValue::S_TIME) {
						at_t = *v1.TimeValue();
					} else {
						at_t = Time(v1.IntValue(nullptr), uberQua->metric);
					}
					acts = acts->next;
					if (acts) {
	   					ResultValue		v2 = EvaluateExpression(acts);
						if (v2.type == TypedValue::S_TIME) {
							dur_t = *v2.TimeValue();
						} else {
							dur_t = Time(v2.IntValue(nullptr), uberQua->metric);
						}
					}
				}
			}
			Instance	*instance = sch->addInstance(nmbuf, chan_id, at_t, dur_t, false);
			if (instance) {
//				for (i=0; i<NIndexer(); i++) {
//					Indexer(i)->AddToSymbolIndex(instance->sym);
//				}
//				for (i=0; i<NArranger(); i++) {
//					Arranger(i)->AddInstanceRepresentation(instance);
//				}
			}
		} else {
			display.reportError("expected non null context at instance");
		}
		break;
	}

	case TypedValue::S_VOICE: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "voice should be undimensioned");
		}

    	Voice		*V;
    	
		if (!uberQua || context != uberQua->sym)
			ParseError(ERROR_ERR, "voice must be in outer context");
	    V = new Voice(currentToken, uberQua);
	    sym = V->sym;
	    V->next = schedulees;
	    schedulees = V;
	
	    GetToken();
	    glob.PushContext(V->sym);
#ifdef VOICE_MAINSTREAM
	    glob.PushContext(V->streamSym);
#endif	    
	    ParseSchedulable(V);

#ifdef VOICE_MAINSTREAM
		glob.PopContext(V->streamSym);
#endif
	    glob.PopContext(V->sym);
		schedulable = V; 
	    break;
	}

/*
TODO FIXME XXXX probably unnecessary ... an alias for a port. i think these are better left of defined
by context automatically and matched by the inputs/outputs in channels
*/
	case TypedValue::S_PORT: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "port should be undimensioned");
		}

		QuaPort		*P;
		if (!uberQua || context != uberQua->sym)
			ParseError(ERROR_ERR, "port must be in outer context.");
		P = findQuaPort(-1, currentToken, QUA_PORT_IO);
		if (P == nullptr) {
			P = new QuaPort(currentToken, QuaPort::Device::NOT, QUA_DEV_GENERIC, QUA_PORT_IO);
//			, quapp->quaSmallIcon, quapp->quaBigIcon);
			fprintf(stderr, "Can't find port %s\n", currentToken);
		}
//		else if (P->uberQua && P->uberQua != uberQua) {
//			P = new QuaPort(currentToken, QuaPort::Device::NOT, QUA_DEV_GENERIC, QUA_PORT_IO);
//			, quapp->quaSmallIcon, quapp->quaBigIcon);
//			fprintf(stderr, "Port already under control of a different sequencer\n");
//		}

//	    P->next = schedulees;
//	    P->metric = uberQua->metric;
//	    schedulees = P;
//	    P->uberQua = uberQua;
		GetToken();
//	    glob.PushContext(P->sym);
//	    sym = P->sym;
//	    ParseSchedulable(P);
//	    glob.PopContext();
//		schedulable = P; 
	    break;
	}
	
	case TypedValue::S_SAMPLE: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "sample should be undimensioned");
		}
		Sample *S;
		if (!uberQua || context != uberQua->sym)
			ParseError(ERROR_ERR, "sample must be in outer context.");

//		entry_ref	fileRef;
		S = new Sample(currentToken, "", uberQua, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
		S->next = schedulees;
	    schedulees = S;
	
	    GetToken();
	    glob.PushContext(S->sym);
	    sym = S->sym;
	    ParseSchedulable(S);

	    glob.PopContext(S->sym);

		schedulable = S; 
	    break;
	}
	
	case TypedValue::S_POOL: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "pool should be undimensioned");
		}

		if (!uberQua || context != uberQua->sym)
			ParseError(ERROR_ERR, "pool must be in outer context.");

		Pool	*S;

	    S = new Pool(currentToken, uberQua, true);
	    S->next = schedulees;
	    schedulees = S;

		GetToken();
	    glob.PushContext(S->sym);
	    sym = S->sym;
	    ParseSchedulable(S);
	    glob.PopContext(S->sym);
		schedulable = S; 
		break;
	}
	
	case TypedValue::S_CHANNEL: {
		if (debug_parse) {
			cout << "define channel " << currentToken << " in " << nIns << ", " << nOuts << " thrue " << audioThru << "," << midiThru << endl;
		}
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "channel should be undimensioned");
		}
		string nm = currentToken;
		GetToken();
		if (nIns < 0) {
			nIns = 2;
		}
		if (nOuts < 0) {
			nOuts = 2;
		}
		Channel	*c = nullptr;
		if (!uberQua)
			ParseError(ERROR_ERR, "channel must be defined in top context");
		else {
			c = uberQua->AddChannel(
						nm,	(short)((chDstIndex >= 0)?chDstIndex:uberQua->nChannel),
						nIns, nOuts,
						audioThru, midiThru,
						false, false, false, false,
						(short)uberQua->nChannel);
			if (c) {
				sym = c->sym;
			}
		}
		if (sym) glob.PushContext(sym);
		Block	*block = ParseBlockInfo(sym, sym);
	    if (sym) glob.PopContext(sym);
		if (block) {
			if (c) {
				c->initBlock = block;
			} else {
				block->DeleteAll();
			}
		}
		break;
	}
	
	case TypedValue::S_VST_PLUGIN: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "vst plugin should be undimensioned");
		}
	    VstPlugin	*vstp;
		vstp = new VstPlugin(dataPath.c_str(), currentToken, doLoadPlugin, mapVstParams, isSynthPlugin, nIns, nOuts, nParam, nProgram);
	    GetToken();
	    ParseFormalsList(vstp->sym, schedSym, doLoadPlugin);
	    sym = vstp->sym;
		vstplugins.add(vstp);
	    break;
	}
	
	case TypedValue::S_LAMBDA: {
		if (ndimensions != 0) {
			ParseError(ERROR_ERR, "lambda should be undimensioned");
		}
	    Lambda	*lambda;
	    lambda = new Lambda(currentToken, context, isLocus, isModal, isOncer, isFixed, isHeld, isInit);
		lambda->resetVal = resetVal;
	    lambda->next = methods;
	    methods = lambda;

		fprintf(stderr, "lambda %s\n", currentToken);
	    GetToken();
	    ParseFormalsList(lambda->sym, schedSym, true);
	    glob.PushContext(lambda->sym);
	    lambda->mainBlock = ParseBlockInfo(lambda->sym, schedSym);
		fprintf(stderr, "lambda block %x\n", (unsigned) lambda->mainBlock);
	    sym = lambda->sym;
	    glob.PopContext(lambda->sym);
	    
	    break;
	}
	
	case TypedValue::S_FILE: {
	    sym = DefineSymbol(currentToken, TypedValue::S_FILE,
						0, 0, context,TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);
	    				

	    for (short i=1; i<=ndimensions; i++) {
	    	sym->size[i] = dimensions[i-1];
	    }

	    GetToken();
//	    glob.PushContext(sym);
//	    Block *X = ParseBlockInfo(sym);
//		if (X)
//			ParseError(ERROR_ERR, "malformed struct");
//		glob.PopContext();
		break;
	}
	
	case TypedValue::S_LIST: {
	    sym = DefineSymbol(currentToken, TypedValue::S_LIST, ndimensions,
	    			0, context,
	    			TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);

	    for (short i=1; i<=ndimensions; i++) {
	    	sym->size[i] = dimensions[i-1];
	    }
	    
	    GetToken();
	    break;
	}
	
	case TypedValue::S_STRUCT: {
	    sym = DefineSymbol(currentToken, TypedValue::S_STRUCT, ndimensions,
	    			(void*)AllocStack(context,TypedValue::S_STRUCT), context,
	    			TypedValue::REF_STACK, false, false, StabEnt::DISPLAY_NOT);
	    for (short i=1; i<=ndimensions; i++) {
	    	sym->size[i] = dimensions[i-1];
	    }

	    glob.PushContext(sym);
	    GetToken();
	    Block *X = ParseBlockInfo(sym, schedSym);
		if (X) {
			ParseError(ERROR_ERR, "structures may not define a code block");
		}
		glob.PopContext(sym);
		EndStructureAllocation(sym);
// propogate final size back up towards stackable: all other
// objects have their own hunk of stack.
		break;
	}
	
	case TypedValue::S_NOTE:
	case TypedValue::S_CTRL:
	case TypedValue::S_PROG:
	case TypedValue::S_SYSC:
	case TypedValue::S_SYSX:
	case TypedValue::S_JOY:
	
	case TypedValue::S_SHORT:
	case TypedValue::S_INT:
	case TypedValue::S_BYTE:
	case TypedValue::S_BOOL:
	case TypedValue::S_LONG:
	case TypedValue::S_FLOAT: {
		long	nobj=1;
		int		i;
	    for (i=0; i<ndimensions; i++) {
	    	nobj *= dimensions[i];
	    }
//		fprintf(stderr, "def sim\n");
	    sym = DefineSymbol(currentToken, type, ndimensions,
	    				(void*)AllocStack(context,(base_type_t)type,nobj),
	    				context,
	    				(int16)(TypedValue::REF_STACK),
						false, false,
						(int16)StabEnt::DISPLAY_NOT);
	    for (i=1; i<=ndimensions; i++) {
	    	sym->size[i] = dimensions[i-1];
	    }
	    GetToken();
	    break;
	}
	
	default:
		ParseError(ERROR_ERR, "unimplimented type %d, near '%s'", type, currentToken);
	}
	if (sym && foundDisplayParameters && uberQua) {
	// TODO XXXX FIXME placeholder for the moment ... not quite sure how this is best implemented ... originally very be-specific ... now, maybe json?
		uberQua->bridge.SetDisplayParameters(sym, nullptr);
	}
	return nullptr;
}

// parse a whole file of script.
// if it contains only definitions, as we'd expect at this stage
// the block list should be empty.
bool
QSParser::ParseProgFile()
{
    GetToken();
    err_cnt = 0;
    while (!atEof()) {
       	Block *b = ParseBlockInfo(uberQua? uberQua->sym: nullptr, nullptr);
		if (b) {
			blocks.push_back(b);
		} else {
			if (debug_parse >= 2) {
				fprintf(stderr, "ParseProgfile: block parse returns null\n");
			}
		}
    }
	ShowErrors();
    return err_cnt == 0;
}

Qua *
QSParser::ParseQua()
{
    GetToken();
    err_cnt = 0;
    if (!atEof()) {
		if (strcmp(currentToken, "define") == 0)
       		GetToken();
		if (currentTokenType == TOK_TYPE) {
			int type = findType(currentToken);
			if (type == TypedValue::S_QUA) {
				GetToken();
				if (uberQua == nullptr) {
					display.reportError("defining qua for null object");
				} else {
					glob.rename(uberQua->sym, currentToken);
					GetToken();

					fprintf(stderr, "doin it\n");
					uberQua->mainBlock = ParseBlockInfo(uberQua->sym, nullptr);
				}
			} else {
				ParseError(ERROR_ERR, "Expected qua definition near '%s'", currentToken);
			}
		} else {
			ParseError(ERROR_ERR, "Expected definition near '%s'", currentToken);
		}
    }
    if (err_cnt != 0 && uberQua != nullptr) {
//    	BMessage	bomsg(B_QUIT_REQUESTED);
//    	bomsg.AddInt32("le bombe", 666);
////    	q->sequencerWindow->PostMessage(&bomsg);
//    	delete q;
//    	q = nullptr;
    }

    return uberQua;
}

void
QSParser::parseChannelId(vector<int> &s)
{
	for (;;) {
		if (strcmp(currentToken, "*") == 0) {
			s.push_back(-1);
		}
		else if (strcmp(currentToken, "!") == 0) {
			s.push_back(0);
		}
		else {
			s.push_back(atoi(currentToken));
		}
		GetToken();
		if (strcmp(currentToken, ",")) {
			break;
		}
		GetToken();
	}
}

#include <vector>
#include <algorithm>
using namespace std;

/**
* portname could have regular expressions ... at least some chars like '*' or '?'
* at the moment, were not really expecting channels or vsts but this is a priority
*/
bool
QSParser::parsePort(int deviceType, string &portName, QuaPort* &port, StabEnt* &sym, int direction, vector<int> &chans)
{
	cout << "parsing port " << portName << endl;
	port = nullptr;
	sym = nullptr;
	if (currentTokenType == TOK_WORD) {	// this would be a symbol .. a vst or a channel or another input
		portName = currentToken;
		sym = glob.findSymbol(currentToken);
		GetToken();
		if (strcmp(currentToken, ":") == 0) {	// might want to do this for setting midi channels
			GetToken();
			parseChannelId(chans);
		}
	} else if (currentTokenType == TOK_VAL){
		if (currentTokenVal.type == TypedValue::S_STRING) {
			GetToken();
			if (strcmp(currentToken, ":") == 0) {
				GetToken();
				parseChannelId(chans);
			}
			int m = qut::maxel(chans);
			port = findQuaPort(deviceType, portName, direction, m > 0? m+1: -1);
			if (port == nullptr) {
				string portTypeName = findAttributeName(deviceType);
				string directionName = portDirectionName(direction);
				ParseError(ERROR_ERR, "match for %s port '%s'/%s/%d not found", portTypeName.c_str(), portName.c_str(), directionName.c_str(), m);
				return false;
			}
		} else {
			parseChannelId(chans);
		}
	} else {
		ParseError(ERROR_ERR, "Expect a port identifier near '%s'", currentToken);
		return false;
	}
	if (chans.size() == 0) {
		chans.push_back(0);
	}
	return true;
}

// called to parse a sequence of blocks in a fragment into
// a single list (eg from an interface control)
Block *
QSParser::ParseBlockSequenceToList(StabEnt *ctxt)
{
	Block	*b, **bp=&b;
	short nc = 0;
	for (;;) {
		*bp = ParseBlockInfo(ctxt, ctxt);
		fprintf(stderr, "parse block info returns %x %x\n", (unsigned) *bp, (unsigned)b);
		if (*bp == nullptr)
			break;
		nc++;
		bp = &(*bp)->next;
	}
	
	if (b == nullptr)
		return nullptr;
		
	if (b->next) {
		Block *l = new Block(Block::C_LIST, Block::LIST_NORM);
		l->crap.list.nChildren = nc;
		l->crap.list.block = b;
		l->crap.list.stuffVar.Set(
			TypedValue::S_BYTE,
			TypedValue::REF_STACK,
			ctxt,
			(long)AllocStack(
				ctxt,
				TypedValue::S_BYTE,
				l->crap.list.nChildren));
		b = l;
	}
	fprintf(stderr, "return ing %x\n", (unsigned)b);
	return b;
}
