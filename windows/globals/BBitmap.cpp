
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "StdDefs.h"
#include "BBitmap.h"
#include "Colors.h"




/////////////////////////////////////////////////////////////////////////////
// BBitmap
BBitmap::BBitmap()
{
    width = 0;
    height = 0;
    mask = NULL;
	clrTransparent = RGB(255,200,200);
	isLoaded = false;
}

BBitmap::~BBitmap()
{
}

/**
* @desc		This method recalcs the size of the loaded bitmap 
* @return	TRUE, if the bitmap size is valid
*/
BOOL BBitmap::Update()
{
    // Get the width and height.
	if (!isLoaded) {
		width = height = 0;
		return false;
	}
    BITMAP bm;
	if (GetBitmap(&bm) == 0) {
		width = height = 0;
		return false;
	}
    width = bm.bmWidth;
    height = bm.bmHeight;

	BOOL bValid = width > 0 && height > 0;
	return(bValid);
}

BOOL
BBitmap::LoadBitmap(LPCTSTR lpszResourceName)
{
	if (CBitmap::LoadBitmap(lpszResourceName)) {
		isLoaded = true;
		Update();
		return true;
	}
	return false;
}

BOOL
BBitmap::LoadBitmap(UINT nIDResource)
{
	if (CBitmap::LoadBitmap(nIDResource)) {
		isLoaded = true;
		Update();
		return true;
	}
	return false;
}

int
BBitmap::GetWidth()
{
    if (width == 0 || height == 0)
        Update();
    return width;
}

int
BBitmap::GetHeight()
{
    if (width == 0 || height == 0)
        Update();
    return height;
}

/**
*
* @return	TRUE, if the bitmap size is valid
*/

BOOL
BBitmap::GetSize(int& nWidth, int& nHeight)
{
	BOOL bValid = width > 0 && height > 0;

	if (bValid == FALSE) {
        bValid = Update();
	}

	nWidth = width;
	nHeight = height;

    return bValid;
}

/**
*	
* @desc	Draw with original size
*/

BOOL
BBitmap::Draw(HDC hDC, int x, int y)
{
	BOOL bValid = width > 0 && height > 0;
	
    if (bValid == TRUE) {

		HDC hdcMem = ::CreateCompatibleDC(hDC);
 
		// Select the bitmap into the memory dc
		HBITMAP hOld = (HBITMAP)::SelectObject(hdcMem,(HBITMAP)(m_hObject));

		::BitBlt(hDC,
				 x, y,
				 width, height,
				 hdcMem,
				 0, 0,
				 SRCCOPY);

		::SelectObject(hdcMem, hOld);
		::DeleteDC(hdcMem); 
		}
    return bValid;

}

/**
*	
* @desc	Draw scaled
*/
BOOL
BBitmap::Draw(HDC hDC, int x, int y,int w,int h)
{
	BOOL bValid = width > 0 && height > 0;
	
    if (bValid == TRUE) {
		HDC hdcMem = ::CreateCompatibleDC(hDC);

		// Select the bitmap into the mem DC.
		HBITMAP hOld =  (HBITMAP)::SelectObject(hdcMem,(HBITMAP)(m_hObject));

		::StretchBlt(hDC,
				 x, y,
				 w, h,
				 hdcMem,
				 0, 0,width,height,
				 SRCCOPY);

		::SelectObject(hdcMem, hOld);
		::DeleteDC(hdcMem); 
	}

    return bValid;
}




/**
* @name	DrawTransparent
* @desc	Draw bitmap transparent. This creates a mask
*		and uses the topleft pixel as transparent color
*/
BOOL
BBitmap::DrawTransparent(HDC hDC, int x, int y)
{
	BOOL bValid = width > 0 && height > 0;

    if (bValid == TRUE) {

		if (!mask)
			bValid = CreateMask(hDC, clrTransparent);
		
		// draw using the default transparent color
		// or the color set by CreateMask
		DrawTransparent(hDC, x, y, clrTransparent);
	}

	return(bValid);
}

BOOL
BBitmap::DrawTransparent(HDC hDC, int x, int y, int btx, int bty)
{
	BOOL bValid = width > 0 && height > 0;

    if (bValid == TRUE) {

		if (!mask)
			bValid = CreateMask(hDC, 0xffffffff, btx, bty);
		
		// draw using the default transparent color
		// or the color set by CreateMask
		DrawTransparent(hDC, x, y, clrTransparent);
	}

	return(bValid);
}

