#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <string>
#include "QuaLexer.h"
#include "QuaParser.hpp"


namespace QSParse {

class ParserDriver{
public:
	ParserDriver();
	virtual ~ParserDriver();

	void parse(std::istream& inStream);

	std::ostream& print(std::ostream &stream);
private:
	QSParse::QuaLexer scanner;
	QSParse::QuaParser parser;
};

}
#endif
