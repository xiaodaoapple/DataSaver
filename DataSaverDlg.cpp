
// DataSaverDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataSaver.h"
#include "DataSaverDlg.h"
#include "afxdialogex.h"
#include "About.h"
#include "DiskFunction.h"
#include "NTFSFunction.h"
#include "ExFATFunction.h"
#include <queue>

#include <iostream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CDataSaverDlg �Ի���
CDataSaverDlg::CDataSaverDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CDataSaverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataSaverDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);

	//�˴�����ťͼƬ������ʵ�ʰ�ť���а�
	DDX_Control(pDX, IDC_NEXT, bmpNext);
	DDX_Control(pDX, IDC_CLOSE, bmpClose);
	DDX_Control(pDX, IDC_ABOUT, bmpAbout);
	DDX_Control(pDX, IDC_BACK, bmpBack);

	DDX_Control(pDX, IDC_LISTDisk, listDisk);
	DDX_Control(pDX, IDC_LISTPartition, listPartition);
	DDX_Control(pDX, IDC_FOLDERTREE, folderTree);
	DDX_Control(pDX, IDC_FILELIST, fileList);
	//	DDX_Control(pDX, IDC_GUIDE, LoadImg);
	DDX_Control(pDX, ID_SHOW, TextShow);
	DDX_Control(pDX, IDC_WARN, ShowGuide);
}

BEGIN_MESSAGE_MAP(CDataSaverDlg, CBitmapDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NEXT, &CDataSaverDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_CLOSE, &CDataSaverDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_ABOUT, &CDataSaverDlg::OnBnClickedAbout)

	ON_NOTIFY(TVN_SELCHANGED, IDC_FOLDERTREE, &CDataSaverDlg::OnTvnSelchangedFoldertree)
	ON_NOTIFY(NM_RCLICK, IDC_FILELIST, &CDataSaverDlg::OnNMRClickFilelist)
	ON_COMMAND(ID_RestoreFile, &CDataSaverDlg::OnRestorefile)
	ON_COMMAND(ID_RestoreFolder, &CDataSaverDlg::OnRestorefolder)
	
	ON_BN_CLICKED(IDC_BACK, &CDataSaverDlg::OnBnClickedBack)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDataSaverDlg ��Ϣ�������

BOOL CDataSaverDlg::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();

	// TODO:  ��ʼ�����Զ���Ƥ��
	if (!SetBitmap(IDB_BACKGROUND, true, RGB(128, 0, 0)))
	{
		AfxMessageBox("����Ƥ��ʧ�ܣ�");
		return FALSE;
	}
	SetStaticTransparent(TRUE);
	SetClickAnywhereMove(TRUE);

	//��ť����ͼƬ
	bmpNext.LoadAllBitmaps(IDB_NEXTBTN, 4);
	bmpClose.LoadAllBitmaps(IDB_CLOSEBTN, 4);
	bmpAbout.LoadAllBitmaps(IDB_ABOUT, 4);
	bmpBack.LoadAllBitmaps(IDB_BACK, 4);
	
	//��������
	font.CreatePointFont(300, _T("΢���ź�"));
	ShowGuide.SetFont(&font);
	ShowGuide.SetWindowTextA(DISKWARN);
	
	TextShow.SetWindowTextA(AUTHOR);
	//GetDlgItem(IDC_WARN)->SetFont(&font);

//	TextShow.ShowWindow(HIDE)
	//��ʼ��ʼ������
//	LoadImg.ShowWindow
//	LoadImg.ShowWindow(0);
	
	//TextShow.SetWindowTextA("hello");
	initVariable(); //��ʼ�����б���
	listAllDisk(); //��������Ӳ��
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDataSaverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBitmapDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDataSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//��ʼ������ϵͳ����
void CDataSaverDlg::initVariable()
{
	//���ز˵���Դ
	menu_restore.LoadMenuA(IDR_Restore);
	//����imagelist ����������ŵĺ���
	s_driver = "";
	imageListPartiton.Create(64, 64, ILC_COLOR24 | ILC_MASK, 1, 1);
	imageListPartiton.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISK)));
	//������ʾ��listControl�ؼ�����imagelist
	listPartition.SetImageList(&imageListPartiton, LVSIL_NORMAL);

	//���5��״̬ͼ�꣬�ֱ��Ǵ���-0���ļ���-1���ļ�-2���ļ��б�ɾ��-3���ļ���ɾ��-4
	imageListPartitonFolderFile.Create(24, 24, ILC_COLOR24 | ILC_MASK, 5, 1);
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISK)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FOLDER)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FILE)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FOLDERDELETE)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FILEDELETE)));
	//��״�ؼ����ļ����ļ� �ؼ�����ͼ��
	folderTree.SetImageList(&imageListPartitonFolderFile, LVSIL_NORMAL);
	fileList.SetImageList(&imageListPartitonFolderFile, LVSIL_SMALL);

	i_window = 1;//��ʼ״̬ΪӲ��ѡ��ģʽ
	listPartition.ShowWindow(HIDE_WINDOW); //�����б�����
	folderTree.ShowWindow(HIDE_WINDOW); //�ļ���������
	fileList.ShowWindow(HIDE_WINDOW); //�ļ��б�����
	bmpBack.ShowWindow(HIDE_WINDOW);  //��һ����ť����

	//��ʼ�������̷��ͷ�����С�Ĺ�ϵ
	int DiskCount = 0;
	//����GetLogicalDrives()�������Ի�ȡϵͳ���߼����������������������ص���һ��32λ�޷����������ݡ�
	DWORD DiskInfo = GetLogicalDrives();
	while (DiskInfo)//ͨ��ѭ�������鿴ÿһλ�����Ƿ�Ϊ1�����Ϊ1�����Ϊ��,���Ϊ0����̲����ڡ�
	{
		if (DiskInfo & 1)//ͨ��λ������߼���������ж��Ƿ�Ϊ1
		{
			++DiskCount;
		}
		DiskInfo = DiskInfo >> 1;//ͨ��λ��������Ʋ�����֤ÿѭ��һ��������λ�������ƶ�һλ��
	}
	TRACE("�߼���������:%d", DiskCount);
	int DSLength = GetLogicalDriveStrings(0, NULL); //�����Ȳ���ʱ��������Ҫ�ĳ���
	//ͨ��GetLogicalDriveStrings()������ȡ�����������ַ�����Ϣ���ȡ�
	char* DStr = new char[DSLength];//�û�ȡ�ĳ����ڶ�������һ��c�����ַ�������
	GetLogicalDriveStrings(DSLength, (LPTSTR)DStr);

	//ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��
	int DType;
	int si = 0;
	BOOL fResult;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;
	for (int i = 0; i<DSLength / 4; ++i)
		//Ϊ����ʾÿ����������״̬����ͨ��ѭ�����ʵ�֣�����DStr�ڲ������������A:\NULLB:\NULLC:\NULL����������Ϣ������DSLength/4���Ի�þ����ѭ����Χ
	{

		//char dir[3] = { DStr[si], ':', '\\' };
		char dir[4] = { DStr[si], ':', '\\','\0' };
		DType = GetDriveType(DStr + i * 4);
		fResult = GetDiskFreeSpaceExA(
			dir,(PULARGE_INTEGER)&i64FreeBytesToCaller,(PULARGE_INTEGER)&i64TotalBytes,(PULARGE_INTEGER)&i64FreeBytes);
		//GetDiskFreeSpaceEx���������Ի�ȡ���������̵Ŀռ�״̬,�������ص��Ǹ�BOOL��������
		
		diskPartiton[i].hasChoose = false; //���ô���δ����ʼ��
		diskPartiton[i].s_diskVolume = dir; //���ô����̷�
		diskPartiton[i].partitionSize = i64TotalBytes /512; //���ô��̵Ĵ�С����λΪ����
		si += 4;
	}
}


