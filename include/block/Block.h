#ifndef _BLOCK
#define _BLOCK

#include <stdio.h>

class Block;
class Voice;
class Stream;
class Qua;
class Channel;
class StabEnt;
class LocusList;
class QuaFX;
class Stacker;
class SamplePlayer;
struct MarkovInfo;
struct PlayInfo;
struct WhirlInfo;
class StabEnt;

#include <unordered_map>
extern std::unordered_map<std::string, int>	streamTypeIndex;
extern std::unordered_map<std::string, int>	objectSubTypeIndex;
extern std::unordered_map<std::string, int>	builtinCommandIndex;
extern std::unordered_map<std::string, int>	systemCommonCodeIndex;

#include "StdDefs.h"

#include "Stream.h"
#include "Sym.h"
#include "Expression.h"
#include "QuaTypes.h"

enum {
	BLOCK_DISPLAY_CLOSE = 0,
	BLOCK_DISPLAY_TEXT = 1,
	BLOCK_DISPLAY_OPEN = 2
};


/* todo perhaps ditch this i think it's historical */
struct BlockDisplayInfo {		
	short			mode;
};


// language elements
struct CastInfo {
	Block		*block;
	base_type_t	type;
};

struct ConditionInfo {
    Block 		*ifBlock,
				*elseBlock,
				*condition;
			
	TypedValue	doEvalVar;
	TypedValue	condVar;
};

struct GuardInfo
{
	Block		*condition;
	Block		*block;
			
	TypedValue	doEvalVar;
	TypedValue	condVar;
};

struct RepeatInfo
{
	Block		*Exp;
	Block		*block;
	TypedValue	countVar;
	TypedValue	boundVar;
};

struct ListInfo
{
	Block 		*block;
	TypedValue	stuffVar;
	short		nChildren;
// members only relevant to LIST_FORK audio blocks, which split into multiplew fx chains.
	float		**tmpbuf1;
	float		**tmpbuf2;
};

struct SleepInfo {
	int			sleepCount;
};

struct OpInfo {
    Block 	*l, *m, *r;
};

struct MarkovInfo; // body in Markov.h


struct GenericPlayInfo
{
	StabEnt				*playerSym;
	TypedValue			clipVar;		// pointer to a clip on stack
	TypedValue			mediaVar;		// pointer to a take ... on stack
	TypedValue			stateVar;		// is playing or running or just stopped on stack
};

struct SamplePlayInfo: public GenericPlayInfo
{
	TypedValue			posVar;			// sample position in frames on stack
	TypedValue			loopmodeVar;	// looping on or off on stack
	TypedValue			directionVar;	// forwards or backwards on stack
	TypedValue			gainVar;		// play gain on stack
	TypedValue			panVar;			// play pan on stack
};

struct TunedSamplePlayInfo: public SamplePlayInfo
{
	TypedValue			pitchVar;		// play pitch on stack
	TypedValue			lastpitchVar;	// previous play pitch on stack
	TypedValue			idxVar;			// play pitch on stack
	TypedValue			lastSampleArray;	// last true channel for interp across boundaries
};

struct MidiSamplePlayInfo: public TunedSamplePlayInfo
{
	TypedValue			rootVar;		// root note for pitching
	TypedValue			minNoteVar;		// min note for pitching
	TypedValue			maxNoteVar;		// max note for pitching
};

struct StreamPlayInfo: public GenericPlayInfo
{
	TypedValue			posVar;			// offset pointer into play stream
	TypedValue			loopmodeVar;	// looping on or off ... on stack
	TypedValue			rateVar;		// play rate ... on stack
	TypedValue			looptimeVar;	// play rate ... on stack
	TypedValue			loopstartVar;	// play rate ... on stack
};



struct CallInfo
{
    Block		*parameters;
    int			frameIndex;
    union {
		Lambda				*lambda;
		char				*name;
		StabEnt				*sym;
		VstPlugin			*vstplugin;
		SamplePlayInfo		*splayer;
		TunedSamplePlayInfo	*tplayer;
		MidiSamplePlayInfo	*flayer;
		GenericPlayInfo		*player;
		StreamPlayInfo		*mplayer;
		MarkovInfo			*markov;
		long				createType;
	}		crap;
};

struct AssignInfo {
    Block 			*atom, *exp;
};


struct FluxInfo {
	Block 			*rateExp,
					*lengthExp,
					*block;
	TypedValue		timeVar;
};


struct DivertInfo
{
	class Block	*block;
	class Block	*clockExp;
};

