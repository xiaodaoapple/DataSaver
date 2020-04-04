#pragma once
#include "BitmapDialog.h"
#include "BmpButton.h"

// About 对话框

class About : public CBitmapDialog
{
	DECLARE_DYNAMIC(About)

public:
	About(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~About();

// 对话框数据
	enum { IDD = IDD_ABOUT_WINDOW };
	//自定义图片按钮
	CBmpButton bmpClose; //关闭按钮


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
};
