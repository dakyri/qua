#ifndef _PARSE
#define _PARSE

class Voice;
class Note;
class Pool;
class Lambda;
class Qua;
class Block;
class Sample;
class StabEnt;
class Schedulable;
class Application;
class ArrangerObject;
class Template;
class VstPluginList;

#include "VstPlugin.h"
#include "BaseVal.h"

#define TOK_NOT		-1
#define TOK_EOF		0
#define TOK_WORD	1
#define TOK_OP		2
#define TOK_VAL		3
#define TOK_TYPE	4

#define Q_PARSE_OK	0
#define Q_PARSE_ERR	1
#define Q_PARSE_DEFER	2

class QSParser
{
public:
					QSParser(Qua *, std::string scrname);
					~QSParser();
					
	virtual void	unGetChar() = 0;
	virtual char	getChar() = 0;
	virtual bool	atEof() = 0;
	virtual void	rewind() = 0;
	virtual void	seek(off_t ind) = 0;
	virtual off_t	position() = 0;
	
	void			SkipSpace();
	void			SkipLine();
	int				GetToken();

	std::string		comment;
	void			AddCharToComment(char c);
	void			ClearComment();

	Block			*ParseBlockSequenceToList(StabEnt *ctxt);
	Block 			*ParseExpression(StabEnt *context, StabEnt *S, bool);
	Block 			*ParseActualsList(StabEnt *V, StabEnt *S, bool);
	Block 			*ParseBlockList(StabEnt *V, StabEnt *S, char *term);
	Block 			*ParseBuiltin(StabEnt *V, StabEnt *S);
	Block 			*ParseAtom(StabEnt *V, StabEnt *S, bool);
	Block 			*ParseDExp(StabEnt *V, StabEnt *S, bool);
	Block 			*ParseCExp(StabEnt *V, StabEnt *S, bool);
	Block 			*ParseBExp(StabEnt *V, StabEnt *S, bool);
	Block 			*ParseAExp(StabEnt *V, StabEnt *S, bool);
	Block			*ParseExpressionList(StabEnt *V, StabEnt *S, char *type, bool);
	void			ParseSchedulable(Schedulable *S);
	Block 			*ParseBlockInfo(StabEnt *V, StabEnt *S);
	void			ParseFormalsList(StabEnt *context, StabEnt *S, bool);
	bool			ParseProgFile();
	class Qua		*ParseQua();
	class QuaPort	*ParsePortId(short *chan);
	void			ParseChannelId(short *chan);
	long			ParseDimentia(uint16 *dim);
	Block			*ParseDefine(StabEnt *context, StabEnt *S);

	void 			ParseError(char *str, ...);
	void			ShowErrors();
	
	int				lineno;
	std::string txtSrcName;
	char			lookaheadChar; // next char, unget()'d
	char			currentToken[1024];
	int				currentTokenType;
	TypedValue		currentTokenVal;
	off_t			currentTokenPosition;

	short			err_cnt;
	
	Schedulable		*schedulees;
	Lambda			*methods;
	Template		*templates;

	VstPluginList	vstplugins;
	
	Qua				*uberQua;

	std::vector<Block*> blocks;
		// any stray block we may pick up and want to keep.
	enum Qualifier {
		NODE		= 1,
		MODAL		= 2,
		ONCER		= 3,
		EXCEPT 	= 4,
		RESET		= 5,
		TOGGLE	= 6,
		HELD		= 7,
		RESETTO	= 8,
		FIXED		= 9,
		LABEL		= 10,
		RANGE		= 11,
		TRIGGER	= 12,
		ENVELOPE	= 13,
		RETURN	= 14,
		MIME		= 16,
		
		ENABLE	= 17,	// whether the vst/whatever component is loadable
		DISABLE	= 18,
		LOAD		= 19,	// load this dll or not, and define it and its parameters, etc
		NOLOAD	= 20,	

		INS		= 21,
		OUTS		= 22,
		ID		= 23,

		MIDITHRU	= 24,
		AUDIOTHRU	= 25,
		NO_MIDITHRU	= 26,
		NO_AUDIOTHRU	= 27,

		MAP_VSTPARAM = 28,
		SYNTH		= 29,
		
		NPARAM	= 30,
		NPROGRAM	= 31,

		DEVICE_AUDIO = 32,
		DEVICE_MIDI = 33,
		DEVICE_JOYSTICK = 34,
		DEVICE_PARALLEL = 35,

		PATH		= 90,	// following need to be out of range of regular types
		TAKES		= 91,
		STREAM		= 92,
		SAMPLE		= 93,

		BUILTIN_EVENT	= 100,
		EVENT_RX		= BUILTIN_EVENT+0,
		EVENT_TX		= BUILTIN_EVENT+1,
		EVENT_RECEIVE	= BUILTIN_EVENT+2,
		EVENT_WAKE		= BUILTIN_EVENT+3,
		EVENT_SLEEP		= BUILTIN_EVENT+4,
		EVENT_START		= BUILTIN_EVENT+5,
		EVENT_STOP		= BUILTIN_EVENT+6,
		EVENT_RECORD	= BUILTIN_EVENT+7,
		EVENT_CUE		= BUILTIN_EVENT+8,
		EVENT_INIT		= BUILTIN_EVENT+9
	};
};

extern std::unordered_map<std::string,int> typeIndex;
extern std::unordered_map<std::string, int> streamTypeIndex;
extern std::unordered_map<std::string, int> objectSubTypeIndex;
extern std::unordered_map<std::string, int> builtinCommandIndex;
extern std::unordered_map<std::string, int> clipPlayerIndex;
extern std::unordered_map<std::string, int> systemCommonCodeIndex;

class TxtParser: public QSParser
{
public:
					TxtParser(char *txt, long len, const char *srcnm, Qua *q);
					~TxtParser();
					
	virtual void	unGetChar();
	virtual char	getChar();
	virtual bool	atEof();
	virtual void	rewind();
	virtual void	seek(off_t ind);
	virtual	off_t	position();

	char			*text_buf;
	int				text_buf_ind;
	off_t			size;
};

class Parser: public QSParser
{
public:
					Parser(FILE *fp, std::string s, Qua *q);
					~Parser();
	
	virtual void	unGetChar();
	virtual char	getChar();
	virtual bool	atEof();
	virtual void	seek(off_t ind);
	virtual void	rewind();
	virtual	off_t	position();

	char			*text_buf;
	int				text_buf_ind;
	off_t			size;
};


char *extension(char *name);
void quascript_name(char *nm, char *buf, long);

#endif
