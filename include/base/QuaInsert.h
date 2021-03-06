#ifndef _QUAINSERT
#define _QUAINSERT

class Executable;
class Schedulable;
class QuaNexion;
class Insertable;
class SymObject;
class QuaNexion;
class ObjectViewContainer;

enum {
	ANY_INSERT = 0,
	INPUT_INSERT = 1,
	OUTPUT_INSERT = 2,
	BIDIRECT_INSERT = 3
};

enum {
	QN_DISINTERESTED=-1,
	QN_DEAD=0,
	QN_UNIDIRECT=1,
	QN_BIDIRECT=3
};


#include <vector>
#include <string>
using namespace std;

#include "StdDefs.h"

class QuaInsert
{
public:
	QuaInsert( Insertable *object, QuaNexion *quanexion, const string &name, int32 id, short type, int32, int32);
	QuaInsert();
	~QuaInsert();
	void Set( Insertable *object, QuaNexion *quanexion, const string &name, int32 id, short type);
	void SetName(char *);

	Insertable *object;
	QuaNexion *quanexion;
	string name;
	int32 id;
	short type;
	int32 width;
	int32 format;
};

class QuaNexion
{
public:
						QuaNexion(QuaInsert *, QuaInsert *, uchar type, uchar width,
								ObjectViewContainer *parent);
						~QuaNexion();
	void				Draw(bool scrub=false);
	
	QuaInsert			*from;
	QuaInsert			*to;
	uchar				type;
	uchar				width;
	ObjectViewContainer *parent;
};

class QuaNexionList: public std::vector<QuaInsert*>
{
public:
	QuaNexion			*Add(QuaInsert *, QuaInsert *, uchar type, uchar width,
								ObjectViewContainer *parent);
	void				Remove(QuaInsert *, QuaInsert *);
	void				Remove(QuaInsert *);
	void				Remove(QuaNexion *);
	QuaNexion			*Find(Insertable *, Insertable *,
								short typ=QN_DISINTERESTED,
								int32 which=0);
	QuaNexion			*Find(QuaInsert *, QuaInsert *);
	QuaNexion			*Find(QuaInsert *);
	int					Count(Schedulable *, Schedulable *);
	QuaNexion			*ItemAt(int);
};

#endif