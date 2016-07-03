#ifndef __MCSCANNER_HPP__
#define __MCSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef  YY_DECL
#define YY_DECL int  QSParse::QuaLexer::yylex()

#include "QuaParser.hpp"

namespace QSParse {

	class QuaLexer : public yyFlexLexer{
	public:

		QuaLexer(std::istream *in) : yyFlexLexer(in),
								  yylval( nullptr ){};

		int yylex(QSParse::QuaParser::semantic_type *lval)
		{
			yylval = lval;
			return( yylex() );
		}


	private:
		int yylex();
		// yyval ptr 
		QSParse::QuaParser::semantic_type *yylval;
	};

}

#endif /* END __MCSCANNER_HPP__ */
