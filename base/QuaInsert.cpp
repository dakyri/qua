#include "qua_version.h"


#include "QuaInsert.h"
#include "Insertable.h"
#include "Sym.h"
#include "QuaAudio.h"
#include "Block.h"
#include "Executable.h"

#if defined(QUA_V_ARRANGER_INTERFACE)
#if defined(WIN32)
#include "include/QuaDisplay.h"
#endif
#endif

QuaInsert::QuaInsert(
				Insertable *o,
				QuaNexion *q,
				char *n,
				int32 i,
				short t,
				int32 w,
				int32 fmt)
{
	object = o;
	quanexion = q;
	if (n) {
		name = new char[strlen(n)+1];
		strcpy(name, n);
	} else {
		name = nullptr;
	}
	id = i;
	type = t;
	width = w;
	format = fmt;
	fprintf(stderr, "new insert %x in %x <%s> %d\n", this, object, name, id);
}

QuaInsert::	QuaInsert()
{
	object = nullptr;
	quanexion = nullptr;
	name = nullptr;
	id = -1;
	type = 0;
	width = 1;
	format = 0;
}

void
QuaInsert::	Set(
				Insertable *o,
				QuaNexion *q,
				char *n,
				int32 i,
				short t)
{
	object = o;
	quanexion = q;
	if (name) {
		delete name;
	}
	if (n) {
		name = new char[strlen(n)+1];
		strcpy(name, n);
	}
	id = i;
	type = t;
#if defined(QUA_V_ARRANGER_INTERFACE)
	/*
	if (object->executable->representation) {
		if (object->executable->representation->Window())
			object->executable->representation->Window()->Lock();
		object->executable->representation->Invalidate();
		if (object->executable->representation->Window())
			object->executable->representation->Window()->Unlock();
	}*/
#endif
}

void
QuaInsert::	SetName(char *n)
{
	if (name) {
		delete name;
	}
	if (n) {
		name = new char[strlen(n)+1];
		strcpy(name, n);
	} else {
		name = "";
	}
#if defined(QUA_V_ARRANGER_INTERFACE)
	/*
	if (object->executable->representation) {
		if (object->executable->representation->Window())
			object->executable->representation->Window()->Lock();
		object->executable->representation->Invalidate();
		if (object->executable->representation->Window())
			object->executable->representation->Window()->Unlock();
	}*/
#endif
}

QuaInsert::~QuaInsert()
{
	if (name)
		delete name;
}

#if defined(QUA_V_ARRANGER_INTERFACE)
/*
void
QuaInsert::Draw(BView *v, short dm)
{
	BPoint	p1, p2, p3;
	v->SetHighColor(black);
	v->StrokeRect(rect);
	rgb_color	col;
	col = quanexion?Darker(green,20):Darker(red,20);
	float	x1=0, x2=0;
	if (type == INPUT_INSERT) {
		p1 = rect.RightTop()+BPoint(-1,1);
		p2 = rect.RightBottom()+BPoint(-1, -1);
		p3.Set(p1.x-6,(rect.top+rect.bottom)/2);
		x1 = 2; x2 = p3.x-1;
		v->SetHighColor(col);
		v->FillTriangle(p1,p2,p3);
		v->SetHighColor(Lighter(col));
		v->StrokeLine(p1,p2);
		v->SetHighColor(Darker(col));
		v->StrokeLine(p2,p3);
		v->SetHighColor(Lighter(col));
		v->StrokeLine(p3,p1);
	} else {
		p1 = rect.RightTop()+BPoint(-7,1);
		p2 = rect.RightBottom()+BPoint(-7,-1);
		p3.Set(p1.x+6,(rect.top+rect.bottom)/2);
		x1 = 2; x2 = p1.x-1;
		v->SetHighColor(col);
		v->FillTriangle(p1,p2,p3);
		v->SetHighColor(Lighter(col));
		v->StrokeLine(p1,p2);
		v->SetHighColor(Darker(col));
		v->StrokeLine(p2,p3);
		v->SetHighColor(Lighter(col));
		v->StrokeLine(p3,p1);
	}
	
	if (dm == OBJECT_DISPLAY_BIG) {
		BRect wr(x1,rect.top+1,x2,rect.bottom-2);
		BRegion	reg; reg.Include(wr);
		
		v->SetFontSize(6);
		v->SetHighColor(black);
		v->SetLowColor(v->ViewColor());
		if (name) {
			v->DrawString(name, BPoint(wr.left,wr.bottom));
		}
	}
}*/
#endif


