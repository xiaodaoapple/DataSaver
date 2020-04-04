// BitmapDialog.cpp : implementation file
//
// Created by David Forrester, January 1, 2001
// Feel free to use this code in any way you want.

#include "stdafx.h"
#include "BitmapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBitmapDialog dialog

//----------------------------------------------------------------
// CBitmapDialog :: CBitmapDialog - constructor for CBitmapDialog
//
// Parameters:
//  lpszTemplateName - the name of the dialog template resource
//  nIDTemplate - the ID of the dialog template resource
//  pParentWnd - the parent window
//
// You can leave any or all of the below NULL or 0 to not use it
//  lpszResourceName - the name of the bitmap resource to load
//  nIDResource - the ID of the bitmap resource to load
//  lpszFilename - the filename of the bitmap file to load
//  pBitmap - the bitmap to use (user is responsible for handling the bitmap)

CBitmapDialog::CBitmapDialog( UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/ )
: CDialog(nIDTemplate, pParentWnd)
{
	m_bTransparent = FALSE;			// No transparency
	m_bStaticTransparent = TRUE;	// Static controls are transparent
	m_bBitmapCreated = FALSE;		// Don't automatically release the bitmap
	m_bBitmapExists = FALSE;		// IS there a bitmap?
	m_bClickAnywhereMove = FALSE;	// Clicking anywhere moves

	// Create a hollow brush used in making static controls transparent
	m_brushHollow = (HBRUSH) GetStockObject (HOLLOW_BRUSH);
	m_hBrush = ::CreateSolidBrush(RGB(255,255,255));
}

CBitmapDialog::~CBitmapDialog()
{
	ReleaseBitmap ();
}

BOOL CBitmapDialog::SetBitmap( UINT nBitmapID, BOOL bTransparent /*= FALSE*/, COLORREF crTransColor /*= RGB(-1, -1, -1)*/ )
{
	// Release the bitmap if it was created
	ReleaseBitmap ();

	m_bTransparent = bTransparent;
	m_colTrans = crTransColor;

	// Load the bitmap
	m_bmBitmap = new CBitmap;
	if (!m_bmBitmap->LoadBitmap (nBitmapID))
		return FALSE;

	// Automatically delete the object
	m_bBitmapCreated = TRUE;
	m_bBitmapExists = TRUE;

	// Make the window transparent if needed
	MakeWindowRgn ();

	return TRUE;
}

BOOL CBitmapDialog::SetBitmap( LPCTSTR szFileName, BOOL bTransparent /*= FALSE*/, COLORREF crTransColor /*= RGB(-1, -1, -1)*/ )
{
	// Release the bitmap if it was created
	ReleaseBitmap ();

	m_bTransparent = bTransparent;
	m_colTrans = crTransColor;

	// Load the bitmap from a file
	HBITMAP hbm = (HBITMAP) LoadImage (NULL, szFileName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL) return FALSE;

	// Get the CBitmap object
	CopyBitmapFrom (CBitmap::FromHandle(hbm));
	//m_bmBitmap = CBitmap::FromHandle (hbm);

	return TRUE;
}

BOOL CBitmapDialog::SetBitmap( CBitmap *pBitmap, BOOL bTransparent /*= FALSE*/, COLORREF crTransColor /*= RGB(-1, -1, -1)*/ )
{
	// Release the bitmap if it was created
	ReleaseBitmap ();

	m_bTransparent = bTransparent;
	m_colTrans = crTransColor;

	// Set the bitmap
	m_bmBitmap = pBitmap;

	// The bitmap exists, but was not created
	m_bBitmapExists = TRUE;

	// Make the window transparent if needed
	MakeWindowRgn ();

	return TRUE;
}
BOOL CBitmapDialog::CopyBitmapFrom (CBitmap *pBitmap)
{
	// Release the bitmap if it was created
	ReleaseBitmap ();

	// Get the bitmap information
	BITMAP bmSrc;
	pBitmap->GetBitmap (&bmSrc);

	// Get a DC to the source bitmap
	CDC dcSrc;
	dcSrc.CreateCompatibleDC (NULL);
	CBitmap *bmSrcOld = dcSrc.SelectObject (pBitmap);

	// Create a new bitmap
	m_bmBitmap = new CBitmap;
	if (!m_bmBitmap->CreateCompatibleBitmap (&dcSrc, bmSrc.bmWidth, bmSrc.bmHeight))
		return FALSE;

	// Get a DC to the destination bitmap
	CDC dcDst;
	dcDst.CreateCompatibleDC (NULL);
	CBitmap *bmDstOld = dcDst.SelectObject (m_bmBitmap);

	// Copy the bitmap
	dcDst.BitBlt (0, 0, bmSrc.bmWidth, bmSrc.bmHeight, &dcSrc, 0, 0, SRCCOPY);

	// Release
	dcSrc.SelectObject (bmSrcOld);
	dcDst.SelectObject (bmDstOld);
	dcSrc.DeleteDC ();
	dcDst.DeleteDC ();

	// Automatically delete the object
	m_bBitmapCreated = TRUE;
	m_bBitmapExists = TRUE;

	// Make the window transparent if needed
	MakeWindowRgn ();

	return TRUE;
}

