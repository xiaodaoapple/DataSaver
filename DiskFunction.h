#pragma once
#include <vector>
using namespace std;
class DiskFunction
{
public:
	DiskFunction();
	~DiskFunction();
	///////////////////////////////////////////////////////////////////


	// 读出磁盘的某个扇区
	static	int ReadDisk(CString, unsigned char*, unsigned _int64);

	//读出指定磁盘的扇区数
	static int ReadDiskByNumber(CString, unsigned char*, unsigned _int64, WORD);

	// 将内容写入磁盘的某个扇区
	static int WriteDisk(CString, unsigned char*, unsigned _int64);

	//突破2G的扇区限制
	static __int64 myFileSeek(HANDLE hf, unsigned __int64 distance, DWORD MoveMethod);

	//将连续字节转为长整数,i为要转的字节数,最高只支持8字节转换
	static unsigned _int64 byte2Int(unsigned char *, int i);

	//从DBR判断磁盘的文件系统格式
	static CString judgeFileSystem(unsigned char *sector);

	//从8位的UTC时间得到SYSTEMTIME类型的结构体,其结构体内容如下
	/*typedef struct SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	}*/
	static SYSTEMTIME getNTFSDateTime(unsigned char *);

	//得到30属性,形参为2个扇区一共1024个字节
	static MFT30h get30h(unsigned char*);
	
	//得到80属性的运行DataList解读，形参为2个扇区一共1024个字节
	static vector<runlistData> get80DataList(unsigned char *);

	//得到80属性，形参为2个扇区，一共1024个字节
	static MFT80h get80h(unsigned char*);
	//得到MFT0号文件记录的BO属性，形参为2个扇区，一共1024个字节
	static MFTB0h getB0h(unsigned char*);

	//从ExFAT4个字节得到最后访问时间。第一个变量为32位dos时间，第二个变量为10ms秒
	static CString getExFATTime(unsigned char*,byte i_time);

	//从ExFAT得到文件名，第一个变量为文件名的起始地址。
	static CString getExFATFileName(unsigned char*,byte );
};