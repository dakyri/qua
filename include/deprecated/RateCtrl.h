#ifndef _RATECTRL
#define _RATECTRL

#include "KeyVal.h"
#include "NumCtrl.h"

class RateCtrl: public NumCtrl
{
public:
				RateCtrl(BRect frame, const char *name,
		          char *label, 
		          const char *text,
					BMessage *msg,
					ulong f1, ulong f2);
				~RateCtrl();
	void		SetValue(long r);
};

extern KeyVal	rates[];
#endif