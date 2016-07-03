%{
/* C++ string header, for string ops below */
#include <string>
/* Implementation of yyFlexScanner */ 
#include "QuaLexer.h"

/* typedef to make the returns for the tokens shorter */
typedef Qua::QuaParser::token token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

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
	#include "MidiDefs.h"
%}
 
%option debug 
%option nodefault 
%option yyclass="QuaLexer" 
%option noyywrap 
%option c++

DIGIT [0-9]
INTEGER {DIGIT}+
EXPONENT [eE][+-]?{INTEGER}
ALF	[a-zA-Z]
ALNUM [a-zA-Z0-9]
IDENT {ALF}{ALNUM}*
REAL {INTEGER}("."{INTEGER})?{EXPONENT}?

%%

[+-]?{INTEGER} { 
	yylval.intval = atof(yytext);
	return token::INT;     
}

[+-]?{REAL} {
	yylval.floatval = atof(yytext);
	return token::FLOAT;
}

{INTEGER}":"{INTEGER}"."{INTEGER} {
	TypedValue *v = new TypedValue();
	yylval.typedval = v;
	return token::TIME;
}

{IDENT} {
	yylval.stringval = new std::string(yytext);
	return token::WORD;
}

\"([^\\\"]|\\.)*\" {
	/*matches string-literal on a single line*/;
	yylval.stringval = new std::string(yytext+1, strlen(yytext)-1);
	return token::STRING;
}


"=" {
	return(token::ASSGN);
}



"[" {
	return(token::LSQB);
}

"]" {
	return(token::RSQB);
}

"{" {
	return(token::LBRA);
}

"}" {
	return(token::RBRA);
}

"(" {
	return(token::LB);
}

")" {
	return(token::RB);
}

"+" {
	return token::OP;
}

"-" {
	return token::OP;
}

"*" {
	return token::OP;
}

"/" {
	return token::OP;
}


room {
	return(token::ROOM);
}

client {
	return(token::CLIENT);
}

join {
	return(token::JOIN);
}

leave {
	return(token::LEAVE);
}

"$"{ID} {
	yylval->sval = new std::string(yytext);  
	return(token::SYMBOL);      
}

{ID} { 
	yylval->sval = new std::string(yytext);  
	return(token::WORD);      
}


%x C_COMMENT

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }

[ \t] {
	// skip white space
}

\n { 
	// skip white space, but perhaps also give an interactive prompt
}

. { 
	; // oops      
}


%%


