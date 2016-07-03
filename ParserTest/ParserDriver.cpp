#include <cctype>
#include <iostream>
#include <cassert>

#include "ParserDriver.h"

using namespace std;

Qua::ParserDriver::ParserDriver()
	: scanner(&cin)
	, parser(scanner, *this)  {
}

Qua::ParserDriver::~ParserDriver() {
}

void 
Qua::ParserDriver::parse(std::istream& inStream)
{
	scanner.yyrestart(&inStream);
	const int accept( 0 );
	if( parser.parse() != accept ) {
		std::cerr << "Unrecognized command!!\n";
	}
}

/*
void 
Cmd::CmdDriver::add_upper()
{ 
	uppercase++;
	chars++;
	words++;
}

void 
Cmd::CmdDriver::add_lower()
{ 
	lowercase++;
	chars++;
	words++;
}

void 
Cmd::CmdDriver::add_word( const std::string &word )
{
	words++;
	chars += word.length();
	for(const char &c : word ){
		if( islower( c ) ) {
			lowercase++;
		} else if ( isupper( c ) )  {
			uppercase++;
		}
	}
}

*/

std::ostream& 
Qua::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}
