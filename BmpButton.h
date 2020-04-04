#if !defined(AFX_BMPBUTTON_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_)
#define AFX_BMPBUTTON_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrackLookButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TrackLookButton window

// 修改CBmpButton 支持 3中 位图按钮
#define BUTTON_NORMAL 0
#define BUTTON_TAB	  1
#define BUTTON_TOOLBAR 2

class CBmpButton : public CButton
{
	DECLARE_DYNAMIC(CBmpButton)

// Construction
public:
	CBmpButton();
	virtual ~CBmpButton();

public:
	BOOL LoadAllBitmaps(UINT nBitmap,LPCTSTR lpType,UINT nCount); // 从资源中加载位图
	BOOL LoadAllBitmaps(UINT nBitmap, UINT nCount);
	BOOL LoadAllBitmaps(LPCSTR lpszFileName, UINT nCount);
	BOOL LoadBitmaps(LPCSTR lpszBitmap,LPCSTR lpszBitmapDown=NULL,LPCSTR lpszBitmapFocus=NULL,  LPCSTR lpszBitmapHover=NULL, LPCSTR lpszBitmapDisabled=NULL);
	BOOL LoadBitmaps(UINT nBitmap,UINT nBitmapDown=0, UINT nBitmapFocus=0, UINT nBitmapHover=0,  UINT nBitmapDisabled=0);

	void SetHandCursor(BOOL bHandCursor);
	void SetTooltipText( LPCSTR szText, BOOL bActivate );
	void ActivateTooltip( BOOL bActivate );

	// 扩展的函数
	void SetFocusStyle(); // 设置按钮的风格为获取焦点时的风格
	void SetUnfocusStyle();  // 设置按钮的风格为失去焦点时的风格
	void SetButtonStyleEx(UINT uStyle);	// 设置扩展属性
	// Generated message map functions
protected:
	BOOL LoadAllBitmaps(CBitmap& bmBitmap, UINT nCount);
	void DrawBitmap(CDC* pDC,CRect rc,CBitmap* pBitmap);
	CBitmap m_bitmap;
	CBitmap m_bitmapFocus;
	CBitmap m_bitmapDown;
	CBitmap m_bitmapHover;
	CBitmap m_bitmapDisabled;
	BOOL m_ChangeCursor;
	
	void Draw3DBorder(CDC* pDC,CRect rc,UINT nOptions);
	void SubBitmap(CBitmap &bDst, const CBitmap &bSrc, const CRect& rRect);
	
	CToolTipCtrl	m_ToolTip;
	BOOL		m_bRaised;
	BOOL		m_bDisabled;
	BOOL		m_bHasFocus;

	BOOL		m_bHover;						// indicates if mouse is over the button
	BOOL		m_bTracking;
	BOOL		m_bLButtonDown;
	UINT		m_nBorder;
	BOOL		m_bMouseCaptured;

	// 扩展的属性
	UINT		m_uButtonStyle;
	//{{AFX_MSG(TrackLookButton)

	afx_msg void OnMouseEnter(UINT nFlags,CPoint point);
	afx_msg void OnMouseLeave(UINT nFlags,CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONEX_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_)
