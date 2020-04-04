
// DataSaverDlg.h : 头文件
//

#pragma once
#include "BitmapDialog.h"
#include "BmpButton.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <map>
#include "NTFSFunction.h"
// CDataSaverDlg 对话框
class CDataSaverDlg : public CBitmapDialog
{
// 构造
public:
	CDataSaverDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DATASAVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCancel();
	//自定义图片按钮
	CBmpButton bmpClose; //关闭按钮
	CBmpButton bmpNext;  //下一步按钮
	CBmpButton bmpAbout; //关于按钮
	CBmpButton bmpBack;  //上一步按钮
	
	diskVolume diskPartiton[26]; //定义26个盘符，存储可能存在的盘符和空间比较。

	int i_window ; //定义windows状态,如果0硬盘选择，如果1分区选择，如果2文件选择

	int i_fileSystem=0; //定义所要展示的文件系统。 0：NTFS文件系统  1：ExFAT

	int i_diskSelect = -1; //定义所选择的硬盘，默认小于0
	CImageList imageListPartiton; //用于为分区列表添加磁盘图标
	CImageList imageListPartitonFolderFile; //用于为文件夹和问价添加图标
	vector<partitionInfor> partitionVector; //分区的vector
	
	CListBox listDisk; //显示所有可恢复的硬盘的list控件
	diskInfor d_disk[26]; //定义磁盘变量，共可以容纳26个硬盘
	CListCtrl listPartition; //显示所有分区的listControl控件
	CTreeCtrl folderTree;  //文件夹和文件的树状列表
	CListCtrl fileList;   //显示文件的列表框

	CMenu menu_restore; //建立菜单对象，用来恢复文件
	//得到选择的恢复的分区
	partitionInfor pif; //选择的要恢复的分区
	CString s_driver; //打开硬盘的字符串

	unsigned int i_cu; //每簇扇区数
	unsigned int i_MFT_begin; //MFT的起始簇
	unsigned int i_PartitionBegin; //分区的起始扇区

	UINT32 i_mftTreeChose; //点击树状列表时，如果是NTFS文件系统，得到被选中文件夹的MFT编号.
	UINT32 i_FATCuTreeChoose;  //点击树状列表时，如果是ExFAT文件系统，得到被选中的文件夹的Cu号码。

	//以下4个变量用来右键菜单的恢复所用。
	NTFSFile NTFSfile_restore; //将要恢复的NTFS文件
	NTFSDirectory NTFSdirectory_restore;//将要恢复的NTFS文件夹

	ExFatFile exFatFile_restore;  //将要恢复的ExFAT文件
	ExFatFolder exFatFolder_restore; //将要恢复的ExFAT文件夹

	//建立MFT号和其（子文件夹与子文件）的二元映射，键值就是其MFT号
	map<UINT32, DirectoryMap> mftDirectory;

	NTFSFunction ns; //定义NTFS的功能变量，其值在listAllNTFSFolder函数初始化

	ExFatBPB exFatBPB; //ExFAT的BPB信息，内容从dbr得到
	//建立簇和其（子文件夹与子文件）的二元映射，键值就是其目录开始簇
	map<UINT32, ExFATDirectoryMap> exFATDirectory;

	UINT32 i_ExFATTreeChose; //点击树状列表时，得到被选中文件夹的簇编号.

	void initVariable(); //初始化系统变量。
	void listAllDisk(); //显示所有可恢复的硬盘并加载到list控件中
	CString getDiskNumber(CString name); //得到磁盘的编号
	void listAllPartition(int i_select);  //显示所选择的硬盘的所有分区，以便选择
	void getNTFSBPBInfor(); //得到BPB相关信息
	
	void listAllNTFSFolder();
	void listAllExFATFolder(); //列出所有ExFAT文件夹
	
	void restoreNTFSFile(CString s_temp, NTFSFile rf);  //输入文件的MFT编号
	void restoreExFATFile(CString s_temp, ExFatFile rf); //恢复ExFAT文件
	
	afx_msg void OnTvnSelchangedFoldertree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRestorefile();
	afx_msg void OnRestorefolder();
	afx_msg void OnBnClickedAbout();
	afx_msg void OnBnClickedBack();
	
	CStatic LoadImg; //用来加载图片
	CStatic TextShow;
	CFont font;
	CStatic ShowGuide;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
