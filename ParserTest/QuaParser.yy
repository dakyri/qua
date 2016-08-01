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
	PortSpec *portval;
	
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
%type <stabval> channel_defn
%type <stabval> input_defn
%type <stabval> output_defn
%type <stabval> voice_defn
%type <stabval> pool_defn
%type <stabval> sample_defn
%type <stabval> lambda_defn
%type <stabval> vst_defn
%type <stabval> vst_param_defn
%type <stabval> event_defn
%type <stabval> simple_defn
%type <stabval> take_defn
%type <stabval> clip_defn
%type <stabval> struct_defn

%type <portval> destination_spec
%type <vectival> chan_id_spec
%type <vectival> chan_id_list

%type <stabval> qua_child_defn_list
%type <stabval> qua_child_defn
%type <stabval> global_defn_list
%type <stabval> global_defn
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
%type <intval> formal_param_initialization
%type <intval> vst_parameters
%type <intval> vst_param_list
%type <typedval> numeric_literal

%type <intval> channel_attribute_list
%type <intval> channel_attribute
%type <intval> destination_attribute_list
%type <intval> destination_attribute
%type <intval> lambda_attribute_list
%type <intval> lambda_attribute
%type <intval> vst_attribute_list
%type <intval> vst_attribute
%type <intval> formal_param_attribute_list
%type <intval> formal_param_attribute

%type <vectival> dimension_list

%token END 0 "end of file"

%token <stringval> WORD
%token <stringval> SYMBOL

%token <typeval> TYPE // simple type eg int, byte ... not individually significant syntactically 
%token <typeval> EVENT // similar to a lambda/function definition, but hooking into specific callbacks
%token <stringval> IDENT

%token <intval> LITERAL_INT
%token <doubleval> LITERAL_FLOAT
%token <typedval> LITERAL_TIME
%token <stringval> LITERAL_STRING

%token QUA
%token VOICE
%token POOL
%token SAMPLE
%token LAMBDA
%token CHANNEL
%token STRUCT
%token INPUT
%token OUTPUT
%token CLIP
%token TAKE
%token VST
%token VSTPARAM

// '\ident' tokens ... some of these are specifically recognized, some are generic
%token <stringval> ATTRIBUTE // an unknown value ... it's not in our internal table but maybe it's a user defined thing
%token <intval> MODIFIER // a ident is in the dictionary and it has the given value
%token M_INS // '\ins' channel attrib ... number of audio lines in
%token M_OUTS // '\outs' ... lines out
%token M_MIDI_THRU // channel attribute 
%token M_AUDIO_THRU // channel attribute
%token M_AUDIO // destination device type
%token M_MIDI // destination device type
%token M_JOYSTICK // destination device type
%token M_PARALLEL // destination device type
%token M_OSC // destination device type
%token M_CHANNEL // destination device type
%token M_SENSOR // destination device type
%token M_LOCUS	// function activation mapping attribute
%token M_MODAL	// function activation mapping attribute
%token M_ONCER	// function activation mapping attribute
%token M_FIXED	// function activation mapping attribute
%token M_HELD	// function activation mapping attribute
%token M_INIT	// function activation mapping attribute
%token M_RESET	// function activation mapping attribute
%token M_RANGE // function param attribute
%token M_PATH // VST definition param
%token M_NPARAMS // VST definition param
%token M_NPROGRAMS // VST definition param
%token M_DOLOAD // VST definition param
%token M_NOLOAD // VST definition param
%token M_SYNTH // VST definition param
%token M_MAPPARAMS // VST definition param
%token M_SIGNATURE // VST definition param


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
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <portval>

%%

script_file
	: qua_defn { $$ = $1->sym; }
	| global_defn_list { $$ = nullptr; }
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
	