void CDataSaverDlg::OnBnClickedNext()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	POSITION pos;
	switch (i_window)
	{
	case 0:
		listPartition.ShowWindow(HIDE_WINDOW); folderTree.ShowWindow(HIDE_WINDOW); fileList.ShowWindow(HIDE_WINDOW);
		break;
	case 1:
		//��ʼ����ѡ��Ĵ���
		int i_select;
		i_select = listDisk.GetCurSel();
		if (i_select < 0)
		{
			MessageBox("��ѡ����Ҫ�ָ���Ӳ��", TITLE, MB_ICONEXCLAMATION);
		}
		else
		{
			bmpBack.ShowWindow(SHOW_OPENWINDOW);  //��һ����ť��ʾ
			
			ShowGuide.SetWindowTextA(PARTITIONWARN);
			
			listPartition.ShowWindow(SHOW_OPENWINDOW);
			listDisk.ShowWindow(HIDE_WINDOW);
			listAllPartition(i_select);
			i_window = 2;
		}
		break;
	case 2:
		listPartition.ShowWindow(SHOW_OPENWINDOW); folderTree.ShowWindow(HIDE_WINDOW);
		fileList.ShowWindow(HIDE_WINDOW);
		pos = listPartition.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox("��ѡ����Ҫ�ָ��ķ���", TITLE, MB_ICONEXCLAMATION);
			
		}	
		else
		{
			while (pos)
			{
				int nItem = listPartition.GetNextSelectedItem(pos);
				pif = partitionVector.at(nItem);
			//	s_driver = d_disk[i_diskSelect].s_driver;
				listPartition.ShowWindow(HIDE_WINDOW); folderTree.ShowWindow(SHOW_OPENWINDOW);
				fileList.ShowWindow(SHOW_OPENWINDOW);
				
				//�жϷ������ͣ����ļ����б���	
				if (pif.fileSystem == "NTFS")
				{
					i_fileSystem = 0;
					mftDirectory.clear();
					listAllNTFSFolder();	
				}
				if (pif.fileSystem == "ExFAT")
				{
					i_fileSystem = 1;
					exFATDirectory.clear();
					listAllExFATFolder();
					
				}				
			}
			ShowGuide.ShowWindow(HIDE_WINDOW); //��ʾ��������
			
			TextShow.SetWindowTextA(RESTOREINFO);
			
			i_window = 3;
			bmpNext.ShowWindow(HIDE_WINDOW); //���һ�����棬������һ����ť
		}
		break;
	default:
		break;
	}
}
void CDataSaverDlg::listAllExFATFolder()
{
	exFATDirectory.clear();  //mapӳ������
	queue<ExFatFolder> folderQueue;  //�����ļ��ж���
	fileList.DeleteAllItems();
	bool b_delete = false; //�ж��ļ������ļ����Ƿ���ɾ���˵�
	folderTree.DeleteAllItems(); //��״�ؼ����
	//���´���õ�ExFAT��DBR��Ϣ
	i_PartitionBegin = pif.partitonBegin;
	unsigned char diskSector[512]; //����洢�����õ��ֽ�
	DiskFunction df; //��ȡ�����õ�ʵ����
	//�򿪴��̣���ȡBPB����
	df.ReadDisk(s_driver, diskSector, i_PartitionBegin);
	exFatBPB.i_cu = pow(2, diskSector[0x6D]); //ÿ��������Ŀ
	i_cu = exFatBPB.i_cu;
	exFatBPB.i_cuBegin = df.byte2Int(&diskSector[0x58], 4) + pif.partitonBegin;
	exFatBPB.i_FATBegin = df.byte2Int(&diskSector[0x50], 4) + pif.partitonBegin;
	exFatBPB.i_FATNumber = df.byte2Int(&diskSector[0x54], 4);
	exFatBPB.i_rootCu = df.byte2Int(&diskSector[0x60], 4);
	exFatBPB.s_driver = s_driver;
	ExFATFunction eff(exFatBPB);
	//����Ŀ¼�����н���
	//�õ���Ŀ¼��ռ�Ĵ����б�
	vector<UINT32> cuVector;
	cuVector = eff.getDiretoryCuVector(exFatBPB.i_rootCu); //�õ���Ŀ¼��ռ�Ĵ���
	//����Ŀ¼��ռ�Ĵ������м������Ҫ�ͷ��ڴ�
	unsigned char * directorySector = new unsigned char[512 * i_cu*cuVector.size()];
	UINT32 i_sector;
	for (int i = 0; i < cuVector.size(); i++)
	{
		i_sector = eff.getSectorFromCu(cuVector.at(i)); //�õ������ڵ�����
		df.ReadDiskByNumber(s_driver, directorySector + i * 512 * i_cu, i_sector, i_cu);
	}
		
	//��ʼ����Ŀ¼���е����ݣ���Ŀ¼ǰ3��32�ֽڿ�ͷ�ֱ�Ϊ0x83(��꣩ 0x81(��λͼ��ʼ�� 0x82����д�ַ���ʼ)
	for (int i = 0; i < 512 * i_cu*cuVector.size(); i = i + 32)
	{
		b_delete = false; //�����ļ�ɾ����־Ϊ��
		if (directorySector[i] == 0) //���Ϊ0��˵���±ߵ�����Ϊ�գ���Ҫ�˳�
			break;
		if (directorySector[i] != 0x85 && directorySector[i]!=0x05)
			continue;
		else
		{
			if (directorySector[i] == 0x05)  //�ļ������ļ��б�ɾ�����Ὣ���ַ���Ϊ0x05
				b_delete = true;
			UINT32 i_sx = df.byte2Int(&directorySector[i + 4], 4); //�õ�����
			i_sx = (i_sx >> 4) & 0x1;  //�ж������λ�������1�������ļ��У�����Ϊ�ļ�
			if (i_sx ==1) //��������������Ŀ¼
			{
				//����ExFAT��Ŀ¼
				ExFatFolder fatFolder;
				if (b_delete)
					fatFolder.b_delete = true;
				else
					fatFolder.b_delete = false;
				
				fatFolder.s_FolderTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //�õ��ļ�������
				
				fatFolder.i_cu = df.byte2Int(&directorySector[i + 52], 4); //�õ��ļ���ʼ��
				int i_number = directorySector[i + 35]; //�ļ������ַ���������UNICODE16����
				//�õ��ļ���
				fatFolder.s_FolderName = df.getExFATFileName(&directorySector[i + 64], i_number);	
				
				exFATDirectory[exFatBPB.i_rootCu].directoryList.push_back(fatFolder); //���ļ���ѹ��mapvector
				folderQueue.push(fatFolder);  //���ļ���ѹ�����
				}
			else
			{
				//����ExFAT�ļ�
				ExFatFile fatFile;
				
				if (b_delete)
					fatFile.b_delete = true;
				else
					fatFile.b_delete = false;

				//�ļ�������ţ���־λΪ3�������־λΪ1
				if (directorySector[i + 33] == 3)
					fatFile.b_series = true;
				else
					fatFile.b_series = false;
				fatFile.s_FileTime= df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //�õ��ļ�����
				fatFile.i_fileSize = df.byte2Int(&directorySector[i + 40], 8); //�õ��ļ���С
				fatFile.i_beginCu = df.byte2Int(&directorySector[i + 52], 4); //�õ��ļ���ʼ��
				int i_number = directorySector[i + 35]; //�ļ������ַ���������UNICODE16����
				//�õ��ļ���
				fatFile.s_FileName = df.getExFATFileName(&directorySector[i + 64], i_number);
				
				exFATDirectory[exFatBPB.i_rootCu].fileList.push_back(fatFile); //���ļ�ѹ��vector
				}

			}
		}
	cuVector.clear();
	delete directorySector; //�ͷ��ڴ�
	//����Ϊ��ʱ������
	while (!folderQueue.empty())
	{
		ExFatFolder folderNode;
		folderNode = folderQueue.front(); //���ض�ͷԪ��
		folderQueue.pop(); //����ͷԪ��ɾ��
		DWORD32 i_folderCu = folderNode.i_cu; //�õ���ǰĿ¼�Ĵغ���

		cuVector = eff.getDiretoryCuVector(i_folderCu); //�õ���Ŀ¼���ڴ���ռ�Ĵ���
		//����Ŀ¼��ռ�Ĵ������м������Ҫ�ͷ��ڴ�
		unsigned char * directorySector = new unsigned char[512 * i_cu*cuVector.size()];
		UINT32 i_sector;
		for (int i = 0; i < cuVector.size(); i++)
		{
			i_sector = eff.getSectorFromCu(cuVector.at(i)); //�õ������ڵ�����
			df.ReadDiskByNumber(s_driver, directorySector + i * 512 * i_cu, i_sector, i_cu);
		}
		//��ʼ����Ŀ����е�����
		for (int i = 0; i < 512 * i_cu*cuVector.size(); i = i + 32)
		{
			if (directorySector[i] == 0) //���Ϊ0��˵���±ߵ�����Ϊ�գ���Ҫ�˳�
				break;
			if (directorySector[i] != 0x85 && directorySector[i] != 0x05)
				continue;
			else
			{
				UINT32 i_sx = df.byte2Int(&directorySector[i + 4], 4); //�õ�����
				i_sx = (i_sx >> 4) & 0x1;  //�ж������λ�������1�������ļ��У�����Ϊ�ļ�
				if (i_sx == 1) //��������������Ŀ¼
				{
					//����ExFAT��Ŀ¼
					ExFatFolder fatFolder;
					fatFolder.b_delete = false;
					fatFolder.s_FolderTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //�õ��ļ�������
					fatFolder.i_cu = df.byte2Int(&directorySector[i + 52], 4); //�õ��ļ���ʼ��
					int i_number = directorySector[i + 35]; //�ļ������ַ���������UNICODE16����
					//�õ��ļ���
					fatFolder.s_FolderName = df.getExFATFileName(&directorySector[i + 64], i_number);
					exFATDirectory[i_folderCu].directoryList.push_back(fatFolder); //���ļ���ѹ��mapvector
					folderQueue.push(fatFolder);  //���ļ���ѹ�����
				}
				else
				{
					//����ExFAT�ļ�
					ExFatFile fatFile;
					fatFile.b_delete = false;
					//�ļ�������ţ���־λΪ3�������־λΪ1
					if (directorySector[i + 33] == 3)
						fatFile.b_series = true;
					else
						fatFile.b_series = false;
					fatFile.s_FileTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //�õ��ļ�����
					fatFile.i_fileSize = df.byte2Int(&directorySector[i + 40], 8); //�õ��ļ���С
					fatFile.i_beginCu = df.byte2Int(&directorySector[i + 52], 4); //�õ��ļ���ʼ��
					int i_number = directorySector[i + 35]; //�ļ������ַ���������UNICODE16����
					//�õ��ļ���
					fatFile.s_FileName = df.getExFATFileName(&directorySector[i + 64], i_number);
					exFATDirectory[i_folderCu].fileList.push_back(fatFile); //���ļ�ѹ��vector
				}
			}
		}
		delete directorySector; //�ͷ��ڴ�
	}
		

		//��ʼ��״�ؼ���ʾ
		HTREEITEM hRoot;     // ���ĸ��ڵ�ľ��     
		hRoot = folderTree.InsertItem("���̸�Ŀ¼", 0, 0);
		folderTree.SetItemData(hRoot, exFatBPB.i_rootCu); //��ExFAT��Ŀ¼���õ�����Ŀ¼��

		folderTree.SetItemData(hRoot, exFatBPB.i_rootCu); //��5��MFT���õ���Ŀ¼��
		
		i_ExFATTreeChose = exFatBPB.i_rootCu; //Ĭ��5�Ÿ�Ŀ¼�Ѿ�ѡ��
		
		//��������
		queue<ExFATDirectoryQueueNode> directoryQueue;
		
		//����Ŀ¼��������Ŀ¼���õ�������
		for (int i = 0; i < exFATDirectory[exFatBPB.i_rootCu].directoryList.size(); i++)
		{
			HTREEITEM hFolder; // �ɱ�ʾ��һ�ļ��нڵ�ľ�� 
			ExFATDirectoryQueueNode folderNode;
			
			if (exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).b_delete == false)
				hFolder = folderTree.InsertItem(exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 1, 1, hRoot, TVI_LAST);
			else
				hFolder = folderTree.InsertItem(exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 3, 3, hRoot, TVI_LAST);

			//����Ŀ¼��MFT�����Ϊ��Ϣ�洢�����ڵ���
			folderTree.SetItemData(hFolder, exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).i_cu);
			folderNode.hFolder = hFolder;
			folderNode.fatDirectory = exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i);
			directoryQueue.push(folderNode);
		}
		//����Ϊ��ʱ������
		
		while (!directoryQueue.empty())
		{
			ExFATDirectoryQueueNode folderNode;
			folderNode = directoryQueue.front(); //���ض�ͷԪ��
			directoryQueue.pop(); //����ͷԪ��ɾ��
			UINT32 i_folderCu = folderNode.fatDirectory.i_cu; //�õ���ǰĿ¼�Ĵ�����λ��

			for (int i = 0; i < exFATDirectory[i_folderCu].directoryList.size(); i++)
			{
				HTREEITEM hItem;
				if (exFATDirectory[i_folderCu].directoryList.at(i).b_delete == false)
				{
					hItem = folderTree.InsertItem(exFATDirectory[i_folderCu].directoryList.at(i).s_FolderName, 1, 1, folderNode.hFolder, TVI_LAST);
				}
				else
				{
					hItem = folderTree.InsertItem(exFATDirectory[i_folderCu].directoryList.at(i).s_FolderName, 3, 3, folderNode.hFolder, TVI_LAST);
				}
				//����Ŀ¼�Ĵر����Ϊ��Ϣ�洢�����ڵ���
				folderTree.SetItemData(hItem, exFATDirectory[i_folderCu].directoryList.at(i).i_cu);
				//����ǰĿ¼����һ��Ŀ¼�������
				
				ExFATDirectoryQueueNode folderNodeTemp;
				folderNodeTemp.hFolder = hItem;
				folderNodeTemp.fatDirectory = exFATDirectory[i_folderCu].directoryList.at(i);
				directoryQueue.push(folderNodeTemp);
			}
		}
		
		folderTree.Expand(hRoot, TVE_EXPAND);
		CRect rect;
		// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
		fileList.GetClientRect(&rect);
		//Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����
		fileList.SetExtendedStyle(fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
		fileList.InsertColumn(0, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 0);
		fileList.InsertColumn(1, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 1);
		fileList.InsertColumn(2, _T("��С"), LVCFMT_CENTER, rect.Width() / 3, 2);
		// ���б���ͼ�ؼ��в����б���������б������ı�   
		//����Ŀ¼�������ļ��к��ļ���ӵ�list�ؼ���
		int k = 0;
		for (int i = 0; i < exFATDirectory[exFatBPB.i_rootCu].directoryList.size(); i++)
		{
			
			CString s_temp;
			//����һ��
			int nRow;
			if (exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 3);
			}
			else
			{
				nRow = fileList.InsertItem(k, exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 1);
			}

			s_temp = exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderTime;
			fileList.SetItemText(nRow, 1, s_temp);
			fileList.SetItemText(nRow, 2, "");
			k++;

		}
		for (int i = 0; i < exFATDirectory[exFatBPB.i_rootCu].fileList.size(); i++)
		{
			//����һ�������Ϊ ?��ţ����ݱ�ǩΪ�ļ����� ��Ҫ��������
			int nRow;
			if (exFATDirectory[exFatBPB.i_rootCu].fileList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, exFATDirectory[exFatBPB.i_rootCu].fileList.at(i).s_FileName, 4);
			}
			else
			{
				nRow = fileList.InsertItem(k, exFATDirectory[exFatBPB.i_rootCu].fileList.at(i).s_FileName, 2);
			}
			CString s_temp;
			//�����ļ���С
			s_temp.Format("%d", exFATDirectory[exFatBPB.i_rootCu].fileList.at(i).i_fileSize / 1024);
			s_temp += "KB";
			fileList.SetItemText(nRow, 2, s_temp);
			s_temp = exFATDirectory[exFatBPB.i_rootCu].fileList.at(i).s_FileTime;
			fileList.SetItemText(nRow, 1, s_temp);
			k++;
		}

}


