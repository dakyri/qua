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

#line 222 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 348 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 4: // SYMBOL

#line 222 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 355 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 7: // IDENT

#line 222 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 362 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 10: // LITERAL_TIME

#line 223 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.typedval))  { delete ((yysym.value.typedval)); ((yysym.value.typedval)) = nullptr; } }
#line 369 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 11: // LITERAL_STRING

#line 222 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 376 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 25: // ATTRIBUTE

#line 222 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 383 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 75: // numeric_literal

#line 223 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.typedval))  { delete ((yysym.value.typedval)); ((yysym.value.typedval)) = nullptr; } }
#line 390 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 86: // destination_spec

#line 225 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.portval))  { delete ((yysym.value.portval)); ((yysym.value.portval)) = nullptr; } }
#line 397 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 87: // chan_id_spec

#line 224 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.vectival))  { delete ((yysym.value.vectival)); ((yysym.value.vectival)) = nullptr; } }
#line 404 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 88: // chan_id_list

#line 224 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.vectival))  { delete ((yysym.value.vectival)); ((yysym.value.vectival)) = nullptr; } }
#line 411 "QuaParser.cpp" // lalr1.cc:614
        break;

      case 125: // dimension_list

#line 224 "QuaParser.yy" // lalr1.cc:614
        { if ((yysym.value.vectival))  { delete ((yysym.value.vectival)); ((yysym.value.vectival)) = nullptr; } }
#line 418 "QuaParser.cpp" // lalr1.cc:614
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
#line 230 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.qua)->sym; }
#line 648 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 3:
#line 231 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 654 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 4:
#line 237 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.block) = (yystack_[1].value.block); }
#line 660 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 5:
#line 238 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Int((yystack_[0].value.intval));
			(yylhs.value.block) = p;
		}
#line 670 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 6:
#line 243 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = TypedValue::Double((yystack_[0].value.doubleval));
			(yylhs.value.block) = p;
		}
#line 680 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 248 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			cout << (yystack_[0].value.stringval) << " string literal not implemented" << endl;
//			p->crap.constant.value = TypedValue::Double($1);
			(yylhs.value.block) = p;
		}
#line 691 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 8:
#line 254 "QuaParser.yy" // lalr1.cc:859
    {
			Block *p = new Block( Block::C_VALUE);
			p->crap.constant.value = *((yystack_[0].value.typedval));
			(yylhs.value.block) = p;
		}
#line 701 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 9:
#line 261 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.block) = (yystack_[0].value.block); }
#line 707 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 265 "QuaParser.yy" // lalr1.cc:859
    {
			(yylhs.value.typedval) = new TypedValue(TypedValue::Int((yystack_[0].value.intval)));
		}
#line 715 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 268 "QuaParser.yy" // lalr1.cc:859
    {
			(yylhs.value.typedval) = new TypedValue(TypedValue::Double((yystack_[0].value.doubleval)));
		}
#line 723 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 271 "QuaParser.yy" // lalr1.cc:859
    {
			(yylhs.value.typedval) = (yystack_[0].value.typedval);
		}
#line 731 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 286 "QuaParser.yy" // lalr1.cc:859
    {
		Qua *q = new Qua(*(yystack_[0].value.stringval), defaultDisplay, false);
		driver.uberQua = q;
	    glob.PushContext(q->sym);
	}
#line 741 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 15:
#line 290 "QuaParser.yy" // lalr1.cc:859
    {
	    StabEnt *sym = glob.PopContext();//q->sym
	    Qua *q = sym->QuaValue();
	    q->mainBlock = (yystack_[1].value.block);
		(yylhs.value.qua) = q;
	}
#line 752 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 298 "QuaParser.yy" // lalr1.cc:859
    {
		driver.nAudioIns = 2;
		driver.nAudioOuts = 2;
		driver.midiThru = false;
		driver.audioThru = false;
		driver.channelId = -1;
	}
#line 764 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 304 "QuaParser.yy" // lalr1.cc:859
    {
//	channel \outs 2 \midithru channel1 { ... }
		if (driver.uberQua != nullptr) {
			StabEnt *sym = nullptr;
			Channel	*c = driver.uberQua->AddChannel(
						*(yystack_[0].value.stringval), (short)((driver.channelId >= 0)? driver.channelId: driver.uberQua->nChannel),
						driver.nAudioIns, driver.nAudioOuts, driver.audioThru, driver.midiThru,
						false, false, false, false,
						(short)driver.uberQua->nChannel);
			if (c) {
				sym = c->sym;
			}
			glob.PushContext(sym);
		}
	}
