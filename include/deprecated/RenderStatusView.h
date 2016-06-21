#ifndef _RSVP
#define _RSVP

class RenderStatusView: public BView
{
public:
						RenderStatusView(BRect, char *);
	virtual void		Draw(BRect r);
	void				SetStatusValue(short val);
	short				percentValue;
};

#endif