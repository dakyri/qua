%{
/* C++ string header, for string ops below */
#include <string>
/* Implementation of yyFlexScanner */ 
#include "QuaLexer.h"

/* typedef to make the returns for the tokens shorter */
typedef QSParse::QuaParser::token token;

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
#include "Dictionary.h"
	
%}
 
%option debug 
%option nodefault 
%option yyclass="QuaLexer" 
%option noyywrap 
%option yylineno
%option c++

%x C_COMMENT

DIGIT [0-9]
INTEGER {DIGIT}+
EXPONENT [eE][+-]?{INTEGER}
ALF	[a-zA-Z_]
ALNUM [a-zA-Z0-9_]
IDENT {ALF}{ALNUM}*
REAL {INTEGER}("."{INTEGER})?{EXPONENT}?

%%

[+-]?{INTEGER} { 
	yylval->intval = atof(yytext);
	return token::LITERAL_INT;     
}

[+-]?{REAL} {
	yylval->floatval = atof(yytext);
	return token::LITERAL_FLOAT;
}

{INTEGER}":"{INTEGER}"."{INTEGER} {
	TypedValue *v = new TypedValue();
	yylval->typedval = v;
	return token::LITERAL_TIME;
}

\"([^\\\"]|\\.)*\" {
	/*matches string-literal on a single line*/;
	yylval->stringval = new std::string(yytext+1, strlen(yytext)-1);
	return token::LITERAL_STRING;
}

"qua" { return token::QUA; }
"voice" { return token::VOICE; }
"sample" { return token::SAMPLE; }
"channel" { return token::CHANNEL; }
"function" { return token::LAMBDA; }
"lambda" { return token::LAMBDA; }
"struct" { return token::STRUCT; }
"input" { return token::INPUT; }
"output" { return token::OUTPUT; }

{IDENT} {
	int type_id = findType(yytext);
	if (type_id != TypedValue::S_UNKNOWN) {
		yylval->typeval = type_id;
		return token::TYPE;
	}
	yylval->stringval = new std::string(yytext);
	return token::IDENT;
}

"=" { return token::ASSGN; }

"[" { return token::LSQB; }
"]" { return token::RSQB; }
"{" { return token::LBRA; }
"}" { return token::RBRA; }
"(" { return token::LB; }
")" { return token::RB; }

"," { return token::COMMA; }
":" { return token::COLON; }
"::" { return token::COLON2; }
";" { return token::SEMI; }
"." { return token::DOT; }

"+" { return token::PLUS; }
"-" { return token::MINUS; }
"*" { return token::STAR; }
"/" { return token::SLASH; }


"\\"{IDENT} { // also return token::ATTRIBUTE from here
	yylval->intval = 0;
	return token::MODIFIER;
}

"\\ins" { return token::M_INS; }
"\\outs" { return token::M_OUTS; }
"\\midithru" { return token::M_STREAM_THRU; }
"\\audiothru" { return token::M_AUDIO_THRU; }
"\\audio" { return token::M_AUDIO; }
"\\midi" { return token::M_MIDI; }
"\\joy" { return token::M_JOYSTICK; }
"\\parallel" { return token::M_PARALLEL; }
"\\osc" { return token::M_OSC; }
"\\channel" { return token::M_CHANNEL; }
"\\sensor" { return token::M_SENSOR; }
"\\vst" { return token::M_VST; } // maybe we'll need to split this it may be in several contexts TODO 

[ \t] {
	// skip white space
}

\n { 
	// skip white space, but perhaps also give an interactive prompt
}

. { 
	; // oops      
}

"/*" 			{ BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }


%%


