#ifndef _COLORSOFEVERYTHING
#define _COLORSOFEVERYTHING

#include <afxwin.h>

typedef COLORREF		rgb_color;
class CDC;
class CRect;

void RaisedBox(CDC *, CRect *, rgb_color, bool fill=true);
void LoweredBox(CDC *, CRect *, rgb_color, bool fill=true);
void DrawGrid(CDC *v,
							CRect	r,
							short	xHeight,
							short	yHeight,
							long	xStart,
							long	xLen,
							long	yStart,
							long	yLen,
							rgb_color	xColor,
							rgb_color	xHighCol1, short high1X,
							rgb_color	xHighCol2, short high2X,
							rgb_color	yColor,
							rgb_color	yHighCol1, short high1Y,
							rgb_color	yHighCol2, short high2Y);

#ifdef _GDIPLUSCOLOR_H
inline Color AlphaColor(uchar a, COLORREF c)
	{ return Gdiplus::Color::MakeARGB(a, GetRValue(c), GetGValue(c), GetBValue(c)); }

#endif
class CFont;
class CWnd;
long AvgStringWidth(const char *s, CFont *f);
void StringExtent(const char *s, CFont *f, CWnd *w, long &cx, long &cy);

rgb_color	StrColor(char *str);
char		*ColorStr(rgb_color color);

rgb_color	Inverse(rgb_color);
rgb_color	Darker(rgb_color, uint8 amt=30);
rgb_color	Lighter(rgb_color, uint8 amt=15);
rgb_color	RedEr(rgb_color, int8 amt=15);
rgb_color	GreenEr(rgb_color, int8 amt=15);
rgb_color	BlueEr(rgb_color, int8 amt=15);

extern rgb_color rgb_white;
extern rgb_color rgb_wtGray;
extern rgb_color rgb_ltGray;
extern rgb_color rgb_mdGray;
extern rgb_color rgb_dkGray;
extern rgb_color rgb_black;
extern rgb_color rgb_red;
extern rgb_color rgb_green;
extern rgb_color rgb_blue;
extern rgb_color rgb_purple;
extern rgb_color rgb_pink;
extern rgb_color rgb_yellow;
extern rgb_color rgb_brown;
extern rgb_color rgb_reddish;
extern rgb_color rgb_yellowish;
extern rgb_color rgb_greenish;
extern rgb_color rgb_turqoiseish;
extern rgb_color rgb_bluish;
extern rgb_color rgb_purplish;
extern rgb_color rgb_lightbrown;
extern rgb_color rgb_aqua;
extern rgb_color rgb_lavender;
extern rgb_color rgb_seablue;
extern rgb_color rgb_lime;
extern rgb_color rgb_mauve;
extern rgb_color rgb_orange;
extern rgb_color rgb_violet;
extern rgb_color rgb_indigo;
extern rgb_color rgb_deepblue;


#endif