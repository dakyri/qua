/*
 * Base64.cpp
 *
 *  Created on: May 4, 2014
 *	  Author: dak
 */

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include "Base64.h"

Base64 Base64::instance;

char Base64::encodingTable[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'};
int Base64::modTable[] = {0, 2, 1};

Base64::Base64() {
	decodingTable = new char[256];

	for (int i = 0; i < 256; i++)
		decodingTable[i] = 0;
	for (int i = 0; i < 64; i++)
		decodingTable[(unsigned char) encodingTable[i]] = i;
}

Base64::~Base64() {
	delete [] decodingTable;
}

char*
Base64::Encode(const unsigned char *data,
		size_t inputLength,
		size_t& outputLength) {
	return instance._Encode(data, inputLength, outputLength);
}

unsigned char*
Base64::Decode(const unsigned char *data,
		size_t inputLength,
		size_t& outputLength) {
	return instance._Decode(data, inputLength, outputLength);
}

std::string
Base64::Encode(
		const unsigned char *data,
		size_t inputLength)
{
	size_t l;
	char *p = instance._Encode(data, inputLength, l);
	std::string enc(p, l);
	delete []p;
	return enc;
}

std::vector<unsigned char>
Base64:: Decode(
		const unsigned char *data,
		size_t inputLength)
{
	size_t l;
	unsigned char *p = instance._Decode(data, inputLength, l);
	std::vector<unsigned char> r;
	r.assign(p, p+l);
	return r;
}


char*
Base64::_Encode(const unsigned char *data,
		size_t inputLength,
		size_t& outputLength) {

	outputLength = 4 * ((inputLength + 2) / 3);

	char* encodedData= new char[outputLength];
	if (encodedData == nullptr) return nullptr;

	for (size_t i = 0, j = 0; i < inputLength;) {

		unsigned octet_a = i < inputLength ? (unsigned char)data[i++] : 0;
		unsigned octet_b = i < inputLength ? (unsigned char)data[i++] : 0;
		unsigned octet_c = i < inputLength ? (unsigned char)data[i++] : 0;

		unsigned triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encodedData[j++] = encodingTable[(triple >> 3 * 6) & 0x3F];
		encodedData[j++] = encodingTable[(triple >> 2 * 6) & 0x3F];
		encodedData[j++] = encodingTable[(triple >> 1 * 6) & 0x3F];
		encodedData[j++] = encodingTable[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < modTable[inputLength % 3]; i++)
		encodedData[outputLength - 1 - i] = '=';

	return encodedData;
}


unsigned char*
Base64::_Decode(const unsigned char *data,
		size_t inputLength,
		size_t& outputLength) {

	if (inputLength % 4 != 0) return nullptr;

	outputLength = inputLength / 4 * 3;
	if (data[inputLength - 1] == '=') (outputLength)--;
	if (data[inputLength - 2] == '=') (outputLength)--;

	unsigned char *decoded_data = new unsigned char[outputLength];
	if (decoded_data == nullptr) return nullptr;

	for (size_t i = 0, j = 0; i < inputLength;) {

		unsigned sextet_a = data[i] == '=' ? 0 & i++ : decodingTable[data[i++]];
		unsigned sextet_b = data[i] == '=' ? 0 & i++ : decodingTable[data[i++]];
		unsigned sextet_c = data[i] == '=' ? 0 & i++ : decodingTable[data[i++]];
		unsigned sextet_d = data[i] == '=' ? 0 & i++ : decodingTable[data[i++]];

		unsigned triple =
				(sextet_a << 3 * 6)
				+ (sextet_b << 2 * 6)
				+ (sextet_c << 1 * 6)
				+ (sextet_d << 0 * 6);

		if (j < outputLength) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < outputLength) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < outputLength) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}
