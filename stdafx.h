
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
#define TITLE "数据拯救者"

#include <vector>
using namespace std;
//定义盘符和空间的关系
typedef struct diskVolume
{
	boolean hasChoose; //判断是否已经被匹配过了
	CString s_diskVolume; //盘符
	unsigned _int64 partitionSize; //分区大小，单位为扇区
};
//以下是磁盘的结构定义
typedef struct diskInfor
{
	boolean isMBR; //判断磁盘是否是MBR磁盘
	CString s_diskName; //磁盘真实的信息
	CString s_diskShow; //磁盘显示的信息
	CString s_driver;  //磁盘读取扇区时需要传递的磁盘信息
	unsigned _int64 diskSize; //磁盘的总大小

};
//以下是分区的结构定义
typedef struct partitionInfor
{
	CString fileSystem; //分区的文件系统
	unsigned _int64 partitonBegin; //分区的开始地址，单位为扇区
	unsigned _int64 partitionSize; //分区的大小，单位为扇区
	int i_cu; //分区每簇扇区数
};

//以下是80H属性，runlist的单个解析结果。
typedef struct runlistData
{
	DWORD i_cuNumber; //簇的数目
	unsigned _int64 i_cuBegin; //开始簇
};
typedef struct MFT30h
{
	CString s_fileName; //文件或文件夹名称
	SYSTEMTIME st_fileTime; //文件或文件夹最后访问时间
	unsigned _int64 i_father; //父目录的文件参考号
};

typedef struct MFT80h
{
	bool b_in; //判断属性是否常驻
	WORD b_start; //当是常驻属性时，其内容开始在扇区中的位置
	unsigned _int64 i_size; //文件的实际大小
	vector <runlistData> runlist; //当文件是非常驻时，其runlist的内容
};
typedef struct MFTB0h
{
	bool b_in; //判断属性是否常驻
	vector <runlistData> runlist; //当文件是非常驻时，其runlist的内容
};
typedef struct NTFSDirectory
{
	bool b_delete; //判断是否删除
	unsigned _int64 i_mft; //目录的MFT号
	MFT30h mft30h; //目录的30h属性
};
typedef struct NTFSFile
{
	bool b_delete; //判断是否删除
	unsigned _int64 i_mft; //文件的MFT号
	MFT30h mft30h; //文件的30h属性
	MFT80h mft80h; //文件的80h属性
};
// 定义ExFAT的子目录的相关信息
typedef struct ExFatFolder
{
	CString s_FolderName; //文件夹名
	CString s_FolderTime; //文件夹创建时间
	bool b_delete; //文件夹是否被删除
	UINT32 i_cu; //文件夹的开始簇
};
typedef struct DirectoryMap
{
	unsigned _int64 i_mft; //节点号和mft号对应,节点必然是目录
	vector <NTFSDirectory> directoryList; //子文件夹的vector
	vector <NTFSFile> fileList;  //子文件的list
};
typedef struct DirectoryQueueNode  //NTFS目录队列
{
	HTREEITEM hFolder;
	NTFSDirectory ntfsDirectory;
};

//此队列用来恢复NTFS子文件夹时，作为递归节点需要保存其当前路径
typedef struct DirectoryRestoreQueue
{
	CString s_path; //当前文件夹的队列
	NTFSDirectory ntfsDirectory;
};

//此队列用来恢复fat子文件夹时，作为递归节点需要保存其当前路径
typedef struct extFatDirectoryRestoreQueue
{
	CString s_path;
	ExFatFolder exFatDirectory;
};

//定义ExFAT的BPB相关信息
typedef struct ExFatBPB
{
	UINT32 i_FATBegin; //fat的起始扇区号
	UINT32 i_FATNumber; //fat的大小
	BYTE i_cu;  //每簇扇区数
	UINT32 i_cuBegin; //首簇扇区号，第一个簇就是2号簇
	UINT32 i_rootCu; //根目录簇的开始
	CString s_driver; //所属的物理硬盘编号
};
//定义ExFAT的文件相关信息
typedef struct ExFatFile
{
	CString s_FileName; //文件名
	CString s_FileTime; //文件创建时间
	bool b_delete; //文件是否被删除
	UINT64 i_fileSize; //文件大小
	UINT32 i_beginCu; //文件的开始簇
	bool b_series; //文件是否连续存放
};

typedef struct ExFATDirectoryQueueNode  //ExFAT目录队列
{
	HTREEITEM hFolder;
	ExFatFolder fatDirectory;
};
typedef struct ExFATDirectoryMap
{
	DWORD32 i_cu; //目录所在的簇的开始，无论占几个簇，都记录其开始簇
	vector <ExFatFolder> directoryList; //子文件夹的vector
	vector <ExFatFile> fileList;  //子文件的list
};
#define DISKWARN "左侧列出了本机所有的硬盘，请选择您要恢复数据的硬盘。"
#define PARTITIONWARN "左侧列出了所选磁盘的所有分区，请选择您要恢复数据的分区\n\n支持NTFS分区和ExFAT分区的数据恢复！"
#define AUTHOR "数据拯救者  BY 李志远 "
#define RESTOREINFO "在需要恢复的文件或者文件夹上点击右键，进行数据恢复！"