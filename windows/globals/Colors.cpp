#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <afxwin.h>
 
#include "StdDefs.h"
#include "Colors.h"

rgb_color rgb_white = RGB(255, 255, 255);
rgb_color rgb_wtGray = RGB(245, 245, 245);
rgb_color rgb_ltGray = RGB(238, 238, 238);
rgb_color rgb_mdGray = RGB(210, 210, 210);
rgb_color rgb_dkGray = RGB(153, 153, 153);
rgb_color rgb_black = RGB(0, 0, 0);
rgb_color rgb_red = RGB(238, 100, 100);
rgb_color rgb_green = RGB(100, 238, 100);
rgb_color rgb_blue = RGB(100, 100, 238);
rgb_color rgb_ltPurple = RGB(238, 100, 238);
rgb_color rgb_purple = RGB(180, 0, 180);
rgb_color rgb_pink = RGB(255, 200, 200);
rgb_color rgb_yellow = RGB(238, 238, 100);
rgb_color rgb_brown = RGB(150, 150, 50);
rgb_color rgb_reddish = RGB(255, 195, 195);
rgb_color rgb_yellowish = RGB(225, 225, 195);
rgb_color rgb_greenish = RGB(195, 255, 195);
rgb_color rgb_turqoiseish = RGB(195, 225, 225);
rgb_color rgb_bluish = RGB(195, 195, 255);
rgb_color rgb_purplish = RGB(225, 195, 225);
rgb_color rgb_lightbrown = RGB(255, 210, 165);
rgb_color rgb_aqua = RGB(165, 255, 210);
rgb_color rgb_lavender = RGB(210, 165, 255);
rgb_color rgb_seablue = RGB(165, 210, 255);
rgb_color rgb_lime = RGB(210, 255, 165);
rgb_color rgb_mauve = RGB(255, 165, 210);
rgb_color rgb_orange = RGB(250, 150, 10);
rgb_color rgb_violet = RGB(200, 165, 210);
rgb_color rgb_indigo = RGB(100, 10, 100);
rgb_color rgb_deepblue = RGB(60, 10, 100);
                    
rgb_color
Lighter(rgb_color col, uchar a)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#endif
	float amt = ((float)a)/100.0;
	if (r < 10) r = 10;
	if (g < 10) g = 10;
	if (b < 10) b = 10;
	if ((int)(r+r*amt) > 255) r = 255; else r += r*amt;
	if ((int)(g+g*amt) > 255) g = 255; else g += g*amt;
	if ((int)(b+b*amt) > 255) b = 255; else b += b*amt;
	
	return RGB(r,g,b);
}
                    
rgb_color
RedEr(rgb_color col, int8 a)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#endif
	float amt = a/100.0;
	if (r < 10) r = 10;
	if ((int)(r+r*amt) > 255)
		r = 255;
	else if ((int)(r+r*amt) < 0)
		r = 0;
	else
		r += r*amt;
	return RGB(r,g,b);
}
                    
rgb_color
BlueEr(rgb_color col, int8 a)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#elif defined(_BEOS)
	uchar r=col.red;
	uchar g=col.green;
	uchar b=col.blue;
#endif
	float amt = a/100.0;
	if (b < 10) b = 10;
	if ((int)(b+b*amt) > 255)
		b = 255;
	else if ((int)(b+b*amt) < 0)
		b = 0;
	else
		b += b*amt;
	return RGB(r,g,b);
}
                    
rgb_color
GreenEr(rgb_color col, int8 a)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#elif defined(_BEOS)
	uchar r=col.red;
	uchar g=col.green;
	uchar b=col.blue;
#endif
	float amt = a/100.0;
	if (g < 10) g = 10;
	if ((int)(g+g*amt) > 255)
		g = 255;
	else if ((int)(g+g*amt) < 0)
		g = 0;
	else
		g += g*amt;
	return RGB(r,g,b);
}
                    
rgb_color
Darker(rgb_color col, uchar a)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#endif
	float amt = a/100.0;
	
	if ((int)(r-r*amt) < 0) r = 0; else r -= r*amt;
	if ((int)(g-g*amt) < 0) g = 0; else g -= g*amt;
	if ((int)(b-b*amt) < 0) b = 0; else b -= b*amt;

	return RGB(r,g,b);
}
                   
rgb_color
Inverse(rgb_color col)
{
#ifdef WIN32
	uchar r=GetRValue(col);
	uchar g=GetGValue(col);
	uchar b=GetBValue(col);
#endif
	r = 255-r;
	g = 255-g;
	b = 255-b;

	return RGB(r,g,b);
}


char *
ColorStr(rgb_color col)
{
	static  char	buf[20];
	sprintf(buf, "0x%x", col);
	return buf;
}

rgb_color
StrColor(char * nm)
{
	rgb_color	col;
	sscanf(nm, "%x", &col);
	return col;
}


#if defined(WIN32)
void
RaisedBox(CDC *v, CRect *box, rgb_color col, bool fill)
{
	v->SetDCBrushColor(col);
	if (fill) v->FillSolidRect(box, col);
	v->Draw3dRect(box, Lighter(col), Darker(col));
}

void
LoweredBox(CDC *v, CRect *box, rgb_color col, bool fill)
{
	v->SetDCBrushColor(col);
	if (fill) v->FillSolidRect(box, col);
	v->Draw3dRect(box, Darker(col), Lighter(col));
}

long AvgStringWidth(char *s, CFont *f)
{
	if ( s==NULL || *s=='\0' || f==NULL) {
		return 0;
	}
	long	l = strlen(s);
	LOGFONT	lf;
	if (f->GetLogFont(&lf)) {
		return l*lf.lfWidth;
	}
	return 0;
}

void StringExtent(char *s, CFont *f, CWnd *w, long &cx, long &cy)
{
	if ( s==NULL || *s=='\0' || f==NULL || w== NULL) {
		cx = 0;
		cy = 0;
	}
	CPaintDC		dc(w);
	dc.SelectObject(f);
	CSize sz = dc.GetTextExtent(s);
	cx = sz.cx;
	cy = sz.cy;
}

#endif