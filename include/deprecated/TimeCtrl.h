#ifndef _TIMECTRL
#define _TIMECTRL

#include "KeyVal.h"
#include "Time.h"
#include "Metric.h"

class TimeCtrl;
class Metric;

class MetricSelect: public BStringView
{
public:
					MetricSelect(BRect rec, char *label, Metric *m,
						BHandler *h,
						uint32 mo=B_FOLLOW_LEFT | B_FOLLOW_TOP,
						uint32 f=B_WILL_DRAW);
					~MetricSelect();
	virtual void	Draw(BRect area);
	virtual void	MakeFocus(bool mf);
	virtual void	MouseDown(BPoint where);
	void			SetFont(BFont *f);
	void			SetValue(Metric *m);
	void			SetFontSize(float f);

	Metric			*metric;
	BHandler		*handler;	
};


class MeasureView: public BTextView
{
public:
					MeasureView(BRect frame, const char *name,
							Metric *m,
         					ulong resizingMode, ulong flags);
         			~MeasureView();
         			
    virtual void	KeyDown(const char *c, int32 nb);
	virtual void	MakeFocus(bool flag = TRUE);
	virtual bool	AcceptsDrop(BMessage *m);
	
	void			SetValue(Time &t);
	void			SetValue(long tick);
	void			SetValue();
	void			SetBounds(long, long, long);
	long			TickValue();
	float			ActualValue();
	void			SetSibling(BView *v);

	Time			theTime;
	TimeCtrl		*controller;
    BView			*sibling;
    
    long			homeValue, upValue, downValue;
};

class TimeCtrl: public BControl
{
public:
					TimeCtrl(BRect frame, const char *name,
		         			 char *label, 
							BMessage *msg,
							Metric *m, short mode,
							ulong f1, ulong f2);
					~TimeCtrl();
	long			TickValue();
	float			ActualValue();
	void			SetValue(long tick);
	void			SetValue(Time &t);
	void			SetBounds(long, long, long);
	void			SetSibling(BView *v);
	const char		*Text();
	virtual void	MakeFocus(bool flag = TRUE);
	virtual void	Draw(BRect reg);
	void			MakeEditable(bool wh=FALSE);
	void			SetText(const char *txt);
	void 			SetDivider(float xCoordinate);
	void			Invoke() { BControl::Invoke(); }

	MeasureView		*textview;
	MetricSelect	*metricSel;		
	float			divider;
	short			drawMode;
};

enum {
	SET_METRIC='mets'
};

#endif