//----------------------------------------------------------------
// CBitmapDialog :: ReleaseBitmap - releases a bitmap if it was
//  created using LoadBitmap or CopyBitmapFrom.

void CBitmapDialog::ReleaseBitmap ()
{
	// Make sure that the bitmap was created using LoadBitmap or CopyBitmapFrom
	if (m_bBitmapCreated)
	{
		// Delete the bitmap
		m_bmBitmap->DeleteObject ();
		delete m_bmBitmap;

		// The bitmap has not been created yet
		m_bBitmapCreated = FALSE;
	}

	m_bBitmapExists = FALSE;
}

//-----------------------------------------------------------------------------------------------------
// 名称: createRgn() 
// 参数: HBITMAP _hBitMap: 一个BITMAP的句柄
// 返回: HRGN句柄
// 描述: 根据一张BMP图片句柄创建一个Rgn句柄，用于设置窗口外观。
//-----------------------------------------------------------------------------------------------------
//HRGN WinImage::createRgn(HBITMAP _hBitMap)

HRGN CBitmapDialog::CreateRgnFromBitmap( HBITMAP hBmp, COLORREF cTransColor)
{
	// get image properties
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( bi, 0, sizeof(BITMAPINFO) + 8 );
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// set window size
	// create temporary dc
	HDC dc = CreateIC( "DISPLAY",NULL,NULL,NULL );
	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
	// allocate memory for color table
	if ( bi->bmiHeader.biBitCount == 8 )
	{
		// actually color table should be appended to this header(BITMAPINFO),
		// so we have to reallocate and copy it
		LPBITMAPINFO old_bi = bi;
		// 255 - because there is one in BITMAPINFOHEADER
		bi = (LPBITMAPINFO)new char[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD) ];
		memcpy( bi, old_bi, sizeof(BITMAPINFO) );
		// release old header
		delete old_bi;
	}
	// get bitmap info header
	BITMAPINFOHEADER& bih = bi->bmiHeader;
	// get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
	// fill bits buffer
	res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
	DeleteDC( dc );

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );
	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)&cTransColor;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if ( bih.biBitCount == 16 )
	{
		// for 16 bit
		cTransColor = ((DWORD)(pClr[0] & 0xf8) >> 3) |
			((DWORD)(pClr[1] & 0xfc) << 3) |
			((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
		//		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
		//				((DWORD)(pClr[1] & 0xf8) << 2) |
		//				((DWORD)(pClr[2] & 0xf8) << 7);
	}

	const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
	// to region data buffer

	// BitPerPixel
	BYTE	Bpp = BYTE(bih.biBitCount >> 3);				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
	// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		first = 0;								// left position of current scan line
	// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask = false;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = 
		(RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RGNDATAHEADER_SIZE;
	pRgnData->iType		= RDH_RECTANGLES;

	INT i = 0, j = 0;
	for ( i = 0; i < bih.biHeight; i++ )
	{
		for ( j = 0; j < bih.biWidth; j++ )
		{
			// get color
			switch ( bih.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] != cTransColor);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)cTransColor);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != cTransColor);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != cTransColor);
				break;
			default:
				ismask = FALSE;
				ASSERT(FALSE);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( !ismask )
				{
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
					// if buffer full reallocate it with more room
					if ( pRgnData->nCount >= dwRectsCount )
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
						// copy current region data to it
						memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
						// delte old region data buffer
						delete pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if ( ismask )		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if ( wasfirst && ismask )
		{
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
			// if buffer full reallocate it with more room
			if ( pRgnData->nCount >= dwRectsCount )
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
				// copy current region data to it
				memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
				// delte old region data buffer
				delete pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}

		pColor -= dwLineBackLen;
	}
	// release image data
	delete pBits;
	delete bi;

	// create region
	HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	delete pRgnData;

	return hRgn;
}

