#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <string>
#include "QuaLexer.h"
#include "QuaParser.hpp"

class Qua;

namespace QSParse {

class ParserDriver{
public:
	ParserDriver(Qua *q);
	virtual ~ParserDriver();

	void parse(std::istream& inStream);

	std::ostream& print(std::ostream &stream);
	void setQua(Qua *q);
private:
	QSParse::QuaLexer scanner;
	QSParse::QuaParser parser;

protected:
	friend QSParse::QuaParser;
	Qua *uberQua;
};

}
#endif
