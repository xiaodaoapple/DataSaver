#if !defined(AFX_BITMAPDIALOG_H__A76F9E74_DF43_11D4_AE27_4854E828E6FD__INCLUDED_)
#define AFX_BITMAPDIALOG_H__A76F9E74_DF43_11D4_AE27_4854E828E6FD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BitmapDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapDialog dialog

class CBitmapDialog : public CDialog
{
// Construction
public:
	// Common constructor
	CBitmapDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	// Destructor (just release the bitmap)
	~CBitmapDialog ();

	BOOL SetBitmap(UINT nBitmapID, BOOL bTransparent = FALSE, COLORREF crTransColor = RGB(-1, -1, -1));
	BOOL SetBitmap(LPCTSTR szFileName, BOOL bTransparent = FALSE, COLORREF crTransColor = RGB(-1, -1, -1));
	BOOL SetBitmap(CBitmap *pBitmap, BOOL bTransparent = FALSE, COLORREF crTransColor = RGB(-1, -1, -1));

	// Static control transparency
	void SetStaticTransparent (BOOL bTransparent) { m_bStaticTransparent = bTransparent; }
	BOOL GetStaticTransparent () { return m_bStaticTransparent; }

	// Clicking anywhere moves
    void SetClickAnywhereMove (BOOL bMove) { m_bClickAnywhereMove = bMove; }
	BOOL GetClickAnywhereMove () { return m_bClickAnywhereMove; }

protected:
	BOOL CopyBitmapFrom (CBitmap *pBitmap);		// Copy of user defined
	void ReleaseBitmap ();
	CBitmap *GetBitmap () { return m_bmBitmap; }

	HRGN CreateRgnFromBitmap(HBITMAP hBitmap, COLORREF cTransColor= RGB(255, 255, 255));


// Dialog Data
	//{{AFX_DATA(CBitmapDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapDialog)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	MakeWindowRgn ();

	// Transparency
	BOOL		m_bTransparent;
	BOOL		m_bStaticTransparent;
	HBRUSH		m_brushHollow;
	COLORREF	m_colTrans;

	// Clicking anywhere moves
	BOOL		m_bClickAnywhereMove;

	// Bitmap and its DC
	BOOL	m_bBitmapCreated, m_bBitmapExists;
	CBitmap	*m_bmBitmap;
	HBRUSH m_hBrush;

	// Generated message map functions
	//{{AFX_MSG(CBitmapDialog)
	afx_msg BOOL OnEraseBkgnd (CDC *pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPDIALOG_H__A76F9E74_DF43_11D4_AE27_4854E828E6FD__INCLUDED_)