numeric_literal
	: LITERAL_INT {
			$$ = new TypedValue(TypedValue::Int($1));
		}
	| LITERAL_FLOAT {
			$$ = new TypedValue(TypedValue::Double($1));
		}
	| LITERAL_TIME {
			$$ = $1;
		}
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
		Qua *q = new Qua(*$2, defaultDisplay, false);
		driver.uberQua = q;
	    glob.PushContext(q->sym);
	} LBRA qua_child_defn_list block RBRA {
	    StabEnt *sym = glob.PopContext();//q->sym
	    Qua *q = sym->QuaValue();
	    q->mainBlock = $6;
		$$ = q;
	}
	;

channel_defn : CHANNEL {
		driver.nAudioIns = 2;
		driver.nAudioOuts = 2;
		driver.midiThru = false;
		driver.audioThru = false;
		driver.channelId = -1;
	} channel_attribute_list IDENT {
//	channel \outs 2 \midithru channel1 { ... }
		if (driver.uberQua != nullptr) {
			StabEnt *sym = nullptr;
			Channel	*c = driver.uberQua->AddChannel(
						*$4, (short)((driver.channelId >= 0)? driver.channelId: driver.uberQua->nChannel),
						driver.nAudioIns, driver.nAudioOuts, driver.audioThru, driver.midiThru,
						false, false, false, false,
						(short)driver.uberQua->nChannel);
			if (c) {
				sym = c->sym;
			}
			glob.PushContext(sym);
		}
	} LBRA channel_child_defn_list block RBRA {
		StabEnt *sym = nullptr;
		if (driver.uberQua != nullptr) {
		    sym = glob.PopContext();
		    Block *block = $8;
			if (block != nullptr && sym != nullptr) {
				Channel *c = sym->ChannelValue();
				if (c) {
					c->initBlock = block;
				} else {
					block->DeleteAll();
				}
			}
		}
		$$ = sym;
	}
	;

	
