
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#include <MessageQueue.h>
#endif

#include "ObjectView.h"
#include "Colors.h"
#include "StdDefs.h"
#include "messid.h"
#include "GlblMessid.h"
#include "QuaObject.h"
#include "QuaInsert.h"
#include "Executable.h"

ObjectView::ObjectView(
				BRect rect,
				char *lbl,
				ObjectViewContainer *cntnr,
				char *name,
				uint32 rsz,
				uint32 flg)
	: BView(rect, name, rsz, flg|B_FRAME_EVENTS)
{
	color = green;
	SetFont( be_plain_font );
	SetFontSize(8);
	label = nullptr;//!!?? of course
	SetLabel(lbl);
	isSelected = FALSE;
	atPoint = rect.LeftTop();
	
	labelPoint.Set(10, (rect.bottom-rect.top)/2-6);

	container = cntnr;
	if (container) {
//		fprintf(stderr, "adding object %s to container\n", lbl);
		container->AddChild(this);
	}
}

void
ObjectView::AttachedToWindow()
{
	atPoint = Frame().LeftTop();
}

void
ObjectView::SetLabel(char *lbl)
{
	if (lbl) {
		if (label)
			delete [] label;
		label = new char[strlen(lbl)+1];
		strcpy(label, lbl);
	} else
		label = nullptr;
	Invalidate();
}

void
ObjectView::Select(bool sel)
{
	isSelected = sel;
	Draw(Bounds());
}

ObjectView::~ObjectView()
{
	if (label)
		delete [] label;
}

void
ObjectView::FrameMoved(BPoint ap)
{
	BRect f = Frame();	// seem to get vals in window co-ords!
	atPoint = f.LeftTop();	//ap;
}

void
ObjectView::FrameResized(float x, float y)
{
	Invalidate();
}

void
ObjectView::MessageReceived(BMessage *inMsg)
{
	BPoint delta(0, 0), where(0, 0);
	if (inMsg->WasDropped()) {
		where = inMsg->DropPoint(&delta);
		ConvertFromScreen(&where);
	}
	switch (inMsg->what) {
	
	case REDISPLAY_OBJECT: {
		BRect		r;
		BMessageQueue	*q = Window()->MessageQueue();
		BMessage	*m=nullptr;
		
		if (inMsg->HasRect("rect")) {
			r = inMsg->FindRect("rect");
		} else {
			r = Bounds();
		}
		for (int count=0; m=q->FindMessage(REDISPLAY_OBJECT, count); count++) {
			ObjectView	*ov=nullptr;
			m->FindPointer("object", (void**)&ov);
			if (ov == this) {
				BRect	s;
				if (m->HasRect("rect")) {
					s = m->FindRect("rect");
					s.left = Min(s.left,r.left);
					s.right = Max(s.right, r.right);
					m->ReplaceRect("rect", s);
				} else {
					s = r;
					m->AddRect("rect", s);
				}
				return;
			}
		}
		Draw(r);
		break;
	}
	
	case SET_COLOR: {
		rgb_color		*p;
		ssize_t			sz;
		
		inMsg->FindData("color", B_RAW_TYPE, (const void **)&p, &sz);
		color.red = p->red;
		color.green = p->green;
		color.blue = p->blue;
		ReDraw();
		break;
	}
	
	case SET_DISPLAY_MODE: {
		rgb_color		*p;
		int32			md = inMsg->FindInt32("display mode");
		SetDisplayMode(md);
		break;
	}
	
	default:
		BView::MessageReceived(inMsg);
		break;
		
	}
}


void
ObjectView::SetColor(rgb_color c)
{
	color = c;
	Invalidate();
}



void
ObjectView::ReDraw()
{
	Draw(Bounds());
}