void CDataSaverDlg::OnBnClickedBack()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	switch (i_window)
	{
	case 0: //��ʼ״̬
		break;
	case 1: 
		
		break;
	case 2://������ʾ����
		bmpBack.ShowWindow(HIDE_WINDOW); //����󷵻ص�1����
		i_window = 1;
		listDisk.ShowWindow(SHOW_OPENWINDOW); //�����б���ʾ
		ShowGuide.ShowWindow(SHOW_OPENWINDOW);
		
		ShowGuide.SetWindowTextA(DISKWARN);
		
		listPartition.ShowWindow(HIDE_WINDOW); //�����б�����
		for (int i = 0; i < 26; i++)
			diskPartiton[i].hasChoose = false;
		break;
	case 3: //�ļ�����ʾ����
		folderTree.ShowWindow(HIDE_WINDOW);
		fileList.ShowWindow(HIDE_WINDOW);
		bmpNext.ShowWindow(SHOW_OPENWINDOW);
		listPartition.ShowWindow(SHOW_OPENWINDOW);

		TextShow.SetWindowTextA(AUTHOR);
		ShowGuide.ShowWindow(SHOW_OPENWINDOW);
		ShowGuide.SetWindowTextA(PARTITIONWARN);
		i_window = 2;
		break;
	default:
		break;
	}
}