#line 784 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 318 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt *sym = nullptr;
		if (driver.uberQua != nullptr) {
		    sym = glob.PopContext();
		    Block *block = (yystack_[1].value.block);
			if (block != nullptr && sym != nullptr) {
				Channel *c = sym->ChannelValue();
				if (c) {
					c->initBlock = block;
				} else {
					block->DeleteAll();
				}
			}
		}
		(yylhs.value.stabval) = sym;
	}
#line 805 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 337 "QuaParser.yy" // lalr1.cc:859
    {
		driver.deviceType = QuaPort::Device::MIDI;
	}
#line 813 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 339 "QuaParser.yy" // lalr1.cc:859
    {
//		input \joy in1 "joystick1"
//		input \midi in1 "In-A USB MidiSport 2x2":*
//		input \midi in1 "In USB Keystation":*
//		input \audio in1 "*"
//		input \audio in1 pluginInstanceId:2,3
		StabEnt *context = glob.TopContext();
		Channel *cha = nullptr;
		StabEnt *sym = nullptr;
		if (context != nullptr && (cha=context->ChannelValue()) != nullptr) {
			Input *s = cha->AddInput(*(yystack_[1].value.stringval), *(yystack_[0].value.portval), true);
			StabEnt *sym = s->sym;
/* // this was in to initialize hardware specific controls on the port
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		}
		(yylhs.value.stabval) = sym;
	}
#line 840 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 21:
#line 363 "QuaParser.yy" // lalr1.cc:859
    {
		driver.deviceType = QuaPort::Device::MIDI;
	}
#line 848 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 365 "QuaParser.yy" // lalr1.cc:859
    {
//		output \audio out1 "ASIO Echo WDM":0,1
//		output \midi out1 pluginInstanceId:2,3
//		output \osc out1 "124.1.1.1:1008"
		StabEnt *context = glob.TopContext();
		Channel *cha = nullptr;
		StabEnt *sym = nullptr;
		if (context != nullptr && (cha=context->ChannelValue()) != nullptr) {
			Output *s = cha->AddOutput(*(yystack_[1].value.stringval), *(yystack_[0].value.portval), true);
			sym = s->sym;
/*	
			if (strcmp(currentToken, "{") == 0) {
				glob.PushContext(s->sym);
				s->initBlock = ParseBlockInfo(s->sym, s->sym);
				glob.PopContext(s->sym);
			}
*/
		}
		(yylhs.value.stabval) = sym;
	}
#line 873 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 388 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.portval) = new PortSpec(driver.deviceType, *(yystack_[1].value.stringval), true, (yystack_[0].value.vectival)); }
#line 879 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 389 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.portval) = new PortSpec(driver.deviceType, *(yystack_[1].value.stringval), false, (yystack_[0].value.vectival)); }
#line 885 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 392 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = nullptr; }
#line 891 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 393 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = new vector<int>({-1}); }
#line 897 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 394 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = (yystack_[0].value.vectival); }
#line 903 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 398 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = new vector<int>(1, (yystack_[0].value.intval)); }
#line 909 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 399 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = (yystack_[2].value.vectival); (yylhs.value.vectival)->push_back((yystack_[0].value.intval)); }
#line 915 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 402 "QuaParser.yy" // lalr1.cc:859
    {
	    Voice *v = new Voice(*(yystack_[0].value.stringval), driver.uberQua);
		driver.schedulables.push_back(v);
	    glob.PushContext(v->sym);
#ifdef VOICE_MAINSTREAM
	    glob.PushContext(v->streamSym);
#endif	    
	}
#line 928 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 409 "QuaParser.yy" // lalr1.cc:859
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
#line 944 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 422 "QuaParser.yy" // lalr1.cc:859
    {
	    Pool *p = new Pool(*(yystack_[0].value.stringval), driver.uberQua, true);
		driver.schedulables.push_back(p);
	    glob.PushContext(p->sym);
	}
#line 954 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 426 "QuaParser.yy" // lalr1.cc:859
    {
	    StabEnt *sym = glob.PopContext(); // p->sym    
		(yylhs.value.stabval) = sym;
		Pool *p = sym->PoolValue();
		if (p != nullptr) {
			p->mainBlock = (yystack_[1].value.block);
		}
	}
