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
/* TODO XXX FIXME ??? best implementation for this may be as a triplet
 */
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
"pool" { return token::POOL; }
"sample" { return token::SAMPLE; }
"channel" { return token::CHANNEL; }
"function" { return token::LAMBDA; }
"lambda" { return token::LAMBDA; }
"struct" { return token::STRUCT; }
"input" { return token::INPUT; }
"output" { return token::OUTPUT; }
"take" { return token::TAKE; }
"clip" { return token::CLIP; }
"vst" { return token::VST; }
"vstparam" { return token::VST; }

{IDENT} {
	int type_id = findType(yytext);
	if (type_id != TypedValue::S_UNKNOWN) {
		yylval->typeval = type_id;
		return token::TYPE;
	}
	type_id = findDfltEventType(yytext);
	if (type_id != TypedValue::S_UNKNOWN) {
		yylval->typeval = type_id;
		return token::EVENT;
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

"\\ins" { return token::M_INS; }
"\\outs" { return token::M_OUTS; }
"\\midithru" { return token::M_MIDI_THRU; }
"\\audiothru" { return token::M_AUDIO_THRU; }
"\\audio" { return token::M_AUDIO; }
"\\midi" { return token::M_MIDI; }
"\\joy" { return token::M_JOYSTICK; }
"\\parallel" { return token::M_PARALLEL; }
"\\osc" { return token::M_OSC; }
"\\sensor" { return token::M_SENSOR; }
"\\locus" { return token::M_LOCUS; }
"\\modal" { return token::M_MODAL; }
"\\oncer" { return token::M_ONCER; }
"\\fixed" { return token::M_FIXED; }
"\\held" { return token::M_HELD; }
"\\init" { return token::M_INIT; }
"\\reset" { return token::M_RESET; }
"\\range" { return token::M_RANGE; }
"\\path" { return token::M_PATH; }
"\\nparam" { return token::M_NPARAMS; }
"\\nprogram" { return token::M_NPROGRAMS; }
"\\doload" { return token::M_DOLOAD; }
"\\noload" { return token::M_NOLOAD; }
"\\synth" { return token::M_SYNTH; }
"\\map" { return token::M_MAPPARAMS; }
"\\id" { return token::M_SIGNATURE; }

"\\"{IDENT} {
 // also return token::ATTRIBUTE from here (ie if is in symbol table. ATTRIBUTE=user defined, string valued
 //"\\channel" { return token::M_CHANNEL; }
	yylval->intval = 0;
	return token::MODIFIER;
}

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


