#include "qua_version.h"
#include "StdDefs.h"
#include "OSCMessage.h"
#include "BaseVal.h"

OSCMessage::OSCMessage(const char *path) {

}

bool
OSCMessage::addInt32(int32 d) {
	return true;
}

bool
OSCMessage::addInt8(uchar d) {
	return true;
}

bool
OSCMessage::addFloat(float d) {
	return true;
}

bool
OSCMessage::addInt16(int16 d) {
	return true;
}

bool
OSCMessage::addInt64(int64 d) {
	return true;
}

bool
OSCMessage::addString(char *d) {
	return true;
}


bool
OSCMessage::add(TypedValue &v) {
	return true;
}