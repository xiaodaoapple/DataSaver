// About.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataSaver.h"
#include "About.h"
#include "afxdialogex.h"


// About �Ի���

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
	//��ť��ͼƬ���а�
	DDX_Control(pDX, IDC_CLOSE, bmpClose);
}


BEGIN_MESSAGE_MAP(About, CBitmapDialog)
	ON_BN_CLICKED(IDC_CLOSE, &About::OnBnClickedClose)
END_MESSAGE_MAP()


// About ��Ϣ�������

BOOL About::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();
	// TODO:  ��ʼ�����Զ���Ƥ��
	if (!SetBitmap(IDB_ABOUT_WINDOW, true, RGB(128, 0, 0)))
	{
		AfxMessageBox("����Ƥ��ʧ�ܣ�");
		return FALSE;
	}
	SetStaticTransparent(TRUE);
	SetClickAnywhereMove(TRUE);
	bmpClose.LoadAllBitmaps(IDB_CLOSEBTN, 4);
	return TRUE;
}
void About::OnBnClickedClose()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CBitmapDialog::OnOK();
}
