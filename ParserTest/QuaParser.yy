%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {QSParse}
%define parser_class_name {QuaParser}

%code requires {

	#include "qua_version.h"
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
	
	namespace QSParse {
		class ParserDriver;
 		class QuaLexer;
	}
}

%lex-param { QuaLexer &scanner }
%parse-param { QuaLexer &scanner }

%lex-param { ParserDriver &driver }
%parse-param { ParserDriver &driver }

%code{
	#include <iostream>
	#include <cstdlib>
	#include <fstream>
	#include <string>
	#include <vector>

	using namespace std;
   /* include for all driver functions */
	#include "ParserDriver.h"
  
   /* this is silly, but I can't figure out a way around */
	static int yylex(QSParse::QuaParser::semantic_type *yylval,
					QSParse::QuaLexer &scanner,
					QSParse::ParserDriver &driver);

}

/* token types */
%union {
	std::string *stringval;
	std::vector<int> *vectival;
	TypedValue *typedval;
	int intval;
	base_type_t typeval;
	StabEnt *stabval;
	float floatval;
	double doubleval;
	Block *block;
	Qua *qua;
}

%type <stabval> script_file;

%type <block> atom
%type <block> expression
%type <block> block

%type <qua> qua_defn
%type <stabval> voice_defn
%type <stabval> sample_defn
%type <stabval> channel_defn
%type <stabval> lambda_defn
%type <stabval> simple_defn
%type <stabval> struct_defn
%type <stabval> input_defn
%type <stabval> output_defn

%type <stabval> qua_child_defn_list
%type <stabval> qua_child_defn
%type <stabval> channel_child_defn_list
%type <stabval> channel_child_defn
%type <stabval> executable_child_defn_list
%type <stabval> executable_child_defn
%type <stabval> schedulable_child_defn_list
%type <stabval> schedulable_child_defn
%type <stabval> struct_child_defn_list
%type <stabval> struct_child_defn
%type <intval> formal_parameters
%type <intval> formal_param_defn
%type <intval> formal_param_list

%type <vectival> dimension_list

%token END 0 "end of file"

%token <stringval> WORD
%token <stringval> SYMBOL

%token <intval> TYPE
%token <stringval> IDENT

%token <intval> LITERAL_INT
%token <doubleval> LITERAL_FLOAT
%token <typedval> LITERAL_TIME
%token <stringval> LITERAL_STRING

%token QUA
%token VOICE
%token SAMPLE
%token LAMBDA
%token CHANNEL
%token STRUCT
%token INPUT
%token OUTPUT
%token <intval> TYPE_MODIFIER

%token ASSGN

%token LSQB
%token RSQB
%token LBRA
%token RBRA
%token LB
%token RB

%token COMMA
%token COLON
%token COLON2
%token SEMI
%token DOT

%token PLUS
%token MINUS
%token STAR
%token SLASH

%token NEWLINE

/* destructor rule for <sval> objects */
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <stringval>
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <typedval>
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <vectival>

%%

script_file
	: qua_defn { $$ = $1->sym; }
	;
	
////////////////////////////
// expressions
////////////////////////////
atom : LB expression RB { $$ = $2; }
	| LITERAL_INT {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Int($1);
			$$ = p;
		}
	| LITERAL_FLOAT {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Double($1);
			$$ = p;
		}
	| LITERAL_STRING {
			Block *p = new Block( Block::C_VALUE);
			cout << $1 << " string literal not implemented" << endl;
//			p->crap.constant.value = TypedValue::Double($1);
			$$ = p;
		}
	| LITERAL_TIME {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = *($1);
			$$ = p;
		}
	;
	
expression : atom { $$ = $1; }
	;
	

////////////////////////////
// blocks/statements
////////////////////////////
block : expression
	;
	

////////////////////////////
// definitions
////////////////////////////
qua_defn : QUA IDENT {
		Qua *q = new Qua(*$2, false);
		driver.uberQua = q;
	    glob.PushContext(q->sym);
	} LBRA qua_child_defn_list block RBRA {
	    StabEnt *sym = glob.PopContext();//q->sym
	    Qua *q = sym->QuaValue();
	    q->mainBlock = $6;
		$$ = q;
	}
	;

channel_defn : CHANNEL IDENT {
//	channel \outs 2 \midithru channel1 { ... }
/*
		if (nIns < 0) {
			nIns = 2;
		}
		if (nOuts < 0) {
			nOuts = 2;
		}
		Channel	*c = uberQua->AddChannel(
						nm,	(short)((chDstIndex >= 0)?chDstIndex:uberQua->nChannel),
						nIns, nOuts,
						audioThru, midiThru,
						false, false, false, false,
						(short)uberQua->nChannel);
			if (c) {
				sym = c->sym;
			}
		if (sym) glob.PushContext(sym);

*/
	} LBRA channel_child_defn_list block RBRA {
	/*
	    if (sym) glob.PopContext(sym);
		if (block) {
			if (c) {
				c->initBlock = block;
			} else {
				block->DeleteAll();
			}
		}
	*/
		$$ = nullptr;
	}
	;

voice_defn : VOICE IDENT {
	    Voice *v = new Voice(*$2, driver.uberQua);
//	    v->next = schedulees;
//	    schedulees = v;	
	    glob.PushContext(v->sym);
#ifdef VOICE_MAINSTREAM
	    glob.PushContext(v->streamSym);
#endif	    
	} formal_parameters LBRA schedulable_child_defn_list block RBRA {
#ifdef VOICE_MAINSTREAM
		glob.PopContext();// V->streamSym
#endif
	    StabEnt *sym = glob.PopContext(); // V->sym    
		$$ = sym;
		Voice *v = sym->VoiceValue();
		if (v != nullptr) {
			v->mainBlock = $7;
		}
	}
	;

