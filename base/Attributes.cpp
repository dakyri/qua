#include "qua_version.h"

///////////////// System headers and underlying types

#include <unordered_map>
#include <string>

#include "StdDefs.h"

#include "Block.h"
#include "Attributes.h"
#include "Dictionary.h"

#include <utility>
using namespace std;

// garbage collection needs to be sorted a bit. notes are the
// main things with properties, and they don't have a default
// destructor
// assignment is always move not copy

Attribute::Attribute(string nm, Attribute::idcode ty, Attribute::value v)
{
	id = ty;
	val = v;
	name = nm;
	next = nullptr;
}

Attribute::~Attribute()
{
	if (next)
		delete next;
}

Attribute *
Attribute::clone()
{
	Attribute *n = new Attribute(name, id, val);
	n->next = next->clone();
	return n;
}

void
AttributeList::clear()
{
	if (items)
		delete items;
	items = nullptr;
}

void
AttributeList::add(string nm, Attribute::idcode ty, Attribute::value val)
{
	Attribute *p = new Attribute(nm, ty, val);
	p->next = items;
	items = p;
}

void
AttributeList::add(Block *b)
{
	while (b!=nullptr) {
		if (b->type == Block::C_VALUE) {
			auto fi = findProperty(b->crap.constant.value.StringValue());
			if (fi != TypedValue::S_UNKNOWN) {
				add(b->crap.constant.value.StringValue(), fi);
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

bool
AttributeList::has(char *pr)
{
	int v = findProperty(pr);
	if (v != STATUS_UNKNOWN) {
		return has(v);
	}
	string s(pr);
	for (Attribute *p = items; p != nullptr; p = p->next) {
		if (p->name == s) {
			return true;
		}
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

AttributeList &
AttributeList::operator=(const AttributeList &o) {
	swap(items, const_cast<AttributeList&>(o).items);
	return *this;
}
/*
AttributeList &
AttributeList::operator=(AttributeList &&o) {
	swap(items, o.items);
	return *this;
}
*/


inline Attribute::value
AttributeList::getValue(char *pr)
{
	return getValue(AttributeId(pr));
}

AttributeList &
AttributeList::clone() {
	return AttributeList(items->clone());
}