/**
* @name	DrawTransparent
* @desc	Draw bitmap transparent. This creates a mask
*		and uses the specified transparent color
*/
BOOL
BBitmap::DrawTransparent(HDC hDC, int x, int y, COLORREF clrTrans)
{
	BOOL bValid = width > 0 && height > 0;
	
    if (bValid == TRUE) {
		if (!mask)
			CreateMask(hDC,clrTrans);
		
		int dx = GetWidth();
		int dy = GetHeight();
		
		// Create a memory DC to which to draw.
		HDC hdcOffScr = ::CreateCompatibleDC(hDC);
		// Create a bitmap for the off-screen DC that is really
		// color-compatible with the destination DC.
		HBITMAP hbmOffScr = ::CreateBitmap(dx, dy, 
			(BYTE)GetDeviceCaps(hDC, PLANES),
			(BYTE)GetDeviceCaps(hDC, BITSPIXEL),
			NULL);
		// Select the buffer bitmap into the off-screen DC.
		HBITMAP hbmOldOffScr = (HBITMAP)::SelectObject(hdcOffScr, hbmOffScr);
		
		// Copy the image of the destination rectangle to the
		// off-screen buffer DC, so we can play with it.
		::BitBlt(hdcOffScr, 0, 0, dx, dy, hDC, x, y, SRCCOPY);
		
		// Create a memory DC for the source image.
		HDC hdcImage = ::CreateCompatibleDC(hDC); 
		HBITMAP hbmOldImage = (HBITMAP)::SelectObject(hdcImage, m_hObject);
		
		// Create a memory DC for the mask.
		HDC hdcMask = ::CreateCompatibleDC(hDC);
		HBITMAP hbmOldMask = (HBITMAP)::SelectObject(hdcMask, mask);
		
		DWORD DSx = SRCINVERT,DSa = SRCAND;
		
		// XOR the image with the destination.
		::SetBkColor(hdcOffScr,rgb_white);
		::BitBlt(hdcOffScr, 0, 0, dx, dy ,hdcImage, 0, 0, DSx);
		// AND the destination with the mask.
		::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcMask, 0,0, DSa);
		// XOR the destination with the image again.
		::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcImage, 0, 0, DSx);
		
		// Copy the resultant image back to the screen DC.
		::BitBlt(hDC, x, y, dx, dy, hdcOffScr, 0, 0, SRCCOPY);
		// Tidy up.
		::SelectObject(hdcOffScr, hbmOldOffScr);
		::SelectObject(hdcImage, hbmOldImage);
		::SelectObject(hdcMask, hbmOldMask);
		::DeleteObject(hbmOffScr);
		::DeleteDC(hdcOffScr);
		::DeleteDC(hdcImage);
		::DeleteDC(hdcMask);
	}
	return(bValid);
}

/**
* @name	DrawTiles
* @desc	This method draws the bitmap tiled inside the 
*		specified rectangle, starting at pos 0,0
*/

BOOL
BBitmap::DrawTiles(HDC hDC, int nWidth, int nHeight)
{
	BOOL bValid = width > 0 && height > 0;
	
	if(bValid == FALSE)
		bValid = Update();

    if (bValid == TRUE) {

		for(int y=0;y < nHeight; y+=height) {
			for(int x=0;x < nWidth; x+=width) {

				Draw(hDC, x, y);

			}
		}
	}
	return(bValid);
}


// private

BOOL
BBitmap::CreateMask(HDC hDC, COLORREF clrTrans, int btx, int bty)
{
	BOOL bValid = width > 0 && height > 0;

    if (bValid == TRUE) {
		
		if (mask) 
			::DeleteObject(mask);
		
		HDC hdcMask = ::CreateCompatibleDC(hDC);
		HDC hdcImage = ::CreateCompatibleDC(hDC);
		
		// Create a monochrome bitmap for the mask.
		mask = ::CreateBitmap(GetWidth(),
			GetHeight(),
			1,
			1,
			NULL);
		
		// Select the mask into its dc.
		HBITMAP hOldMask = (HBITMAP)::SelectObject(hdcMask, mask);
		// Select the image into its dc.
		HBITMAP hOldImage = (HBITMAP)::SelectObject(hdcImage, m_hObject);
		
		// Take the top-left pixel as transparency color
		if(clrTrans != clrTransparent) {
			if (clrTrans == 0xffffffff) {
				clrTransparent = ::GetPixel(hdcImage, btx, bty);
			} else {
				clrTransparent = clrTrans;
			}
		}
		::SetBkColor(hdcImage, clrTransparent);
		
		// Create the mask.
		::BitBlt(hdcMask,
			0, 0,
			width, height,
			hdcImage,
			0, 0,
			SRCCOPY);
		
		// clean up
		::SelectObject(hdcMask, hOldMask);
		::SelectObject(hdcImage, hOldImage);
		::DeleteDC(hdcMask);
		::DeleteDC(hdcImage);
	}

	return(bValid);
}