void ObjectView::Draw(BRect region)
{
	BRect	bnd = Bounds();
	BRegion	regi; regi.Include(region);
	ConstrainClippingRegion(&regi);

	if (isSelected) {
		SetDrawingMode(B_OP_OVER);
		SetHighColor(black);
		FillRect(bnd, B_SOLID_HIGH);
		SetViewColor(black);
		SetHighColor(color);
	} else {
		SetDrawingMode(B_OP_OVER);
		SetHighColor(color);
		FillRect(bnd, B_SOLID_HIGH);
		SetViewColor(color);
		SetHighColor(black);
	}
	StrokeRect(bnd, B_SOLID_HIGH);
	if (label) {
		DrawString(label, labelPoint);
		if (container) {
			BRect	cb = container->Bounds();
			int	d = (cb.right-cb.left-30);
			int		llbl;
			for (	llbl = labelPoint.x + (1+(((int32)region.left) / d)) * d;
					llbl < region.right;
					llbl += d) {
				DrawString(label, BPoint(llbl,labelPoint.y));
			}
		}
	}
}

void
ObjectView::MouseDown(BPoint where)
{
	long		channel, quant;
	BRect		area = Bounds();

	ulong		mods = modifiers(); // Key mods???
	ulong		buts;
	BMessage	*msg;
	BPoint		pt;
	drawing_mode	cur_mode = DrawingMode();
	long		clicks;
	
	GetMouse(&pt, &buts);
	msg = Window()->CurrentMessage();
	clicks=msg->FindInt32("clicks");
	if (clicks > 2) clicks -= 2;
	if (clicks == 1) {
		if (buts & B_SECONDARY_MOUSE_BUTTON) {
			BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
			
			BPoint			orig = where;
			ConvertToScreen(&where);
			
			BMessage	*msg;
			BMenuItem	*item;
			
			if (displayMode != OBJECT_DISPLAY_SMALL) {
				msg = new BMessage(SET_DISPLAY_MODE);
				msg->AddInt32("display mode", OBJECT_DISPLAY_SMALL);
				item = new BMenuItem("Close", msg);
				qMenu->AddItem(item);
				item->SetTarget(this);
			}
			if (displayMode != OBJECT_DISPLAY_BIG) {
				msg = new BMessage(SET_DISPLAY_MODE);
				msg->AddInt32("display mode", OBJECT_DISPLAY_BIG);
				item = new BMenuItem("Open", msg);
				qMenu->AddItem(item);
				item->SetTarget(this);
			}
				
			qMenu->SetAsyncAutoDestruct(true);
			qMenu->Go(where, true, false, true);
		} else {
			msg = new BMessage(MOVE_OBJECT);
		 	msg->AddPointer("object_view", this);
		 	
			if (mods & B_SHIFT_KEY) {
				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
				((ObjectViewContainer *)Parent())->Select(this);
			}
		
			DragMessage(msg, area);
		}
	} else if (clicks > 1) {	// edit object
		if (displayMode == OBJECT_DISPLAY_BIG) {
			SetDisplayMode(OBJECT_DISPLAY_SMALL);
		} else if (displayMode == OBJECT_DISPLAY_SMALL) {
			SetDisplayMode(OBJECT_DISPLAY_BIG);
		}
	} else {
	}
}

//QuaNexion *
//ObjectView::FindQuaNexion(ObjectView *to, short typ, int32 which)
//{
//	return container->quaNexions.Find(this, to, typ, which);
//}
//
QuaNexion *
ObjectView::AddQuaNexion(QuaInsert *ifr, QuaInsert *itr, rgb_color c, uchar ty, uchar w)
{
	QuaNexion *y = container->quaNexions.Add(ifr, itr, c, ty, w, (ObjectViewContainer *)Parent());
	return y;
}

void
ObjectView::RemoveQuaNexion(QuaInsert *to)
{
	container->RemoveQuaNexion(to);
}

void
ObjectView::MoveTo(float x, float y)
{
	MoveTo(BPoint(x, y));
}

void
ObjectView::MoveTo(BPoint where)
{
	BView::MoveTo(where);
}

