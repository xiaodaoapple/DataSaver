// About.cpp : 实现文件
//

#include "stdafx.h"
#include "DataSaver.h"
#include "About.h"
#include "afxdialogex.h"


// About 对话框

IMPLEMENT_DYNAMIC(About, CDialogEx)

About::About(CWnd* pParent /*=NULL*/)
: CBitmapDialog(About::IDD, pParent)
{

}

About::~About()
{
}

void About::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//按钮和图片进行绑定
	DDX_Control(pDX, IDC_CLOSE, bmpClose);
}


BEGIN_MESSAGE_MAP(About, CBitmapDialog)
	ON_BN_CLICKED(IDC_CLOSE, &About::OnBnClickedClose)
END_MESSAGE_MAP()


// About 消息处理程序

BOOL About::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();
	// TODO:  开始进行自定义皮肤
	if (!SetBitmap(IDB_ABOUT_WINDOW, true, RGB(128, 0, 0)))
	{
		AfxMessageBox("加载皮肤失败！");
		return FALSE;
	}
	SetStaticTransparent(TRUE);
	SetClickAnywhereMove(TRUE);
	bmpClose.LoadAllBitmaps(IDB_CLOSEBTN, 4);
	return TRUE;
}
void About::OnBnClickedClose()
{
	// TODO:  在此添加控件通知处理程序代码
	CBitmapDialog::OnOK();
}
