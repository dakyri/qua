#include "qua_version.h"

///////////////// System headers and underlying types
#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include <unordered_map>
#include <string>

#include "StdDefs.h"

#include "Block.h"
#include "Properties.h"

// garbage collection needs to be sorted a bit. notes are the
// main things with properties, and they don't have a default
// destructor

std::unordered_map<std::string,int32> propertyIndex = {
	{"cell start",	PROP_CELL_START},
	{"phrase end",	PROP_PHRASE_END}
};

Property::Property(property_id ty, property_value val)
{
	id = ty;
	value = val;
	next = nullptr;
}

Property::~Property()
{
	if (next)
		delete next;
}

void
Attributable::ClearProperties()
{
	if (properties)
		delete properties;
	properties = nullptr;
}

void
Attributable::AddProperty(property_id ty, property_value val)
{
	Property *p = new Property(ty, val);
	p->next = properties;
	properties = p;
}

void
Attributable::AddProperties(Block *b)
{
	while (b!=nullptr) {
		if (b->type == Block::C_VALUE) {
			auto fi = propertyIndex.find(b->crap.constant.value.StringValue());
			if (fi != propertyIndex.end()) {
				AddProperty(fi->second);
			} else {
				fprintf(stderr, "unknown property %s\n", b->crap.constant.value.StringValue());
			}
		}
		b = b->next;
	}
}
	
bool
Attributable::HasProperty(property_id pid)
{
	for (Property *p = properties; p!=nullptr; p=p->next) {
		if (p->id == pid)
			return true;
	}
	return false;
}

property_value
Attributable::PropertyValue(property_id pid)
{
	for (Property *p = properties; p!=nullptr; p=p->next) {
		if (p->id == pid)
			return p->value;
	}
	return 0;
}