#line 967 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 437 "QuaParser.yy" // lalr1.cc:859
    {
		Sample *s = new Sample(*(yystack_[0].value.stringval), "", driver.uberQua, MAX_BUFFERS_PER_SAMPLE, MAX_REQUESTS_PER_SAMPLE);
		driver.schedulables.push_back(s);
	    glob.PushContext(s->sym);
	}
#line 977 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 441 "QuaParser.yy" // lalr1.cc:859
    {
	    StabEnt *sym = glob.PopContext();//S->sym
		(yylhs.value.stabval) = sym;
		Sample *s = sym->SampleValue();
		if (s != nullptr) {
			s->mainBlock = (yystack_[1].value.block);
		}
	}
#line 990 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 451 "QuaParser.yy" // lalr1.cc:859
    {
		driver.isLocus = false;
		driver.isModal = false;
		driver.isOncer = false;
		driver.isFixed = false;
		driver.isHeld = false;
		driver.isInit = false;
		driver.hasResetVal = false;
	}
#line 1004 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 459 "QuaParser.yy" // lalr1.cc:859
    {
	    Lambda	*lambda = new Lambda(*(yystack_[0].value.stringval), glob.TopContext(),
						driver.isLocus, driver.isModal, driver.isOncer,
	    				driver.isFixed, driver.isHeld, driver.isInit);
	    if (driver.hasResetVal) {
			lambda->resetVal = TypedValue::Int(driver.resetVal);
		}
		driver.lambdas.push_back(lambda);
	    glob.PushContext(lambda->sym);
	}
#line 1019 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 468 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt *sym = glob.PopContext();
		Lambda *lambda = sym->LambdaValue();
	    lambda->mainBlock = (yystack_[1].value.block);
		(yylhs.value.stabval) = sym;
	}
#line 1030 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 39:
#line 476 "QuaParser.yy" // lalr1.cc:859
    {
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
	}
#line 1053 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 493 "QuaParser.yy" // lalr1.cc:859
    {
	    VstPlugin	*vstp = new VstPlugin(driver.path, *(yystack_[0].value.stringval), driver.doLoadPlugin, driver.mapVstParams,
	    	driver.isSynthPlugin, driver.nAudioIns, driver.nAudioOuts, driver.numParams, driver.numPrograms);
		driver.vstplugins.push_back(vstp);
		glob.PushContext(vstp->sym);
	}
#line 1064 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 41:
#line 498 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt* sym = glob.PopContext();
		(yylhs.value.stabval) = sym;
	}
#line 1073 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 42:
#line 504 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt* context = glob.TopContext();
		int eventType = (yystack_[1].value.typeval);
		Block *b = (yystack_[0].value.block);
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
#line 1127 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 650 "QuaParser.yy" // lalr1.cc:859
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
		StabEnt *sym = DefineSymbol(*(yystack_[0].value.stringval), TypedValue::S_STRUCT, ndimensions,
	    			(void*)AllocStack(context,TypedValue::S_STRUCT), context,
	    			TypedValue::REF_STACK, false, false, StabEnt::DISPLAY_NOT);
	    for (short i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }

	    glob.PushContext(sym);
	}
#line 1153 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 670 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt *sym = glob.PopContext();
		EndStructureAllocation(sym);
// propogate final size back up towards stackable: all other objects have their own hunk of stack.
	}
#line 1163 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 677 "QuaParser.yy" // lalr1.cc:859
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
	    StabEnt *sym = DefineSymbol(*(yystack_[0].value.stringval), (yystack_[2].value.typeval), ndimensions,
	    				(void*)AllocStack(context,(base_type_t)(yystack_[2].value.typeval), nobj),
	    				context, (int16)(TypedValue::REF_STACK),
						false, false, (int16)StabEnt::DISPLAY_NOT);
	    for (int i=1; i<=ndimensions; i++) {
	    	sym->size[i] = vi->at(i-1);
	    }
	    break;
	}
#line 1189 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 701 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1195 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 702 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1201 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 703 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1207 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 707 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1213 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 708 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1219 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 712 "QuaParser.yy" // lalr1.cc:859
    {
	
		int32 type = (yystack_[0].value.typeval);
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

	}
