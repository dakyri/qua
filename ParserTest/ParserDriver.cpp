#include <cctype>
#include <iostream>
#include <cassert>

#include "ParserDriver.h"

using namespace std;

QSParse::ParserDriver::ParserDriver(Qua *q)
	: scanner(&cin)
	, parser(scanner, *this)
	, uberQua(q) {
}

QSParse::ParserDriver::~ParserDriver() {
}

void 
QSParse::ParserDriver::parse(std::istream& inStream)
{
	scanner.yyrestart(&inStream);
	const int accept( 0 );
	if( parser.parse() != accept ) {
		std::cerr << "Unrecognized command!!\n";
	}
}

void
QSParse::ParserDriver::setQua(Qua *q) {
	uberQua = q;
}

std::ostream& 
QSParse::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}
