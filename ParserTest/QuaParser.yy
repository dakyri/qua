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
	TypedValue *typedval;
	int intval;
	float floatval;
	double doubleval;
	Block *block;
	Qua *qua;
}

%type <block> atom
%type <block> expression
/*
%type <block> block
%type <qua> qua_definition
%type <intval> qua_child_defn_list
 */
%token END 0 "end of file"

%token <stringval> WORD
%token <stringval> SYMBOL
%token <stringval> IDENT
%token <intval> INT
%token <doubleval> FLOAT
%token <typedval> TIME
%token <stringval> STRING
%token <intval> TYPE_ATTRIBUTE

%token QUA

%token ASSGN

%token LSQB
%token RSQB
%token LBRA
%token RBRA
%token LB
%token RB

%token PLUS
%token MINUS
%token STAR
%token SLASH

%token NEWLINE

/* destructor rule for <sval> objects */
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <stringval>
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <typedval>

%%

atom : LB expression RB { $$ = $2; }
	| INT {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Int($1);
			$$ = p;
		}
	| FLOAT {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Double($1);
			$$ = p;
		}
	| STRING{
			Block *p = new Block( Block::C_VALUE);
//			p->crap.constant.value = TypedValue::Double($1);
			$$ = p;
		}
	| TIME {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = *($1);
			$$ = p;
		}
	;
	
expression : atom { $$ = $1; }
	;
	/*
qua_definition : QUA IDENT LBRA qua_child_defn_list block RBRA {
	}
*/
	
/*
list_option : END | command_list END;

command_list : command  | command_list command;

command : create_room_command { }
		| join_room_command { }
		| create_client_command { }
		| WORD	{ }
		| NEWLINE { }
	;
	
create_room_command : ROOM expression {};

join_room_command : JOIN expression {}; 

create_client_command : CLIENT expression {};

expression	: WORD
		;
*/

%%
 

void 
QSParse::QuaParser::error( const std::string &err_message )
{
   std::cerr << "Error: " << err_message << "\n"; 
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

