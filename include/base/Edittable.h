#ifndef __EDITTABLE_H
#define __EDITTABLE_H

class Edittable
{
public:
	Edittable() { editor = NULL; }
	virtual ~Edittable() {}
	virtual void		Edit() {}
	class Editor		*editor;
};

#endif