Insertable::Insertable()
{
}

QuaInsert *
Insertable::FindInsert(char *nm, int32 id, short typ)
{
	for (auto ins: inserts) {
//		fprintf(stderr, "find %d %x %d %s\n", i, ins, ins->id, ins->name);
		if (id >= 0 && ins->id == id && (typ == ANY_INSERT || typ == ins->type))
			return ins;
		if (nm && ins->name && strcmp(nm, ins->name) == 0 && (typ == ANY_INSERT || typ == ins->type))
			return ins;
	}
	return nullptr;
}

#if defined(QUA_V_ARRANGER_INTERFACE)
void
Insertable::SetupInsertDisplay()
{
//	QuaSymbolBridge	*o = executable->interfaceBridge;
	/*
	if (!o) {
		return;
	}
	if (o->Window())
		o->Window()->Lock();
	o->SetFontSize(10);
	if (o->displayMode == OBJECT_DISPLAY_SMALL) {
		float	w = 20 + o->StringWidth(o->label);
		float	h = 19;		
		o->ResizeTo(w + inserts.CountItems()*8, h);
		for (short i=0; i<inserts.CountItems(); i++) {
			QuaInsert	*ins = (QuaInsert *)inserts.ItemAt(i);
			ins->rect.Set(w+8*i,1,w+8*i+8, 18);
		}
	
	} else {
		float w = o->StringWidth(o->label);
		w = Max(w,45);
		float	h = o->labelPoint.y+1;
		o->ResizeTo(w, h + inserts.CountItems()*8);
		for (short i=0; i<inserts.CountItems(); i++) {
			QuaInsert	*ins = (QuaInsert *)inserts.ItemAt(i);
			ins->rect.Set(1,h+i*8,w,h+8*i+8);
		}
	}
	o->Invalidate();
	if (o->Parent())
		o->Parent()->Invalidate();
	if (o->Window())
		o->Window()->Unlock();
	*/
}
#endif

void
Insertable::RemoveInsert(short ind)
{
	QuaInsert	*i = inserts[ind];
	inserts.erase(inserts.begin()+ind);
	fprintf(stderr, "remove insert %x %x %x\n", (unsigned)i, (unsigned)i->quanexion, (unsigned)i->quanexion?i->quanexion->parent:nullptr);
	if (i->quanexion && i->quanexion->parent) {
		if (i->quanexion->from == i)
			i->quanexion->to->quanexion = nullptr;
		else
			i->quanexion->from->quanexion = nullptr;

#if defined(QUA_V_ARRANGER_INTERFACE)
//		insertableBridge.RemoveQuaNexion(i);
#endif
	}
	delete i;
}

void
Insertable::RemoveInsertById(int32 id)
{
	int i = 0;
	for (auto ci: inserts) {
		if (ci->id == id) {
			RemoveInsert(i);
			return;
		}
		i++;
	}
}

QuaInsert *
Insertable::AddInsert(char *nm, int32 id, short typ, int32 w, int32 f)
{
	QuaInsert	*q = new QuaInsert(this, nullptr, nm, id, typ, w, f);
	inserts.push_back(q);
#ifdef QUA_V_ARRANGER_INTERFACE
	SetupInsertDisplay();
#endif
	return q;
}

Insertable::~Insertable()
{

	for (auto ins = inserts.begin(); ins != inserts.end();) {
		delete *ins;
		ins = inserts.erase(ins);
#if defined(QUA_V_ARRANGER_INTERFACE)
//		insertableBridge.RemoveQuaNexion(ins);
#endif
	}
}
