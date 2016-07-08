// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "QuaParser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "QuaParser.hpp"

// User implementation prologue.

#line 51 "QuaParser.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 46 "QuaParser.yy" // lalr1.cc:413

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


#line 71 "QuaParser.cpp" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "QuaParser.yy" // lalr1.cc:479
namespace QSParse {
#line 138 "QuaParser.cpp" // lalr1.cc:479

  /// Build a parser object.
  QuaParser::QuaParser (QuaLexer &scanner_yyarg, ParserDriver &driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

  QuaParser::~QuaParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  QuaParser::syntax_error::syntax_error (const std::string& m)
    : std::runtime_error (m)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  QuaParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  QuaParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
  {
    value = other.value;
  }


  template <typename Base>
  inline
  QuaParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v)
    : Base (t)
    , value (v)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  QuaParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  inline
  QuaParser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  QuaParser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  QuaParser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  QuaParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
  }

  // by_type.
  inline
  QuaParser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  QuaParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  QuaParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  QuaParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  QuaParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  QuaParser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  QuaParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  QuaParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  QuaParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  QuaParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  QuaParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  QuaParser::symbol_number_type
  QuaParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  QuaParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  QuaParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  QuaParser::stack_symbol_type&
  QuaParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }


  template <typename Base>
  inline
  void
  QuaParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 3: // WORD

#line 156 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 348 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 4: // SYMBOL

#line 156 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 355 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 6: // IDENT

#line 156 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 362 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 9: // LITERAL_TIME

#line 157 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.typedval))  { delete ((yysym.value.typedval)); ((yysym.value.typedval)) = nullptr; } }
#line 369 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 10: // LITERAL_STRING

#line 156 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 376 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 62: // dimension_list

#line 158 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.vectival))  { delete ((yysym.value.vectival)); ((yysym.value.vectival)) = nullptr; } }
#line 383 "QuaParser.cpp" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  QuaParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  QuaParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  QuaParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  QuaParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  QuaParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  QuaParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  QuaParser::debug_level_type
  QuaParser::debug_level () const
  {
    return yydebug_;
  }

  void
  QuaParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline QuaParser::state_type
  QuaParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  QuaParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  QuaParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  QuaParser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, scanner, driver));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 163 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.qua)->sym; }
#line 613 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 3:
#line 169 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.block) = (yystack_[1].value.block); }
#line 619 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 4:
#line 170 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Int((yystack_[0].value.intval));
			(yylhs.value.block) = p;
		}
#line 629 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 5:
#line 175 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Double((yystack_[0].value.doubleval));
			(yylhs.value.block) = p;
		}
#line 639 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 6:
#line 180 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			cout << (yystack_[0].value.stringval) << " string literal not implemented" << endl;
//			p->crap.constant.value = TypedValue::Double($1);
			(yylhs.value.block) = p;
		}
#line 650 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 186 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = *((yystack_[0].value.typedval));
			(yylhs.value.block) = p;
		}
#line 660 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 8:
#line 193 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.block) = (yystack_[0].value.block); }
#line 666 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 207 "QuaParser.yy" // lalr1.cc:859
    {
		Qua *q = new Qua(*(yystack_[0].value.stringval), false);
		driver.uberQua = q;
	    glob.PushContext(q->sym);
	}
#line 676 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 211 "QuaParser.yy" // lalr1.cc:859
    {
	    StabEnt *sym = glob.PopContext();//q->sym
	    Qua *q = sym->QuaValue();
	    q->mainBlock = (yystack_[1].value.block);
		(yylhs.value.qua) = q;
	}
#line 687 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 219 "QuaParser.yy" // lalr1.cc:859
    {
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
	}
#line 714 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 240 "QuaParser.yy" // lalr1.cc:859
    {
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
		(yylhs.value.stabval) = nullptr;
	}
