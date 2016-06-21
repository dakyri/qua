#ifndef _OBJECTVIEW
#define _OBJECTVIEW

class ObjectView;
class ObjectViewContainer;
class QuaInsert;
class Insertable;
class Schedulable;


class ObjectView: public BView {
public:
						ObjectView(BRect rect, char *lbl, ObjectViewContainer *, char *nm, uint32 rsz, uint32 flgs);
						~ObjectView();

	virtual void		ReDraw();
	virtual void		Draw(BRect region);
	virtual void		MouseDown(BPoint where);
	virtual void		MessageReceived(BMessage *msg);
	virtual void		MoveTo(BPoint p);
	virtual void		MoveTo(float, float);
	virtual void		Select(bool sel=TRUE);
	virtual void		FrameMoved(BPoint);
	virtual void		FrameResized(float, float);
	virtual void		AttachedToWindow();

	void				SetLabel(char *lbl);
	void				SetColor(rgb_color c);

	BPoint				atPoint;
	
	QuaNexion			*FindQuaNexion(QuaInsert *, short dir, int32 which=0);
	QuaNexion			*AddQuaNexion(QuaInsert *, QuaInsert *, rgb_color c, uchar ty, uchar w);
	void				RemoveQuaNexion(QuaInsert *);
	
	virtual bool		SetDisplayMode(short);
	short				displayMode;
	
	rgb_color			color;
	char				*label;
	bool				isSelected;
	
	BPoint				labelPoint;

	ObjectViewContainer	*container;
};

class ObjectViewContainer: public BView {
public:
						ObjectViewContainer(BRect rect, char *lbl, uint32 rsz=0, uint32 flg=B_WILL_DRAW);

	virtual void		Draw(BRect);
	virtual void		MessageReceived(BMessage *inMsg);
	virtual void		MouseDown(BPoint p);
	virtual void		MakeFocus(bool foc);
	virtual void		KeyDown(const char *c, int32 nb);
	virtual void		ArrangeChildren();
	
	QuaNexion			*AddQuaNexion(ObjectView *, QuaInsert *, ObjectView *, QuaInsert *, rgb_color c, uchar ty, uchar w);
	void				RemoveQuaNexion(ObjectView *, QuaInsert *);
	void				RemoveQuaNexion(QuaInsert *, QuaInsert *);
	void				RemoveQuaNexion(QuaInsert *);
	
	void				DeSelectAll();
	void				SelectAll();
	void				Select(ObjectView *obj);
	void				AddSelection(ObjectView *obj);
	int					CountSelected();
	ObjectView			*Selected(int ind);

	bool				MessageDropped(
									BMessage *message,
									BPoint where,
									BPoint delta);

	BList				selectedObjects;
	BPoint				atX, lastRB;
	float				widestRight;
	
	QuaNexionList		quaNexions;
};

enum {
	OBJECT_DISPLAY_NIKON=0,
	OBJECT_DISPLAY_SMALL=1,
	OBJECT_DISPLAY_BIG=2
};


#endif