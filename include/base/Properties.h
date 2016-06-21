#ifndef _PROPERTY
#define _PROPERTY

typedef short property_id;
typedef short property_value;

#include <unordered_map>
#include <string>
using namespace std;

class Property {
public:
					Property(property_id ty, property_value val);
					~Property();
	property_id		id;
	property_value	value;
	Property		*next;
};

extern	property_id	PropertyId(char *);

class Attributable
{
public:
	void			ClearProperties();
	void			AddProperty(property_id, property_value val=0);
	void			AddProperties(class Block *);
	bool			HasProperty(property_id);
	property_value	PropertyValue(property_id);

	inline bool
	HasProperty(char *pr)
	{
		return HasProperty(PropertyId(pr));
	}
	
	inline property_value
	PropertyValue(char *pr)
	{
		return PropertyValue(PropertyId(pr));
	}
	
	inline void
	AddProperty(char *pr, property_value val=0)
	{
		AddProperty(PropertyId(pr), val);
	}
	
	Property		*properties;
};

extern unordered_map<string, int32> propertyIndex;

enum {
	PROP_UNKNOWN	= 0,
// internally used properties, pre-defined
	PROP_CELL_START = 1,
	PROP_PHRASE_END = 2,
	BASE_PROPERTY_ID = 3		// base for new properties
};

#endif
