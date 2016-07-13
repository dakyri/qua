#pragma once

#include <string>
#include "StdDefs.h"
#include "BaseVal.h"

using namespace std;

class OSCMessage {
public:
	OSCMessage(const char *path);

	bool addInt32(int32 d);
	bool addInt8(uchar d);
	bool addFloat(float d);
	bool addInt16(int16 d);
	bool addInt64(int64 d);
	bool addString(char *d);
	bool add(TypedValue &v);

	string path;
};