void CDataSaverDlg::OnBnClickedClose()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (MessageBox("ȷ��Ҫ�˳���", TITLE, MB_YESNO | MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		PostQuitMessage(0);
	}
}
//��ȡע����������е�Ӳ����ӵ�listbox�б���ȥ
void CDataSaverDlg::listAllDisk()
{
	//��ʼ��ʱ����Ҫ����ѡ���Ӳ������Ϊ��
	i_diskSelect = -1;

	HKEY hkey;
	byte sz[256];
	byte sz1[256];
	DWORD dwtype, sl = 256;
	DWORD s2 = 256;
	int j = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum", \
		NULL, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{
		for (int i = 0; i<26; i++)
		{
			CString id;
			id.Format("%d", i);
			if (RegQueryValueEx(hkey, id, NULL, &dwtype, (LPBYTE)sz, &sl) == ERROR_SUCCESS)
			{
				CString str = (CString)sz;
				CString str_show;
				//	diskList.AddString(sz);
				d_disk[i].s_diskName = str;
				//disk[i] = str;
				//list_disk.push_back(sz);
				//���´����ע����н�ȡ��Ӧ�Ĵ����ַ�����ȫ�������Ѿ����뵽��list_disk�б���
				int i_temp1, i_temp2, i_temp3, i_temp4;
				i_temp1 = str.Find('_');
				i_temp2 = str.Find('&', i_temp1);
				i_temp3 = str.Find('_', i_temp2);
				i_temp4 = str.Find('\\', i_temp3);
				str_show = str.Mid(i_temp1 + 1, i_temp2 - i_temp1 - 1) + " " + str.Mid(i_temp2 + 1, i_temp3 - i_temp2 - 1) + " " + str.Mid(i_temp3, i_temp4 - i_temp3);
				//��Ӳ�̽�ȡ������ַ����洢��Ӳ����Ϣ�
				d_disk[i].s_diskShow = str_show;
				//j��ֵ��Ϊ���յ�Ӳ����
				j++;
			}
			sl = 256;
		}
		RegCloseKey(hkey);
	}
	for (int i = 0; i < j; i++)
	{
		CString driver;
		byte diskSector[512], diskSectorGPT[512];
		DiskFunction df;
		driver = getDiskNumber(d_disk[i].s_diskName);
		d_disk[i].s_driver = driver;
		// �����̵�MBR��
		df.ReadDisk(driver, diskSector, 0);
		
		if (diskSector[450] == 0XEE)
		{
			d_disk[i].isMBR = false;
			//GPT���̵Ļ���ȥ��ȡGPTͷ��������̵Ĵ�С
			df.ReadDisk(driver, diskSectorGPT, 1);
			unsigned _int64 i_begin, i_end; //GPTӲ�̵���ʼ�����źͽ��������ţ���ռ8���ֽ�
			i_begin = df.byte2Int(&diskSectorGPT[40], 8);
			i_end = df.byte2Int(&diskSectorGPT[48], 8);
			//�õ����̴�С����λΪG
			d_disk[i].diskSize = (i_end - i_begin) / (2 * 1024 * 1024);
			CString s_temp;
			s_temp.Format("%u", d_disk[i].diskSize);

			d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " GPT ����" + " " + s_temp + "G";
			listDisk.AddString(d_disk[i].s_diskShow);
			continue;
		}
		else
		{
			d_disk[i].isMBR = true;
			unsigned _int64 i_total = 0;
			//��4������������������ӵõ�Ӳ�̵���������Ŀ
			i_total = df.byte2Int(&diskSector[458], 4) + df.byte2Int(&diskSector[474], 4) + df.byte2Int(&diskSector[490], 4) + df.byte2Int(&diskSector[506], 4);
			d_disk[i].diskSize = i_total / (2 * 1024 * 1024);
			CString s_temp;
			s_temp.Format("%u", d_disk[i].diskSize);
			//���Ӳ�̲���1G��С���ͱ�����MB����ʾ
			if (d_disk[i].diskSize == 0)
			{
				i_total = i_total / (2 * 1024);
				s_temp.Format("%u", i_total);
				d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " MBR ����" + " " + s_temp + "MB";
			}
			else
			{
				d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " MBR ����" + " " + s_temp + "G";
			}

		}
		listDisk.AddString(d_disk[i].s_diskShow);
	}

}
//�õ�������̵ı��
CString CDataSaverDlg::getDiskNumber(CString name)
{
	HKEY hkey;
	char sz[256];
	DWORD dwtype, sl = 256;
	int number = 0;
	for (int i = 0; i < 26; i++)
	{
		if (name.Compare(d_disk[i].s_diskName) == 0)
		{
			number = i;
			break;
		}
	}
	CString driver = "\\\\.\\PHYSICALDRIVE";
	CString num;
	num.Format("%d", number);
	driver += num;
	return driver;
}
//��ʾ��ѡ���Ӳ�̵����з������Ա�ѡ��
void CDataSaverDlg::listAllPartition(int i_disk_select)
{
	listPartition.DeleteAllItems();
	partitionVector.clear();
	//����Ӳ�̵��ַ���
	s_driver = d_disk[i_disk_select].s_driver;
	byte diskSector[512]; //����洢�����õ��ֽ�
	DiskFunction df; //��ȡ�����õ�ʵ����
	//���������MBR���̣�����з����Ķ�ȡ
	if (d_disk[i_disk_select].isMBR == true)
	{
		if (df.ReadDisk(d_disk[i_disk_select].s_driver, diskSector, 0) == 0)
		{
			MessageBox("��ȡ����ʧ��", TITLE, MB_ICONEXCLAMATION);
			return ;
		}
		else
		{
			//��4����������������Ҫ4��ѭ��
			for (int i = 0; i < 4; i++)
			{
				int i_type = 450 + i * 16; //450Ϊ��һ���������ı�־�ж�λ
				//07��ʾ����������ע��NTFS��EXTFAT����07����Ҫ���Ŷ�������ж�
				if (diskSector[i_type] == 0X07)
				{
					byte partitionSector[512]; //����洢DBR���ֽ�
					partitionInfor pi;
					
					//pi.fileSystem = "NTFS";
					pi.partitionSize = df.byte2Int(&diskSector[i_type + 8], 4);
					pi.partitonBegin = df.byte2Int(&diskSector[i_type + 4], 4);
					df.ReadDisk(d_disk[i_disk_select].s_driver, partitionSector, pi.partitonBegin); //�õ�DBR����
					if (partitionSector[1] == 0x52)
						pi.fileSystem = "NTFS";
					else
						pi.fileSystem = "ExFAT";
					partitionVector.push_back(pi);
				}
				//0B��0C����FAT32����
				if (diskSector[i_type] == 0X0B || diskSector[i_type] == 0X0C)
				{
					partitionInfor pi;
					pi.fileSystem = "FAT32";
					pi.partitionSize = df.byte2Int(&diskSector[i_type + 8], 4);
					pi.partitonBegin = df.byte2Int(&diskSector[i_type + 4], 4);
					partitionVector.push_back(pi);
				}
				

				//05��0F������չ����
				if (diskSector[i_type] == 0X05 || diskSector[i_type] == 0X0F)
				{
					//�õ���չ�����Ŀ�ʼ������
					unsigned int i_extend_begin = df.byte2Int(&diskSector[i_type + 4], 4);
					//�õ���չ�������ܴ�С
					unsigned int i_extend_total = df.byte2Int(&diskSector[i_type + 8], 4);
					unsigned int i_logical_begin = i_extend_begin;
					//�߼����̺�EBR���ܴ�С
					unsigned int i_logical_total = 0;
					//���߼�����������ĿС���ܵ���չ������Ŀʱ��ѭ������
					unsigned char logicalSector[512]; //����洢�߼������õ��ֽ�

					while (i_logical_total < i_extend_total)
					{
						//��ȡEBR
						df.ReadDisk(d_disk[i_disk_select].s_driver, logicalSector, i_logical_begin);
						partitionInfor pi;
						int i_EBR;
						if (logicalSector[450] == 0X07) //�����07�п�����NTFS��ExFAT
						{
							
							pi.partitionSize = df.byte2Int(&logicalSector[450 + 8], 4);
							i_EBR = df.byte2Int(&logicalSector[450 + 4], 4);
							pi.partitonBegin = i_EBR + i_logical_begin;


							//NTFS��ExFAT����07�����Ա���ȥDBR�ж�
							byte partitionSector[512]; //����洢DBR���ֽ�
							df.ReadDisk(d_disk[i_disk_select].s_driver, partitionSector, pi.partitonBegin); //�õ�DBR����
							if (partitionSector[1] == 0x52)
								pi.fileSystem = "NTFS";
							else
								pi.fileSystem = "ExFAT";

							partitionVector.push_back(pi);
							i_logical_total = i_logical_total + i_EBR + pi.partitionSize;
						}
						//0B��0C����FAT32����
						if (logicalSector[i_type] == 0X0B || logicalSector[i_type] == 0X0C)
						{
							pi.fileSystem = "FAT32";
							pi.partitionSize = df.byte2Int(&logicalSector[450 + 8], 4);
							pi.partitonBegin = df.byte2Int(&logicalSector[450 + 4], 4) + i_logical_begin;
							partitionVector.push_back(pi);
							i_logical_total = i_logical_total + i_EBR + pi.partitionSize;
						}
						i_logical_begin += pi.partitionSize + i_EBR;
					}
				}
			}

		}

	}  //�����GPT���̣���Ҫ��������Ĺ�������
	else
	{
		//GPT���̴ӵڶ���������ʼ��ÿ128���ֽڹ���һ��������
		for (int i = 2; i < 34; i++)
		{
			if (df.ReadDisk(d_disk[i_disk_select].s_driver, diskSector, i) == 0)
			{
				MessageBox("��ȡ����ʧ��", TITLE, MB_ICONEXCLAMATION);
				return ;
			}
			else
			{
				partitionInfor pi;
				unsigned _int64 i_partitionBegin, i_partitionEnd, i_partitionSize, i_partitionName;
				//ÿ�������洢4��������
				int j;
				int i_begin = 32;  //��i_begin��ʼ��8���ֽ�Ϊ������ʼ��ַ
				for (j = 0; j < 4; j++)
				{

					i_partitionBegin = df.byte2Int(&diskSector[i_begin], 8);
					i_partitionEnd = df.byte2Int(&diskSector[i_begin + 8], 8);
					//�õ�������С
					i_partitionSize = i_partitionEnd - i_partitionBegin;
					//������Ϊ0ʱ��˵������Ѿ�û�з�����
					if (i_partitionSize == 0)
						break;
					//���������������Ϊ0X42006100 basic
					i_partitionName = df.byte2Int(&diskSector[i_begin + 24], 4);
					//���������������������뵽������Ϣvector��
					if (i_partitionName == 0X610042)
					{
						pi.partitonBegin = i_partitionBegin;
						pi.partitionSize = i_partitionEnd - pi.partitonBegin;
						unsigned char dbrSector[512]; //����洢DBR�����õ��ֽ�

						if (df.ReadDisk(d_disk[i_disk_select].s_driver, dbrSector, pi.partitonBegin) == 0)
						{
							MessageBox("��ȡ����ʧ��", TITLE, MB_ICONEXCLAMATION);
							return ;
						}
						//��dbr�õ��ļ�ϵͳ����

						pi.fileSystem = df.judgeFileSystem(&dbrSector[3]);


						partitionVector.push_back(pi);
					}
					i_begin = i_begin + 128;
				}
				//���������Ѿ�û�з�����û�б�Ҫ������ȡ��
				if (j < 4)
					break;
			}
		}
	}
	//partitionVector.size();
	for (int i = 0; i < partitionVector.size(); i++)
	{
		CString s_temp;
		unsigned _int64 i_temp;
		i_temp = partitionVector[i].partitionSize / (2 * 1024 * 1024);
		if (i_temp == 0)
		{
			i_temp = partitionVector[i].partitionSize / (2 * 1024);
			s_temp.Format("%u", i_temp);
			//��ʼ�̷��Ϳռ��ƥ�� ����ʵ�ʲ��Է���������10�����������
			int j = 0;
			for (j = 0; j < 26; j++)
				if ((diskPartiton[j].partitionSize >= partitionVector[i].partitionSize - 10 && diskPartiton[j].partitionSize <= partitionVector[i].partitionSize + 10) && diskPartiton[j].hasChoose == false)
				{
					diskPartiton[j].hasChoose = true;
					listPartition.InsertItem(i,diskPartiton[j].s_diskVolume+"\n"+ partitionVector[i].fileSystem + "\n" + s_temp + "MB", 0);
					break;
				}
			//���û��ƥ����̷�����������ʾ
			if (j>=26)
				listPartition.InsertItem(i,partitionVector[i].fileSystem + "\n" + s_temp + "MB", 0);
		}
		else
		{
			s_temp.Format("%u", i_temp);
			//��ʼ�̷��Ϳռ��ƥ��
			int j = 0;
			for (j = 0; j < 26; j++)
				if ((diskPartiton[j].partitionSize >= partitionVector[i].partitionSize - 10 && diskPartiton[j].partitionSize <= partitionVector[i].partitionSize + 10) && diskPartiton[j].hasChoose == false)
				{
					diskPartiton[j].hasChoose = true;
					listPartition.InsertItem(i, diskPartiton[j].s_diskVolume + "\n" + partitionVector[i].fileSystem + "\n" + s_temp + "GB", 0);
					break;
				}
			if (j >= 26)
				listPartition.InsertItem(i, partitionVector[i].fileSystem + "\n" + s_temp + "GB", 0);
		}
	}

}
//��״�ؼ��������е��ļ��к��ļ�
void CDataSaverDlg::listAllNTFSFolder()
{
	mftDirectory.clear();  //mapӳ������
	folderTree.DeleteAllItems(); //��״�ؼ����
	getNTFSBPBInfor();//�õ�������BPB��Ϣ
	//��NTFS���ܱ������г�ʼ��
	ns.i_cu = i_cu;
	ns.i_MFT_begin = i_MFT_begin;
	ns.i_PartitionBegin = i_PartitionBegin;
	
	//�õ�0���ļ���¼�ŵ�MFT��������ȡ��������������diskSector���顣
	unsigned _int64 i_sector = ns.getNTFSSectorNumber(0);
	unsigned char diskSector[1024]; //����洢�����õ��ֽ�
	DiskFunction df; //��ȡ�����õ�ʵ����


	df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);

	diskSector[510] = diskSector[0x32];
	diskSector[511] = diskSector[0x33];
	diskSector[1022] = diskSector[0x34];
	diskSector[1023] = diskSector[0x35];
	MFT30h mft30;
	MFT80h mft80;
	MFTB0h mftb0;
	mft30 = df.get30h(diskSector); //�õ�MFT��30h����
	mft80 = df.get80h(diskSector); //�õ�MFT��80h����
	mftb0 = df.getB0h(diskSector);
	UINT32 i_cuSize = 0;
	UINT32 i_mftNumber = 0;
	//�õ�mft��ռ�Ĵ�����ÿ�ص����������Դ���������2�õ�MFT������¼��
	for (int i = 0; i < mft80.runlist.size(); i++)
	{
		i_cuSize += mft80.runlist.at(i).i_cuNumber;
	}
	i_mftNumber = i_cuSize*i_cu / 2;

	//�����ǲ��Դ���

	//ÿ�ζ�һ����
	unsigned char * cuSector = new unsigned char[512 * i_cu];
	//���غܶ�ʱ��һ���ض�Ч��̫�����1024��
	unsigned char * sectorLarge = new unsigned char[512 * i_cu * 1024]; //����1024���ص��ֽڴ�С������1��Ϊ4k��������4M�ռ�
	for (int i = 0; i < mft80.runlist.size(); i++)
	{
		unsigned _int64 i_cuBegin = mft80.runlist.at(i).i_cuBegin;
		DWORD i_cuNumber = mft80.runlist.at(i).i_cuNumber; //�õ��صĸ���

		DWORD32 d_temp = i_cuNumber; //�Ѿ���ȡ�Ĵ���
		//����������1024�ص�ʱ���ļ��Ƚϴ�һ�ζ�1024��
		if (i_cuNumber > 1024)
		{
			for (int j = 0; j < i_cuNumber - 1024; j += 1024)
			{

				i_sector = ns.getNTFSSectorFromCu(i_cuBegin);
				i_cuBegin += 1024;
				df.ReadDiskByNumber(s_driver, sectorLarge, i_sector, i_cu * 1024);
				d_temp -= 1024;
				for (int k = 0; k < 1024 * i_cu / 2; k++) //1024������������MFT��Ŀ
				{
					sectorLarge[k * 2 * 512 + 510] = sectorLarge[k * 2 * 512 + 0x32];
					sectorLarge[k * 2 * 512 + 511] = sectorLarge[k * 2 * 512 + 0x33];
					sectorLarge[k * 2 * 512 + 1022] = sectorLarge[k * 2 * 512 + 0x34];
					sectorLarge[k * 2 * 512 + 1023] = sectorLarge[k * 2 * 512 + 0x35];
					//�ж��Ƿ����ļ���,����ǣ����һ����ĸΪ0x46
					if (sectorLarge[k * 2 * 512] != 0x46)
						continue;
					//�ж��Ƿ��ǿյ��ļ��ţ�����ǿյģ���0x38λ�ò�Ϊ10h
					if (sectorLarge[k * 2 * 512 + 0x38] != 0x10)
						continue;
					DWORD32 i_mftNumber = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4);
					//	TRACE("k=%d MFT=%d\n", k,i_mftNumber);

					if (i_mftNumber < 27)
						continue;
					NTFSDirectory ntfsDirectory;
					NTFSFile ntfsFile;
					switch (sectorLarge[k * 2 * 512 + 0x16])
					{
					case 0: //�ļ���ɾ��
						ntfsFile.b_delete = true;
						ntfsFile.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //�õ��ļ��ļ�¼��
						ntfsFile.mft30h = df.get30h(sectorLarge + k * 2 * 512);
						ntfsFile.mft80h = df.get80h(sectorLarge + k * 2 * 512);
						//����Ӧ��Ŀ¼��MFT�ŵ����ļ�vector������ļ��������ӦMFT�����ڣ���Ὠ��
						mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
						break;
					case 1: //�ļ�ʹ����
						ntfsFile.b_delete = false;
						ntfsFile.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //�õ��ļ��ļ�¼��
						ntfsFile.mft30h = df.get30h(sectorLarge + k * 2 * 512);
						ntfsFile.mft80h = df.get80h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
						//mftNode[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile); //���ļ����뵽��Ŀ¼���ڵ��ļ�vector��
						break;
					case 2: //Ŀ¼��ɾ��
						ntfsDirectory.b_delete = true;
						ntfsDirectory.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //�õ�Ŀ¼�ļ�¼��
						ntfsDirectory.mft30h = df.get30h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						//mftNode[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						break;
					case 3: //Ŀ¼ʹ����
						ntfsDirectory.b_delete = false;
						ntfsDirectory.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //�õ�Ŀ¼�ļ�¼��
						ntfsDirectory.mft30h = df.get30h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						//mftNode[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						break;
					default:
						break;
					}
				} //1024���ؽ�������
			} //�ش���1024ʱ��for�������
		}
		//��ʣ��صĸ���
		for (int j = 0; j < d_temp; j++) //С��1024�������
		{
			i_sector = ns.getNTFSSectorFromCu(i_cuBegin + j);
			df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //�õ���һ���صĴ�С������
			//kΪMFT��¼�ĸ���
			for (int k = 0; k < i_cu / 2; k++)
			{
				cuSector[k * 2 * 512 + 510] = cuSector[k * 2 * 512 + 0x32];
				cuSector[k * 2 * 512 + 511] = cuSector[k * 2 * 512 + 0x33];
				cuSector[k * 2 * 512 + 1022] = cuSector[k * 2 * 512 + 0x34];
				cuSector[k * 2 * 512 + 1023] = cuSector[k * 2 * 512 + 0x35];
				//�ж��Ƿ����ļ���,����ǣ����һ����ĸΪ0x46
				if (cuSector[k * 2 * 512] != 0x46)
					continue;
				//�ж��Ƿ��ǿյ��ļ��ţ�����ǿյģ���0x38λ�ò�Ϊ10h
				if (cuSector[k * 2 * 512 + 0x38] != 0x10)
					continue;
				DWORD32 i_mftNumber = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4);
				if (i_mftNumber < 27)
					continue;
				NTFSDirectory ntfsDirectory;
				NTFSFile ntfsFile;
				switch (cuSector[k * 2 * 512 + 0x16])
				{
				case 0: //�ļ���ɾ��
					ntfsFile.b_delete = true;
					ntfsFile.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //�õ��ļ��ļ�¼��
					ntfsFile.mft30h = df.get30h(cuSector + k * 2 * 512);
					ntfsFile.mft80h = df.get80h(cuSector + k * 2 * 512);
					//����Ӧ��Ŀ¼��MFT�ŵ����ļ�vector������ļ��������ӦMFT�����ڣ���Ὠ��
					mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
					break;
				case 1: //�ļ�ʹ����
					ntfsFile.b_delete = false;
					ntfsFile.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //�õ��ļ��ļ�¼��
					ntfsFile.mft30h = df.get30h(cuSector + k * 2 * 512);
					ntfsFile.mft80h = df.get80h(cuSector + k * 2 * 512);
					mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
					break;
				case 2: //Ŀ¼��ɾ��
					ntfsDirectory.b_delete = true;
					ntfsDirectory.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //�õ�Ŀ¼�ļ�¼��
					ntfsDirectory.mft30h = df.get30h(cuSector + k * 2 * 512);
					mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
					break;
				case 3: //Ŀ¼ʹ����
					ntfsDirectory.b_delete = false;
					ntfsDirectory.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //�õ�Ŀ¼�ļ�¼��
					ntfsDirectory.mft30h = df.get30h(cuSector + k * 2 * 512);
					mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
					break;
				default:
					break;
				}
			}
		}
	}

	HTREEITEM hRoot;     // ���ĸ��ڵ�ľ��     
	hRoot = folderTree.InsertItem("���̸�Ŀ¼", 0, 0);
	folderTree.SetItemData(hRoot, 5); //��5��MFT���õ���Ŀ¼��
	i_mftTreeChose = 5; //Ĭ��5�Ÿ�Ŀ¼�Ѿ�ѡ��
	//��������
	queue<DirectoryQueueNode> directoryQueue;
	//����Ŀ¼��������Ŀ¼���õ�������
	for (int i = 0; i < mftDirectory[5].directoryList.size(); i++)
	{
		HTREEITEM hFolder; // �ɱ�ʾ��һ�ļ��нڵ�ľ�� 
		DirectoryQueueNode folderNode;
		if (mftDirectory[5].directoryList.at(i).b_delete == false)
			hFolder = folderTree.InsertItem(mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 1, 1, hRoot, TVI_LAST);
		else
			hFolder = folderTree.InsertItem(mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 3, 3, hRoot, TVI_LAST);

		//����Ŀ¼��MFT�����Ϊ��Ϣ�洢�����ڵ���
		folderTree.SetItemData(hFolder, mftDirectory[5].directoryList.at(i).i_mft);
		folderNode.hFolder = hFolder;
		folderNode.ntfsDirectory = mftDirectory[5].directoryList.at(i);
		directoryQueue.push(folderNode);
	}
	//����Ϊ��ʱ������
	while (!directoryQueue.empty())
	{
		DirectoryQueueNode folderNode;
		folderNode = directoryQueue.front(); //���ض�ͷԪ��
		directoryQueue.pop(); //����ͷԪ��ɾ��
		unsigned _int64 i_mft = folderNode.ntfsDirectory.i_mft; //�õ���ǰĿ¼��mft����

		for (int i = 0; i < mftDirectory[i_mft].directoryList.size(); i++)
		{
			HTREEITEM hItem;
			if (mftDirectory[i_mft].directoryList.at(i).b_delete == false)
			{
				hItem = folderTree.InsertItem(mftDirectory[i_mft].directoryList.at(i).mft30h.s_fileName, 1, 1, folderNode.hFolder, TVI_LAST);
			}
			else
			{
				hItem = folderTree.InsertItem(mftDirectory[i_mft].directoryList.at(i).mft30h.s_fileName, 3, 3, folderNode.hFolder, TVI_LAST);
			}
			//����Ŀ¼��MFT�����Ϊ��Ϣ�洢�����ڵ���
			folderTree.SetItemData(hItem, mftDirectory[i_mft].directoryList.at(i).i_mft);
			//����ǰĿ¼����һ��Ŀ¼�������
			DirectoryQueueNode folderNodeTemp;
			folderNodeTemp.hFolder = hItem;
			folderNodeTemp.ntfsDirectory = mftDirectory[i_mft].directoryList.at(i);
			directoryQueue.push(folderNodeTemp);
		}
	}
	folderTree.Expand(hRoot, TVE_EXPAND);
	CRect rect;
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	fileList.GetClientRect(&rect);
	//Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����
	fileList.SetExtendedStyle(fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	fileList.InsertColumn(0, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 0);
	fileList.InsertColumn(1, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 1);
	fileList.InsertColumn(2, _T("��С"), LVCFMT_CENTER, rect.Width() / 3, 2);
	// ���б���ͼ�ؼ��в����б���������б������ı�   
	//����Ŀ¼�������ļ��к��ļ���ӵ�list�ؼ���
	int k = 0;
	for (int i = 0; i < mftDirectory[5].directoryList.size(); i++)
	{
		SYSTEMTIME st;
		CString s_temp;
		//����һ��
		int nRow;
		if (mftDirectory[5].directoryList.at(i).b_delete == true)
		{
			nRow = fileList.InsertItem(k, mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 3);
		}
		else
		{
			nRow = fileList.InsertItem(k, mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 1);
		}

		st = mftDirectory[5].directoryList.at(i).mft30h.st_fileTime;
		s_temp.Format(("%04d-%02d-%02d %02d:%02d:%02d"),
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond);
		fileList.SetItemText(nRow, 1, s_temp);
		fileList.SetItemText(nRow, 2, "");
		k++;

	}
	for (int i = 0; i < mftDirectory[5].fileList.size(); i++)
	{
		//����һ�������ΪMFT��ţ����ݱ�ǩΪ30����
		int nRow;
		if (mftDirectory[5].fileList.at(i).b_delete == true)
		{
			nRow = fileList.InsertItem(k, mftDirectory[5].fileList.at(i).mft30h.s_fileName, 4);
		}
		else
		{
			nRow = fileList.InsertItem(k, mftDirectory[5].fileList.at(i).mft30h.s_fileName, 2);
		}
		SYSTEMTIME st;
		st = mftDirectory[5].fileList.at(i).mft30h.st_fileTime;
		CString s_temp;
		//�����ļ���С
		s_temp.Format("%d", mftDirectory[5].fileList.at(i).mft80h.i_size / 1024);
		s_temp += "KB";
		fileList.SetItemText(nRow, 2, s_temp);
		s_temp.Format(("%04d-%02d-%02d %02d:%02d:%02d"),
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond);
		fileList.SetItemText(nRow, 1, s_temp);
		k++;
	}

}
//�õ�BPB�����Ϣ
void CDataSaverDlg::getNTFSBPBInfor()
{
	i_PartitionBegin = pif.partitonBegin;
	unsigned char diskSector[512]; //����洢�����õ��ֽ�
	DiskFunction df; //��ȡ�����õ�ʵ����

	//�򿪴��̣���ȡBPB����
	if (df.ReadDisk(s_driver, diskSector, i_PartitionBegin) == 0)
	{
		MessageBox("��ȡ����ʧ��", TITLE, MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		i_cu = diskSector[13];
		i_MFT_begin = df.byte2Int(&diskSector[48], 8);
	}
}
//�����״�б�ʱ��ˢ���ļ��б�Ի���
void CDataSaverDlg::OnTvnSelchangedFoldertree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UINT32 i_get; //��������е��ļ��е�MFT���
	HTREEITEM hItem = folderTree.GetSelectedItem();
	*pResult = 0;
	i_get = folderTree.GetItemData(hItem);
	//����ļ�ϵͳ��־��1����������ExFAT
	if (i_fileSystem == 1)
	{
		i_FATCuTreeChoose = i_get;
		//���List�����е�����
		fileList.DeleteAllItems();
		//������ѡ�е�Ŀ¼�������ļ��к��ļ���ӵ�list�ؼ���
		int k = 0;
		for (int i = 0; i < exFATDirectory[i_get].directoryList.size(); i++)
		{
			CString s_temp;
			//����һ��
			int nRow;
			if (exFATDirectory[i_get].directoryList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, exFATDirectory[i_get].directoryList.at(i).s_FolderName, 3);
			}
			else
			{
				nRow = fileList.InsertItem(k, exFATDirectory[i_get].directoryList.at(i).s_FolderName, 1);
			}

			s_temp = exFATDirectory[i_get].directoryList.at(i).s_FolderTime;
			fileList.SetItemText(nRow, 1, s_temp);
			fileList.SetItemText(nRow, 2, "");
			k++;

		}
		for (int i = 0; i < exFATDirectory[i_get].fileList.size(); i++)
		{
			//����һ�������ΪMFT��ţ����ݱ�ǩΪ30����
			int nRow;
			if (exFATDirectory[i_get].fileList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, exFATDirectory[i_get].fileList.at(i).s_FileName, 4);
			}
			else
			{
				nRow = fileList.InsertItem(k, exFATDirectory[i_get].fileList.at(i).s_FileName, 2);
			}
			
			CString s_temp;
			//�����ļ���С
			s_temp.Format("%d", exFATDirectory[i_get].fileList.at(i).i_fileSize / 1024);
			s_temp += "KB";
			fileList.SetItemText(nRow, 2, s_temp);

			s_temp = exFATDirectory[i_get].fileList.at(i).s_FileTime;
			fileList.SetItemText(nRow, 1, s_temp);
			k++;
		}
	}
	//����ļ�ϵͳ��־��0����������NTFS
	if (i_fileSystem == 0)
	{
		i_mftTreeChose = i_get;
		//���List�����е�����
		fileList.DeleteAllItems();
		//������ѡ�е�Ŀ¼�������ļ��к��ļ���ӵ�list�ؼ���
		int k = 0;
		for (int i = 0; i < mftDirectory[i_get].directoryList.size(); i++)
		{
			SYSTEMTIME st;
			CString s_temp;
			//����һ��
			int nRow;
			if (mftDirectory[i_get].directoryList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, mftDirectory[i_get].directoryList.at(i).mft30h.s_fileName, 3);
			}
			else
			{
				nRow = fileList.InsertItem(k, mftDirectory[i_get].directoryList.at(i).mft30h.s_fileName, 1);
			}

			st = mftDirectory[i_get].directoryList.at(i).mft30h.st_fileTime;
			s_temp.Format(("%04d-%02d-%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay,
				st.wHour, st.wMinute, st.wSecond);
			fileList.SetItemText(nRow, 1, s_temp);
			fileList.SetItemText(nRow, 2, "");
			k++;

		}
		for (int i = 0; i < mftDirectory[i_get].fileList.size(); i++)
		{
			//����һ�������ΪMFT��ţ����ݱ�ǩΪ30����
			int nRow;
			if (mftDirectory[i_get].fileList.at(i).b_delete == true)
			{
				nRow = fileList.InsertItem(k, mftDirectory[i_get].fileList.at(i).mft30h.s_fileName, 4);
			}
			else
			{
				nRow = fileList.InsertItem(k, mftDirectory[i_get].fileList.at(i).mft30h.s_fileName, 2);
			}
			SYSTEMTIME st;
			st = mftDirectory[i_get].fileList.at(i).mft30h.st_fileTime;
			CString s_temp;
			//�����ļ���С
			s_temp.Format("%d", mftDirectory[i_get].fileList.at(i).mft80h.i_size / 1024);
			s_temp += "KB";
			fileList.SetItemText(nRow, 2, s_temp);

			s_temp.Format(("%04d-%02d-%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay,
				st.wHour, st.wMinute, st.wSecond);
			fileList.SetItemText(nRow, 1, s_temp);
			k++;
		}
	}
	
}
//�ָ�NTFS�ļ����ָ�ָ��MFT���ļ�
void CDataSaverDlg::restoreNTFSFile(CString s_path, NTFSFile rf)
{
	WORD i_start; //���ݿ�ʼ��ƫ����
	unsigned char diskSector[1024]; //
	DiskFunction df; //��ȡ�����õ�ʵ����
	byte *b_large = new byte[1024 * i_cu * 512]; //���ÿ��8����������4M���ڴ棬�����ָ����ļ���
	if (s_path.IsEmpty()) //�����ļ��ָ�
	{
		//�����ļ���ѡ��Ի���
		TCHAR filePath[1024];
		BROWSEINFO bi;
		bi.hwndOwner = this->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = TEXT("��ѡ������ļ����ļ���");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (NTFSfile_restore.mft80h.b_in == true) //����ǳ�פ����
		{
			i_start = NTFSfile_restore.mft80h.b_start;
			//�õ��ļ���Ӧ��MFT��¼�Ŷ�Ӧ��������
			unsigned _int64 i_sector = ns.getNTFSSectorNumber(NTFSfile_restore.i_mft);
			df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);
			diskSector[510] = diskSector[0x32];
			diskSector[511] = diskSector[0x33];
			diskSector[1022] = diskSector[0x34];
			diskSector[1023] = diskSector[0x35];
			//��ʼ�ָ�����
			if (pidl == NULL)
				return;
			if (SHGetPathFromIDList(pidl, filePath))
			{
				//AfxMessageBox(filePath+rf.mft30h.s_fileName);
				CString s_get = filePath; //��·���洢������
				CString s = "\\";
				CString s_temp;
				if (s_get.GetLength() < 4)
				{
					s_temp = s_get + rf.mft30h.s_fileName; //ѡ���·�����ü�"\"
				}
				else
				{
					s_temp = s_get + s + rf.mft30h.s_fileName; //ѡ���ļ���·����Ҫ��ת���
				}

				FILE *f;
				int i_file = fopen_s(&f, s_temp, "wb"); //ָ��·�������ļ�
				fwrite(&diskSector[i_start], 1, rf.mft80h.i_size, f);
				fclose(f);
				MessageBox(rf.mft30h.s_fileName + "�ɹ����ָ�", TITLE, MB_ICONINFORMATION);
				
			}
		}
		else //�ļ��Ƿǳ�פ����
		{
			//ÿ�ζ�һ����
			unsigned char * cuSector = new unsigned char[512 * i_cu];
			if (pidl == NULL)
				return;
			if (SHGetPathFromIDList(pidl, filePath))
			{
				CString s_get = filePath; //��·���洢������
				CString s = "\\";
				CString s_temp;
				if (s_get.GetLength() < 4)
				{
					s_temp = s_get + rf.mft30h.s_fileName; //ѡ���·�����ü�"\"
				}
				else
				{
					s_temp = s_get + s + rf.mft30h.s_fileName; //ѡ���ļ���·����Ҫ��ת���
				}
				FILE *f;
				int i_file = fopen_s(&f, s_temp, "wb");

				DWORD64 i_remain = 0; //ʣ��Ļ�û��д�뵽�ļ��е��ֽ�
				i_remain = rf.mft80h.i_size; //��ʼ��СΪ�ļ����ֽ���
				DWORD32 i_sectorPerCu = 512 * i_cu; //ÿ�ص��ֽ�����
				//д��ָ�����ֽ�
				for (int i = 0; i < rf.mft80h.runlist.size(); i++)
				{
					DWORD32 i_cuSize = rf.mft80h.runlist.at(i).i_cuNumber; //��һ��runlist�Ĵ���
					DWORD32 i_begin = rf.mft80h.runlist.at(i).i_cuBegin;   //��ʼ��
					unsigned _int64 i_sector; //�����������
					DWORD32 d_temp = i_cuSize; //�Ѿ���ȡ�Ĵ���
					//����������1024�ص�ʱ���ļ��Ƚϴ�һ�ζ�1024��
					if (i_cuSize > 1024)
					{

						for (int k = 0; k < i_cuSize - 1024; k += 1024)
						{

							i_sector = ns.getNTFSSectorFromCu(i_begin);
							i_begin += 1024;
							df.ReadDiskByNumber(s_driver, b_large, i_sector, i_cu * 1024);
							fwrite(b_large, 1, 512 * i_cu * 1024, f);
							i_remain -= 512 * i_cu * 1024;
							d_temp -= 1024;

						}
					}
					for (int j = 0; j < d_temp; j++)
					{
						i_sector = ns.getNTFSSectorFromCu(i_begin + j); //ÿ���ر�Ŷ�Ӧ������
						df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //�����������
						int k;
						if (i_remain > i_sectorPerCu)  //���ʣ��δд�ֽ�������ÿ�ص��ֽ���������ÿ��дһ��
						{
							fwrite(cuSector, 1, 512 * i_cu, f); //�ļ���дһ������
							i_remain -= i_sectorPerCu;
						}
						else
						{
							fwrite(cuSector, 1, i_remain, f); //�ļ���дʣ�಻��1�ص�����
						}
					}
				}
				fclose(f);
				MessageBox(rf.mft30h.s_fileName + "�ɹ����ָ�", TITLE, MB_ICONINFORMATION);
			}
		}
	}
	else //�ļ��лָ��ݹ�����
	{
		if (NTFSfile_restore.mft80h.b_in == true) //����ǳ�פ����
		{
			i_start = NTFSfile_restore.mft80h.b_start;
			//�õ��ļ���Ӧ��MFT��¼�Ŷ�Ӧ��������
			unsigned _int64 i_sector = ns.getNTFSSectorNumber(NTFSfile_restore.i_mft);
			df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);
			diskSector[510] = diskSector[0x32];
			diskSector[511] = diskSector[0x33];
			diskSector[1022] = diskSector[0x34];
			diskSector[1023] = diskSector[0x35];

			CString s_temp = s_path + rf.mft30h.s_fileName;
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb"); //ָ��·�������ļ�
			fwrite(&diskSector[i_start], 1, rf.mft80h.i_size, f);
			fclose(f);
		}
		else //�ļ��Ƿǳ�פ����
		{
			//ÿ�ζ�һ����
			unsigned char * cuSector = new unsigned char[512 * i_cu];
			CString s_temp = s_path + rf.mft30h.s_fileName;
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb");
			DWORD64 i_remain = 0; //ʣ��Ļ�û��д�뵽�ļ��е��ֽ�
			i_remain = rf.mft80h.i_size; //��ʼ��СΪ�ļ����ֽ���
			DWORD32 i_sectorPerCu = 512 * i_cu; //ÿ�ص��ֽ�����

			//д��ָ�����ֽ�
			for (int i = 0; i < rf.mft80h.runlist.size(); i++)
			{
				DWORD32 i_cuSize = rf.mft80h.runlist.at(i).i_cuNumber; //��һ��runlist�Ĵ���
				DWORD32 i_begin = rf.mft80h.runlist.at(i).i_cuBegin;   //��ʼ��
				unsigned _int64 i_sector; //�����������
				DWORD32 d_temp = i_cuSize; //�Ѿ���ȡ�Ĵ���
				//����������1024�ص�ʱ���ļ��Ƚϴ�һ�ζ�1024��
				if (i_cuSize > 1024)
				{

					for (int k = 0; k < i_cuSize - 1024; k += 1024)
					{

						i_sector = ns.getNTFSSectorFromCu(i_begin);
						i_begin += 1024;
						df.ReadDiskByNumber(s_driver, b_large, i_sector, i_cu * 1024);
						fwrite(b_large, 1, 512 * i_cu * 1024, f);
						i_remain -= 512 * i_cu * 1024;
						d_temp -= 1024;

					}
				}
				for (int j = 0; j < d_temp; j++)
				{
					i_sector = ns.getNTFSSectorFromCu(i_begin + j); //ÿ���ر�Ŷ�Ӧ������
					df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //�����������
					int k;
					if (i_remain > i_sectorPerCu)  //���ʣ��δд�ֽ�������ÿ�ص��ֽ���������ÿ��дһ��
					{
						fwrite(cuSector, 1, 512 * i_cu, f); //�ļ���дһ������
						i_remain -= i_sectorPerCu;
					}
					else
					{
						fwrite(cuSector, 1, i_remain, f); //�ļ���дʣ�಻��1�ص�����
					}
				}
			}
			fclose(f);
		}
	}
	delete b_large; //����Ҫ�������ǵ��ͷ��ڴ棬��Ȼ�ڴ�����ռ��
}