bool
ObjectView::SetDisplayMode(short dm)
{
	displayMode = dm;
	Invalidate();
	return true;
}

ObjectViewContainer::ObjectViewContainer(BRect area, char *lbl, uint32 rsz, uint32 flg):
	BView(area, lbl, rsz, flg)
{
	selectedObjects.MakeEmpty();
	atX = BPoint(5,5);
	lastRB = BPoint(0,0);
}

void
ObjectViewContainer::DeSelectAll()
{
	int		i;
	for (i=0; i<selectedObjects.CountItems(); i++) {
		((ObjectView *)selectedObjects.ItemAt(i))->Select(FALSE);
	}
	selectedObjects.MakeEmpty();
}

void
ObjectViewContainer::SelectAll()
{		// are ALL the children ObjectView ????
	;
}

void
ObjectViewContainer::Select(ObjectView *obj)
{
	DeSelectAll();
	AddSelection(obj);
}


void
ObjectViewContainer::Draw(BRect area)
{
	for (short i=0; i<quaNexions.CountItems(); i++) {
		QuaNexion *p = quaNexions.ItemAt(i);
		p->Draw();
	}
}

void
ObjectViewContainer::MouseDown(BPoint p)
{
	MakeFocus(true);
	Window()->Activate();
	DeSelectAll();
}

void
ObjectViewContainer::MakeFocus(bool foc)
{
	BView::MakeFocus(foc);
	Parent()->Invalidate();
}