#line 732 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 255 "QuaParser.yy" // lalr1.cc:859
    {
	    Voice *v = new Voice(*(yystack_[0].value.stringval), driver.uberQua);
//	    v->next = schedulees;
//	    schedulees = v;	
	    glob.PushContext(v->sym);
#ifdef VOICE_MAINSTREAM
	    glob.PushContext(v->streamSym);
#endif	    
	}
#line 746 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 15:
#line 263 "QuaParser.yy" // lalr1.cc:859
    {
#ifdef VOICE_MAINSTREAM
		glob.PopContext();// V->streamSym
#endif
	    StabEnt *sym = glob.PopContext(); // V->sym    
		(yylhs.value.stabval) = sym;
		Voice *v = sym->VoiceValue();
		if (v != nullptr) {
			v->mainBlock = (yystack_[1].value.block);
		}
	}
#line 762 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 276 "QuaParser.yy" // lalr1.cc:859
    {
		Sample *s = new Sample(*(yystack_[0].value.stringval), nullptr, driver.uberQua, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
//	    s->next = schedulees;
//	    schedulees = s;
	    glob.PushContext(s->sym);
	}
#line 773 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 281 "QuaParser.yy" // lalr1.cc:859
    {
	    StabEnt *sym = glob.PopContext();//S->sym
		(yylhs.value.stabval) = sym;
		Sample *s = sym->SampleValue();
		if (s != nullptr) {
			s->mainBlock = (yystack_[1].value.block);
		}
	}
#line 786 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 291 "QuaParser.yy" // lalr1.cc:859
    {
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
	}
#line 805 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 304 "QuaParser.yy" // lalr1.cc:859
    {
/*
	    lambda->mainBlock = ParseBlockInfo(lambda->sym, schedSym);
	    sym = lambda->sym;
	    glob.PopContext(lambda->sym);
*/
		(yylhs.value.stabval) = nullptr;
	}
#line 818 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 314 "QuaParser.yy" // lalr1.cc:859
    {
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
		(yylhs.value.stabval) = nullptr;
	}
#line 843 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 21:
#line 336 "QuaParser.yy" // lalr1.cc:859
    {
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
		(yylhs.value.stabval) = nullptr;
	}
#line 866 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 356 "QuaParser.yy" // lalr1.cc:859
    {
		long nobj=1;
		int ndimensions = 0;
		std::vector<int> *vi = (yystack_[1].value.vectival);
		if (vi != nullptr) {
			ndimensions = vi->size();
		    for (auto it = vi->begin(); it != vi->end(); ++it) {
		    	nobj *= (*it);

		    }
		}
		StabEnt *context = glob.TopContext();
	    StabEnt *sym = DefineSymbol(*(yystack_[0].value.stringval), (yystack_[2].value.intval), ndimensions,
	    				(void*)AllocStack(context,(base_type_t)(yystack_[2].value.intval), nobj),
	    				context,
	    				(int16)(TypedValue::REF_STACK),
						false, false,
						(int16)StabEnt::DISPLAY_NOT);
	    for (int i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }
	    break;
	}
#line 894 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 382 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 900 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 383 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 906 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 384 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 912 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 385 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 918 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 389 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 924 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 390 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 930 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 391 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 936 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 392 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 942 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 397 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 948 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 401 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 954 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 402 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 960 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 405 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 966 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 406 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 972 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 407 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 978 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 411 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 984 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 412 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 990 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 419 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = nullptr; }
#line 996 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 41:
#line 420 "QuaParser.yy" // lalr1.cc:859
    {
	/* TODO really would be awesome for arrays to be dynamic and hang onto this expression
	   and use it at runtime ... current memory management is simplistic */
		std::vector<int> *vi = (yystack_[3].value.vectival);
		if (vi == nullptr) {
			vi = new vector<int>();
		}
		Block *b = (yystack_[1].value.block);
		ResultValue	v = EvaluateExpression(b, nullptr, nullptr, nullptr);
		if (v.Blocked()) {
			error("cannot evaluate dimension");
			vi->push_back(1);
		} else {
			vi->push_back(v.IntValue(nullptr));
		}
		b->DeleteAll();
		(yylhs.value.vectival) = vi;
	}
#line 1019 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 42:
#line 440 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1025 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 441 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1031 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 444 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1037 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 445 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1043 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 448 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1049 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 449 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1055 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 452 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1061 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 453 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1067 "QuaParser.cpp" // lalr1.cc:859
    break;


#line 1071 "QuaParser.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  QuaParser::error (const syntax_error& yyexc)
  {
    error (yyexc.what());
  }

  // Generate an error message.
  std::string
  QuaParser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char QuaParser::yypact_ninf_ = -18;

  const signed char QuaParser::yytable_ninf_ = -1;

  const signed char
  QuaParser::yypact_[] =
  {
      -8,     3,    10,   -18,   -18,   -18,    -6,   -18,    19,   -18,
     -18,   -18,   -18,    18,    24,    29,    39,    30,   -18,   -18,
      25,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,
      22,   -18,    26,    26,    27,    31,   -18,    32,    33,    34,
     -18,   -18,   -18,   -14,   -18,   -18,   -18,    11,    -3,   -18,
     -18,    11,    11,   -18,    28,   -18,   -18,   -18,    47,    53,
      36,   -18,   -18,   -18,   -18,   -18,   -18,    37,   -18,   -18,
      40,     2,   -18,   -18,   -18,   -18,   -18,   -18,   -18,    30,
      41,   -18
  };

  const unsigned char
  QuaParser::yydefact_[] =
  {
       0,     0,     0,     2,    10,     1,     0,    42,     0,     4,
       5,     7,     6,     0,     0,     0,     0,     0,     8,     9,
       0,    23,    24,    25,    26,    43,    14,    16,    18,    12,
       0,    11,    34,    34,     0,     0,     3,    39,     0,     0,
      48,    44,    35,     0,    37,    46,    46,     0,     0,    36,
      39,     0,     0,    40,     0,    32,    33,    49,     0,     0,
       0,    27,    29,    30,    28,    45,    38,     0,    47,    31,
       0,     0,    19,    20,    21,    13,    15,    17,    22,     0,
       0,    41
  };

  const signed char
  QuaParser::yypgoto_[] =
  {
     -18,   -18,   -18,   -17,    -5,   -18,   -18,   -18,   -18,   -18,
     -18,   -18,   -18,    -7,   -18,   -18,   -18,    17,   -18,   -18,
     -18,    20,    35,   -18,    16,   -18,   -18,   -18,    23,   -18
  };

  const signed char
  QuaParser::yydefgoto_[] =
  {
      -1,     2,    18,    19,    20,     3,     6,    21,    35,    22,
      32,    23,    33,    55,    34,    62,    63,    56,    25,    65,
      68,    69,    38,    43,    44,    71,     8,    48,    51,    47
  };

  const unsigned char
  QuaParser::yytable_[] =
  {
      30,    24,    53,     1,     9,    10,    11,    12,    78,     4,
       5,    15,    49,    50,    58,    59,    53,     7,     9,    10,
      11,    12,    17,    79,    26,    15,     9,    10,    11,    12,
      27,    13,    14,    15,    16,    28,    17,     9,    10,    11,
      12,    61,    54,    60,    17,    29,    67,    70,    36,    31,
      40,    37,    72,    73,    41,    17,    45,    46,    42,    74,
      75,    76,    80,    81,    77,    64,    66,    57,    39,    52
  };

  const unsigned char
  QuaParser::yycheck_[] =
  {
      17,     8,     5,    11,     7,     8,     9,    10,     6,     6,
       0,    14,    26,    27,    17,    18,     5,    23,     7,     8,
       9,    10,    25,    21,     6,    14,     7,     8,     9,    10,
       6,    12,    13,    14,    15,     6,    25,     7,     8,     9,
      10,    48,    47,    48,    25,     6,    51,    52,    26,    24,
      23,    25,    24,     6,    23,    25,    23,    23,    26,     6,
      24,    24,    79,    22,    24,    48,    50,    47,    33,    46
  };

  const unsigned char
  QuaParser::yystos_[] =
  {
       0,    11,    38,    42,     6,     0,    43,    23,    63,     7,
       8,     9,    10,    12,    13,    14,    15,    25,    39,    40,
      41,    44,    46,    48,    50,    55,     6,     6,     6,     6,
      40,    24,    47,    49,    51,    45,    26,    25,    59,    59,
      23,    23,    26,    60,    61,    23,    23,    66,    64,    26,
      27,    65,    65,     5,    41,    50,    54,    58,    17,    18,
      41,    50,    52,    53,    54,    56,    61,    41,    57,    58,
      41,    62,    24,     6,     6,    24,    24,    24,     6,    21,
      40,    22
  };

  const unsigned char
  QuaParser::yyr1_[] =
  {
       0,    37,    38,    39,    39,    39,    39,    39,    40,    41,
      43,    42,    45,    44,    47,    46,    49,    48,    51,    50,
      52,    53,    54,    55,    55,    55,    55,    56,    56,    56,
      56,    57,    58,    58,    59,    59,    59,    60,    60,    61,
      62,    62,    63,    63,    64,    64,    65,    65,    66,    66
  };

  const unsigned char
  QuaParser::yyr2_[] =
  {
       0,     2,     1,     3,     1,     1,     1,     1,     1,     1,
       0,     7,     0,     7,     0,     8,     0,     8,     0,     7,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     2,     3,     1,     3,     0,
       0,     4,     0,     2,     0,     2,     0,     2,     0,     2
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const QuaParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "WORD", "SYMBOL", "TYPE",
  "IDENT", "LITERAL_INT", "LITERAL_FLOAT", "LITERAL_TIME",
  "LITERAL_STRING", "QUA", "VOICE", "SAMPLE", "LAMBDA", "CHANNEL",
  "STRUCT", "INPUT", "OUTPUT", "TYPE_MODIFIER", "ASSGN", "LSQB", "RSQB",
  "LBRA", "RBRA", "LB", "RB", "COMMA", "COLON", "COLON2", "SEMI", "DOT",
  "PLUS", "MINUS", "STAR", "SLASH", "NEWLINE", "$accept", "script_file",
  "atom", "expression", "block", "qua_defn", "$@1", "channel_defn", "$@2",
  "voice_defn", "$@3", "sample_defn", "$@4", "lambda_defn", "$@5",
  "input_defn", "output_defn", "simple_defn", "qua_child_defn",
  "channel_child_defn", "schedulable_child_defn", "executable_child_defn",
  "formal_parameters", "formal_param_list", "formal_param_defn",
  "dimension_list", "qua_child_defn_list", "channel_child_defn_list",
  "schedulable_child_defn_list", "executable_child_defn_list", YY_NULLPTR
  };


  const unsigned short int
  QuaParser::yyrline_[] =
  {
       0,   163,   163,   169,   170,   175,   180,   186,   193,   200,
     207,   207,   219,   219,   255,   255,   276,   276,   291,   291,
     314,   336,   356,   382,   383,   384,   385,   389,   390,   391,
     392,   397,   401,   402,   405,   406,   407,   411,   412,   415,
     419,   420,   440,   441,   444,   445,   448,   449,   452,   453
  };

  // Print the state stack on the debug stream.
  void
  QuaParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  QuaParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  QuaParser::token_number_type
  QuaParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36
    };
    const unsigned int user_token_number_max_ = 291;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 5 "QuaParser.yy" // lalr1.cc:1167
} // QSParse
#line 1453 "QuaParser.cpp" // lalr1.cc:1167
#line 456 "QuaParser.yy" // lalr1.cc:1168

 

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

