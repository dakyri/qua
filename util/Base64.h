/*
 * Base64.h
 *
 *  Created on: May 4, 2014
 *      Author: dak
 */

#ifndef BASE64_H_
#define BASE64_H_

class Base64 {
public:
	static char *Encode(
			const unsigned char *data,
			size_t inputLength,
			size_t& outputLength);
	static std::string Encode(
			const unsigned char *data,
			size_t inputLength);
	static unsigned char *Decode(
			const unsigned char *data,
			size_t inputLength,
			size_t& outputLength);
	static std::vector<unsigned char> Decode(
					const unsigned char *data,
					size_t inputLength);

protected:
	Base64();
	virtual ~Base64();

	static Base64 instance;

	char *_Encode(
			const unsigned char *data,
			size_t inputLength,
			size_t& outputLength);
	unsigned char *_Decode(
			const unsigned char *data,
			size_t inputLength,
			size_t& outputLength);
	static char encodingTable[];
	char *decodingTable = nullptr;
	static int modTable[];
};
#endif /* BASE64_H_ */
