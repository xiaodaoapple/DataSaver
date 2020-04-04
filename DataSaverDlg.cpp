
// DataSaverDlg.cpp : 实现文件
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
// CDataSaverDlg 对话框
CDataSaverDlg::CDataSaverDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CDataSaverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataSaverDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);

	//此处将按钮图片变量和实际按钮进行绑定
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


// CDataSaverDlg 消息处理程序

BOOL CDataSaverDlg::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();

	// TODO:  开始进行自定义皮肤
	if (!SetBitmap(IDB_BACKGROUND, true, RGB(128, 0, 0)))
	{
		AfxMessageBox("加载皮肤失败！");
		return FALSE;
	}
	SetStaticTransparent(TRUE);
	SetClickAnywhereMove(TRUE);

	//按钮加载图片
	bmpNext.LoadAllBitmaps(IDB_NEXTBTN, 4);
	bmpClose.LoadAllBitmaps(IDB_CLOSEBTN, 4);
	bmpAbout.LoadAllBitmaps(IDB_ABOUT, 4);
	bmpBack.LoadAllBitmaps(IDB_BACK, 4);
	
	//字体设置
	font.CreatePointFont(300, _T("微软雅黑"));
	ShowGuide.SetFont(&font);
	ShowGuide.SetWindowTextA(DISKWARN);
	
	TextShow.SetWindowTextA(AUTHOR);
	//GetDlgItem(IDC_WARN)->SetFont(&font);

//	TextShow.ShowWindow(HIDE)
	//开始初始化工作
//	LoadImg.ShowWindow
//	LoadImg.ShowWindow(0);
	
	//TextShow.SetWindowTextA("hello");
	initVariable(); //初始化所有变量
	listAllDisk(); //加载所有硬盘
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataSaverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBitmapDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//初始化所有系统变量
void CDataSaverDlg::initVariable()
{
	//加载菜单资源
	menu_restore.LoadMenuA(IDR_Restore);
	//设置imagelist ，以下是序号的含义
	s_driver = "";
	imageListPartiton.Create(64, 64, ILC_COLOR24 | ILC_MASK, 1, 1);
	imageListPartiton.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISK)));
	//分区显示的listControl控件加载imagelist
	listPartition.SetImageList(&imageListPartiton, LVSIL_NORMAL);

	//添加5个状态图标，分别是磁盘-0、文件夹-1、文件-2、文件夹被删除-3、文件被删除-4
	imageListPartitonFolderFile.Create(24, 24, ILC_COLOR24 | ILC_MASK, 5, 1);
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISK)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FOLDER)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FILE)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FOLDERDELETE)));
	imageListPartitonFolderFile.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FILEDELETE)));
	//树状控件和文件夹文件 控件加载图标
	folderTree.SetImageList(&imageListPartitonFolderFile, LVSIL_NORMAL);
	fileList.SetImageList(&imageListPartitonFolderFile, LVSIL_SMALL);

	i_window = 1;//初始状态为硬盘选择模式
	listPartition.ShowWindow(HIDE_WINDOW); //分区列表隐藏
	folderTree.ShowWindow(HIDE_WINDOW); //文件夹树隐藏
	fileList.ShowWindow(HIDE_WINDOW); //文件列表隐藏
	bmpBack.ShowWindow(HIDE_WINDOW);  //上一步按钮隐藏

	//初始化分区盘符和分区大小的关系
	int DiskCount = 0;
	//利用GetLogicalDrives()函数可以获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据。
	DWORD DiskInfo = GetLogicalDrives();
	while (DiskInfo)//通过循环操作查看每一位数据是否为1，如果为1则磁盘为真,如果为0则磁盘不存在。
	{
		if (DiskInfo & 1)//通过位运算的逻辑与操作，判断是否为1
		{
			++DiskCount;
		}
		DiskInfo = DiskInfo >> 1;//通过位运算的右移操作保证每循环一次所检查的位置向右移动一位。
	}
	TRACE("逻辑磁盘数量:%d", DiskCount);
	int DSLength = GetLogicalDriveStrings(0, NULL); //当长度不够时，返回需要的长度
	//通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度。
	char* DStr = new char[DSLength];//用获取的长度在堆区创建一个c风格的字符串数组
	GetLogicalDriveStrings(DSLength, (LPTSTR)DStr);

	//通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
	int DType;
	int si = 0;
	BOOL fResult;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;
	for (int i = 0; i<DSLength / 4; ++i)
		//为了显示每个驱动器的状态，则通过循环输出实现，由于DStr内部保存的数据是A:\NULLB:\NULLC:\NULL，这样的信息，所以DSLength/4可以获得具体大循环范围
	{

		//char dir[3] = { DStr[si], ':', '\\' };
		char dir[4] = { DStr[si], ':', '\\','\0' };
		DType = GetDriveType(DStr + i * 4);
		fResult = GetDiskFreeSpaceExA(
			dir,(PULARGE_INTEGER)&i64FreeBytesToCaller,(PULARGE_INTEGER)&i64TotalBytes,(PULARGE_INTEGER)&i64FreeBytes);
		//GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据
		
		diskPartiton[i].hasChoose = false; //设置磁盘未被初始化
		diskPartiton[i].s_diskVolume = dir; //设置磁盘盘符
		diskPartiton[i].partitionSize = i64TotalBytes /512; //设置磁盘的大小，单位为扇区
		si += 4;
	}
}


