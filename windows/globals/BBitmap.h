#ifndef _BITMAP_H
#define _BITMAP_H

#include <afxwin.h>

class BBitmap : public CBitmap
{
public:

    BOOL Draw(HDC hDC, int x, int y);
	BOOL Draw(HDC hDC, int x, int y,int w,int h);
    BOOL DrawTransparent(HDC hDC, int x, int y);
 	BOOL DrawTransparent(HDC hDC, int x, int y, COLORREF clrTrans);
 	BOOL DrawTransparent(HDC hDC, int x, int y, int bx, int by);
 
	BOOL DrawTiles(HDC hDC, int nWidth, int nHeight);

    int GetWidth();
    int GetHeight();
    BOOL GetSize(int& nWidth, int& nHeight);

    BBitmap();
    virtual ~BBitmap();

	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmap(UINT nIDResource);
	bool		isLoaded;
protected:

    BOOL Update();
	BOOL CreateMask(HDC hDC,COLORREF clrTrans, int btx=0, int bty=0);

private:

    int			width;
    int			height;
    HBITMAP		mask;  
	COLORREF	clrTransparent;

};



#endif // _BITMAP_H
