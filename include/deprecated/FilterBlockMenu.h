#ifndef _FILTERBLOCKMENU
#define _FILTERBLOCKMENU

class QuaQua;

class FilterBlockMenu: public BMenu
{
public:
					FilterBlockMenu(Block *B, BHandler *h, QuaQua *q );
	virtual void	AttachedToWindow();
	BHandler		*handler;
	QuaQua			*uberQua;
	Block			*block;
};
#endif
