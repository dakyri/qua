/*
 * Uri.h
 *
 *  Created on: May 7, 2014
 *      Author: dak
 */

#ifndef URI_H_
#define URI_H_

class Url {
public:
	static char *Encode(
			const char *data,
			size_t inputLength,
			size_t& outputLength);
	static std::string Encode(std::string);
	static char *Decode(
			const char* data,
			size_t inputLength,
			size_t& outputLength);
	static std::string Decode(std::string);
protected:
	static Url instance;

	Url();
	virtual ~Url();
	char* _Encode(
			const char* data,
			size_t inputLength,
			size_t& outputLength);
	char* _Decode(
			const char* data,
			size_t inputLength,
			size_t& outputLength);
};

#endif /* URI_H_ */