void CDataSaverDlg::OnBnClickedNext()
{
	// TODO:  在此添加控件通知处理程序代码
	POSITION pos;
	switch (i_window)
	{
	case 0:
		listPartition.ShowWindow(HIDE_WINDOW); folderTree.ShowWindow(HIDE_WINDOW); fileList.ShowWindow(HIDE_WINDOW);
		break;
	case 1:
		//开始处理选择的磁盘
		int i_select;
		i_select = listDisk.GetCurSel();
		if (i_select < 0)
		{
			MessageBox("请选择需要恢复的硬盘", TITLE, MB_ICONEXCLAMATION);
		}
		else
		{
			bmpBack.ShowWindow(SHOW_OPENWINDOW);  //上一步按钮显示
			
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
			MessageBox("请选择需要恢复的分区", TITLE, MB_ICONEXCLAMATION);
			
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
				
				//判断分区类型，对文件进行遍历	
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
			ShowGuide.ShowWindow(HIDE_WINDOW); //提示文字隐藏
			
			TextShow.SetWindowTextA(RESTOREINFO);
			
			i_window = 3;
			bmpNext.ShowWindow(HIDE_WINDOW); //最后一个界面，隐藏下一步按钮
		}
		break;
	default:
		break;
	}
}
void CDataSaverDlg::listAllExFATFolder()
{
	exFATDirectory.clear();  //map映射对清空
	queue<ExFatFolder> folderQueue;  //建立文件夹队列
	fileList.DeleteAllItems();
	bool b_delete = false; //判断文件或者文件夹是否是删除了的
	folderTree.DeleteAllItems(); //树状控件清空
	//以下代码得到ExFAT的DBR信息
	i_PartitionBegin = pif.partitonBegin;
	unsigned char diskSector[512]; //定义存储扇区用的字节
	DiskFunction df; //读取磁盘用的实体类
	//打开磁盘，读取BPB参数
	df.ReadDisk(s_driver, diskSector, i_PartitionBegin);
	exFatBPB.i_cu = pow(2, diskSector[0x6D]); //每簇扇区数目
	i_cu = exFatBPB.i_cu;
	exFatBPB.i_cuBegin = df.byte2Int(&diskSector[0x58], 4) + pif.partitonBegin;
	exFatBPB.i_FATBegin = df.byte2Int(&diskSector[0x50], 4) + pif.partitonBegin;
	exFatBPB.i_FATNumber = df.byte2Int(&diskSector[0x54], 4);
	exFatBPB.i_rootCu = df.byte2Int(&diskSector[0x60], 4);
	exFatBPB.s_driver = s_driver;
	ExFATFunction eff(exFatBPB);
	//到根目录区进行解析
	//得到根目录所占的簇数列表
	vector<UINT32> cuVector;
	cuVector = eff.getDiretoryCuVector(exFatBPB.i_rootCu); //得到根目录所占的簇数
	//定义目录所占的簇数，切记最后需要释放内存
	unsigned char * directorySector = new unsigned char[512 * i_cu*cuVector.size()];
	UINT32 i_sector;
	for (int i = 0; i < cuVector.size(); i++)
	{
		i_sector = eff.getSectorFromCu(cuVector.at(i)); //得到簇所在的扇区
		df.ReadDiskByNumber(s_driver, directorySector + i * 512 * i_cu, i_sector, i_cu);
	}
		
	//开始解析目录簇中的内容，根目录前3个32字节开头分别为0x83(卷标） 0x81(簇位图开始） 0x82（大写字符开始)
	for (int i = 0; i < 512 * i_cu*cuVector.size(); i = i + 32)
	{
		b_delete = false; //设置文件删除标志为假
		if (directorySector[i] == 0) //如果为0，说明下边的数据为空，需要退出
			break;
		if (directorySector[i] != 0x85 && directorySector[i]!=0x05)
			continue;
		else
		{
			if (directorySector[i] == 0x05)  //文件或者文件夹被删除，会将首字符变为0x05
				b_delete = true;
			UINT32 i_sx = df.byte2Int(&directorySector[i + 4], 4); //得到属性
			i_sx = (i_sx >> 4) & 0x1;  //判断其第五位，如果是1，则是文件夹，否则为文件
			if (i_sx ==1) //接下来解析的是目录
			{
				//定义ExFAT子目录
				ExFatFolder fatFolder;
				if (b_delete)
					fatFolder.b_delete = true;
				else
					fatFolder.b_delete = false;
				
				fatFolder.s_FolderTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //得到文件夹日期
				
				fatFolder.i_cu = df.byte2Int(&directorySector[i + 52], 4); //得到文件开始簇
				int i_number = directorySector[i + 35]; //文件名的字符数，采用UNICODE16编码
				//得到文件名
				fatFolder.s_FolderName = df.getExFATFileName(&directorySector[i + 64], i_number);	
				
				exFATDirectory[exFatBPB.i_rootCu].directoryList.push_back(fatFolder); //将文件夹压入mapvector
				folderQueue.push(fatFolder);  //将文件夹压入队列
				}
			else
			{
				//定义ExFAT文件
				ExFatFile fatFile;
				
				if (b_delete)
					fatFile.b_delete = true;
				else
					fatFile.b_delete = false;

				//文件连续存放，标志位为3，否则标志位为1
				if (directorySector[i + 33] == 3)
					fatFile.b_series = true;
				else
					fatFile.b_series = false;
				fatFile.s_FileTime= df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //得到文件日期
				fatFile.i_fileSize = df.byte2Int(&directorySector[i + 40], 8); //得到文件大小
				fatFile.i_beginCu = df.byte2Int(&directorySector[i + 52], 4); //得到文件开始簇
				int i_number = directorySector[i + 35]; //文件名的字符数，采用UNICODE16编码
				//得到文件名
				fatFile.s_FileName = df.getExFATFileName(&directorySector[i + 64], i_number);
				
				exFATDirectory[exFatBPB.i_rootCu].fileList.push_back(fatFile); //将文件压入vector
				}

			}
		}
	cuVector.clear();
	delete directorySector; //释放内存
	//队列为空时，结束
	while (!folderQueue.empty())
	{
		ExFatFolder folderNode;
		folderNode = folderQueue.front(); //返回队头元素
		folderQueue.pop(); //将队头元素删除
		DWORD32 i_folderCu = folderNode.i_cu; //得到当前目录的簇号码

		cuVector = eff.getDiretoryCuVector(i_folderCu); //得到子目录所在簇所占的簇数
		//定义目录所占的簇数，切记最后需要释放内存
		unsigned char * directorySector = new unsigned char[512 * i_cu*cuVector.size()];
		UINT32 i_sector;
		for (int i = 0; i < cuVector.size(); i++)
		{
			i_sector = eff.getSectorFromCu(cuVector.at(i)); //得到簇所在的扇区
			df.ReadDiskByNumber(s_driver, directorySector + i * 512 * i_cu, i_sector, i_cu);
		}
		//开始解析目标簇中的内容
		for (int i = 0; i < 512 * i_cu*cuVector.size(); i = i + 32)
		{
			if (directorySector[i] == 0) //如果为0，说明下边的数据为空，需要退出
				break;
			if (directorySector[i] != 0x85 && directorySector[i] != 0x05)
				continue;
			else
			{
				UINT32 i_sx = df.byte2Int(&directorySector[i + 4], 4); //得到属性
				i_sx = (i_sx >> 4) & 0x1;  //判断其第五位，如果是1，则是文件夹，否则为文件
				if (i_sx == 1) //接下来解析的是目录
				{
					//定义ExFAT子目录
					ExFatFolder fatFolder;
					fatFolder.b_delete = false;
					fatFolder.s_FolderTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //得到文件夹日期
					fatFolder.i_cu = df.byte2Int(&directorySector[i + 52], 4); //得到文件开始簇
					int i_number = directorySector[i + 35]; //文件名的字符数，采用UNICODE16编码
					//得到文件名
					fatFolder.s_FolderName = df.getExFATFileName(&directorySector[i + 64], i_number);
					exFATDirectory[i_folderCu].directoryList.push_back(fatFolder); //将文件夹压入mapvector
					folderQueue.push(fatFolder);  //将文件夹压入队列
				}
				else
				{
					//定义ExFAT文件
					ExFatFile fatFile;
					fatFile.b_delete = false;
					//文件连续存放，标志位为3，否则标志位为1
					if (directorySector[i + 33] == 3)
						fatFile.b_series = true;
					else
						fatFile.b_series = false;
					fatFile.s_FileTime = df.getExFATTime(&directorySector[i + 8], directorySector[i + 20]); //得到文件日期
					fatFile.i_fileSize = df.byte2Int(&directorySector[i + 40], 8); //得到文件大小
					fatFile.i_beginCu = df.byte2Int(&directorySector[i + 52], 4); //得到文件开始簇
					int i_number = directorySector[i + 35]; //文件名的字符数，采用UNICODE16编码
					//得到文件名
					fatFile.s_FileName = df.getExFATFileName(&directorySector[i + 64], i_number);
					exFATDirectory[i_folderCu].fileList.push_back(fatFile); //将文件压入vector
				}
			}
		}
		delete directorySector; //释放内存
	}
		

		//开始树状控件显示
		HTREEITEM hRoot;     // 树的根节点的句柄     
		hRoot = folderTree.InsertItem("磁盘根目录", 0, 0);
		folderTree.SetItemData(hRoot, exFatBPB.i_rootCu); //将ExFAT根目录设置到树根目录中

		folderTree.SetItemData(hRoot, exFatBPB.i_rootCu); //将5号MFT设置到根目录中
		
		i_ExFATTreeChose = exFatBPB.i_rootCu; //默认5号根目录已经选中
		
		//建立队列
		queue<ExFATDirectoryQueueNode> directoryQueue;
		
		//将根目录的所有子目录放置到队列里
		for (int i = 0; i < exFATDirectory[exFatBPB.i_rootCu].directoryList.size(); i++)
		{
			HTREEITEM hFolder; // 可表示任一文件夹节点的句柄 
			ExFATDirectoryQueueNode folderNode;
			
			if (exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).b_delete == false)
				hFolder = folderTree.InsertItem(exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 1, 1, hRoot, TVI_LAST);
			else
				hFolder = folderTree.InsertItem(exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).s_FolderName, 3, 3, hRoot, TVI_LAST);

			//将本目录的MFT编号作为信息存储到树节点上
			folderTree.SetItemData(hFolder, exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i).i_cu);
			folderNode.hFolder = hFolder;
			folderNode.fatDirectory = exFATDirectory[exFatBPB.i_rootCu].directoryList.at(i);
			directoryQueue.push(folderNode);
		}
		//队列为空时，结束
		
		while (!directoryQueue.empty())
		{
			ExFATDirectoryQueueNode folderNode;
			folderNode = directoryQueue.front(); //返回队头元素
			directoryQueue.pop(); //将队头元素删除
			UINT32 i_folderCu = folderNode.fatDirectory.i_cu; //得到当前目录的簇所在位置

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
				//将本目录的簇编号作为信息存储到树节点上
				folderTree.SetItemData(hItem, exFATDirectory[i_folderCu].directoryList.at(i).i_cu);
				//将当前目录的下一级目录放入队列
				
				ExFATDirectoryQueueNode folderNodeTemp;
				folderNodeTemp.hFolder = hItem;
				folderNodeTemp.fatDirectory = exFATDirectory[i_folderCu].directoryList.at(i);
				directoryQueue.push(folderNodeTemp);
			}
		}
		
		folderTree.Expand(hRoot, TVE_EXPAND);
		CRect rect;
		// 获取编程语言列表视图控件的位置和大小   
		fileList.GetClientRect(&rect);
		//为列表视图控件添加全行选中和栅格风格
		fileList.SetExtendedStyle(fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
		fileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, rect.Width() / 3, 0);
		fileList.InsertColumn(1, _T("日期"), LVCFMT_CENTER, rect.Width() / 3, 1);
		fileList.InsertColumn(2, _T("大小"), LVCFMT_CENTER, rect.Width() / 3, 2);
		// 在列表视图控件中插入列表项，并设置列表子项文本   
		//将根目录的所有文件夹和文件添加到list控件里
		int k = 0;
		for (int i = 0; i < exFATDirectory[exFatBPB.i_rootCu].directoryList.size(); i++)
		{
			
			CString s_temp;
			//插入一项
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
			//插入一项，其索引为 ?编号，内容标签为文件名称 需要考虑索引
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
			//插入文件大小
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
	// TODO:  在此添加控件通知处理程序代码
	switch (i_window)
	{
	case 0: //初始状态
		break;
	case 1: 
		
		break;
	case 2://分区显示界面
		bmpBack.ShowWindow(HIDE_WINDOW); //点击后返回到1界面
		i_window = 1;
		listDisk.ShowWindow(SHOW_OPENWINDOW); //磁盘列表显示
		ShowGuide.ShowWindow(SHOW_OPENWINDOW);
		
		ShowGuide.SetWindowTextA(DISKWARN);
		
		listPartition.ShowWindow(HIDE_WINDOW); //分区列表隐藏
		for (int i = 0; i < 26; i++)
			diskPartiton[i].hasChoose = false;
		break;
	case 3: //文件夹显示界面
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
	// TODO:  在此添加控件通知处理程序代码
	if (MessageBox("确定要退出吗？", TITLE, MB_YESNO | MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		PostQuitMessage(0);
	}
}
//读取注册表，并将所有的硬盘添加到listbox列表中去
void CDataSaverDlg::listAllDisk()
{
	//初始化时，需要将被选择的硬盘设置为空
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
				//以下代码从注册表中截取对应的磁盘字符，其全部名称已经放入到了list_disk列表中
				int i_temp1, i_temp2, i_temp3, i_temp4;
				i_temp1 = str.Find('_');
				i_temp2 = str.Find('&', i_temp1);
				i_temp3 = str.Find('_', i_temp2);
				i_temp4 = str.Find('\\', i_temp3);
				str_show = str.Mid(i_temp1 + 1, i_temp2 - i_temp1 - 1) + " " + str.Mid(i_temp2 + 1, i_temp3 - i_temp2 - 1) + " " + str.Mid(i_temp3, i_temp4 - i_temp3);
				//将硬盘截取过后的字符串存储到硬盘信息里。
				d_disk[i].s_diskShow = str_show;
				//j的值就为最终的硬盘数
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
		// 读磁盘的MBR区
		df.ReadDisk(driver, diskSector, 0);
		
		if (diskSector[450] == 0XEE)
		{
			d_disk[i].isMBR = false;
			//GPT磁盘的话，去读取GPT头来计算磁盘的大小
			df.ReadDisk(driver, diskSectorGPT, 1);
			unsigned _int64 i_begin, i_end; //GPT硬盘的起始扇区号和结束扇区号，各占8个字节
			i_begin = df.byte2Int(&diskSectorGPT[40], 8);
			i_end = df.byte2Int(&diskSectorGPT[48], 8);
			//得到磁盘大小，单位为G
			d_disk[i].diskSize = (i_end - i_begin) / (2 * 1024 * 1024);
			CString s_temp;
			s_temp.Format("%u", d_disk[i].diskSize);

			d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " GPT 磁盘" + " " + s_temp + "G";
			listDisk.AddString(d_disk[i].s_diskShow);
			continue;
		}
		else
		{
			d_disk[i].isMBR = true;
			unsigned _int64 i_total = 0;
			//将4个主分区的扇区数相加得到硬盘的总扇区数目
			i_total = df.byte2Int(&diskSector[458], 4) + df.byte2Int(&diskSector[474], 4) + df.byte2Int(&diskSector[490], 4) + df.byte2Int(&diskSector[506], 4);
			d_disk[i].diskSize = i_total / (2 * 1024 * 1024);
			CString s_temp;
			s_temp.Format("%u", d_disk[i].diskSize);
			//如果硬盘不到1G大小，就必须用MB来显示
			if (d_disk[i].diskSize == 0)
			{
				i_total = i_total / (2 * 1024);
				s_temp.Format("%u", i_total);
				d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " MBR 磁盘" + " " + s_temp + "MB";
			}
			else
			{
				d_disk[i].s_diskShow = d_disk[i].s_diskShow + " " + " MBR 磁盘" + " " + s_temp + "G";
			}

		}
		listDisk.AddString(d_disk[i].s_diskShow);
	}

}
//得到物理磁盘的编号
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
//显示所选择的硬盘的所有分区，以便选择
void CDataSaverDlg::listAllPartition(int i_disk_select)
{
	listPartition.DeleteAllItems();
	partitionVector.clear();
	//处理硬盘的字符串
	s_driver = d_disk[i_disk_select].s_driver;
	byte diskSector[512]; //定义存储扇区用的字节
	DiskFunction df; //读取磁盘用的实体类
	//如果磁盘是MBR磁盘，则进行分区的读取
	if (d_disk[i_disk_select].isMBR == true)
	{
		if (df.ReadDisk(d_disk[i_disk_select].s_driver, diskSector, 0) == 0)
		{
			MessageBox("读取扇区失败", TITLE, MB_ICONEXCLAMATION);
			return ;
		}
		else
		{
			//共4个主分区，所以需要4次循环
			for (int i = 0; i < 4; i++)
			{
				int i_type = 450 + i * 16; //450为第一个主分区的标志判定位
				//07表示是主分区，注意NTFS和EXTFAT都是07，需要接着对其进行判断
				if (diskSector[i_type] == 0X07)
				{
					byte partitionSector[512]; //定义存储DBR的字节
					partitionInfor pi;
					
					//pi.fileSystem = "NTFS";
					pi.partitionSize = df.byte2Int(&diskSector[i_type + 8], 4);
					pi.partitonBegin = df.byte2Int(&diskSector[i_type + 4], 4);
					df.ReadDisk(d_disk[i_disk_select].s_driver, partitionSector, pi.partitonBegin); //得到DBR扇区
					if (partitionSector[1] == 0x52)
						pi.fileSystem = "NTFS";
					else
						pi.fileSystem = "ExFAT";
					partitionVector.push_back(pi);
				}
				//0B和0C都是FAT32分区
				if (diskSector[i_type] == 0X0B || diskSector[i_type] == 0X0C)
				{
					partitionInfor pi;
					pi.fileSystem = "FAT32";
					pi.partitionSize = df.byte2Int(&diskSector[i_type + 8], 4);
					pi.partitonBegin = df.byte2Int(&diskSector[i_type + 4], 4);
					partitionVector.push_back(pi);
				}
				

				//05和0F都是扩展分区
				if (diskSector[i_type] == 0X05 || diskSector[i_type] == 0X0F)
				{
					//得到扩展分区的开始扇区数
					unsigned int i_extend_begin = df.byte2Int(&diskSector[i_type + 4], 4);
					//得到扩展分区的总大小
					unsigned int i_extend_total = df.byte2Int(&diskSector[i_type + 8], 4);
					unsigned int i_logical_begin = i_extend_begin;
					//逻辑磁盘和EBR的总大小
					unsigned int i_logical_total = 0;
					//当逻辑扇区的总数目小于总的扩展扇区数目时候，循环迭代
					unsigned char logicalSector[512]; //定义存储逻辑扇区用的字节

					while (i_logical_total < i_extend_total)
					{
						//读取EBR
						df.ReadDisk(d_disk[i_disk_select].s_driver, logicalSector, i_logical_begin);
						partitionInfor pi;
						int i_EBR;
						if (logicalSector[450] == 0X07) //如果是07有可能是NTFS和ExFAT
						{
							
							pi.partitionSize = df.byte2Int(&logicalSector[450 + 8], 4);
							i_EBR = df.byte2Int(&logicalSector[450 + 4], 4);
							pi.partitonBegin = i_EBR + i_logical_begin;


							//NTFS和ExFAT都是07，所以必须去DBR判断
							byte partitionSector[512]; //定义存储DBR的字节
							df.ReadDisk(d_disk[i_disk_select].s_driver, partitionSector, pi.partitonBegin); //得到DBR扇区
							if (partitionSector[1] == 0x52)
								pi.fileSystem = "NTFS";
							else
								pi.fileSystem = "ExFAT";

							partitionVector.push_back(pi);
							i_logical_total = i_logical_total + i_EBR + pi.partitionSize;
						}
						//0B和0C都是FAT32分区
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

	}  //如果是GPT磁盘，需要进入下面的工作流程
	else
	{
		//GPT磁盘从第二个扇区开始，每128个字节构成一个分区项
		for (int i = 2; i < 34; i++)
		{
			if (df.ReadDisk(d_disk[i_disk_select].s_driver, diskSector, i) == 0)
			{
				MessageBox("读取扇区失败", TITLE, MB_ICONEXCLAMATION);
				return ;
			}
			else
			{
				partitionInfor pi;
				unsigned _int64 i_partitionBegin, i_partitionEnd, i_partitionSize, i_partitionName;
				//每个扇区存储4个分区项
				int j;
				int i_begin = 32;  //从i_begin开始的8个字节为分区起始地址
				for (j = 0; j < 4; j++)
				{

					i_partitionBegin = df.byte2Int(&diskSector[i_begin], 8);
					i_partitionEnd = df.byte2Int(&diskSector[i_begin + 8], 8);
					//得到分区大小
					i_partitionSize = i_partitionEnd - i_partitionBegin;
					//当分区为0时，说明后边已经没有分区了
					if (i_partitionSize == 0)
						break;
					//数据区域分区名字为0X42006100 basic
					i_partitionName = df.byte2Int(&diskSector[i_begin + 24], 4);
					//如果分区是数据区，则加入到分区信息vector中
					if (i_partitionName == 0X610042)
					{
						pi.partitonBegin = i_partitionBegin;
						pi.partitionSize = i_partitionEnd - pi.partitonBegin;
						unsigned char dbrSector[512]; //定义存储DBR扇区用的字节

						if (df.ReadDisk(d_disk[i_disk_select].s_driver, dbrSector, pi.partitonBegin) == 0)
						{
							MessageBox("读取扇区失败", TITLE, MB_ICONEXCLAMATION);
							return ;
						}
						//从dbr得到文件系统类型

						pi.fileSystem = df.judgeFileSystem(&dbrSector[3]);


						partitionVector.push_back(pi);
					}
					i_begin = i_begin + 128;
				}
				//分区项后边已经没有分区，没有必要继续读取。
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
			//开始盘符和空间的匹配 经过实际测试发现有正负10个扇区的误差
			int j = 0;
			for (j = 0; j < 26; j++)
				if ((diskPartiton[j].partitionSize >= partitionVector[i].partitionSize - 10 && diskPartiton[j].partitionSize <= partitionVector[i].partitionSize + 10) && diskPartiton[j].hasChoose == false)
				{
					diskPartiton[j].hasChoose = true;
					listPartition.InsertItem(i,diskPartiton[j].s_diskVolume+"\n"+ partitionVector[i].fileSystem + "\n" + s_temp + "MB", 0);
					break;
				}
			//如果没有匹配的盘符，则完整显示
			if (j>=26)
				listPartition.InsertItem(i,partitionVector[i].fileSystem + "\n" + s_temp + "MB", 0);
		}
		else
		{
			s_temp.Format("%u", i_temp);
			//开始盘符和空间的匹配
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
//树状控件加载所有的文件夹和文件
void CDataSaverDlg::listAllNTFSFolder()
{
	mftDirectory.clear();  //map映射对清空
	folderTree.DeleteAllItems(); //树状控件清空
	getNTFSBPBInfor();//得到分区的BPB信息
	//对NTFS功能变量进行初始化
	ns.i_cu = i_cu;
	ns.i_MFT_begin = i_MFT_begin;
	ns.i_PartitionBegin = i_PartitionBegin;
	
	//得到0号文件记录号的MFT，连续读取两个扇区，放入diskSector数组。
	unsigned _int64 i_sector = ns.getNTFSSectorNumber(0);
	unsigned char diskSector[1024]; //定义存储扇区用的字节
	DiskFunction df; //读取磁盘用的实体类


	df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);

	diskSector[510] = diskSector[0x32];
	diskSector[511] = diskSector[0x33];
	diskSector[1022] = diskSector[0x34];
	diskSector[1023] = diskSector[0x35];
	MFT30h mft30;
	MFT80h mft80;
	MFTB0h mftb0;
	mft30 = df.get30h(diskSector); //得到MFT的30h属性
	mft80 = df.get80h(diskSector); //得到MFT的80h属性
	mftb0 = df.getB0h(diskSector);
	UINT32 i_cuSize = 0;
	UINT32 i_mftNumber = 0;
	//得到mft所占的簇数，每簇的扇区数乘以簇数，除以2得到MFT的最大记录号
	for (int i = 0; i < mft80.runlist.size(); i++)
	{
		i_cuSize += mft80.runlist.at(i).i_cuNumber;
	}
	i_mftNumber = i_cuSize*i_cu / 2;

	//以下是测试代码

	//每次读一个簇
	unsigned char * cuSector = new unsigned char[512 * i_cu];
	//当簇很多时候，一个簇读效率太差，增大1024倍
	unsigned char * sectorLarge = new unsigned char[512 * i_cu * 1024]; //定义1024个簇的字节大小，常规1簇为4k，定义了4M空间
	for (int i = 0; i < mft80.runlist.size(); i++)
	{
		unsigned _int64 i_cuBegin = mft80.runlist.at(i).i_cuBegin;
		DWORD i_cuNumber = mft80.runlist.at(i).i_cuNumber; //得到簇的个数

		DWORD32 d_temp = i_cuNumber; //已经读取的簇数
		//当簇数大于1024簇的时候，文件比较大，一次读1024簇
		if (i_cuNumber > 1024)
		{
			for (int j = 0; j < i_cuNumber - 1024; j += 1024)
			{

				i_sector = ns.getNTFSSectorFromCu(i_cuBegin);
				i_cuBegin += 1024;
				df.ReadDiskByNumber(s_driver, sectorLarge, i_sector, i_cu * 1024);
				d_temp -= 1024;
				for (int k = 0; k < 1024 * i_cu / 2; k++) //1024个簇所包含的MFT数目
				{
					sectorLarge[k * 2 * 512 + 510] = sectorLarge[k * 2 * 512 + 0x32];
					sectorLarge[k * 2 * 512 + 511] = sectorLarge[k * 2 * 512 + 0x33];
					sectorLarge[k * 2 * 512 + 1022] = sectorLarge[k * 2 * 512 + 0x34];
					sectorLarge[k * 2 * 512 + 1023] = sectorLarge[k * 2 * 512 + 0x35];
					//判断是否是文件号,如果是，则第一个字母为0x46
					if (sectorLarge[k * 2 * 512] != 0x46)
						continue;
					//判断是否是空的文件号，如果是空的，则0x38位置不为10h
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
					case 0: //文件被删除
						ntfsFile.b_delete = true;
						ntfsFile.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //得到文件的记录号
						ntfsFile.mft30h = df.get30h(sectorLarge + k * 2 * 512);
						ntfsFile.mft80h = df.get80h(sectorLarge + k * 2 * 512);
						//将对应父目录的MFT号的子文件vector添加新文件，如果对应MFT不存在，则会建立
						mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
						break;
					case 1: //文件使用中
						ntfsFile.b_delete = false;
						ntfsFile.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //得到文件的记录号
						ntfsFile.mft30h = df.get30h(sectorLarge + k * 2 * 512);
						ntfsFile.mft80h = df.get80h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
						//mftNode[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile); //将文件加入到父目录所在的文件vector中
						break;
					case 2: //目录被删除
						ntfsDirectory.b_delete = true;
						ntfsDirectory.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //得到目录的记录号
						ntfsDirectory.mft30h = df.get30h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						//mftNode[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						break;
					case 3: //目录使用中
						ntfsDirectory.b_delete = false;
						ntfsDirectory.i_mft = df.byte2Int(sectorLarge + k * 2 * 512 + 0x2c, 4); //得到目录的记录号
						ntfsDirectory.mft30h = df.get30h(sectorLarge + k * 2 * 512);

						mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						//mftNode[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
						break;
					default:
						break;
					}
				} //1024个簇解析结束
			} //簇大于1024时的for情况结束
		}
		//读剩余簇的个数
		for (int j = 0; j < d_temp; j++) //小于1024簇情况下
		{
			i_sector = ns.getNTFSSectorFromCu(i_cuBegin + j);
			df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //得到了一个簇的大小的数据
			//k为MFT记录的个数
			for (int k = 0; k < i_cu / 2; k++)
			{
				cuSector[k * 2 * 512 + 510] = cuSector[k * 2 * 512 + 0x32];
				cuSector[k * 2 * 512 + 511] = cuSector[k * 2 * 512 + 0x33];
				cuSector[k * 2 * 512 + 1022] = cuSector[k * 2 * 512 + 0x34];
				cuSector[k * 2 * 512 + 1023] = cuSector[k * 2 * 512 + 0x35];
				//判断是否是文件号,如果是，则第一个字母为0x46
				if (cuSector[k * 2 * 512] != 0x46)
					continue;
				//判断是否是空的文件号，如果是空的，则0x38位置不为10h
				if (cuSector[k * 2 * 512 + 0x38] != 0x10)
					continue;
				DWORD32 i_mftNumber = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4);
				if (i_mftNumber < 27)
					continue;
				NTFSDirectory ntfsDirectory;
				NTFSFile ntfsFile;
				switch (cuSector[k * 2 * 512 + 0x16])
				{
				case 0: //文件被删除
					ntfsFile.b_delete = true;
					ntfsFile.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //得到文件的记录号
					ntfsFile.mft30h = df.get30h(cuSector + k * 2 * 512);
					ntfsFile.mft80h = df.get80h(cuSector + k * 2 * 512);
					//将对应父目录的MFT号的子文件vector添加新文件，如果对应MFT不存在，则会建立
					mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
					break;
				case 1: //文件使用中
					ntfsFile.b_delete = false;
					ntfsFile.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //得到文件的记录号
					ntfsFile.mft30h = df.get30h(cuSector + k * 2 * 512);
					ntfsFile.mft80h = df.get80h(cuSector + k * 2 * 512);
					mftDirectory[ntfsFile.mft30h.i_father].fileList.push_back(ntfsFile);
					break;
				case 2: //目录被删除
					ntfsDirectory.b_delete = true;
					ntfsDirectory.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //得到目录的记录号
					ntfsDirectory.mft30h = df.get30h(cuSector + k * 2 * 512);
					mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
					break;
				case 3: //目录使用中
					ntfsDirectory.b_delete = false;
					ntfsDirectory.i_mft = df.byte2Int(cuSector + k * 2 * 512 + 0x2c, 4); //得到目录的记录号
					ntfsDirectory.mft30h = df.get30h(cuSector + k * 2 * 512);
					mftDirectory[ntfsDirectory.mft30h.i_father].directoryList.push_back(ntfsDirectory);
					break;
				default:
					break;
				}
			}
		}
	}

	HTREEITEM hRoot;     // 树的根节点的句柄     
	hRoot = folderTree.InsertItem("磁盘根目录", 0, 0);
	folderTree.SetItemData(hRoot, 5); //将5号MFT设置到根目录中
	i_mftTreeChose = 5; //默认5号根目录已经选中
	//建立队列
	queue<DirectoryQueueNode> directoryQueue;
	//将根目录的所有子目录放置到队列里
	for (int i = 0; i < mftDirectory[5].directoryList.size(); i++)
	{
		HTREEITEM hFolder; // 可表示任一文件夹节点的句柄 
		DirectoryQueueNode folderNode;
		if (mftDirectory[5].directoryList.at(i).b_delete == false)
			hFolder = folderTree.InsertItem(mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 1, 1, hRoot, TVI_LAST);
		else
			hFolder = folderTree.InsertItem(mftDirectory[5].directoryList.at(i).mft30h.s_fileName, 3, 3, hRoot, TVI_LAST);

		//将本目录的MFT编号作为信息存储到树节点上
		folderTree.SetItemData(hFolder, mftDirectory[5].directoryList.at(i).i_mft);
		folderNode.hFolder = hFolder;
		folderNode.ntfsDirectory = mftDirectory[5].directoryList.at(i);
		directoryQueue.push(folderNode);
	}
	//队列为空时，结束
	while (!directoryQueue.empty())
	{
		DirectoryQueueNode folderNode;
		folderNode = directoryQueue.front(); //返回队头元素
		directoryQueue.pop(); //将队头元素删除
		unsigned _int64 i_mft = folderNode.ntfsDirectory.i_mft; //得到当前目录的mft号码

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
			//将本目录的MFT编号作为信息存储到树节点上
			folderTree.SetItemData(hItem, mftDirectory[i_mft].directoryList.at(i).i_mft);
			//将当前目录的下一级目录放入队列
			DirectoryQueueNode folderNodeTemp;
			folderNodeTemp.hFolder = hItem;
			folderNodeTemp.ntfsDirectory = mftDirectory[i_mft].directoryList.at(i);
			directoryQueue.push(folderNodeTemp);
		}
	}
	folderTree.Expand(hRoot, TVE_EXPAND);
	CRect rect;
	// 获取编程语言列表视图控件的位置和大小   
	fileList.GetClientRect(&rect);
	//为列表视图控件添加全行选中和栅格风格
	fileList.SetExtendedStyle(fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	fileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, rect.Width() / 3, 0);
	fileList.InsertColumn(1, _T("日期"), LVCFMT_CENTER, rect.Width() / 3, 1);
	fileList.InsertColumn(2, _T("大小"), LVCFMT_CENTER, rect.Width() / 3, 2);
	// 在列表视图控件中插入列表项，并设置列表子项文本   
	//将根目录的所有文件夹和文件添加到list控件里
	int k = 0;
	for (int i = 0; i < mftDirectory[5].directoryList.size(); i++)
	{
		SYSTEMTIME st;
		CString s_temp;
		//插入一项
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
		//插入一项，其索引为MFT编号，内容标签为30名称
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
		//插入文件大小
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
//得到BPB相关信息
void CDataSaverDlg::getNTFSBPBInfor()
{
	i_PartitionBegin = pif.partitonBegin;
	unsigned char diskSector[512]; //定义存储扇区用的字节
	DiskFunction df; //读取磁盘用的实体类

	//打开磁盘，读取BPB参数
	if (df.ReadDisk(s_driver, diskSector, i_PartitionBegin) == 0)
	{
		MessageBox("读取扇区失败", TITLE, MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		i_cu = diskSector[13];
		i_MFT_begin = df.byte2Int(&diskSector[48], 8);
	}
}
//点击树状列表时，刷新文件列表对话框
void CDataSaverDlg::OnTvnSelchangedFoldertree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	UINT32 i_get; //点击的树中的文件夹的MFT编号
	HTREEITEM hItem = folderTree.GetSelectedItem();
	*pResult = 0;
	i_get = folderTree.GetItemData(hItem);
	//如果文件系统标志是1，则点击的是ExFAT
	if (i_fileSystem == 1)
	{
		i_FATCuTreeChoose = i_get;
		//清空List中所有的内容
		fileList.DeleteAllItems();
		//将树中选中的目录的所有文件夹和文件添加到list控件里
		int k = 0;
		for (int i = 0; i < exFATDirectory[i_get].directoryList.size(); i++)
		{
			CString s_temp;
			//插入一项
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
			//插入一项，其索引为MFT编号，内容标签为30名称
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
			//插入文件大小
			s_temp.Format("%d", exFATDirectory[i_get].fileList.at(i).i_fileSize / 1024);
			s_temp += "KB";
			fileList.SetItemText(nRow, 2, s_temp);

			s_temp = exFATDirectory[i_get].fileList.at(i).s_FileTime;
			fileList.SetItemText(nRow, 1, s_temp);
			k++;
		}
	}
	//如果文件系统标志是0，则点击的是NTFS
	if (i_fileSystem == 0)
	{
		i_mftTreeChose = i_get;
		//清空List中所有的内容
		fileList.DeleteAllItems();
		//将树中选中的目录的所有文件夹和文件添加到list控件里
		int k = 0;
		for (int i = 0; i < mftDirectory[i_get].directoryList.size(); i++)
		{
			SYSTEMTIME st;
			CString s_temp;
			//插入一项
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
			//插入一项，其索引为MFT编号，内容标签为30名称
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
			//插入文件大小
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
//恢复NTFS文件，恢复指定MFT的文件
void CDataSaverDlg::restoreNTFSFile(CString s_path, NTFSFile rf)
{
	WORD i_start; //数据开始的偏移量
	unsigned char diskSector[1024]; //
	DiskFunction df; //读取磁盘用的实体类
	byte *b_large = new byte[1024 * i_cu * 512]; //如果每簇8扇区，定义4M的内存，用来恢复大文件。
	if (s_path.IsEmpty()) //单个文件恢复
	{
		//设置文件夹选择对话框
		TCHAR filePath[1024];
		BROWSEINFO bi;
		bi.hwndOwner = this->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = TEXT("请选择放置文件的文件夹");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (NTFSfile_restore.mft80h.b_in == true) //如果是常驻属性
		{
			i_start = NTFSfile_restore.mft80h.b_start;
			//得到文件对应的MFT记录号对应的扇区号
			unsigned _int64 i_sector = ns.getNTFSSectorNumber(NTFSfile_restore.i_mft);
			df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);
			diskSector[510] = diskSector[0x32];
			diskSector[511] = diskSector[0x33];
			diskSector[1022] = diskSector[0x34];
			diskSector[1023] = diskSector[0x35];
			//开始恢复数据
			if (pidl == NULL)
				return;
			if (SHGetPathFromIDList(pidl, filePath))
			{
				//AfxMessageBox(filePath+rf.mft30h.s_fileName);
				CString s_get = filePath; //将路径存储下来。
				CString s = "\\";
				CString s_temp;
				if (s_get.GetLength() < 4)
				{
					s_temp = s_get + rf.mft30h.s_fileName; //选择根路径不用加"\"
				}
				else
				{
					s_temp = s_get + s + rf.mft30h.s_fileName; //选择文件夹路径需要加转义符
				}

				FILE *f;
				int i_file = fopen_s(&f, s_temp, "wb"); //指定路径建立文件
				fwrite(&diskSector[i_start], 1, rf.mft80h.i_size, f);
				fclose(f);
				MessageBox(rf.mft30h.s_fileName + "成功被恢复", TITLE, MB_ICONINFORMATION);
				
			}
		}
		else //文件是非常驻属性
		{
			//每次读一个簇
			unsigned char * cuSector = new unsigned char[512 * i_cu];
			if (pidl == NULL)
				return;
			if (SHGetPathFromIDList(pidl, filePath))
			{
				CString s_get = filePath; //将路径存储下来。
				CString s = "\\";
				CString s_temp;
				if (s_get.GetLength() < 4)
				{
					s_temp = s_get + rf.mft30h.s_fileName; //选择根路径不用加"\"
				}
				else
				{
					s_temp = s_get + s + rf.mft30h.s_fileName; //选择文件夹路径需要加转义符
				}
				FILE *f;
				int i_file = fopen_s(&f, s_temp, "wb");

				DWORD64 i_remain = 0; //剩余的还没有写入到文件中的字节
				i_remain = rf.mft80h.i_size; //初始大小为文件的字节数
				DWORD32 i_sectorPerCu = 512 * i_cu; //每簇的字节数。
				//写入指定的字节
				for (int i = 0; i < rf.mft80h.runlist.size(); i++)
				{
					DWORD32 i_cuSize = rf.mft80h.runlist.at(i).i_cuNumber; //第一个runlist的簇数
					DWORD32 i_begin = rf.mft80h.runlist.at(i).i_cuBegin;   //开始簇
					unsigned _int64 i_sector; //定义扇区编号
					DWORD32 d_temp = i_cuSize; //已经读取的簇数
					//当簇数大于1024簇的时候，文件比较大，一次读1024簇
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
						i_sector = ns.getNTFSSectorFromCu(i_begin + j); //每个簇编号对应的扇区
						df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //读到相关数据
						int k;
						if (i_remain > i_sectorPerCu)  //如果剩余未写字节数大于每簇的字节数，可以每次写一簇
						{
							fwrite(cuSector, 1, 512 * i_cu, f); //文件里写一簇数据
							i_remain -= i_sectorPerCu;
						}
						else
						{
							fwrite(cuSector, 1, i_remain, f); //文件里写剩余不到1簇的数据
						}
					}
				}
				fclose(f);
				MessageBox(rf.mft30h.s_fileName + "成功被恢复", TITLE, MB_ICONINFORMATION);
			}
		}
	}
	else //文件夹恢复递归所来
	{
		if (NTFSfile_restore.mft80h.b_in == true) //如果是常驻属性
		{
			i_start = NTFSfile_restore.mft80h.b_start;
			//得到文件对应的MFT记录号对应的扇区号
			unsigned _int64 i_sector = ns.getNTFSSectorNumber(NTFSfile_restore.i_mft);
			df.ReadDiskByNumber(s_driver, diskSector, i_sector, 2);
			diskSector[510] = diskSector[0x32];
			diskSector[511] = diskSector[0x33];
			diskSector[1022] = diskSector[0x34];
			diskSector[1023] = diskSector[0x35];

			CString s_temp = s_path + rf.mft30h.s_fileName;
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb"); //指定路径建立文件
			fwrite(&diskSector[i_start], 1, rf.mft80h.i_size, f);
			fclose(f);
		}
		else //文件是非常驻属性
		{
			//每次读一个簇
			unsigned char * cuSector = new unsigned char[512 * i_cu];
			CString s_temp = s_path + rf.mft30h.s_fileName;
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb");
			DWORD64 i_remain = 0; //剩余的还没有写入到文件中的字节
			i_remain = rf.mft80h.i_size; //初始大小为文件的字节数
			DWORD32 i_sectorPerCu = 512 * i_cu; //每簇的字节数。

			//写入指定的字节
			for (int i = 0; i < rf.mft80h.runlist.size(); i++)
			{
				DWORD32 i_cuSize = rf.mft80h.runlist.at(i).i_cuNumber; //第一个runlist的簇数
				DWORD32 i_begin = rf.mft80h.runlist.at(i).i_cuBegin;   //开始簇
				unsigned _int64 i_sector; //定义扇区编号
				DWORD32 d_temp = i_cuSize; //已经读取的簇数
				//当簇数大于1024簇的时候，文件比较大，一次读1024簇
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
					i_sector = ns.getNTFSSectorFromCu(i_begin + j); //每个簇编号对应的扇区
					df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //读到相关数据
					int k;
					if (i_remain > i_sectorPerCu)  //如果剩余未写字节数大于每簇的字节数，可以每次写一簇
					{
						fwrite(cuSector, 1, 512 * i_cu, f); //文件里写一簇数据
						i_remain -= i_sectorPerCu;
					}
					else
					{
						fwrite(cuSector, 1, i_remain, f); //文件里写剩余不到1簇的数据
					}
				}
			}
			fclose(f);
		}
	}
	delete b_large; //很重要！！！记得释放内存，不然内存无限占用
}

void CDataSaverDlg::OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码

	//获取鼠标坐标
	CPoint ptCurSel(0, 0);
	GetCursorPos(&ptCurSel);
	//转换为当前控件坐标系的坐标
	fileList.ScreenToClient(&ptCurSel);
	LVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = ptCurSel;
	//判断坐标命中第几项
	int nItem = fileList.HitTest(&HitTestInfo);
	//如果有选中项
	if (nItem != -1)
	{
		//读取选中项内容
		char szTmp[MAX_PATH];
		fileList.GetItemText(nItem, 0, szTmp, MAX_PATH);
		//弹出菜单设置，默认按钮都不能选
		CPoint point;
		GetCursorPos(&point);
		CMenu* pSubMenu = menu_restore.GetSubMenu(0);
		pSubMenu->EnableMenuItem(ID_RestoreFile, MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_GRAYED);
		CString s_temp = szTmp; //得到字符串
		CString s_get;

		//如果是ExFAT文件系统，进入下边的处理逻辑
		if (i_fileSystem == 1)
		{
			//在树中选中得到的簇记录里的vector的目录列表中查找是否等于右键选中的list内容
			for (int i = 0; i < exFATDirectory[i_ExFATTreeChose].directoryList.size(); i++)
			{
				s_get = exFATDirectory[i_ExFATTreeChose].directoryList.at(i).s_FolderName;
				if (!s_get.Compare(s_temp)) //相等返回0
				{
					//两个字符串相等
					//字符串相等，且找到的点击的文件夹是删除过的文件夹，文件夹右键菜单才可用。
					if (exFATDirectory[i_ExFATTreeChose].directoryList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_ENABLED);
						exFatFolder_restore = exFATDirectory[i_ExFATTreeChose].directoryList.at(i);
					}
					break; //跳出for循环
				}
			}
			//在树中选中得到的mft记录里的vector的文件列表中查找是否等于右键选中的list内容
			for (int i = 0; i < exFATDirectory[i_ExFATTreeChose].fileList.size(); i++)
			{
				s_get = exFATDirectory[i_ExFATTreeChose].fileList.at(i).s_FileName;
				if (!s_get.Compare(s_temp)) //相等返回0
				{
					//两个字符串相等
					//字符串相等，且找到的点击的文件是删除过的文件，文件恢复右键菜单才可用。
					if (exFATDirectory[i_ExFATTreeChose].fileList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFile, MF_ENABLED);
						exFatFile_restore = exFATDirectory[i_ExFATTreeChose].fileList.at(i);
					}
					break; //跳出for循环
				}
			}
		}

		if (i_fileSystem == 0) //如果是NTFS文件系统，是下边的处理逻辑
		{
			//在树中选中得到的mft记录里的vector的目录列表中查找是否等于右键选中的list内容
			for (int i = 0; i < mftDirectory[i_mftTreeChose].directoryList.size(); i++)
			{
				s_get = mftDirectory[i_mftTreeChose].directoryList.at(i).mft30h.s_fileName;
				if (!s_get.Compare(s_temp)) //相等返回0
				{
					//两个字符串相等
					//字符串相等，且找到的点击的文件夹是删除过的文件夹，文件夹右键菜单才可用。
					if (mftDirectory[i_mftTreeChose].directoryList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFolder, MF_ENABLED);
						NTFSdirectory_restore = mftDirectory[i_mftTreeChose].directoryList.at(i);
					}
					break; //跳出for循环
				}
			}
			//在树中选中得到的mft记录里的vector的文件列表中查找是否等于右键选中的list内容
			for (int i = 0; i < mftDirectory[i_mftTreeChose].fileList.size(); i++)
			{
				s_get = mftDirectory[i_mftTreeChose].fileList.at(i).mft30h.s_fileName;
				if (!s_get.Compare(s_temp)) //相等返回0
				{
					//两个字符串相等
					//字符串相等，且找到的点击的文件是删除过的文件，文件恢复右键菜单才可用。
					if (mftDirectory[i_mftTreeChose].fileList.at(i).b_delete == true)
					{
						pSubMenu->EnableMenuItem(ID_RestoreFile, MF_ENABLED);
						NTFSfile_restore = mftDirectory[i_mftTreeChose].fileList.at(i);
					}
					break; //跳出for循环
				}
			}
		}
		

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	*pResult = 0;
}

//文件恢复处理程序
void CDataSaverDlg::OnRestorefile()
{
	// TODO:  在此添加命令处理程序代码
	if (i_fileSystem==0)
		restoreNTFSFile("", NTFSfile_restore);
	if (i_fileSystem == 1)
		restoreExFATFile("", exFatFile_restore);

}
void CDataSaverDlg::restoreExFATFile(CString s_path, ExFatFile rf)
{
	WORD i_start; //数据开始的偏移量
	unsigned char diskSector[1024]; //
	DiskFunction df; //读取磁盘用的实体类
	
	ExFATFunction ns(exFatBPB); //ExFAT的的功能函数
	
	byte *b_large = new byte[1024 * i_cu * 512]; //如果每簇64扇区，定义32M的内存，用来恢复大文件。
	if (s_path.IsEmpty()) //单个文件恢复
	{
		//设置文件夹选择对话框
		TCHAR filePath[1024];
		BROWSEINFO bi;
		bi.hwndOwner = this->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = TEXT("请选择放置文件的文件夹");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		//每次读一个簇
		unsigned char * cuSector = new unsigned char[512 * i_cu];
		if (pidl == NULL)
			return;
		if (SHGetPathFromIDList(pidl, filePath))
		{
			CString s_get = filePath; //将路径存储下来。
			CString s = "\\";
			CString s_temp;
			if (s_get.GetLength() < 4)
			{
				s_temp = s_get + rf.s_FileName; //选择根路径不用加"\"
			}
			else
			{
				s_temp = s_get + s + rf.s_FileName; //选择文件夹路径需要加转义符
			}
			FILE *f;
			int i_file = fopen_s(&f, s_temp, "wb");
			DWORD64 i_remain = 0; //剩余的还没有写入到文件中的字节
			i_remain = rf.i_fileSize; //初始大小为文件的字节数
			DWORD32 i_sectorPerCu = 512 * i_cu; //每簇的字节数。
			//写入指定的字节
			DWORD32 i_cuSize = rf.i_fileSize / (i_cu * 512); //得到文件所占用的簇数，因簇数需要考虑有余数的情况
			if (rf.i_fileSize % (i_cu * 512) != 0) //如果有余数，所占用的簇需要加1
				i_cuSize++;
			
			DWORD32 i_begin = rf.i_beginCu;   //开始簇
			
			unsigned _int64 i_sector; //定义扇区编号
			DWORD32 d_temp = i_cuSize; //已经读取的簇数
			//当簇数大于1024簇的时候，文件比较大，一次读1024簇
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
						i_sector = ns.getSectorFromCu(i_begin + j); //每个簇编号对应的扇区
						df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //读到相关数据
						int k;
						if (i_remain > i_sectorPerCu)  //如果剩余未写字节数大于每簇的字节数，可以每次写一簇
						{
							fwrite(cuSector, 1, 512 * i_cu, f); //文件里写一簇数据
							i_remain -= i_sectorPerCu;
						}
						else
						{
							fwrite(cuSector, 1, i_remain, f); //文件里写剩余不到1簇的数据
						}
					}
				
				fclose(f);
				MessageBox(rf.s_FileName + "成功被恢复", TITLE, MB_ICONINFORMATION);
			}
	}
	
	else //文件夹恢复递归所来
	{	//每次读一个簇
		unsigned char * cuSector = new unsigned char[512 * i_cu];
		CString s_temp = s_path + rf.s_FileName;
		FILE *f;
		int i_file = fopen_s(&f, s_temp, "wb");
		DWORD64 i_remain = 0; //剩余的还没有写入到文件中的字节
		i_remain = rf.i_fileSize; //初始大小为文件的字节数
		DWORD32 i_sectorPerCu = 512 * i_cu; //每簇的字节数。
		//写入指定的字节
		DWORD32 i_begin = rf.i_beginCu;   //开始簇
		unsigned _int64 i_sector; //定义扇区编号
		DWORD32 d_temp = rf.i_fileSize / i_sectorPerCu; //已经读取的簇数
		if (rf.i_fileSize%i_sectorPerCu != 0)
			d_temp++;
		DWORD32 i_cuSize = d_temp;
		//当簇数大于1024簇的时候，文件比较大，一次读1024簇
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
					i_sector = ns.getSectorFromCu(i_begin + j); //每个簇编号对应的扇区
					df.ReadDiskByNumber(s_driver, cuSector, i_sector, i_cu); //读到相关数据
					int k;
					if (i_remain > i_sectorPerCu)  //如果剩余未写字节数大于每簇的字节数，可以每次写一簇
					{
						fwrite(cuSector, 1, 512 * i_cu, f); //文件里写一簇数据
						i_remain -= i_sectorPerCu;
					}
					else
					{
						fwrite(cuSector, 1, i_remain, f); //文件里写剩余不到1簇的数据
					}
				}
			
			fclose(f);
	
	}
	delete b_large; //很重要！！！记得释放内存，不然内存无限占用
}
//文件夹恢复处理程序
void CDataSaverDlg::OnRestorefolder()
{
	//设置文件夹选择对话框
	TCHAR filePath[1024];
	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("请选择放置所要恢复文件夹所存放的路径");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL)
		return;
	if (SHGetPathFromIDList(pidl, filePath))
	{
		
		if (i_fileSystem == 1) //ExFAT文件夹恢复
		{
			CString s = filePath;
			CString s_folder;
			if (s.GetLength() < 4) //根路径
			{
				s_folder = filePath + exFatFolder_restore.s_FolderName;
			}
			else //不是根路径需要加路径符号
			{
				CString s_temp1 = "\\";
				s_folder = filePath + s_temp1 + exFatFolder_restore.s_FolderName;
			}
			CreateDirectory(s_folder, NULL);
			CString s_path = s_folder + "\\"; //需要传递给要恢复文件的路径
			
			queue<extFatDirectoryRestoreQueue> subDirectory;
			extFatDirectoryRestoreQueue drq;//节点变量
			drq.s_path = s_path; //子文件夹所在的路径
			for (int i = 0; i < exFATDirectory[exFatFolder_restore.i_cu].fileList.size(); i++)  //将本目录下的所有文件恢复
			{
				restoreExFATFile(s_path, exFATDirectory[exFatFolder_restore.i_cu].fileList.at(i)); //将其ExFATFile的vector恢复

			}
			for (int i = 0; i < exFATDirectory[exFatFolder_restore.i_cu].directoryList.size(); i++) //本目录的子目录放入队列中
			{
				extFatDirectoryRestoreQueue drq;//节点变量
				drq.s_path = s_path; //子文件夹所在的路径
				drq.exFatDirectory = exFATDirectory[exFatFolder_restore.i_cu].directoryList.at(i);
				subDirectory.push(drq);  //queue的插入函数，换成list后，修改函数
				//	subDirectory.push_back(drq);
			}
			//如果队列非空，则进行递归调用
			CString s_subDirectoryPath;

			//尝试使用指针
			while (!subDirectory.empty())
			{
				extFatDirectoryRestoreQueue *nd = &subDirectory.front(); //得到队头元素
				s_subDirectoryPath = nd->s_path + nd->exFatDirectory.s_FolderName; //得到子文件夹的路径
				//建立子文件夹
				CreateDirectory(s_subDirectoryPath, NULL);
				s_subDirectoryPath = s_subDirectoryPath + "\\";
				for (int i = 0; i < exFATDirectory[nd->exFatDirectory.i_cu].fileList.size(); i++)
				{
					restoreExFATFile(s_subDirectoryPath, exFATDirectory[nd->exFatDirectory.i_cu].fileList.at(i));
				}
				for (int i = 0; i < exFATDirectory[nd->exFatDirectory.i_cu].directoryList.size(); i++)
				{
					extFatDirectoryRestoreQueue drq;//节点变量
					drq.s_path = s_subDirectoryPath; //子文件夹所在的路径
					drq.exFatDirectory = exFATDirectory[nd->exFatDirectory.i_cu].directoryList.at(i);
					subDirectory.push(drq); //queue的插入函数，换成list后，修改函数
					//subDirectory.push_back(drq);
				}
				nd = NULL;
				subDirectory.pop();
			}
		}
		if (i_fileSystem == 0) //NTFS文件恢复
		{
			CString s = filePath;
			CString s_folder;
			if (s.GetLength() < 4) //根路径
			{
				s_folder = filePath + NTFSdirectory_restore.mft30h.s_fileName;
			}
			else //不是根路径需要加路径符号
			{
				CString s_temp1 = "\\";
				s_folder = filePath + s_temp1 + NTFSdirectory_restore.mft30h.s_fileName;
			}
			CreateDirectory(s_folder, NULL);
			CString s_path = s_folder + "\\"; //需要传递给要恢复文件的路径
			
			queue<DirectoryRestoreQueue> subDirectory;
			DirectoryRestoreQueue drq;//节点变量
			drq.s_path = s_path; //子文件夹所在的路径
			for (int i = 0; i < mftDirectory[NTFSdirectory_restore.i_mft].fileList.size(); i++)  //将本目录下的所有文件恢复
			{
				restoreNTFSFile(s_path, mftDirectory[NTFSdirectory_restore.i_mft].fileList.at(i)); //将其NTFSFile的vector恢复

			}
			for (int i = 0; i < mftDirectory[NTFSdirectory_restore.i_mft].directoryList.size(); i++) //本目录的子目录放入队列中
			{
				DirectoryRestoreQueue drq;//节点变量
				drq.s_path = s_path; //子文件夹所在的路径
				drq.ntfsDirectory = mftDirectory[NTFSdirectory_restore.i_mft].directoryList.at(i);
				subDirectory.push(drq);  //queue的插入函数，换成list后，修改函数
				//	subDirectory.push_back(drq);
			}
			//如果队列非空，则进行递归调用
			CString s_subDirectoryPath;

			//尝试使用指针
			while (!subDirectory.empty())
			{
				DirectoryRestoreQueue *nd = &subDirectory.front(); //得到队头元素
				s_subDirectoryPath = nd->s_path + nd->ntfsDirectory.mft30h.s_fileName; //得到子文件夹的路径
				//建立子文件夹
				CreateDirectory(s_subDirectoryPath, NULL);
				s_subDirectoryPath = s_subDirectoryPath + "\\";
				for (int i = 0; i < mftDirectory[nd->ntfsDirectory.i_mft].fileList.size(); i++)
				{
					restoreNTFSFile(s_subDirectoryPath, mftDirectory[nd->ntfsDirectory.i_mft].fileList.at(i));
				}
				for (int i = 0; i < mftDirectory[nd->ntfsDirectory.i_mft].directoryList.size(); i++)
				{
					DirectoryRestoreQueue drq;//节点变量
					drq.s_path = s_subDirectoryPath; //子文件夹所在的路径
					drq.ntfsDirectory = mftDirectory[nd->ntfsDirectory.i_mft].directoryList.at(i);
					subDirectory.push(drq); //queue的插入函数，换成list后，修改函数
					//subDirectory.push_back(drq);
				}
				nd = NULL;
				subDirectory.pop();
			}
		}
		
		MessageBox(NTFSdirectory_restore.mft30h.s_fileName + "成功被恢复", TITLE, MB_ICONINFORMATION);
	}
}


void CDataSaverDlg::OnBnClickedAbout()
{
	// TODO:  在此添加控件通知处理程序代码
	About ab;
	ab.DoModal();
}




HBRUSH CDataSaverDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBitmapDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if (pWnd == GetDlgItem(IDC_WARN))
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	return hbr;
}