struct ArrayRef
{
	Block		*base;
	Block		*index;
};

struct WithInfo
{
	Block		*object;
	Block		*condition;
	Block		*withBlock;
	Block		*withoutBlock;
	StabEnt		*objectType;
};

struct StructureRef
{
	Block		*base;
	StabEnt		*member;
	Block		*unresolved;
};

struct ConstInfo
{
	TypedValue	value;
	char		*stringValue;
};

// sin
// .... root note, min, max, trigger?
// .... tune fine, coarse
// saw
// .... root note, min, max, trigger?
// .... tune fine, coarse
// square
// .... root note, min, max, trigger?
// .... tune fine, coarse
// .... width
// triangle
// .... root note, min, max, trigger?
// .... tune fine, coarse
// noise
// .... root note, min, max, trigger?
// .... tune fine, coarse
// .... color

// adsr
// .... attack time
// .... attack level
// .... decay time
// .... decay level
// .... sustain time
// .... release time
// exp adsr
// .... attack time
// .... attack level
// .... attack alpha
// .... decay time
// .... decay level
// .... decay alpha
// .... sustain time
// .... release time
// .... release alpha

// cheesyhp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
// cheesybp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
// cheesylp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
// mooghp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
// moogbp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
// mooglp
// .... root note, mi, max, trigger?
// .... cutoff
// .... resonance
typedef bool (Block::*BlockFnPtr)(void *x, void *y, int z);
typedef void * (Block::*VBlockFnPtr)(void *x, void *y, int z,long&s);

class Executable;

union BlockInfo {
	Channel			*channel;
	CallInfo		call;
	AssignInfo		assign;
	
	ConditionInfo	iff;
	ConditionInfo	foreach;
	GuardInfo		guard;
	WithInfo		with;
	
	OpInfo			op;
	SleepInfo		sleep;
	DivertInfo		divert;
	RepeatInfo		repeat;
	ListInfo		list;
	FluxInfo		flux;
	
	StabEnt			*sym;
	ConstInfo		constant;
	CastInfo		cast;
	StructureRef	structureRef;
	ArrayRef		arrayRef;
	
	Block			*block;
	char			*name;
};

status_t		SaveMainBlock(class Block *, FILE *fp,
					short indent, StabEnt *dec, bool forceBrace, bool saveInits,
					Stacker *stacker, QuasiStack *stack);

class LocusList;

class Block {
public:
							Block();
							Block(ulong typ, ulong styp=C_UNKNOWN);
							Block(StabEnt *);
							Block(Block *B, StabEnt *newContext, bool unwind);
							~Block();
	void					Setup(bool);
	bool					Init(Executable *V);
	bool					Reset(QuasiStack *);
	bool					AllocateExecStack(Stackable *);
	bool					DoAlloc(void *V, void *z, int x);
	bool					DoInitPost(void *V, void *z,int x);
	void					*DoInitPre(void *V, void *z,int x,long&s);
	bool					DoReset(void *V, void *z, int x);
	void					*DoStack(void *V, void *B, int x,long&s);
	bool					StackOMatic(QuasiStack *N, short);
	
	bool					Traverse(BlockFnPtr f, void *x,
									VBlockFnPtr g, void *y, int z);
	void					FlushOutput(bool ClrFlg);
	bool					DoFlush(void * ClrFlg,void *z, int zz);
	bool					DoDelete(void * ClrFlg, void *z, int zz);
	void					DeleteAll();
	Block					*Sibling(short n);
	StabEnt					*TypeSymbol();
	
	void					Dump(FILE *, short);

	void					Set(ulong t, ulong st=C_UNKNOWN);
	void					Set(TypedValue&v, char*n=nullptr);

    ulong					type;
    ulong					subType;
    BlockInfo				crap;
    Block					*next;

	StabEnt					*labelSym;
	char					*comment;

	BlockDisplayInfo		displayInfo;

	inline bool isOperator() { return type == C_UNOP || type == C_BINOP || type == C_IFOP; }



	enum {
		LIST_NORM 	= 1,
		LIST_SEQ	= 2,
		LIST_PAR	= 3,
		LIST_FORK	 = 4
	};

	enum {
		OP_LT		= 0,
		OP_GT		= 1,
		OP_EQ		= 2,
		OP_NEQ		= 3,
		OP_LE		= 4,
		OP_GE		= 5,

		OP_MUL		= 6,
		OP_DIV		= 7,
		OP_ADD		= 8,
		OP_SUB		= 9,
		OP_MOD		= 10,