//----------------------------------------------------------------
// CBitmapDialog :: MakeWindowRgn - makes a window region from
//  the bitmap and uses it if on transparent mode.

void CBitmapDialog::MakeWindowRgn ()
{
	CRgn rgn;
	BITMAP bm;
	m_bmBitmap->GetBitmap (&bm);
	int width = bm.bmWidth;
	int height = bm.bmHeight;
	if (!m_bTransparent)
	{
		// Set the window region to the full window
		CRect rc;
		GetWindowRect (rc);
		rgn.CreateRectRgn (0, 0, width, height);
	}
	else
	{
		rgn.Attach((HGDIOBJ)CreateRgnFromBitmap((HBITMAP)m_bmBitmap->GetSafeHandle(), m_colTrans));
		SetWindowRgn (rgn, TRUE);
	}

	SetWindowPos( NULL, 0,0, width,height,SWP_NOMOVE   |SWP_NOOWNERZORDER   );
}

BEGIN_MESSAGE_MAP(CBitmapDialog, CDialog)
	//{{AFX_MSG_MAP(CBitmapDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapDialog message handlers

//----------------------------------------------------------------
// CBitmapDialog :: OnEraseBkgnd - normally erases the background.
//  We override this function to replace it with window drawing
//  code.

BOOL CBitmapDialog::OnEraseBkgnd (CDC *pDC)
{
	//return FALSE;
	// If no bitmap is loaded, behave like a normal dialog box
	if (!m_bBitmapExists)
		return CDialog :: OnEraseBkgnd (pDC);

	// Get the client rectangle of the window
	CRect rc;
	GetClientRect (rc);

	// Get a DC for the bitmap
	CDC dcImage;
	dcImage.CreateCompatibleDC (pDC);
	CBitmap *pOldBitmap = dcImage.SelectObject (m_bmBitmap);

	// Get bitmap width and height
	BITMAP bm;
	m_bmBitmap->GetBitmap (&bm);

	//// Use the minimum width and height
	//int width = min (bm.bmWidth, rc.Width());
	//int height = min (bm.bmHeight, rc.Height());

	// Draw the bitmap as the window background
	pDC->BitBlt (0, 0, rc.Width(), rc.Height(), &dcImage, 0, 0, SRCCOPY);

	// Release
	dcImage.SelectObject (pOldBitmap);
	dcImage.DeleteDC ();

	// Return value: Nonzero if it erases the background.
	return TRUE;
}

//----------------------------------------------------------------
// CBitmapDialog :: OnLButtonDown - left mouse button is clicked
//  on the window

void CBitmapDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);

	// Fool windows into thinking that we clicked on the caption
	if (m_bClickAnywhereMove)
		PostMessage (WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
}

////----------------------------------------------------------------
//// CBitmapDialog :: OnCtlColor - set the colors for controls
//
//HBRUSH CBitmapDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
//{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//	
//	// TODO: Change any attributes of the DC here
//
//	// Make static controls transparent
//	if (m_bStaticTransparent && nCtlColor == CTLCOLOR_STATIC)
//	{
//		// Make sure that it's not a slider control
//		char lpszClassName[256];
//		GetClassName (pWnd->m_hWnd, lpszClassName, 255);
//		if (strcmp (lpszClassName, TRACKBAR_CLASS) == 0)
//			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//
//		pDC->SetBkMode (TRANSPARENT);
//		return m_brushHollow;
//	}
//	
//	// TODO: Return a different brush if the default is not desired
//	return hbr;
//}


void CBitmapDialog::OnPaint()
{
	CPaintDC	dc(this); // device context for painting
	if (m_bBitmapExists)
	{
		CDC			memDc;
		memDc.CreateCompatibleDC( &dc );
		memDc.SelectObject( m_bmBitmap );

		CRect rDialog;
		GetClientRect(&rDialog);

		dc.StretchBlt( rDialog.left, rDialog.top, 
			rDialog.Width(), rDialog.Height(), &memDc, 
			0, 0,rDialog.Width(), rDialog.Height(), SRCCOPY );
	}
}
