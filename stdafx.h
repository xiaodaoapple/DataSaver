
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
#define TITLE "����������"

#include <vector>
using namespace std;
//�����̷��Ϳռ�Ĺ�ϵ
typedef struct diskVolume
{
	boolean hasChoose; //�ж��Ƿ��Ѿ���ƥ�����
	CString s_diskVolume; //�̷�
	unsigned _int64 partitionSize; //������С����λΪ����
};
//�����Ǵ��̵Ľṹ����
typedef struct diskInfor
{
	boolean isMBR; //�жϴ����Ƿ���MBR����
	CString s_diskName; //������ʵ����Ϣ
	CString s_diskShow; //������ʾ����Ϣ
	CString s_driver;  //���̶�ȡ����ʱ��Ҫ���ݵĴ�����Ϣ
	unsigned _int64 diskSize; //���̵��ܴ�С

};
//�����Ƿ����Ľṹ����
typedef struct partitionInfor
{
	CString fileSystem; //�������ļ�ϵͳ
	unsigned _int64 partitonBegin; //�����Ŀ�ʼ��ַ����λΪ����
	unsigned _int64 partitionSize; //�����Ĵ�С����λΪ����
	int i_cu; //����ÿ��������
};

//������80H���ԣ�runlist�ĵ������������
typedef struct runlistData
{
	DWORD i_cuNumber; //�ص���Ŀ
	unsigned _int64 i_cuBegin; //��ʼ��
};
typedef struct MFT30h
{
	CString s_fileName; //�ļ����ļ�������
	SYSTEMTIME st_fileTime; //�ļ����ļ���������ʱ��
	unsigned _int64 i_father; //��Ŀ¼���ļ��ο���
};

typedef struct MFT80h
{
	bool b_in; //�ж������Ƿ�פ
	WORD b_start; //���ǳ�פ����ʱ�������ݿ�ʼ�������е�λ��
	unsigned _int64 i_size; //�ļ���ʵ�ʴ�С
	vector <runlistData> runlist; //���ļ��Ƿǳ�פʱ����runlist������
};
typedef struct MFTB0h
{
	bool b_in; //�ж������Ƿ�פ
	vector <runlistData> runlist; //���ļ��Ƿǳ�פʱ����runlist������
};
typedef struct NTFSDirectory
{
	bool b_delete; //�ж��Ƿ�ɾ��
	unsigned _int64 i_mft; //Ŀ¼��MFT��
	MFT30h mft30h; //Ŀ¼��30h����
};
typedef struct NTFSFile
{
	bool b_delete; //�ж��Ƿ�ɾ��
	unsigned _int64 i_mft; //�ļ���MFT��
	MFT30h mft30h; //�ļ���30h����
	MFT80h mft80h; //�ļ���80h����
};
// ����ExFAT����Ŀ¼�������Ϣ
typedef struct ExFatFolder
{
	CString s_FolderName; //�ļ�����
	CString s_FolderTime; //�ļ��д���ʱ��
	bool b_delete; //�ļ����Ƿ�ɾ��
	UINT32 i_cu; //�ļ��еĿ�ʼ��
};
typedef struct DirectoryMap
{
	unsigned _int64 i_mft; //�ڵ�ź�mft�Ŷ�Ӧ,�ڵ��Ȼ��Ŀ¼
	vector <NTFSDirectory> directoryList; //���ļ��е�vector
	vector <NTFSFile> fileList;  //���ļ���list
};
typedef struct DirectoryQueueNode  //NTFSĿ¼����
{
	HTREEITEM hFolder;
	NTFSDirectory ntfsDirectory;
};

//�˶��������ָ�NTFS���ļ���ʱ����Ϊ�ݹ�ڵ���Ҫ�����䵱ǰ·��
typedef struct DirectoryRestoreQueue
{
	CString s_path; //��ǰ�ļ��еĶ���
	NTFSDirectory ntfsDirectory;
};

//�˶��������ָ�fat���ļ���ʱ����Ϊ�ݹ�ڵ���Ҫ�����䵱ǰ·��
typedef struct extFatDirectoryRestoreQueue
{
	CString s_path;
	ExFatFolder exFatDirectory;
};

//����ExFAT��BPB�����Ϣ
typedef struct ExFatBPB
{
	UINT32 i_FATBegin; //fat����ʼ������
	UINT32 i_FATNumber; //fat�Ĵ�С
	BYTE i_cu;  //ÿ��������
	UINT32 i_cuBegin; //�״������ţ���һ���ؾ���2�Ŵ�
	UINT32 i_rootCu; //��Ŀ¼�صĿ�ʼ
	CString s_driver; //����������Ӳ�̱��
};
//����ExFAT���ļ������Ϣ
typedef struct ExFatFile
{
	CString s_FileName; //�ļ���
	CString s_FileTime; //�ļ�����ʱ��
	bool b_delete; //�ļ��Ƿ�ɾ��
	UINT64 i_fileSize; //�ļ���С
	UINT32 i_beginCu; //�ļ��Ŀ�ʼ��
	bool b_series; //�ļ��Ƿ��������
};

typedef struct ExFATDirectoryQueueNode  //ExFATĿ¼����
{
	HTREEITEM hFolder;
	ExFatFolder fatDirectory;
};
typedef struct ExFATDirectoryMap
{
	DWORD32 i_cu; //Ŀ¼���ڵĴصĿ�ʼ������ռ�����أ�����¼�俪ʼ��
	vector <ExFatFolder> directoryList; //���ļ��е�vector
	vector <ExFatFile> fileList;  //���ļ���list
};
#define DISKWARN "����г��˱������е�Ӳ�̣���ѡ����Ҫ�ָ����ݵ�Ӳ�̡�"
#define PARTITIONWARN "����г�����ѡ���̵����з�������ѡ����Ҫ�ָ����ݵķ���\n\n֧��NTFS������ExFAT���������ݻָ���"
#define AUTHOR "����������  BY ��־Զ "
#define RESTOREINFO "����Ҫ�ָ����ļ������ļ����ϵ���Ҽ����������ݻָ���"