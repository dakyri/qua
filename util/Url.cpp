/*
 * Uri.cpp
 *
 *  Created on: May 7, 2014
 *      Author: dak
 */

#include <cstring>
#include <string>
#include <vector>
#include <ctype.h>
#include "Url.h"

Url Url::instance;

Url::Url() {
}

Url::~Url() {
}

char
Hex2Char(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

char
Char2Hex(char code) {
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}


char*
Url::Encode(const char *data,
		size_t inputLength,
		size_t& outputLength) {
	return instance._Encode(data, inputLength, outputLength);
}

char*
Url::Decode(const char *data,
		size_t inputLength,
		size_t& outputLength) {
	return instance._Decode(data, inputLength, outputLength);
}

std::string
Url::Encode(std::string s){
	size_t o;
	char *p = instance._Encode(s.c_str(), s.size(), o);
	std::string res(p, o);
	delete []p;
	return res;
}
std::string
Url::Decode(std::string s) {
	size_t o;
	char *p = instance._Decode(s.c_str(), s.size(), o);
	std::string res(p, o);
	delete []p;
	return res;
}

char*
Url::_Encode(
		const char *data,
		size_t inputLength,
		size_t& outputLength) {
	char *buf = new char[inputLength * 3 + 1];
	char *pbuf = buf;
	outputLength = inputLength;
	for (size_t i=0; i<inputLength; i++) {
		if (isalnum(data[i]) || data[i] == '-' || data[i] == '_' || data[i] == '.' || data[i] == '~') {
			*pbuf++ = data[i];
		} else if (data[i] == ' ') {
			*pbuf++ = '+';
		} else {
			*pbuf++ = '%';
			*pbuf++ = Char2Hex(data[i] >> 4);
			*pbuf++ = Char2Hex(data[i] & 0x0f);
			outputLength += 2;
		}
	}
	*pbuf='\0';
	return buf;
}

char *
Url::_Decode(
		const char *data,
		size_t inputLength,
		size_t& outputLength) {
	char* buf = new char[inputLength + 1];
	char* pbuf = buf;
	outputLength = inputLength;
	for (size_t i=0; i<inputLength; i++) {
		if (data[i] == '%') {
			if (i<inputLength-2) {
				*pbuf++ = Hex2Char(data[i+1]) << 4 | Hex2Char(data[i+2]);
				outputLength -= 2;
			}
		} else if (data[i] == '+') {
			*pbuf++ = ' ';
		} else {
			*pbuf++ = data[i];
		}
	}
	*pbuf = '\0';
	return buf;
}