void CDataSaverDlg::OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//��ȡ�������
	CPoint ptCurSel(0, 0);
	GetCursorPos(&ptCurSel);
	//ת��Ϊ��ǰ�ؼ�����ϵ������
	fileList.ScreenToClient(&ptCurSel);
	LVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = ptCurSel;
	//�ж��������еڼ���
	int nItem = fileList.HitTest(&HitTestInfo);
	//�����ѡ����
	if (nItem != -1)
	{
		//��ȡѡ��������
		char szTmp[MAX_PATH];
		fileList.GetItemText(nItem, 0, szTmp, MAX_PATH);
		//�����˵����ã�Ĭ�ϰ�ť������ѡ
		CPoint point;
		GetCursorPos(&point);
		CMenu* pSubMenu = menu_restore.GetSubMenu(0);
		pSubMenu->EnableMenuItem(ID_RestoreFile, MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_GRAYED);
		CString s_temp = szTmp; //�õ��ַ���
		CString s_get;

		//�����ExFAT�ļ�ϵͳ�������±ߵĴ����߼�
		if (i_fileSystem == 1)
		{
			//������ѡ�еõ��Ĵؼ�¼���vector��Ŀ¼�б��в����Ƿ�����Ҽ�ѡ�е�list����
			for (int i = 0; i < exFATDirectory[i_ExFATTreeChose].directoryList.size(); i++)
			{
				s_get = exFATDirectory[i_ExFATTreeChose].directoryList.at(i).s_FolderName;
				if (!s_get.Compare(s_temp)) //��ȷ���0
				{
					//�����ַ������
					//�ַ�����ȣ����ҵ��ĵ�����ļ�����ɾ�������ļ��У��ļ����Ҽ��˵��ſ��á�
					if (exFATDirectory[i_ExFATTreeChose].directoryList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_ENABLED);
						exFatFolder_restore = exFATDirectory[i_ExFATTreeChose].directoryList.at(i);
					}
					break; //����forѭ��
				}
			}
			//������ѡ�еõ���mft��¼���vector���ļ��б��в����Ƿ�����Ҽ�ѡ�е�list����
			for (int i = 0; i < exFATDirectory[i_ExFATTreeChose].fileList.size(); i++)
			{
				s_get = exFATDirectory[i_ExFATTreeChose].fileList.at(i).s_FileName;
				if (!s_get.Compare(s_temp)) //��ȷ���0
				{
					//�����ַ������
					//�ַ�����ȣ����ҵ��ĵ�����ļ���ɾ�������ļ����ļ��ָ��Ҽ��˵��ſ��á�
					if (exFATDirectory[i_ExFATTreeChose].fileList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFile, MF_ENABLED);
						exFatFile_restore = exFATDirectory[i_ExFATTreeChose].fileList.at(i);
					}
					break; //����forѭ��
				}
			}
		}

		if (i_fileSystem == 0) //�����NTFS�ļ�ϵͳ�����±ߵĴ����߼�
		{
			//������ѡ�еõ���mft��¼���vector��Ŀ¼�б��в����Ƿ�����Ҽ�ѡ�е�list����
			for (int i = 0; i < mftDirectory[i_mftTreeChose].directoryList.size(); i++)
			{
				s_get = mftDirectory[i_mftTreeChose].directoryList.at(i).mft30h.s_fileName;
				if (!s_get.Compare(s_temp)) //��ȷ���0
				{
					//�����ַ������
					//�ַ�����ȣ����ҵ��ĵ�����ļ�����ɾ�������ļ��У��ļ����Ҽ��˵��ſ��á�
					if (mftDirectory[i_mftTreeChose].directoryList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_ENABLED);
						NTFSdirectory_restore = mftDirectory[i_mftTreeChose].directoryList.at(i);
					}
					break; //����forѭ��
				}
			}
			//������ѡ�еõ���mft��¼���vector���ļ��б��в����Ƿ�����Ҽ�ѡ�е�list����
			for (int i = 0; i < mftDirectory[i_mftTreeChose].fileList.size(); i++)
			{
				s_get = mftDirectory[i_mftTreeChose].fileList.at(i).mft30h.s_fileName;
				if (!s_get.Compare(s_temp)) //��ȷ���0
				{
					//�����ַ������
					//�ַ�����ȣ����ҵ��ĵ�����ļ���ɾ�������ļ����ļ��ָ��Ҽ��˵��ſ��á�
					if (mftDirectory[i_mftTreeChose].fileList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFile, MF_ENABLED);
						NTFSfile_restore = mftDirectory[i_mftTreeChose].fileList.at(i);
					}
					break; //����forѭ��
				}
			}
		}
		

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	*pResult = 0;
}

