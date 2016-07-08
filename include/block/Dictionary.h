#pragma once

#include "StdDefs.h"

/*
extern std::unordered_map<std::string, int> typeIndex;
extern std::unordered_map<std::string, int> streamTypeIndex;
extern std::unordered_map<std::string, int> objectSubTypeIndex;
extern std::unordered_map<std::string, int> builtinCommandIndex;
extern std::unordered_map<std::string, int> clipPlayerIndex;
extern std::unordered_map<std::string, int> systemCommonCodeIndex;
*/


int32 findType(const char *s);
int32 findConstant(const char *s);
int32 findMidiNote(const char *s);
int32 findDfltEventType(const char *s);
int32 findTypeAttribute(const char *s);

string findClipPlayer(int type);
string findTypeName(int type);