#line 1247 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 52:
#line 734 "QuaParser.yy" // lalr1.cc:859
    {
	
		int32 type = (yystack_[4].value.typeval);
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
			sym = DefineSymbol(*(yystack_[1].value.stringval), type, 0,
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
#line 1287 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 54:
#line 772 "QuaParser.yy" // lalr1.cc:859
    {
 		 driver.hasInit = true;
 		 driver.initValue = *(yystack_[0].value.typedval);
 	}
#line 1296 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 55:
#line 778 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1302 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 779 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1308 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 780 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1314 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 58:
#line 784 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1320 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 785 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1326 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 789 "QuaParser.yy" // lalr1.cc:859
    {
		driver.displayMode = StabEnt::DISPLAY_CTL;
		driver.hasInit = false;
		driver.hasRange = false;
		driver.isEnveloped = false;
	}
#line 1337 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 794 "QuaParser.yy" // lalr1.cc:859
    {
		StabEnt *context = glob.TopContext();
		StabEnt *sym = DefineSymbol(*(yystack_[1].value.stringval), TypedValue::S_VST_PARAM, 0,
				(void*)(yystack_[3].value.intval), context, (int16)TypedValue::REF_VALUE,
				false, driver.isEnveloped, (int16)driver.displayMode);
		if (driver.hasRange) {
			sym->SetBounds(driver.minValue, driver.maxValue);
		}
		if (driver.hasInit) {
			sym->SetInit(driver.initValue);
		}
		(yylhs.value.stabval) = sym;
	}
#line 1355 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 810 "QuaParser.yy" // lalr1.cc:859
    { driver.nAudioIns = (yystack_[0].value.intval); }
#line 1361 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 811 "QuaParser.yy" // lalr1.cc:859
    { driver.nAudioOuts = (yystack_[0].value.intval); }
#line 1367 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 812 "QuaParser.yy" // lalr1.cc:859
    { driver.midiThru = true; }
#line 1373 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 813 "QuaParser.yy" // lalr1.cc:859
    { driver.audioThru = true; }
#line 1379 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 817 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::AUDIO; }
#line 1385 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 818 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::MIDI; }
#line 1391 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 819 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::JOYSTICK; }
#line 1397 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 820 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::PARALLEL; }
#line 1403 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 821 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::OSC; }
#line 1409 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 823 "QuaParser.yy" // lalr1.cc:859
    { driver.deviceType = QuaPort::Device::SENSOR; }
#line 1415 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 828 "QuaParser.yy" // lalr1.cc:859
    { driver.isLocus = true; }
#line 1421 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 829 "QuaParser.yy" // lalr1.cc:859
    { driver.isModal = true; }
#line 1427 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 74:
#line 830 "QuaParser.yy" // lalr1.cc:859
    { driver.isOncer = true; }
#line 1433 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 831 "QuaParser.yy" // lalr1.cc:859
    { driver.isFixed = true; }
#line 1439 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 76:
#line 832 "QuaParser.yy" // lalr1.cc:859
    { driver.isHeld = true; }
#line 1445 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 77:
#line 833 "QuaParser.yy" // lalr1.cc:859
    { driver.isInit = true; }
#line 1451 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 834 "QuaParser.yy" // lalr1.cc:859
    { driver.hasResetVal = true; driver.resetVal = (yystack_[0].value.intval); }
#line 1457 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 838 "QuaParser.yy" // lalr1.cc:859
    { driver.nAudioIns = (yystack_[0].value.intval); }
#line 1463 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 839 "QuaParser.yy" // lalr1.cc:859
    { driver.nAudioOuts = (yystack_[0].value.intval); }
#line 1469 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 840 "QuaParser.yy" // lalr1.cc:859
    { driver.path = *(yystack_[0].value.stringval); }
#line 1475 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 841 "QuaParser.yy" // lalr1.cc:859
    { driver.numParams = (yystack_[0].value.intval); }
#line 1481 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 842 "QuaParser.yy" // lalr1.cc:859
    { driver.numPrograms = (yystack_[0].value.intval); }
#line 1487 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 843 "QuaParser.yy" // lalr1.cc:859
    { driver.doLoad = true; }
#line 1493 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 844 "QuaParser.yy" // lalr1.cc:859
    { driver.doLoadPlugin = false; }
#line 1499 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 845 "QuaParser.yy" // lalr1.cc:859
    { driver.isSynthPlugin = true; }
#line 1505 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 846 "QuaParser.yy" // lalr1.cc:859
    { driver.mapVstParams = true; }
#line 1511 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 847 "QuaParser.yy" // lalr1.cc:859
    { 
		string s(*(yystack_[0].value.stringval));
		if (s.size() == 4) {
			driver.signature = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
		}
	}
#line 1522 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 856 "QuaParser.yy" // lalr1.cc:859
    {}
#line 1528 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 861 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1534 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 862 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1540 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 92:
#line 866 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1546 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 93:
#line 867 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1552 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 94:
#line 868 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1558 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 95:
#line 869 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1564 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 96:
#line 870 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1570 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 97:
#line 874 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1576 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 98:
#line 875 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1582 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 99:
#line 876 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1588 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 100:
#line 877 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1594 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 101:
#line 882 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1600 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 102:
#line 883 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1606 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 103:
#line 887 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1612 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 104:
#line 888 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1618 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 105:
#line 889 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1624 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 106:
#line 893 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1630 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 107:
#line 894 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1636 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 108:
#line 897 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.vectival) = nullptr; }
#line 1642 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 109:
#line 898 "QuaParser.yy" // lalr1.cc:859
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
#line 1665 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 110:
#line 919 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1671 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 111:
#line 920 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1677 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 112:
#line 923 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = QuaPort::Device::MIDI; }
#line 1683 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 113:
#line 924 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = (yystack_[0].value.intval); }
#line 1689 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 114:
#line 927 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1695 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 115:
#line 928 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1701 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 116:
#line 931 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1707 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 117:
#line 932 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1713 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 118:
#line 935 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1719 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 119:
#line 936 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 1725 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 120:
#line 939 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1731 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 121:
#line 940 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1737 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 122:
#line 943 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1743 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 123:
#line 944 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1749 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 124:
#line 948 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1755 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 125:
#line 949 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1761 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 126:
#line 952 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1767 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 127:
#line 953 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1773 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 128:
#line 956 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1779 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 129:
#line 957 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1785 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 130:
#line 960 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = nullptr; }
#line 1791 "QuaParser.cpp" // lalr1.cc:859
    break;

  case 131:
#line 961 "QuaParser.yy" // lalr1.cc:859
    { (yylhs.value.stabval) = (yystack_[0].value.stabval); }
#line 1797 "QuaParser.cpp" // lalr1.cc:859
    break;


#line 1801 "QuaParser.cpp" // lalr1.cc:859
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


  const signed char QuaParser::yypact_ninf_ = -119;

  const signed char QuaParser::yytable_ninf_ = -1;

  const short int
  QuaParser::yypact_[] =
  {
      15,    22,    48,  -119,    52,  -119,  -119,  -119,  -119,  -119,
    -119,  -119,   -50,  -119,  -119,  -119,    75,    59,    36,  -119,
    -119,  -119,  -119,  -119,  -119,  -119,    64,  -119,  -119,    68,
      73,    45,    77,    86,  -119,  -119,  -119,  -119,    56,  -119,
    -119,  -119,  -119,  -119,   113,   119,   134,  -119,    29,  -119,
    -119,    84,  -119,  -119,  -119,  -119,  -119,  -119,    24,  -119,
      85,  -119,  -119,  -119,  -119,  -119,  -119,  -119,  -119,  -119,
    -119,    83,  -119,    -2,    88,    -5,  -119,    24,    24,    24,
      63,  -119,  -119,  -119,    74,  -119,  -119,  -119,  -119,    76,
    -119,    89,    91,    92,  -119,   139,   142,  -119,  -119,  -119,
    -119,  -119,   146,    25,   144,  -119,   129,  -119,  -119,  -119,
      97,  -119,  -119,    -3,  -119,  -119,  -119,    98,  -119,  -119,
    -119,  -119,  -119,  -119,     0,     0,     0,  -119,   101,   120,
    -119,     6,     8,  -119,    19,    29,    99,  -119,  -119,  -119,
     100,   102,    12,   120,  -119,  -119,  -119,  -119,    94,  -119,
      29,  -119,   101,  -119,  -119,  -119,  -119,  -119,  -119,   104,
    -119,  -119,  -119,  -119,  -119,  -119,   120,   103,   106,  -119,
    -119,  -119,  -119,  -119,  -119,  -119,    67,    90,     7,    62,
    -119,  -119,  -119,  -119,  -119,  -119,  -119,    62,  -119,  -119,
    -119,  -119,   105,   105,  -119,  -119,    -6,  -119,  -119,  -119,
    -119,   107,   156,  -119
  };

  const unsigned char
  QuaParser::yydefact_[] =
  {
     122,     0,     0,     2,     3,    14,     1,    36,    39,    91,
      90,   123,     0,   114,   116,   120,     0,     0,     0,    37,
      72,    73,    74,    75,    76,    77,     0,   115,    40,     0,
       0,     0,     0,     0,    84,    85,    86,    87,     0,   117,
       5,     6,     8,     7,     0,     0,     0,    16,     0,     9,
      13,     0,    92,    93,    94,    95,    96,   121,    46,    78,
      55,    79,    80,    81,    82,    83,    88,    30,    32,    34,
     110,     0,    15,     0,     0,     0,    41,    46,    46,    46,
       0,     4,    51,    47,     0,    49,   128,    60,    56,     0,
      58,     0,     0,     0,    17,     0,     0,    64,    65,   111,
     118,    48,     0,     0,     0,    57,     0,   126,   126,   126,
       0,    62,    63,     0,    50,   108,   108,     0,   103,   104,
     105,   129,   118,    59,     0,     0,     0,   124,    53,     0,
     119,     0,     0,    38,     0,     0,     0,   102,   127,   101,
       0,     0,     0,     0,    52,    10,    11,    12,     0,    45,
       0,    43,    53,    42,    31,    33,    35,    19,    21,     0,
      99,   100,    97,    98,   125,    54,     0,     0,     0,    61,
     112,   112,    18,    89,   109,   130,     0,     0,     0,     0,
      66,    67,    68,    69,    70,    71,   113,     0,    44,   106,
     107,   131,    25,    25,    20,    22,     0,    23,    24,    28,
      26,    27,     0,    29
  };

  const signed char
  QuaParser::yypgoto_[] =
  {
    -119,  -119,  -119,   -47,   -89,   -46,  -119,  -119,  -119,  -119,
    -119,  -119,  -119,  -119,  -119,   -22,   -27,  -119,  -119,  -119,
    -119,  -119,  -119,  -119,    -4,  -119,  -119,  -119,  -119,  -119,
    -119,    -9,  -119,  -118,    54,  -119,    69,  -119,    18,  -119,
    -119,    66,  -119,  -119,  -119,  -119,  -119,  -119,  -119,  -119,
    -119,  -119,    70,  -119,    58,  -119,     4,  -119,  -119,    55,
    -119,  -119,  -119,    31,  -119,  -119
  };

  const short int
  QuaParser::yydefgoto_[] =
  {
      -1,     2,    49,    50,   148,    51,     3,    12,    52,    70,
     110,   160,   170,   161,   171,   194,   197,   201,    53,    77,
      54,    78,    55,    79,   118,    13,    58,    10,    14,    60,
     137,   119,   168,   120,    74,    84,    85,   100,   144,    76,
      89,    90,   104,    99,   186,    27,    39,   130,    11,    57,
     164,   138,   139,   191,   131,    80,   176,    16,    17,   113,
      18,     4,   142,   124,   103,   178
  };

  const unsigned char
  QuaParser::yytable_[] =
  {
       9,    71,   199,    82,   128,   115,   135,    15,    40,    41,
      42,    43,   115,   149,    56,   151,     7,   115,   116,    87,
      40,    41,    42,    43,   163,   116,   152,     1,     7,     5,
     115,   157,   158,    40,    41,    42,    43,    40,    41,    42,
      43,     7,   129,   116,    40,    41,    42,    43,     6,    44,
      45,    46,     7,    47,   165,    88,    63,   117,    83,    48,
     190,   150,   200,   150,   129,   188,    28,    66,     7,   192,
      94,    48,    59,   193,   179,     8,    61,   173,   136,   140,
     141,    62,    19,    73,    48,    64,    29,    30,    48,   153,
      95,    96,    97,    98,    65,    48,   159,   187,   180,   181,
     182,   183,   184,   167,   185,    31,    32,    33,    34,    35,
      36,    37,    38,    20,    21,    22,    23,    24,    25,    26,
      67,   180,   181,   182,   183,   184,    68,   185,   145,   146,
     147,    91,    92,    93,   101,   102,   105,   106,   162,   125,
     126,    69,    72,    81,    75,    86,   107,   111,   108,   109,
     112,    82,   122,    87,   127,   143,   133,   154,   155,   174,
     156,   166,   172,   175,   203,   195,   198,   196,   202,   189,
     169,   114,   123,   121,   132,   177,     0,   134
  };

  const short int
  QuaParser::yycheck_[] =
  {
       4,    48,     8,     5,     7,     5,     6,    57,     8,     9,
      10,    11,     5,     7,    18,     7,    16,     5,    18,    24,
       8,     9,    10,    11,   142,    18,     7,    12,    16,     7,
       5,    19,    20,     8,     9,    10,    11,     8,     9,    10,
      11,    16,    45,    18,     8,     9,    10,    11,     0,    13,
      14,    15,    16,    17,   143,    60,    11,   103,    60,    59,
     178,    55,    68,    55,    45,    58,     7,    11,    16,     7,
       7,    59,     8,    11,     7,    23,     8,   166,   124,   125,
     126,     8,     7,    59,    59,     8,    27,    28,    59,   135,
      27,    28,    29,    30,     8,    59,   142,     7,    31,    32,
      33,    34,    35,   150,    37,    46,    47,    48,    49,    50,
      51,    52,    53,    38,    39,    40,    41,    42,    43,    44,
       7,    31,    32,    33,    34,    35,     7,    37,     8,     9,
      10,    77,    78,    79,    60,    61,    60,    61,   142,   108,
     109,     7,    58,    60,    59,    57,    57,     8,    57,    57,
       8,     5,     8,    24,    57,    54,    58,    58,    58,    56,
      58,    67,    58,    57,     8,   187,   193,    62,    61,   178,
     152,   102,   106,   103,   116,   171,    -1,   122
  };

  const unsigned char
  QuaParser::yystos_[] =
  {
       0,    12,    72,    77,   132,     7,     0,    16,    23,    95,
      98,   119,    78,    96,    99,    57,   128,   129,   131,     7,
      38,    39,    40,    41,    42,    43,    44,   116,     7,    27,
      28,    46,    47,    48,    49,    50,    51,    52,    53,   117,
       8,     9,    10,    11,    13,    14,    15,    17,    59,    73,
      74,    76,    79,    89,    91,    93,    95,   120,    97,     8,
     100,     8,     8,    11,     8,     8,    11,     7,     7,     7,
      80,    74,    58,    59,   105,    59,   110,    90,    92,    94,
     126,    60,     5,    60,   106,   107,    57,    24,    60,   111,
     112,   105,   105,   105,     7,    27,    28,    29,    30,   114,
     108,    60,    61,   135,   113,    60,    61,    57,    57,    57,
      81,     8,     8,   130,   107,     5,    18,    76,    95,   102,
     104,   123,     8,   112,   134,   134,   134,    57,     7,    45,
     118,   125,   125,    58,   130,     6,    76,   101,   122,   123,
      76,    76,   133,    54,   109,     8,     9,    10,    75,     7,
      55,     7,     7,    76,    58,    58,    58,    19,    20,    76,
      82,    84,    95,   104,   121,    75,    67,    74,   103,   109,
      83,    85,    58,    75,    56,    57,   127,   127,   136,     7,
      31,    32,    33,    34,    35,    37,   115,     7,    58,   102,
     104,   124,     7,    11,    86,    86,    62,    87,    87,     8,
      68,    88,    61,     8
  };

  const unsigned char
  QuaParser::yyr1_[] =
  {
       0,    71,    72,    72,    73,    73,    73,    73,    73,    74,
      75,    75,    75,    76,    78,    77,    80,    81,    79,    83,
      82,    85,    84,    86,    86,    87,    87,    87,    88,    88,
      90,    89,    92,    91,    94,    93,    96,    97,    95,    99,
     100,    98,   101,   103,   102,   104,   105,   105,   105,   106,
     106,   108,   107,   109,   109,   110,   110,   110,   111,   111,
     113,   112,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   115,   116,   116,   116,   116,   116,   116,   116,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   118,
     119,   119,   120,   120,   120,   120,   120,   121,   121,   121,
     121,   122,   122,   123,   123,   123,   124,   124,   125,   125,
     126,   126,   127,   127,   128,   128,   129,   129,   130,   130,
     131,   131,   132,   132,   133,   133,   134,   134,   135,   135,
     136,   136
  };

  const unsigned char
  QuaParser::yyr2_[] =
  {
       0,     2,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     7,     0,     0,     9,     0,
       5,     0,     5,     2,     2,     0,     2,     2,     1,     3,
       0,     8,     0,     8,     0,     8,     0,     0,    10,     0,
       0,     6,     2,     0,     7,     3,     0,     2,     3,     1,
       3,     0,     5,     0,     2,     0,     2,     3,     1,     3,
       0,     6,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     2,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     2,     0,     2,     0,     2,     0,     2,     0,     2,
       0,     2,     0,     2,     0,     2,     0,     2,     0,     2,
       0,     2
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const QuaParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "WORD", "SYMBOL", "TYPE",
  "EVENT", "IDENT", "LITERAL_INT", "LITERAL_FLOAT", "LITERAL_TIME",
  "LITERAL_STRING", "QUA", "VOICE", "POOL", "SAMPLE", "LAMBDA", "CHANNEL",
  "STRUCT", "INPUT", "OUTPUT", "CLIP", "TAKE", "VST", "VSTPARAM",
  "ATTRIBUTE", "MODIFIER", "M_INS", "M_OUTS", "M_MIDI_THRU",
  "M_AUDIO_THRU", "M_AUDIO", "M_MIDI", "M_JOYSTICK", "M_PARALLEL", "M_OSC",
  "M_CHANNEL", "M_SENSOR", "M_LOCUS", "M_MODAL", "M_ONCER", "M_FIXED",
  "M_HELD", "M_INIT", "M_RESET", "M_RANGE", "M_PATH", "M_NPARAMS",
  "M_NPROGRAMS", "M_DOLOAD", "M_NOLOAD", "M_SYNTH", "M_MAPPARAMS",
  "M_SIGNATURE", "ASSGN", "LSQB", "RSQB", "LBRA", "RBRA", "LB", "RB",
  "COMMA", "COLON", "COLON2", "SEMI", "DOT", "PLUS", "MINUS", "STAR",
  "SLASH", "NEWLINE", "$accept", "script_file", "atom", "expression",
  "numeric_literal", "block", "qua_defn", "$@1", "channel_defn", "$@2",
  "$@3", "input_defn", "$@4", "output_defn", "$@5", "destination_spec",
  "chan_id_spec", "chan_id_list", "voice_defn", "$@6", "pool_defn", "$@7",
  "sample_defn", "$@8", "lambda_defn", "$@9", "$@10", "vst_defn", "$@11",
  "$@12", "event_defn", "struct_defn", "$@14", "simple_defn",
  "formal_parameters", "formal_param_list", "formal_param_defn", "$@15",
  "formal_param_initialization", "vst_parameters", "vst_param_list",
  "vst_param_defn", "$@16", "channel_attribute", "destination_attribute",
  "lambda_attribute", "vst_attribute", "formal_param_attribute",
  "global_defn", "qua_child_defn", "channel_child_defn",
  "schedulable_child_defn", "executable_child_defn", "struct_child_defn",
  "dimension_list", "channel_attribute_list", "destination_attribute_list",
  "lambda_attribute_list", "vst_attribute_list",
  "formal_param_attribute_list", "qua_child_defn_list", "global_defn_list",
  "channel_child_defn_list", "schedulable_child_defn_list",
  "executable_child_defn_list", "struct_child_defn_list", YY_NULLPTR
  };


  const unsigned short int
  QuaParser::yyrline_[] =
  {
       0,   230,   230,   231,   237,   238,   243,   248,   254,   261,
     265,   268,   271,   279,   286,   286,   298,   304,   298,   337,
     337,   363,   363,   388,   389,   392,   393,   394,   398,   399,
     402,   402,   422,   422,   437,   437,   451,   459,   451,   476,
     493,   476,   504,   650,   650,   677,   701,   702,   703,   707,
     708,   712,   712,   771,   772,   778,   779,   780,   784,   785,
     789,   789,   810,   811,   812,   813,   817,   818,   819,   820,
     821,   823,   828,   829,   830,   831,   832,   833,   834,   838,
     839,   840,   841,   842,   843,   844,   845,   846,   847,   856,
     861,   862,   866,   867,   868,   869,   870,   874,   875,   876,
     877,   882,   883,   887,   888,   889,   893,   894,   897,   898,
     919,   920,   923,   924,   927,   928,   931,   932,   935,   936,
     939,   940,   943,   944,   948,   949,   952,   953,   956,   957,
     960,   961
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70
    };
    const unsigned int user_token_number_max_ = 325;
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
#line 2294 "QuaParser.cpp" // lalr1.cc:1167
#line 964 "QuaParser.yy" // lalr1.cc:1168

 

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