input_defn : INPUT {
		driver.deviceType = QuaPort::Device::MIDI;
	} destination_attribute_list IDENT destination_spec {
//		input \joy in1 "joystick1"
//		input \midi in1 "In-A USB MidiSport 2x2":*
//		input \midi in1 "In USB Keystation":*
//		input \audio in1 "*"
//		input \audio in1 pluginInstanceId:2,3
		StabEnt *context = glob.TopContext();
		Channel *cha = nullptr;
		StabEnt *sym = nullptr;
		if (context != nullptr && (cha=context->ChannelValue()) != nullptr) {
			Input *s = cha->AddInput(*$4, *$5, true);
			StabEnt *sym = s->sym;
/* // this was in to initialize hardware specific controls on the port
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		}
		$$ = sym;
	}
	;
	
output_defn : OUTPUT {
		driver.deviceType = QuaPort::Device::MIDI;
	} destination_attribute_list IDENT destination_spec {
//		output \audio out1 "ASIO Echo WDM":0,1
//		output \midi out1 pluginInstanceId:2,3
//		output \osc out1 "124.1.1.1:1008"
		StabEnt *context = glob.TopContext();
		Channel *cha = nullptr;
		StabEnt *sym = nullptr;
		if (context != nullptr && (cha=context->ChannelValue()) != nullptr) {
			Output *s = cha->AddOutput(*$4, *$5, true);
			sym = s->sym;
/*	
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		}
		$$ = sym;
	}
	;
	
destination_spec
	: IDENT chan_id_spec 			{ $$ = new PortSpec(driver.deviceType, *$1, true, $2); }
	| LITERAL_STRING chan_id_spec	{ $$ = new PortSpec(driver.deviceType, *$1, false, $2); }
	;

chan_id_spec : { $$ = nullptr; }
	| COLON STAR { $$ = new vector<int>({-1}); }
	| COLON chan_id_list  { $$ = $2; }
	;
	
chan_id_list
	: LITERAL_INT { $$ = new vector<int>(1, $1); }
	| chan_id_list COMMA LITERAL_INT { $$ = $1; $$->push_back($3); }
	;

voice_defn : VOICE IDENT {
	    Voice *v = new Voice(*$2, driver.uberQua);
		driver.schedulables.push_back(v);
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

pool_defn : POOL IDENT {
	    Pool *p = new Pool(*$2, driver.uberQua, true);
		driver.schedulables.push_back(p);
	    glob.PushContext(p->sym);
	} formal_parameters LBRA schedulable_child_defn_list block RBRA {
	    StabEnt *sym = glob.PopContext(); // p->sym    
		$$ = sym;
		Pool *p = sym->PoolValue();
		if (p != nullptr) {
			p->mainBlock = $7;
		}
	}
	;


sample_defn : SAMPLE IDENT {
		Sample *s = new Sample(*$2, "", driver.uberQua, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
		driver.schedulables.push_back(s);
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

lambda_defn : LAMBDA {
		driver.isLocus = false;
		driver.isModal = false;
		driver.isOncer = false;
		driver.isFixed = false;
		driver.isHeld = false;
		driver.isInit = false;
		driver.hasResetVal = false;
	} lambda_attribute_list IDENT {
	    Lambda	*lambda = new Lambda(*$4, glob.TopContext(),
						driver.isLocus, driver.isModal, driver.isOncer,
	    				driver.isFixed, driver.isHeld, driver.isInit);
	    if (driver.hasResetVal) {
			lambda->resetVal = TypedValue::Int(driver.resetVal);
		}
		driver.lambdas.push_back(lambda);
	    glob.PushContext(lambda->sym);
	} formal_parameters LBRA executable_child_defn_list block RBRA {
		StabEnt *sym = glob.PopContext();
		Lambda *lambda = sym->LambdaValue();
	    lambda->mainBlock = $9;
		$$ = sym;
	}
	;
	
vst_defn : VST {
//vst \path "F:/progs/VstPlugins/FuzzBox.dll" \id 'EuFA'
//	\noload
//	\ins 1 \outs 2 FuzzBox(
//		vstparam 0 Boost
//		vstparam 1 ClipBack
//		vstparam 2 Volume
//	)
		driver.nAudioIns = 2;
		driver.nAudioOuts = 2;
		driver.signature = 0;
		driver.isSynthPlugin = false;
		driver.doLoadPlugin = false;
		driver.mapVstParams = false;
		driver.numParams = -1;
		driver.numPrograms = -1;
		driver.path = "";
	} vst_attribute_list IDENT {
	    VstPlugin	*vstp = new VstPlugin(driver.path, *$4, driver.doLoadPlugin, driver.mapVstParams,
	    	driver.isSynthPlugin, driver.nAudioIns, driver.nAudioOuts, driver.numParams, driver.numPrograms);
		driver.vstplugins.push_back(vstp);
		glob.PushContext(vstp->sym);
	} vst_parameters {
		StabEnt* sym = glob.PopContext();
		$$ = sym;
	}
	;
	
event_defn : EVENT block {
		StabEnt* context = glob.TopContext();
		int eventType = $1;
		Block *b = $2;
		Event *ev = nullptr;
		QuasiStack *qs = nullptr;

		if (context->type == TypedValue::S_TEMPLATE) {
			error("template events are broken");
			b->DeleteAll();
		} else if (context->type == TypedValue::S_CHANNEL) {
			Channel *cha = context->ChannelValue();
			switch (eventType) {
				case Attribute::EVENT_TX: ev = &cha->tx; qs = cha->txStack; break;
				case Attribute::EVENT_RX: ev = &cha->rx; qs = cha->rxStack; break;
			}
			if (ev && qs) {
				*ev = b;
				qs->CheckMulchSize();
			} else {
				error("event not valid in channel");
				b->DeleteAll();
			}
		} else {
			Schedulable	*sch = context->SchedulableValue();
			if (sch != nullptr) {
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
					*ev = b;
				} else {
					error("event not valid in schedulable");
					b->DeleteAll();
				}
			} else {
				error("event not valid in schedulable");
				b->DeleteAll();
			}
		}
	}
	;
	
take_defn
	: TAKE IDENT LITERAL_STRING {
		StabEnt *parent = glob.TopContext();
		if (Qua::isSampleFile(*$3)) {
			if (parent->type == TypedValue::S_SAMPLE) {
				Sample	*samp = parent->SampleValue();
				samp->addSampleTake(*$2, *$3, false);
			} else {
				error("Expected sample parent for '"+(*$3)+"'");
			}
		} else if (Qua::isMidiFile(*$3)) {
			if (parent->type == TypedValue::S_VOICE) {
				Voice	*v = parent->VoiceValue();
				v->addStreamTake(*$2, *$3, false);
			} else if (parent->type == TypedValue::S_POOL) {
				Pool	*p = parent->PoolValue();
				p->addStreamTake(*$2, *$3, false);
			} else {
				error("Expected midi parent for '"+(*$3)+"'");
			}
		} else {
			error("Unknown data in '"+(*$3)+"'");
		}

	}
	| TAKE IDENT block {
		StabEnt *parent = glob.TopContext();
		Time	dur = Time::zero;
		Block *block = $3;
		if (parent->type == TypedValue::S_VOICE) {
			Voice	*v = parent->VoiceValue();
			StreamTake *s = v->addStreamTake(*$2, dur, false);
			s->initBlock = block;
		} else if (parent->type == TypedValue::S_POOL) {
			Pool	*p = parent->PoolValue();
			StreamTake *s = p->addStreamTake(*$2, dur, false);
			s->initBlock = block;
		} else {
			error("Expected midi parent for '"+(*$3)+"'");
			if (block != nullptr) {
				block->DeleteAll();
			}
		}
	}
	;
	
clip_defn
	: CLIP IDENT {
		driver.at_t.Set("0:0.0");
		driver.dur_t.Set("0:0.0");
		driver.take = nullptr; // TODO XXX FIXME we're assuming this is defined and this is a bad assumptrion
	} clip_initialization {
		StabEnt *context = glob.TopContext();
		Take *take = nullptr;
		String nm = *$2;

		if (context->type == TypedValue::S_VOICE) {
			Voice *v = context->VoiceValue();
			if (take != null && take->type == Take::STREAM) {
				v->AddClip(nm, (StreamTake *)driver.take, driver.at_t, driver.dur_t, false);
			}
		} else if (context->type == TypedValue::S_SAMPLE) {
			Sample *s = context->SampleValue();
			if (take != null && take->type == Take::SAMPLE) {
				s->AddClip(nm, (SampleTake*)driver.take, driver.at_t, driver.dur_t, false);
			}
		} else if (context->type == TypedValue::S_POOL) {
			Pool *p = context->PoolValue();
			if (take != null && take->type == Take::STREAM) {
				v->AddClip(nm, (StreamTake *)driver.take, driver.at_t, driver.dur_t, false);
			}
		} else if (context->type == TypedValue::S_QUA) {
			Qua *q = context->QuaValue();
			q->addClip(nm, driver.at_t, driver.dur_t, false);
		}
	}
	;
	
clip_initialization: 
	| LB IDENT COMMA LITERAL_TIME COMMA LITERAL_TIME RB {
		driver.at_t = *$4;
		driver.dur_t = *$6;
	}
	| LB IDENT COMMA LITERAL_TIME RB {
		driver.at_t = *$4;
	}
	| LB LITERAL_TIME COMMA LITERAL_TIME RB {
		driver.at_t = *$2;
		driver.dur_t = *$4;
	}
	| LB LITERAL_TIME RB {
		driver.at_t = *$2;
	}
	;
	
struct_defn : STRUCT dimension_list IDENT {
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
		StabEnt *sym = DefineSymbol(*$3, TypedValue::S_STRUCT, ndimensions,
	    			(void*)AllocStack(context,TypedValue::S_STRUCT), context,
	    			TypedValue::REF_STACK, false, false, StabEnt::DISPLAY_NOT);
	    for (short i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }

	    glob.PushContext(sym);
	} LBRA struct_child_defn_list RBRA {
		StabEnt *sym = glob.PopContext();
		EndStructureAllocation(sym);
// propogate final size back up towards stackable: all other objects have their own hunk of stack.
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
	    				context, (int16)(TypedValue::REF_STACK),
						false, false, (int16)StabEnt::DISPLAY_NOT);
	    for (int i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }
	    break;
	}
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
	: TYPE {
	
		int32 type = $1;
		driver.displayMode = StabEnt::DISPLAY_NOT;
		driver.hasInit = false;
		driver.hasRange = false;
		driver.isEnveloped = false;
		switch(type) {
	    	case TypedValue::S_INT:
	    	case TypedValue::S_BYTE:
	    	case TypedValue::S_BOOL:
	    	case TypedValue::S_SHORT:
	    	case TypedValue::S_LONG:
	    	case TypedValue::S_FLOAT:
	    		driver.displayMode = StabEnt::DISPLAY_CTL;
				driver.isEnveloped = true;
	    		break;
	    	case TypedValue::S_TIME:
	    		driver.displayMode = StabEnt::DISPLAY_CTL;
	    		break;
		}

	} formal_param_attribute_list IDENT formal_param_initialization {
	
		int32 type = $1;
		StabEnt *context = glob.TopContext();
		StabEnt *sym = nullptr;
		switch(type) {
	    	case TypedValue::S_INT:
	    	case TypedValue::S_BYTE:
	    	case TypedValue::S_BOOL:
	    	case TypedValue::S_SHORT:
	    	case TypedValue::S_LONG:
	    	case TypedValue::S_FLOAT:
	    		break;
	    	case TypedValue::S_TIME:
	    		break;
	    	case TypedValue::S_EXPRESSION:
	    	case TypedValue::S_POOL:
	    		break;
	    	default:
	    		type = TypedValue::S_UNKNOWN;
	    		error("invalid type");
		}

		if (type != TypedValue::S_UNKNOWN) { // only ever want to skip unused vst params
			sym = DefineSymbol(*$4, type, 0,
						(void*)AllocStack(context,(base_type_t)type), context, (int16)TypedValue::REF_STACK,
						false, driver.isEnveloped, (int16)driver.displayMode);
			if (driver.hasRange) {
				sym->SetBounds(driver.minValue, driver.maxValue);
			}
			if (driver.hasInit) {
				sym->SetInit(driver.initValue);
			}
		}
	}
	;

formal_param_initialization: 
 	| ASSGN numeric_literal {
 		 driver.hasInit = true;
 		 driver.initValue = *$2;
 	}
	;

vst_parameters : { $$ = 0; }
	| LB RB  { $$ = 0; }
	| LB vst_param_list RB { $$ = 0; }
	;
	
vst_param_list
	: vst_param_defn { $$ = 0; }
	| vst_param_list COMMA vst_param_defn { $$ = 0; }
	;
	
vst_param_defn
	: VSTPARAM {
		driver.displayMode = StabEnt::DISPLAY_CTL;
		driver.hasInit = false;
		driver.hasRange = false;
		driver.isEnveloped = false;
	} LITERAL_INT formal_param_attribute_list IDENT formal_param_initialization {
		StabEnt *context = glob.TopContext();
		StabEnt *sym = DefineSymbol(*$5, TypedValue::S_VST_PARAM, 0,
				(void*)$3, context, (int16)TypedValue::REF_VALUE,
				false, driver.isEnveloped, (int16)driver.displayMode);
		if (driver.hasRange) {
			sym->SetBounds(driver.minValue, driver.maxValue);
		}
		if (driver.hasInit) {
			sym->SetInit(driver.initValue);
		}
		$$ = sym;
	}
	;
	
channel_attribute
	: M_INS LITERAL_INT { driver.nAudioIns = $2; }
	| M_OUTS LITERAL_INT { driver.nAudioOuts = $2; }
	| M_MIDI_THRU { driver.midiThru = true; }
	| M_AUDIO_THRU { driver.audioThru = true; }
	;
	
destination_attribute
	: M_AUDIO	{ driver.deviceType = QuaPort::Device::AUDIO; }
	| M_MIDI	{ driver.deviceType = QuaPort::Device::MIDI; }
	| M_JOYSTICK { driver.deviceType = QuaPort::Device::JOYSTICK; }
	| M_PARALLEL { driver.deviceType = QuaPort::Device::PARALLEL; }
	| M_OSC		{ driver.deviceType = QuaPort::Device::OSC; }
//	| M_CHANNEL	{ driver.deviceType = QuaPort::Device::CHANNEL; }
	| M_SENSOR	{ driver.deviceType = QuaPort::Device::SENSOR; }
//	| M_VST 	{ driver.deviceType = QuaPort::Device::VST; }
	;

lambda_attribute
	: M_LOCUS	{ driver.isLocus = true; }
	| M_MODAL	{ driver.isModal = true; }
	| M_ONCER	{ driver.isOncer = true; }
	| M_FIXED	{ driver.isFixed = true; }
	| M_HELD	{ driver.isHeld = true; }
	| M_INIT	{ driver.isInit = true; }
	| M_RESET LITERAL_INT	{ driver.hasResetVal = true; driver.resetVal = $2; }	
	;
	
vst_attribute
	: M_INS LITERAL_INT { driver.nAudioIns = $2; }
	| M_OUTS LITERAL_INT { driver.nAudioOuts = $2; }
	| M_PATH LITERAL_STRING { driver.path = *$2; }
	| M_NPARAMS LITERAL_INT { driver.numParams = $2; }
	| M_NPROGRAMS LITERAL_INT { driver.numPrograms = $2; }
	| M_DOLOAD { driver.doLoad = true; }
	| M_NOLOAD { driver.doLoadPlugin = false; }
	| M_SYNTH { driver.isSynthPlugin = true; }
	| M_MAPPARAMS { driver.mapVstParams = true; }
	| M_SIGNATURE LITERAL_STRING { 
		string s(*$2);
		if (s.size() == 4) {
			driver.signature = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
		}
	}
	;	

formal_param_attribute
	: M_RANGE numeric_literal MINUS numeric_literal {}
	;
	

global_defn
	: vst_defn  { $$ = $1; }
	| lambda_defn { $$ = $1; }
	;
		
qua_child_defn
	: channel_defn { $$ = $1; }
	| voice_defn { $$ = $1; }
	| pool_defn { $$ = $1; }
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
	| event_defn { $$ = $1; }
	;

executable_child_defn
	: lambda_defn { $$ = $1; }
	| struct_defn { $$ = $1; }
	| simple_defn { $$ = $1; }
	;
	
struct_child_defn
	: struct_defn { $$ = $1; }
	| simple_defn { $$ = $1; }
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
	
	
channel_attribute_list : { $$ = 0; }
	| channel_attribute_list channel_attribute { $$ = 0; }
	;
	
destination_attribute_list : { $$ = QuaPort::Device::MIDI; }
	| destination_attribute_list destination_attribute { $$ = $2; }
	;
	
lambda_attribute_list : { $$ = 0; }
	| lambda_attribute_list lambda_attribute { $$ = 0; }
	;

vst_attribute_list : { $$ = 0; }
	| vst_attribute_list vst_attribute { $$ = 0; }
	;

formal_param_attribute_list : { $$ = 0; }	
	| formal_param_attribute_list formal_param_attribute { $$ = 0; }
	;
		
qua_child_defn_list : { $$ = nullptr; } 
	| qua_child_defn_list qua_child_defn { $$ = $2; }
	;

global_defn_list : { $$ = nullptr; } 
	| global_defn_list global_defn { $$ = $2; }
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

struct_child_defn_list : { $$ = nullptr; } 
	| struct_child_defn_list struct_child_defn { $$ = $2; }
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

