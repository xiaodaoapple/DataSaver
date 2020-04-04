// CBmpButton.cpp : implementation file
//

#include "stdafx.h"
#include "BmpButton.h"

//使用CImage
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RGB_BUTTON_BLACK    (GetSysColor(COLOR_WINDOWFRAME))
#define RGB_BUTTON_WHITE    (GetSysColor(COLOR_BTNHIGHLIGHT))
#define RGB_BUTTON_LIGHT    (GetSysColor(COLOR_BTNFACE))
#define RGB_BUTTON_DARK     (GetSysColor(COLOR_BTNSHADOW))

#define BORDER_CLEAR		0x0000L
#define BORDER_PUSHED		0x0001L
#define BORDER_NONPUSHED	0x0002L

/////////////////////////////////////////////////////////////////////////////
// CBmpButton
IMPLEMENT_DYNAMIC(CBmpButton, CButton)

CBmpButton::CBmpButton()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_bMouseCaptured=FALSE;
	m_bLButtonDown=FALSE;
	m_bHasFocus=FALSE;
	m_bDisabled=FALSE;

	m_nBorder=BORDER_CLEAR;
	m_bRaised=FALSE;
	m_ChangeCursor = TRUE;
	m_ToolTip.m_hWnd = NULL;

	// 默认是正常按钮
	m_uButtonStyle = BUTTON_NORMAL;
}

CBmpButton::~CBmpButton()
{

}


BEGIN_MESSAGE_MAP(CBmpButton, CButton)
	//{{AFX_MSG_MAP(CBmpButton)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_ENABLE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpButton message handlers

void CBmpButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags,point);
	if (!m_bMouseCaptured || GetCapture()!=this || m_nBorder==BORDER_CLEAR)
	{
		SetCapture();
		m_bMouseCaptured=TRUE;
		OnMouseEnter(nFlags,point);
	} else
	{
		CRect rc;
		this->GetClientRect(&rc);
		if (!rc.PtInRect(point))
		{
			OnMouseLeave(nFlags,point);
			m_bMouseCaptured=FALSE;
			ReleaseCapture();
		}
	}
}

void CBmpButton::OnKillFocus(CWnd* pNewWnd) 
{
	if(m_uButtonStyle == BUTTON_TAB || m_uButtonStyle == BUTTON_TOOLBAR)
	{

	}
	else
	{
		m_nBorder=BORDER_CLEAR;
		m_bHasFocus=FALSE;
		m_bRaised=FALSE;
	}
	CButton::OnKillFocus(pNewWnd);
	Invalidate();
	UpdateWindow();
}

void CBmpButton::OnSetFocus(CWnd* pOldWnd) 
{
	m_nBorder=m_bLButtonDown?BORDER_PUSHED:BORDER_NONPUSHED;
	m_bHasFocus=TRUE;
	m_bRaised=TRUE;
	CButton::OnSetFocus(pOldWnd);
	Invalidate();
	UpdateWindow();
}

void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int	 iSaveDC;
	CDC* pDC;
	CBrush brush(RGB_BUTTON_LIGHT);
	CRect rc;
	CBitmap*	pBitmap=NULL;
	
	pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	VERIFY(pDC);
	rc.CopyRect(&lpDrawItemStruct->rcItem);
	iSaveDC=pDC->SaveDC();
	
	pDC->SetBkMode(TRANSPARENT);
	
	if(m_uButtonStyle == BUTTON_TAB)
	{
		// 自己修改的 我们只根据是否有焦点来绘制
		if(m_bHasFocus)
		{
			pBitmap=&m_bitmapDown;
		}
		else
		{
			pBitmap=&m_bitmapFocus;
		}
	}
	else
	{
		if (m_bHasFocus)
		{
			pBitmap=&m_bitmapFocus;
			if (pBitmap->m_hObject==NULL) pBitmap=&m_bitmap; //Simulate some bitmap;
		}
		else
		{
			pDC->SetTextColor(RGB_BUTTON_BLACK);
			pBitmap=&m_bitmap;
		}

		if (m_bRaised)
		{
			pBitmap=&m_bitmapFocus;
			if (pBitmap->m_hObject==NULL) pBitmap=&m_bitmap; //Simulate some bitmap;
		} 

		if(m_bHover)
		{
			pBitmap=&m_bitmapHover;
			if (pBitmap->m_hObject==NULL) pBitmap=&m_bitmap; //Simulate some bitmap;
		}

		m_bDisabled=(::GetWindowLong(m_hWnd,GWL_STYLE) & WS_DISABLED);
		if (m_bDisabled)
		{
			pBitmap=&m_bitmapDisabled;
		}
		else 
		{
			if (m_bLButtonDown)
			{
				pBitmap=&m_bitmapDown;
				if (pBitmap->m_hObject==NULL) pBitmap=&m_bitmap; //Simulate some bitmap;
			}
		}
	}
	
	
	if (pBitmap->m_hObject)
		{
			CRect rcBitmap(rc);
			DrawBitmap(pDC,rcBitmap,pBitmap);
		}
	
	pDC->RestoreDC(iSaveDC);
}