//�ļ��ָ��������
void CDataSaverDlg::OnRestorefile()
{
	// TODO:  �ڴ���������������
	if (i_fileSystem==0)
		restoreNTFSFile("", NTFSfile_restore);
	if (i_fileSystem == 1)
		restoreExFATFile("", exFatFile_restore);

}
void CDataSaverDlg::restoreExFATFile(CString s_path, ExFatFile rf)
{
	WORD i_start; //���ݿ�ʼ��ƫ����
	unsigned char diskSector[1024]; //
	DiskFunction df; //��ȡ�����õ�ʵ����
	
	ExFATFunction ns(exFatBPB); //ExFAT�ĵĹ��ܺ���
	
	byte *b_large = new byte[1024 * i_cu * 512]; //���ÿ��64����������32M���ڴ棬�����ָ����ļ���
	if (s_path.IsEmpty()) //�����ļ��ָ�
	{
		//�����ļ���ѡ��Ի���
		TCHAR filePath[1024];
		BROWSEINFO bi;
		bi.hwndOwner = this->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = TEXT("��ѡ������ļ����ļ���");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		//ÿ�ζ�һ����
		unsigned char * cuSector = new unsigned char[512 * i_cu];
		if (pidl == NULL)
			return;
		if (SHGetPathFromIDList(pidl, filePath))
		{
			CString s_get = filePath; //��·���洢������
			CString s = "\\";
			CString s_temp;
			if (s_get.GetLength() < 4)
			{
				s_temp = s_get + rf.s_FileName; //ѡ���·�����ü�"\"
			}
			else
			{
				s_temp = s_get + s + rf.s_FileName; //ѡ���ļ���·����Ҫ��ת���
			}
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb");
			DWORD64 i_remain = 0; //ʣ��Ļ�û��д�뵽�ļ��е��ֽ�
			i_remain = rf.i_fileSize; //��ʼ��СΪ�ļ����ֽ���
			DWORD32 i_sectorPerCu = 512 * i_cu; //ÿ�ص��ֽ�����
			//д��ָ�����ֽ�
			DWORD32 i_cuSize = rf.i_fileSize / (i_cu * 512); //�õ��ļ���ռ�õĴ������������Ҫ���������������
			if (rf.i_fileSize % (i_cu * 512) != 0) //�������������ռ�õĴ���Ҫ��1
				i_cuSize++;
			
			DWORD32 i_begin = rf.i_beginCu;   //��ʼ��
			
			unsigned _int64 i_sector; //�����������
			DWORD32 d_temp = i_cuSize; //�Ѿ���ȡ�Ĵ���
			//����������1024�ص�ʱ���ļ��Ƚϴ�һ�ζ�1024��
			if (i_cuSize > 1024)
				{
					for (int k = 0; k < i_cuSize - 1024; k += 1024)
					{
							i_sector = ns.getSectorFromCu(i_begin);
							i_begin += 1024;
							df.ReadDiskByNumber(s_driver, b_large, i_sector, i_cu * 1024);
							fwrite(b_large, 1, 512 * i_cu * 1024, f);
							i_remain -= 512 * i_cu * 1024;
							d_temp -= 1024;

						}
					}
					for (int j = 0; j < d_temp; j++)
					{
						i_sector = ns.getSectorFromCu(i_begin + j); //ÿ���ر�Ŷ�Ӧ������
						df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //�����������
						int k;
						if (i_remain > i_sectorPerCu)  //���ʣ��δд�ֽ�������ÿ�ص��ֽ���������ÿ��дһ��
						{
							fwrite(cuSector, 1, 512 * i_cu, f); //�ļ���дһ������
							i_remain -= i_sectorPerCu;
						}
						else
						{
							fwrite(cuSector, 1, i_remain, f); //�ļ���дʣ�಻��1�ص�����
						}
					}
				
				fclose(f);
				MessageBox(rf.s_FileName + "�ɹ����ָ�", TITLE, MB_ICONINFORMATION);
			}
	}
	
	else //�ļ��лָ��ݹ�����
	{	//ÿ�ζ�һ����
		unsigned char * cuSector = new unsigned char[512 * i_cu];
		CString s_temp = s_path + rf.s_FileName;
		FILE *f;
		int i_file = fopen_s(&f, s_temp, "wb");
		DWORD64 i_remain = 0; //ʣ��Ļ�û��д�뵽�ļ��е��ֽ�
		i_remain = rf.i_fileSize; //��ʼ��СΪ�ļ����ֽ���
		DWORD32 i_sectorPerCu = 512 * i_cu; //ÿ�ص��ֽ�����
		//д��ָ�����ֽ�
		DWORD32 i_begin = rf.i_beginCu;   //��ʼ��
		unsigned _int64 i_sector; //�����������
		DWORD32 d_temp = rf.i_fileSize / i_sectorPerCu; //�Ѿ���ȡ�Ĵ���
		if (rf.i_fileSize%i_sectorPerCu != 0)
			d_temp++;
		DWORD32 i_cuSize = d_temp;
		//����������1024�ص�ʱ���ļ��Ƚϴ�һ�ζ�1024��
		if (i_cuSize > 1024)
			{
				for (int k = 0; k < i_cuSize - 1024; k += 1024)
					{
						i_sector = ns.getSectorFromCu(i_begin);
						i_begin += 1024;
						df.ReadDiskByNumber(s_driver, b_large, i_sector, i_cu * 1024);
						fwrite(b_large, 1, 512 * i_cu * 1024, f);
						i_remain -= 512 * i_cu * 1024;
						d_temp -= 1024;
					}
				}
				for (int j = 0; j < d_temp; j++)
				{
					i_sector = ns.getSectorFromCu(i_begin + j); //ÿ���ر�Ŷ�Ӧ������
					df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //�����������
					int k;
					if (i_remain > i_sectorPerCu)  //���ʣ��δд�ֽ�������ÿ�ص��ֽ���������ÿ��дһ��
					{
						fwrite(cuSector, 1, 512 * i_cu, f); //�ļ���дһ������
						i_remain -= i_sectorPerCu;
					}
					else
					{
						fwrite(cuSector, 1, i_remain, f); //�ļ���дʣ�಻��1�ص�����
					}
				}
			
			fclose(f);
	
	}
	delete b_large; //����Ҫ�������ǵ��ͷ��ڴ棬��Ȼ�ڴ�����ռ��
}
//�ļ��лָ��������
void CDataSaverDlg::OnRestorefolder()
{
	//�����ļ���ѡ��Ի���
	TCHAR filePath[1024];
	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("��ѡ�������Ҫ�ָ��ļ�������ŵ�·��");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL)
		return;
	if (SHGetPathFromIDList(pidl, filePath))
	{
		
		if (i_fileSystem == 1) //ExFAT�ļ��лָ�
		{
			CString s = filePath;
			CString s_folder;
			if (s.GetLength() < 4) //��·��
			{
				s_folder = filePath + exFatFolder_restore.s_FolderName;
			}
			else //���Ǹ�·����Ҫ��·������
			{
				CString s_temp1 = "\\";
				s_folder = filePath + s_temp1 + exFatFolder_restore.s_FolderName;
			}
			CreateDirectory(s_folder, NULL);
			CString s_path = s_folder + "\\"; //��Ҫ���ݸ�Ҫ�ָ��ļ���·��
			
			queue<extFatDirectoryRestoreQueue> subDirectory;
			extFatDirectoryRestoreQueue drq;//�ڵ����
			drq.s_path = s_path; //���ļ������ڵ�·��
			for (int i = 0; i < exFATDirectory[exFatFolder_restore.i_cu].fileList.size(); i++)  //����Ŀ¼�µ������ļ��ָ�
			{
				restoreExFATFile(s_path, exFATDirectory[exFatFolder_restore.i_cu].fileList.at(i)); //����ExFATFile��vector�ָ�

			}
			for (int i = 0; i < exFATDirectory[exFatFolder_restore.i_cu].directoryList.size(); i++) //��Ŀ¼����Ŀ¼���������
			{
				extFatDirectoryRestoreQueue drq;//�ڵ����
				drq.s_path = s_path; //���ļ������ڵ�·��
				drq.exFatDirectory = exFATDirectory[exFatFolder_restore.i_cu].directoryList.at(i);
				subDirectory.push(drq);  //queue�Ĳ��뺯��������list���޸ĺ���
				//	subDirectory.push_back(drq);
			}
			//������зǿգ�����еݹ����
			CString s_subDirectoryPath;

			//����ʹ��ָ��
			while (!subDirectory.empty())
			{
				extFatDirectoryRestoreQueue *nd = &subDirectory.front(); //�õ���ͷԪ��
				s_subDirectoryPath = nd->s_path + nd->exFatDirectory.s_FolderName; //�õ����ļ��е�·��
				//�������ļ���
				CreateDirectory(s_subDirectoryPath, NULL);
				s_subDirectoryPath = s_subDirectoryPath + "\\";
				for (int i = 0; i < exFATDirectory[nd->exFatDirectory.i_cu].fileList.size(); i++)
				{
					restoreExFATFile(s_subDirectoryPath, exFATDirectory[nd->exFatDirectory.i_cu].fileList.at(i));
				}
				for (int i = 0; i < exFATDirectory[nd->exFatDirectory.i_cu].directoryList.size(); i++)
				{
					extFatDirectoryRestoreQueue drq;//�ڵ����
					drq.s_path = s_subDirectoryPath; //���ļ������ڵ�·��
					drq.exFatDirectory = exFATDirectory[nd->exFatDirectory.i_cu].directoryList.at(i);
					subDirectory.push(drq); //queue�Ĳ��뺯��������list���޸ĺ���
					//subDirectory.push_back(drq);
				}
				nd = NULL;
				subDirectory.pop();
			}
		}
		if (i_fileSystem == 0) //NTFS�ļ��ָ�
		{
			CString s = filePath;
			CString s_folder;
			if (s.GetLength() < 4) //��·��
			{
				s_folder = filePath + NTFSdirectory_restore.mft30h.s_fileName;
			}
			else //���Ǹ�·����Ҫ��·������
			{
				CString s_temp1 = "\\";
				s_folder = filePath + s_temp1 + NTFSdirectory_restore.mft30h.s_fileName;
			}
			CreateDirectory(s_folder, NULL);
			CString s_path = s_folder + "\\"; //��Ҫ���ݸ�Ҫ�ָ��ļ���·��
			
			queue<DirectoryRestoreQueue> subDirectory;
			DirectoryRestoreQueue drq;//�ڵ����
			drq.s_path = s_path; //���ļ������ڵ�·��
			for (int i = 0; i < mftDirectory[NTFSdirectory_restore.i_mft].fileList.size(); i++)  //����Ŀ¼�µ������ļ��ָ�
			{
				restoreNTFSFile(s_path, mftDirectory[NTFSdirectory_restore.i_mft].fileList.at(i)); //����NTFSFile��vector�ָ�

			}
			for (int i = 0; i < mftDirectory[NTFSdirectory_restore.i_mft].directoryList.size(); i++) //��Ŀ¼����Ŀ¼���������
			{
				DirectoryRestoreQueue drq;//�ڵ����
				drq.s_path = s_path; //���ļ������ڵ�·��
				drq.ntfsDirectory = mftDirectory[NTFSdirectory_restore.i_mft].directoryList.at(i);
				subDirectory.push(drq);  //queue�Ĳ��뺯��������list���޸ĺ���
				//	subDirectory.push_back(drq);
			}
			//������зǿգ�����еݹ����
			CString s_subDirectoryPath;

			//����ʹ��ָ��
			while (!subDirectory.empty())
			{
				DirectoryRestoreQueue *nd = &subDirectory.front(); //�õ���ͷԪ��
				s_subDirectoryPath = nd->s_path + nd->ntfsDirectory.mft30h.s_fileName; //�õ����ļ��е�·��
				//�������ļ���
				CreateDirectory(s_subDirectoryPath, NULL);
				s_subDirectoryPath = s_subDirectoryPath + "\\";
				for (int i = 0; i < mftDirectory[nd->ntfsDirectory.i_mft].fileList.size(); i++)
				{
					restoreNTFSFile(s_subDirectoryPath, mftDirectory[nd->ntfsDirectory.i_mft].fileList.at(i));
				}
				for (int i = 0; i < mftDirectory[nd->ntfsDirectory.i_mft].directoryList.size(); i++)
				{
					DirectoryRestoreQueue drq;//�ڵ����
					drq.s_path = s_subDirectoryPath; //���ļ������ڵ�·��
					drq.ntfsDirectory = mftDirectory[nd->ntfsDirectory.i_mft].directoryList.at(i);
					subDirectory.push(drq); //queue�Ĳ��뺯��������list���޸ĺ���
					//subDirectory.push_back(drq);
				}
				nd = NULL;
				subDirectory.pop();
			}
		}
		
		MessageBox(NTFSdirectory_restore.mft30h.s_fileName + "�ɹ����ָ�", TITLE, MB_ICONINFORMATION);
	}
}


void CDataSaverDlg::OnBnClickedAbout()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	About ab;
	ab.DoModal();
}




HBRUSH CDataSaverDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBitmapDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	if (pWnd == GetDlgItem(IDC_WARN))
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	return hbr;
}
