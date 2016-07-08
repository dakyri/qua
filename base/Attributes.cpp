#include "qua_version.h"

///////////////// System headers and underlying types

#include <unordered_map>
#include <string>

#include "StdDefs.h"

#include "Block.h"
#include "Attributes.h"

// garbage collection needs to be sorted a bit. notes are the
// main things with properties, and they don't have a default
// destructor

std::unordered_map<std::string,int32> propertyIndex = {
	{"cell start",	Attribute::CELL_START},
	{"phrase end",	Attribute::PHRASE_END}
};

Attribute::Attribute(Attribute::idcode ty, Attribute::value v)
{
	id = ty;
	val = v;
	next = nullptr;
}

Attribute::~Attribute()
{
	if (next)
		delete next;
}

void
AttributeList::clear()
{
	if (items)
		delete items;
	items = nullptr;
}

void
AttributeList::add(Attribute::idcode ty, Attribute::value val)
{
	Attribute *p = new Attribute(ty, val);
	p->next = items;
	items = p;
}

void
AttributeList::add(Block *b)
{
	while (b!=nullptr) {
		if (b->type == Block::C_VALUE) {
			auto fi = propertyIndex.find(b->crap.constant.value.StringValue());
			if (fi != propertyIndex.end()) {
				add(fi->second);
			} else {
				fprintf(stderr, "unknown property %s\n", b->crap.constant.value.StringValue());
			}
		}
		b = b->next;
	}
}
	
bool
AttributeList::has(Attribute::idcode pid)
{
	for (Attribute *p = items; p!=nullptr; p=p->next) {
		if (p->id == pid)
			return true;
	}
	return false;
}

Attribute::value &
AttributeList::getValue(Attribute::idcode pid)
{
	for (Attribute *p = items; p!=nullptr; p=p->next) {
		if (p->id == pid)
			return p->val;
	}
	return Attribute::value();
}