void	CBmpButton::OnMouseEnter(UINT nFlags,CPoint point)
{
	m_bLButtonDown=(nFlags & MK_LBUTTON);
	m_nBorder=m_bLButtonDown?BORDER_PUSHED:BORDER_NONPUSHED;
	//if (m_bLButtonDown)
	//{
	m_bRaised=TRUE;
	Invalidate();
	UpdateWindow();
}

void	CBmpButton::OnMouseLeave(UINT nFlags,CPoint point)
{
	if(m_uButtonStyle == BUTTON_TAB || m_uButtonStyle == BUTTON_TOOLBAR)
	{
		// 如果鼠标按下了 并且离开了，那么我们产生鼠标单击消息
		if(m_bLButtonDown == TRUE)
		{
			GetParent()->SendMessage(WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),
				(LPARAM) m_hWnd);
		}

	}
	m_nBorder=BORDER_CLEAR;
	m_bLButtonDown=FALSE;
	m_bRaised=FALSE;
	Invalidate();
	UpdateWindow();
}

void CBmpButton::Draw3DBorder(CDC* pDC,CRect rc,UINT nOptions)
{
	switch (nOptions)
	{
	case BORDER_CLEAR:
						pDC->Draw3dRect(rc,RGB_BUTTON_LIGHT,RGB_BUTTON_LIGHT);
						break;
	case BORDER_PUSHED:
						pDC->Draw3dRect(rc,RGB_BUTTON_DARK,RGB_BUTTON_WHITE);
						break;
	case BORDER_NONPUSHED:
						pDC->Draw3dRect(rc,RGB_BUTTON_WHITE,RGB_BUTTON_DARK);
						break;
	default:			break;
	}
}

void CBmpButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLButtonDown=TRUE;
	m_bRaised=FALSE;
	if (GetFocus()!=this)
	{
		this->SetFocus();
		return;
	}
	m_nBorder=BORDER_PUSHED;

	CButton::OnLButtonDown(nFlags,point);
	Invalidate();
	UpdateWindow();
}


void CBmpButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLButtonDown=FALSE;
	m_bRaised=TRUE;
	if (GetFocus()!=this)
	{
		
		this->SetFocus();
	
	} else
	{
		m_nBorder=BORDER_NONPUSHED;
		Invalidate();
		UpdateWindow();
	}
	
	GetParent()->SendMessage(WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),
				(LPARAM) m_hWnd);
}

void CBmpButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_bLButtonDown=TRUE;
	if (GetFocus()!=this)
	{
		this->SetFocus();
		return;
	}
	m_nBorder=BORDER_PUSHED;

	Invalidate();
	UpdateWindow();
	CButton::OnLButtonDblClk(nFlags,point);	
	m_nBorder=BORDER_CLEAR;
}

int CBmpButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	m_bDisabled=(lpCreateStruct->style & WS_DISABLED);
	return 0;
}

void CBmpButton::OnEnable(BOOL bEnable) 
{
	m_bDisabled=!bEnable;
	CButton::OnEnable(bEnable);
}

// 从资源中加载位图
BOOL CBmpButton::LoadAllBitmaps(UINT nBitmap,LPCTSTR lpType,UINT nCount)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nBitmap),lpType); 

	CImage  m_Image;
	HBITMAP hBitmap;
	if(hRsrc == NULL)
	{
		return FALSE;
	}

	DWORD dwLen = SizeofResource(hInst, hRsrc);

	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);

	if (!lpRsrc)
	{
		return FALSE;
	}

	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, dwLen);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,dwLen);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	CBitmap src;

	m_Image.Load(pstm);
	hBitmap = m_Image.Detach();
	src.Attach(hBitmap); //关联位图对象
	GlobalUnlock(m_hMem);
	pstm->Release();

	GlobalFree(m_hMem);
	FreeResource(lpRsrc);
	return LoadAllBitmaps(src, nCount);
}


BOOL CBmpButton::LoadBitmaps(UINT nBitmap, UINT nBitmapDown, UINT nBitmapFocus, UINT nBitmapHover, UINT nBitmapDisabled)
{
	return LoadBitmaps(MAKEINTRESOURCE(nBitmap),
						MAKEINTRESOURCE(nBitmapFocus),
						MAKEINTRESOURCE(nBitmapHover),
						MAKEINTRESOURCE(nBitmapDown),
						MAKEINTRESOURCE(nBitmapDisabled));
}

