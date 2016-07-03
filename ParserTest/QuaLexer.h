#ifndef __MCSCANNER_HPP__
#define __MCSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef  YY_DECL
#define YY_DECL int  Qua::QuaLexer::yylex()

#include "QuaParser.hpp"

namespace Qua {

	class QuaLexer : public yyFlexLexer{
	public:

		QuaLexer(std::istream *in) : yyFlexLexer(in),
								  yylval( nullptr ){};

		int yylex(Qua::QuaParser::semantic_type *lval)
		{
			yylval = lval;
			return( yylex() );
		}


	private:
		int yylex();
		// yyval ptr 
		Qua::QuaParser::semantic_type *yylval;
	};

}

#endif /* END __MCSCANNER_HPP__ */
