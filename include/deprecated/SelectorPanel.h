#ifndef _SELECTORPANEL_H
#define _SELECTORPANEL_H

class Stacker;

struct StuffInfo
{
					StuffInfo(BRect &, Stacker *, int, int);
					~StuffInfo();
					
	Stacker			*stacker;
	int				lblind;
	BRect			rect;
	int				mode;
};

class SelectorPanel: public BControl
{
public:
					SelectorPanel(BRect r, BMessage *msg);
					~SelectorPanel();
	virtual void	Draw(BRect r);

	virtual void	MouseDown(BPoint p);
	virtual void	MouseMoved(BPoint, uint32, const BMessage *);
	virtual void	MouseUp(BPoint);

	short			tracking;
	BPoint			track_point;
	short			track_button;
	
	void			AddStuff(Stacker *, int ind, int md);
	void			DeleteStuff(Stacker *);
	void			DrawItem(StuffInfo *);
	
	BList			stuff;
	float			stuffWidth;
	rgb_color		stuffColor;
};


#endif