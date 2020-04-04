
// DataSaverDlg.h : ͷ�ļ�
//

#pragma once
#include "BitmapDialog.h"
#include "BmpButton.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <map>
#include "NTFSFunction.h"
// CDataSaverDlg �Ի���
class CDataSaverDlg : public CBitmapDialog
{
// ����
public:
	CDataSaverDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DATASAVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCancel();
	//�Զ���ͼƬ��ť
	CBmpButton bmpClose; //�رհ�ť
	CBmpButton bmpNext;  //��һ����ť
	CBmpButton bmpAbout; //���ڰ�ť
	CBmpButton bmpBack;  //��һ����ť
	
	diskVolume diskPartiton[26]; //����26���̷����洢���ܴ��ڵ��̷��Ϳռ�Ƚϡ�

	int i_window ; //����windows״̬,���0Ӳ��ѡ�����1����ѡ�����2�ļ�ѡ��

	int i_fileSystem=0; //������Ҫչʾ���ļ�ϵͳ�� 0��NTFS�ļ�ϵͳ  1��ExFAT

	int i_diskSelect = -1; //������ѡ���Ӳ�̣�Ĭ��С��0
	CImageList imageListPartiton; //����Ϊ�����б���Ӵ���ͼ��
	CImageList imageListPartitonFolderFile; //����Ϊ�ļ��к��ʼ����ͼ��
	vector<partitionInfor> partitionVector; //������vector
	
	CListBox listDisk; //��ʾ���пɻָ���Ӳ�̵�list�ؼ�
	diskInfor d_disk[26]; //������̱���������������26��Ӳ��
	CListCtrl listPartition; //��ʾ���з�����listControl�ؼ�
	CTreeCtrl folderTree;  //�ļ��к��ļ�����״�б�
	CListCtrl fileList;   //��ʾ�ļ����б��

	CMenu menu_restore; //�����˵����������ָ��ļ�
	//�õ�ѡ��Ļָ��ķ���
	partitionInfor pif; //ѡ���Ҫ�ָ��ķ���
	CString s_driver; //��Ӳ�̵��ַ���

	unsigned int i_cu; //ÿ��������
	unsigned int i_MFT_begin; //MFT����ʼ��
	unsigned int i_PartitionBegin; //��������ʼ����

	UINT32 i_mftTreeChose; //�����״�б�ʱ�������NTFS�ļ�ϵͳ���õ���ѡ���ļ��е�MFT���.
	UINT32 i_FATCuTreeChoose;  //�����״�б�ʱ�������ExFAT�ļ�ϵͳ���õ���ѡ�е��ļ��е�Cu���롣

	//����4�����������Ҽ��˵��Ļָ����á�
	NTFSFile NTFSfile_restore; //��Ҫ�ָ���NTFS�ļ�
	NTFSDirectory NTFSdirectory_restore;//��Ҫ�ָ���NTFS�ļ���

	ExFatFile exFatFile_restore;  //��Ҫ�ָ���ExFAT�ļ�
	ExFatFolder exFatFolder_restore; //��Ҫ�ָ���ExFAT�ļ���

	//����MFT�ź��䣨���ļ��������ļ����Ķ�Ԫӳ�䣬��ֵ������MFT��
	map<UINT32, DirectoryMap> mftDirectory;

	NTFSFunction ns; //����NTFS�Ĺ��ܱ�������ֵ��listAllNTFSFolder������ʼ��

	ExFatBPB exFatBPB; //ExFAT��BPB��Ϣ�����ݴ�dbr�õ�
	//�����غ��䣨���ļ��������ļ����Ķ�Ԫӳ�䣬��ֵ������Ŀ¼��ʼ��
	map<UINT32, ExFATDirectoryMap> exFATDirectory;

	UINT32 i_ExFATTreeChose; //�����״�б�ʱ���õ���ѡ���ļ��еĴر��.

	void initVariable(); //��ʼ��ϵͳ������
	void listAllDisk(); //��ʾ���пɻָ���Ӳ�̲����ص�list�ؼ���
	CString getDiskNumber(CString name); //�õ����̵ı��
	void listAllPartition(int i_select);  //��ʾ��ѡ���Ӳ�̵����з������Ա�ѡ��
	void getNTFSBPBInfor(); //�õ�BPB�����Ϣ
	
	void listAllNTFSFolder();
	void listAllExFATFolder(); //�г�����ExFAT�ļ���
	
	void restoreNTFSFile(CString s_temp, NTFSFile rf);  //�����ļ���MFT���
	void restoreExFATFile(CString s_temp, ExFatFile rf); //�ָ�ExFAT�ļ�
	
	afx_msg void OnTvnSelchangedFoldertree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRestorefile();
	afx_msg void OnRestorefolder();
	afx_msg void OnBnClickedAbout();
	afx_msg void OnBnClickedBack();
	
	CStatic LoadImg; //��������ͼƬ
	CStatic TextShow;
	CFont font;
	CStatic ShowGuide;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