void CBmpButton::DrawBitmap(CDC * pDC, CRect rc,CBitmap * pBitmap)
{
	//Centers a bitmap in a given rectangle
	//If necessary clips the bitmap if outfits the rc
	CDC memDC;
	CBitmap* pOld=NULL;
	memDC.CreateCompatibleDC(pDC);

	ASSERT(pBitmap->m_hObject!=NULL);
	//pBitmap->GetBitmap(&bmpInfo);
	
	pOld=memDC.SelectObject((CBitmap*) pBitmap);
	if (pOld==NULL) return; //Destructors will clean up

	pDC->BitBlt(rc.left,rc.top,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);
	
	memDC.SelectObject(pOld);	
}

BOOL CBmpButton::LoadBitmaps(LPCSTR lpszBitmap, LPCSTR lpszBitmapDown, LPCSTR lpszBitmapFocus, LPCSTR lpszBitmapHover, LPCSTR lpszBitmapDisabled)
{
	//Delete old ones
	m_bitmap.DeleteObject();
	m_bitmapDown.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapHover.DeleteObject();
	m_bitmapDisabled.DeleteObject();
	
	if (!m_bitmap.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to Load First bitmap of CButton\n");
		return FALSE;
	}
	BITMAP   bmpinfo;   
	m_bitmap.GetBitmap(&bmpinfo);   
	int   bmpWidth   =   bmpinfo.bmWidth ;//!!attention!!
	int   bmpHeight   =   bmpinfo.bmHeight;   


	SetWindowPos( NULL, 0,0, bmpWidth,bmpHeight,SWP_NOMOVE   |SWP_NOOWNERZORDER   );
	BOOL bAllLoaded=TRUE;
	if (lpszBitmapDown!=NULL)
	{
		if (!m_bitmapDown.LoadBitmap(lpszBitmapDown))
		{
			TRACE0("Failed to Load First bitmap of CButton\n");
			return bAllLoaded=FALSE;
		}
	}

	if (lpszBitmapFocus!=NULL)
	{
		if (!m_bitmapFocus.LoadBitmap(lpszBitmapFocus))
		{
			TRACE0("Failed to Load First bitmap of CButton\n");
			return bAllLoaded=FALSE;
		}
	}
	
	if (lpszBitmapHover!=NULL)
	{
		if (!m_bitmapHover.LoadBitmap(lpszBitmapHover))
		{
			TRACE0("Failed to Load First bitmap of CButton\n");
			return bAllLoaded=FALSE;
		}
	}

	if (lpszBitmapDisabled!=NULL)
	{
		if (!m_bitmapDisabled.LoadBitmap(lpszBitmapDisabled))
		{
			TRACE0("Failed to Load bitmap of CButton\n");
			return bAllLoaded=FALSE;
		}
	}

	return bAllLoaded;
}



LRESULT CBmpButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bHover=TRUE;
	if(m_uButtonStyle == BUTTON_TAB)
	{
		if (m_bHasFocus == TRUE) // 如果有焦点，则说明TAB选中状态
		{
			m_bHasFocus = TRUE;
			m_bHover = FALSE;
		}
	}
	Invalidate();
	return S_OK;
}


LRESULT CBmpButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	Invalidate();
	m_bTracking = FALSE;
	m_bHover=FALSE;
	return 0;
}

BOOL CBmpButton::LoadAllBitmaps( UINT nBitmap, UINT nCount )
{
	CBitmap  src;
	src.LoadBitmap(nBitmap);

	return LoadAllBitmaps(src, nCount);
}