void
ObjectViewContainer::MessageReceived(BMessage *inMsg)
{
	BPoint delta(0, 0), where(0, 0);
	if (inMsg->WasDropped()) {
		where = inMsg->DropPoint(&delta);
		ConvertFromScreen(&where);
	}
	switch (inMsg->what) {
	
	case MOVE_OBJECT: {
		if (inMsg->HasPointer("qua_object")) {		
			QuaObject		*obj;
			inMsg->FindPointer("qua_object", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			obj->MoveTo(where);
		} else if (inMsg->HasPointer("sym_object")) {		
			SymObject		*obj;
			inMsg->FindPointer("sym_object", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			obj->MoveTo(where);
		}
		break;
	}
	
	default:
		BView::MessageReceived(inMsg);
		break;
		
	}
}



void
ObjectViewContainer::AddSelection(ObjectView *obj)
{
	if (obj->isSelected)
		selectedObjects.RemoveItem(obj);
	else
		selectedObjects.AddItem(obj);
	obj->Select(!obj->isSelected);
}

int
ObjectViewContainer::CountSelected()
{
	return selectedObjects.CountItems();
}

QuaNexion *
ObjectViewContainer::AddQuaNexion(ObjectView *v1, QuaInsert *p1, ObjectView *v2, QuaInsert *p2, rgb_color c, uchar ty, uchar w)
{
	QuaNexion *q = quaNexions.Add(p1,p2,c,ty,w,this);
	
	p1->Draw(v1, v1->displayMode);
	p2->Draw(v2, v2->displayMode);
//	if (v1->Looper())
//		v1->Looper()->Lock();
//	v1->Invalidate();
//	if (v1->Looper())
//		v1->Looper()->Unlock();
//	if (v2->Window())
//		v2->Window()->Lock();
//	v2->Invalidate();
//	if (v2->Window())
//		v2->Window()->Unlock();
	return q;
}


void
ObjectViewContainer::KeyDown(const char *c, int32 nb)
{
	if (nb == 1) {
		char	theChar = c[0];
		if (theChar == B_DELETE) {
			BList	L(selectedObjects);
			DeSelectAll();
			for (short i=0; i<L.CountItems(); i++) {
				ObjectView *p = (ObjectView *)L.ItemAt(i);
				RemoveChild(p);
			}
		} else 
			BView::KeyDown(c, nb);
	} else {
		BView::KeyDown(c, nb);
	}
}


void
ObjectViewContainer::RemoveQuaNexion(QuaInsert *p1, QuaInsert *p2)
{
	if (Window())
		Window()->Lock();
	QuaNexion *p = (QuaNexion *)quaNexions.Find(p1,p2);
	p1->quanexion = nullptr;
	p2->quanexion = nullptr;
	quaNexions.RemoveItem(p);
	delete p;
	Invalidate();
	if (Window())
		Window()->Unlock();
}
void
ObjectViewContainer::RemoveQuaNexion(QuaInsert *p1)
{
	if (Window())
		Window()->Lock();
	QuaNexion *p = (QuaNexion *)quaNexions.Find(p1);
	p1->quanexion = nullptr;
	quaNexions.RemoveItem(p);
	delete p;
	Invalidate();
	if (Window())
		Window()->Unlock();
}

ObjectView *
ObjectViewContainer::Selected(int ind)
{
	return ((ObjectView *)selectedObjects.ItemAt(ind));
}

bool
ObjectViewContainer::MessageDropped(BMessage *message,BPoint where,
								BPoint delta)
{
	if (message->what == MOVE_OBJECT) {
		if (message->HasPointer("object_view")) {
			ObjectView	*obj;		
			message->FindPointer("object_view", (void **)&obj);
			where = where - delta - Bounds().LeftTop();
			obj->MoveTo(where);
			return TRUE;
		}
	}
	return false;
}

void
ObjectViewContainer::ArrangeChildren()
{
	;
}


QuaNexion::QuaNexion(QuaInsert *p1, QuaInsert *p2, rgb_color c, uchar ty, uchar w, ObjectViewContainer *v)
{
	from = p1;
	to = p2;
	to->quanexion = from->quanexion = this;
	color = c;
	type = ty;
	width = w>0?w:1;
	parent = v;
	Draw();
}

void
QuaNexion::Draw(bool scrub)
{
	if (from && to && parent && from->object && to->object
			&& from->object->executable && to->object->executable
			&& from->object->executable->symObject
			&& to->object->executable->symObject) {
		parent->SetHighColor(scrub?parent->ViewColor():color);
		parent->SetPenSize(width*2);
		SymObject	*f = from->object->executable->symObject;
		SymObject	*t = to->object->executable->symObject;
//		fprintf(stderr, "%s %g %g, %g %g %g %g\n", from->name, f->atPoint.x, f->atPoint.y, from->rect.left, from->rect.top, from->rect.right, from->rect.bottom);
//		fprintf(stderr, "%s %g %g, %g %g %g %g\n", to->name, t->atPoint.x, t->atPoint.y, to->rect.left, to->rect.top, to->rect.right, to->rect.bottom);
		BPoint	p0, p1, p2, p3;
		
		switch (f->displayMode) {
		case OBJECT_DISPLAY_SMALL:
			if (f->atPoint.y < t->atPoint.y + to->rect.bottom) {
				p0 = f->atPoint + BPoint(
									(from->rect.right+from->rect.left)/2,
									from->rect.bottom);
				p1 = p0 + BPoint(0,5);
			} else {
				p0 = f->atPoint + BPoint(
									(from->rect.right+from->rect.left)/2,
									from->rect.top);
				p1 = p0 + BPoint(0,-5);
			}
			break;
		case OBJECT_DISPLAY_BIG:
		default:
			if (f->atPoint.x < t->atPoint.x + to->rect.left) {
				p0 = f->atPoint + BPoint(
									from->rect.right,
									(from->rect.top+from->rect.bottom)/2);
				p1 = p0 + BPoint(5,0);
			} else {
				p0 = f->atPoint + BPoint(
									from->rect.left,
									(from->rect.top+from->rect.bottom)/2);
				p1 = p0 + BPoint(-5,0);
			}
		break;
		}
		
		switch (t->displayMode) {
		case OBJECT_DISPLAY_SMALL:
			if (p1.y > t->atPoint.y + to->rect.bottom) {
				p3 = t->atPoint + BPoint(
									(to->rect.right+to->rect.left)/2,
									to->rect.bottom);
				p2 = p3 + BPoint(0,5);
			} else {
				p3 = t->atPoint + BPoint(
									(to->rect.right+to->rect.left)/2,
									to->rect.top);
				p2 = p3 + BPoint(0,-5);
			}
			break;
		case OBJECT_DISPLAY_BIG:
		default:
			if (p1.x > t->atPoint.x + to->rect.right) {
				p3 = t->atPoint + BPoint(
									to->rect.right,
									(to->rect.top+to->rect.bottom)/2);
				p2 = p3 + BPoint(5,0);
			} else {
				p3 = t->atPoint + BPoint(
									to->rect.left,
									(to->rect.top+to->rect.bottom)/2);
				p2 = p3 + BPoint(-5,0);
			}
		break;
		}
		
		parent->StrokeLine(p0, p1);
		parent->StrokeLine(p1, p2);
		parent->StrokeLine(p2, p3);
	}
//	fprintf(stderr, "Draw done\n");
}

QuaNexion::~QuaNexion()
{
	Draw(true);
}

QuaNexion*
QuaNexionList::ItemAt(int i)
{
	return (QuaNexion *)BList::ItemAt(i);
}

QuaNexion *
QuaNexionList::Find(Insertable *p1, Insertable *p2, short typ, int32 which)
{
	int cnt=0;
	for (short i=0; i<CountItems(); i++) {
		QuaNexion	*p = (QuaNexion*)ItemAt(i);
		if ((  ((p2==nullptr || p->to->object == p2)
				 && (p1==nullptr || p->from->object == p1))
			||((p1==nullptr || p->to->object == p1)
				 && (p2==nullptr || p->from->object == p2)))
		 && (typ == QN_DISINTERESTED || p->type == typ)
		 && cnt++ == which)
			return p;
	}
	return nullptr;
}

QuaNexion *
QuaNexionList::Find(QuaInsert *p1, QuaInsert *p2)
{
	int cnt=0;
	for (short i=0; i<CountItems(); i++) {
		QuaNexion	*p = (QuaNexion*)ItemAt(i);
		if ((p1==nullptr||p->from == p1) && (p2==nullptr||p->to == p2))
			return p;
	}
	return nullptr;
}

QuaNexion *
QuaNexionList::Find(QuaInsert *p1)
{
	int cnt=0;
	for (short i=0; i<CountItems(); i++) {
		QuaNexion	*p = (QuaNexion*)ItemAt(i);
		if (p->from == p1 || p->to == p1)
			return p;
	}
	return nullptr;
}

//
//int
//QuaNexionList::Count(Insertable *p1, Insertable *p2)
//{
//	int		cnt = 0;
//	for (short i=0; i<CountItems(); i++) {
//		QuaNexion	*p = (QuaNexion*)ItemAt(i);
//		if ((!p1||p->from->object->symObject == p1) && (!p2||p->to->object->symObject == p2))
//			cnt++;
//	}
//	return cnt;
//}

QuaNexion *
QuaNexionList::Add(QuaInsert * p1, QuaInsert *p2, rgb_color c, uchar ty, uchar w, ObjectViewContainer *v)
{
	QuaNexion	*q = new QuaNexion(p1, p2, c,ty,w, v);
	BList::AddItem(q);
	return q;
}

void
QuaNexionList::Remove(QuaInsert *p1, QuaInsert *p2)
{
	int		cnt = 0;
	for (short i=0; i<CountItems(); i++) {
		QuaNexion	*p = (QuaNexion*)ItemAt(i);
		if ( (!p1||p->from == p1) &&
			 (!p2||p->to == p2)) {
			BList::RemoveItem(p);
			delete p;
			break;
		} else {
			cnt++;
		}
	}
}

void
QuaNexionList::Remove(QuaNexion *p1)
{
	if (p1) {
		BList::RemoveItem(p1);
		delete p1;
	}
}