		OP_AND		= 11,
		OP_OR		= 12,
		OP_NOT		= 13,

		OP_BAND		= 14,
		OP_BOR		= 15,
		OP_BNOT		= 16,

		OP_UMINUS	= 17
	};

	enum {
		BUILTIN_NONE		= 0,
		BUILTIN_ROLL		= 1,
		BUILTIN_RAMP		= 2,
		BUILTIN_SELECT		= 3,

		BUILTIN_CREATE		= 4,

	#ifdef QUA_V_OLD_BUILTIN
		BUILTIN_NOTE		= 4,
		BUILTIN_CTRL		= 5,
		BUILTIN_BEND		= 6,
		BUILTIN_SYSX		= 7,
		BUILTIN_SYSC		= 8,
		BUILTIN_MESG		= 9,
		BUILTIN_PROG		= 10,
	#endif

		BUILTIN_SIN			= 11,
		BUILTIN_COS			= 12,
		BUILTIN_TAN			= 13,
		BUILTIN_ASIN		= 14,
		BUILTIN_ACOS		= 15,
		BUILTIN_ATAN		= 16,
		BUILTIN_EXP			= 17,
		BUILTIN_POW			= 18,
		BUILTIN_LOG			= 19,
		BUILTIN_SINH		= 20,
		BUILTIN_COSH		= 21,
		BUILTIN_TANH		= 22,
		BUILTIN_BARS		= 23,
		BUILTIN_BEATS		= 24,
		BUILTIN_IS			= 25,
		BUILTIN_FIND		= 26,
		BUILTIN_DELETE		= 27,
		BUILTIN_FLUSH		= 28,
		BUILTIN_INSERT		= 29,
		BUILTIN_COALESCE	= 30,

		BUILTIN_LFO			= 31,
		BUILTIN_SINLFO		= 32,
		BUILTIN_SAWLFO		= 33,
		BUILTIN_SQUARELFO	= 34,
		BUILTIN_EXPLFO		= 34,
		BUILTIN_RANDLFO		= 36
	};

	enum {
		QSP_PLAYSAMPLE	= 0
	};


	enum {
		C_UNKNOWN	= 0,

	// language structures
		C_FLUX		= 1,
		C_OUTPUT	= 2,
		C_IF		= 3,
		C_GUARD		= 4,
		C_ASSIGN	= 6,
		C_CALL		= 7,
		C_BREAK		= 8,
		C_WAKE		= 9,
		C_SUSPEND	= 10,
		C_INPUT		= 11,
		C_DIVERT	= 20,
		C_STATEOF	= 21,		// state machine
		C_WAIT		= 22,
		C_WITH		= 23,
		C_FOREACH	= 24,
		C_REPEAT	= 25,

	// List elements
		C_LIST		= 26,

	// atomic operations
		C_VALUE		= 27,
		C_SYM		= 28,
		C_ARRAY_REF	= 29,
		C_NAME		= 30,
		C_STRUCTURE_REF	= 31,

	// operators
		C_UNOP		= 32,
		C_BINOP		= 33,
		C_IFOP		= 34,
		C_CAST		= 35,

		C_BUILTIN	= 36,	// for a generic builtin that does not need other substructures

		C_SAMPLE_PLAYER = 38,	// play samples at pitch, no frills
		C_STREAM_PLAYER = 39,
		C_GENERIC_PLAYER = 40,
		C_VST = 41,
		C_SCHEDULE = 42,
		C_UNLINKED_CALL	= 43,
		C_MARKOV_PLAYER = 44,
		C_MIDI_PLAYER = 45,		// play samples with a midi trigger and pitch
		C_TUNEDSAMPLE_PLAYER = 46,		// play samples with a midi trigger and pitch

		C_OSCILLATOR = 47,
		C_FILTER = 48
	};


};

enum BlockStatus {
	BLOCK_INCOMPLETE = 0,
	BLOCK_COMPLETE = 1,
	BLOCK_COOKED = 2	// so outputs, and other things don't block
						// in lists which are incomplete... i.e.
						// always do in a list...
};


Block			*LoadBlock(FILE *fp, Qua *uberQ);

bool			Esrap(Block *b, char *buf, long &pos, long len,
						bool do_indent, short indent, short crlf);
						
flag			UpdateActiveBlock(
						Qua *uberQ,
						Stream *in,
						Block  *B,
						Time &tag_time,
						Stacker *stacker, 
						StabEnt *stackCtxt,
						QuasiStack *stack,
						short r,
						bool generate_on_the_fly);

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif
