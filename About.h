#pragma once
#include "BitmapDialog.h"
#include "BmpButton.h"

// About �Ի���

class About : public CBitmapDialog
{
	DECLARE_DYNAMIC(About)

public:
	About(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~About();

// �Ի�������
	enum { IDD = IDD_ABOUT_WINDOW };
	//�Զ���ͼƬ��ť
	CBmpButton bmpClose; //�رհ�ť


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
};
