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
	int				GetToken(bool worms=false);

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
	bool parsePort(int deviceKind, string &portName, QuaPort* &port, StabEnt* &sym, vector<int> &channels);
	void parseChannelId(vector<int> &channels);
	long ParseDimentia(uint16 *dim);
	Block *ParseDefine(StabEnt *context, StabEnt *S);

	void 			ParseError(char *str, ...);
	void			ShowErrors();
	
	int				lineno;
	std::string txtSrcName;
	char			lookaheadChar; // next char, unget()'d
	char			currentToken[1024];
	int				currentTokenType;
	TypedValue		currentTokenVal;
	std::string		currentTokenStringLiteral;
	off_t			currentTokenPosition;

	short			err_cnt;
	
	Schedulable		*schedulees;
	Lambda			*methods;
	Template		*templates;

	VstPluginList	vstplugins;
	
	Qua				*uberQua;

	std::vector<Block*> blocks;
		// any stray block we may pick up and want to keep.

};

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

string quascript_name(const string & nm);

#endif