sample_defn : SAMPLE IDENT {
		Sample *s = new Sample(*$2, nullptr, driver.uberQua, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
//	    s->next = schedulees;
//	    schedulees = s;
	    glob.PushContext(s->sym);
	} formal_parameters LBRA schedulable_child_defn_list block RBRA {
	    StabEnt *sym = glob.PopContext();//S->sym
		$$ = sym;
		Sample *s = sym->SampleValue();
		if (s != nullptr) {
			s->mainBlock = $7;
		}
	}
	;

lambda_defn : LAMBDA IDENT {
/*
	    Lambda	*lambda = new Lambda(currentToken, context,
	    			uberQua, isLocus, isModal, isOncer,
	    			isFixed, isHeld, isInit);
		lambda->resetVal = resetVal;
	    lambda->next = methods;
	    methods = lambda;

	    GetToken();
	    ParseFormalsList(lambda->sym, schedSym, true);
	    glob.PushContext(lambda->sym);
*/
	} LBRA executable_child_defn_list block RBRA {
/*
	    lambda->mainBlock = ParseBlockInfo(lambda->sym, schedSym);
	    sym = lambda->sym;
	    glob.PopContext(lambda->sym);
*/
		$$ = nullptr;
	}
	;
	
input_defn : INPUT IDENT {
//		input in1 \joy "joystick1"
//		input in1 \midi "In-A USB MidiSport 2x2":*
//		input in1 \midi "In USB Keystation":*
//		input in1 \audio "*"
//		input in1 \audio pluginInstanceId:2,3
/*
			Channel		*cha=context->ChannelValue();
			Input *s = cha->AddInput(nm, trxport[0], sch[0], true);
			for (short i=1; i<nport; i++) {
				s->setPortInfo(trxport[i], sch[i], i);
			}
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		$$ = nullptr;
	}
	;
	
output_defn : OUTPUT IDENT {
//		output out1 \audio "ASIO Echo WDM":0,1
//		output out1 \midi pluginInstanceId:2,3
//		output out1 \osc "124.1.1.1:1008"
/*
			Channel		*cha=context->ChannelValue();
			Output *s = cha->AddOutput(nm, trxport[0], sch[0], true);
			for (short i=1; i<nport; i++) {
				s->setPortInfo(trxport[i], sch[i], i);
			}
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		$$ = nullptr;
	}
	;
	
simple_defn : TYPE dimension_list IDENT {
		long nobj=1;
		int ndimensions = 0;
		std::vector<int> *vi = $2;
		if (vi != nullptr) {
			ndimensions = vi->size();
		    for (auto it = vi->begin(); it != vi->end(); ++it) {
		    	nobj *= (*it);

		    }
		}
		StabEnt *context = glob.TopContext();
	    StabEnt *sym = DefineSymbol(*$3, $1, ndimensions,
	    				(void*)AllocStack(context,(base_type_t)$1, nobj),
	    				context,
	    				(int16)(TypedValue::REF_STACK),
						false, false,
						(int16)StabEnt::DISPLAY_NOT);
	    for (int i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }
	    break;
	}
	;

qua_child_defn
	: channel_defn { $$ = $1; }
	| voice_defn { $$ = $1; }
	| sample_defn { $$ = $1; }
	| lambda_defn { $$ = $1; }
	;
	
channel_child_defn
	: lambda_defn { $$ = $1; }
	| simple_defn { $$ = $1; }
	| input_defn { $$ = $1; }
	| output_defn { $$ = $1; }
	;
	
	
schedulable_child_defn
	: executable_child_defn { $$ = $1; }
	;

executable_child_defn
	: lambda_defn { $$ = $1; }
	| simple_defn { $$ = $1; }
	;
	
formal_parameters : { $$ = 0; }
	| LB RB  { $$ = 0; }
	| LB formal_param_list RB { $$ = 0; }
	;
	
formal_param_list
	: formal_param_defn { $$ = 0; }
	| formal_param_list COMMA formal_param_defn { $$ = 0; }
	;
	
formal_param_defn
	:
	;

dimension_list : { $$ = nullptr; }
	| dimension_list LSQB expression RSQB {
	/* TODO really would be awesome for arrays to be dynamic and hang onto this expression
	   and use it at runtime ... current memory management is simplistic */
		std::vector<int> *vi = $1;
		if (vi == nullptr) {
			vi = new vector<int>();
		}
		Block *b = $3;
		ResultValue	v = EvaluateExpression(b, nullptr, nullptr, nullptr);
		if (v.Blocked()) {
			error("cannot evaluate dimension");
			vi->push_back(1);
		} else {
			vi->push_back(v.IntValue(nullptr));
		}
		b->DeleteAll();
		$$ = vi;
	}
	;
	
qua_child_defn_list : { $$ = nullptr; } 
	| qua_child_defn_list qua_child_defn { $$ = $2; }
	;

channel_child_defn_list : { $$ = nullptr; } 
	| channel_child_defn_list channel_child_defn { $$ = $2; }
	;

schedulable_child_defn_list : { $$ = nullptr; } 
	| schedulable_child_defn_list schedulable_child_defn { $$ = $2; }
	;
	
executable_child_defn_list : { $$ = nullptr; } 
	| executable_child_defn_list executable_child_defn { $$ = $2; }
	;

%%
 

void 
QSParse::QuaParser::error( const std::string &err_message )
{
   std::cerr << "Error: " << err_message << ", near line " << scanner.lineno() << "\n"; 
}


/* include for access to scanner.yylex */
#include "QuaLexer.h"
static int 
yylex( QSParse::QuaParser::semantic_type *yylval,
	   QSParse::QuaLexer  &scanner,
	   QSParse::ParserDriver   &driver )
{
   return( scanner.yylex(yylval) );
}

