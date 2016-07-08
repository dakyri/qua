#ifndef _INSERTABLE
#define _INSERTABLE

class QuaInsert;
class Executable;

#include "QuaDisplay.h"

class Insertable
{
public:
						Insertable();
						~Insertable();
					
	QuaInsert			*AddInsert(char *name, int32 id, short typ, int32, int32);
	QuaInsert			*FindInsert(char *name, int32 id, short typ);
	void				RemoveInsert(short index);
	void				RemoveInsertById(int32 id);
	virtual void		SetupInsertDisplay();

	std::vector<QuaInsert*>	inserts;
};
#endif