BOOL CBmpButton::LoadAllBitmaps( LPCSTR lpszFileName, UINT nCount )
{
	CBitmap  src;
	HBITMAP hbm = (HBITMAP) ::LoadImage (NULL, lpszFileName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
	{
		return FALSE;
	}
	src.Attach(hbm);

	return LoadAllBitmaps(src, nCount);
}

BOOL CBmpButton::LoadAllBitmaps( CBitmap& bmBitmap, UINT nCount )
{
	m_bitmap.DeleteObject();
	m_bitmapDown.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapHover.DeleteObject();
	m_bitmapDisabled.DeleteObject();
	BITMAP   bmpinfo;   
	bmBitmap.GetBitmap(&bmpinfo);   
	int   bmpWidth   =   bmpinfo.bmWidth  / nCount;//!!attention!!
	int   bmpHeight   =   bmpinfo.bmHeight;   
	SetWindowPos( NULL, 0,0, bmpWidth,bmpHeight,SWP_NOMOVE   |SWP_NOOWNERZORDER   );

	CRect	rcSubRect = CRect(0, 0, bmpWidth, bmpHeight);

	UINT nIndex = 0;
	rcSubRect.MoveToX(bmpWidth*nIndex);
	SubBitmap(m_bitmap, bmBitmap, rcSubRect);

	if(++nIndex < nCount)
	{
		rcSubRect.MoveToX(bmpWidth*nIndex);
	}
	else
	{
		rcSubRect.MoveToX(0);
	}
	SubBitmap(m_bitmapDown, bmBitmap, rcSubRect);

	if(++nIndex < nCount)
	{
		rcSubRect.MoveToX(bmpWidth*nIndex);
	}
	else
	{
		rcSubRect.MoveToX(0);
	}
	SubBitmap(m_bitmapFocus, bmBitmap, rcSubRect);

	if(++nIndex < nCount)
	{
		rcSubRect.MoveToX(bmpWidth*nIndex);
	}
	else
	{
		rcSubRect.MoveToX(0);
	}
	SubBitmap(m_bitmapHover, bmBitmap, rcSubRect);

	if(++nIndex < nCount)
	{
		rcSubRect.MoveToX(bmpWidth*nIndex);
	}
	else
	{
		rcSubRect.MoveToX(0);
	}
	SubBitmap(m_bitmapDisabled, bmBitmap, rcSubRect);

	return TRUE;
}

void CBmpButton::SubBitmap(CBitmap &bDst, const CBitmap &bSrc, const CRect& rRect)
{  
	CDC dcSrc, dcDst;

	//CClientDC clientDC= CClientDC(NULL);
	CClientDC clientDC(NULL);
	bDst.CreateCompatibleBitmap( &clientDC,
		rRect.Width(), rRect.Height() );

	dcSrc.CreateCompatibleDC( NULL );
	dcDst.CreateCompatibleDC( NULL );
	dcSrc.SelectObject( bSrc );
	dcDst.SelectObject( bDst );

	dcDst.StretchBlt( 0, 0, rRect.Width(), rRect.Height(), &dcSrc, 
		rRect.left, rRect.top, rRect.Width(), rRect.Height(), SRCCOPY );
}

BOOL CBmpButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*if ( m_ChangeCursor )
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);*/
	return TRUE;
}

void CBmpButton::SetHandCursor( BOOL bHandCursor )
{
	m_ChangeCursor = bHandCursor;
}


void CBmpButton::SetTooltipText( LPCSTR szText, BOOL bActivate )
{
	if ( CString(szText).IsEmpty() )return;
	if(m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this);
		m_ToolTip.Activate(FALSE);
	}

	if ( m_ToolTip.GetToolCount() == 0 )
	{
		CRect rButton;
		GetClientRect( rButton );
		m_ToolTip.AddTool(this, (LPCTSTR)szText, rButton, 1);
	}
	m_ToolTip.UpdateTipText((LPCTSTR)szText, this, 1);
	m_ToolTip.Activate( bActivate );
}

void CBmpButton::ActivateTooltip( BOOL bActivate )
{
	if ( m_ToolTip.GetToolCount() == 0 )
	{
		return ;
	}
	m_ToolTip.Activate( bActivate );
}
BOOL CBmpButton::PreTranslateMessage(MSG* pMsg)
{
	if(IsWindow(m_ToolTip.m_hWnd))
	{
		m_ToolTip.RelayEvent( pMsg );

	}

	return CButton::PreTranslateMessage(pMsg);
}

void CBmpButton::SetButtonStyleEx(UINT uStyle)
{
	m_uButtonStyle = uStyle;
}

void CBmpButton::SetFocusStyle()
{
	if(m_uButtonStyle == BUTTON_TAB)
	{
		m_bHasFocus = TRUE;
	}
	else if(m_uButtonStyle == BUTTON_TOOLBAR)
	{
		m_nBorder=m_bLButtonDown?BORDER_PUSHED:BORDER_NONPUSHED;
		m_bHasFocus=TRUE;
		m_bRaised=TRUE;
	}
	
	Invalidate();
	UpdateWindow();
}

void CBmpButton::SetUnfocusStyle()
{
	if(m_uButtonStyle == BUTTON_TAB)
	{
		m_bHasFocus=FALSE;
	}
	else if(m_uButtonStyle == BUTTON_TOOLBAR)
	{
		m_nBorder=BORDER_CLEAR;
		m_bHasFocus=FALSE;
		m_bRaised=FALSE;
		
	}
	Invalidate();
	UpdateWindow();
}
