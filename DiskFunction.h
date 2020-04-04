#pragma once
#include <vector>
using namespace std;
class DiskFunction
{
public:
	DiskFunction();
	~DiskFunction();
	///////////////////////////////////////////////////////////////////


	// �������̵�ĳ������
	static	int ReadDisk(CString, unsigned char*, unsigned _int64);

	//����ָ�����̵�������
	static int ReadDiskByNumber(CString, unsigned char*, unsigned _int64, WORD);

	// ������д����̵�ĳ������
	static int WriteDisk(CString, unsigned char*, unsigned _int64);

	//ͻ��2G����������
	static __int64 myFileSeek(HANDLE hf, unsigned __int64 distance, DWORD MoveMethod);

	//�������ֽ�תΪ������,iΪҪת���ֽ���,���ֻ֧��8�ֽ�ת��
	static unsigned _int64 byte2Int(unsigned char *, int i);

	//��DBR�жϴ��̵��ļ�ϵͳ��ʽ
	static CString judgeFileSystem(unsigned char *sector);

	//��8λ��UTCʱ��õ�SYSTEMTIME���͵Ľṹ��,��ṹ����������
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

	//�õ�30����,�β�Ϊ2������һ��1024���ֽ�
	static MFT30h get30h(unsigned char*);
	
	//�õ�80���Ե�����DataList������β�Ϊ2������һ��1024���ֽ�
	static vector<runlistData> get80DataList(unsigned char *);

	//�õ�80���ԣ��β�Ϊ2��������һ��1024���ֽ�
	static MFT80h get80h(unsigned char*);
	//�õ�MFT0���ļ���¼��BO���ԣ��β�Ϊ2��������һ��1024���ֽ�
	static MFTB0h getB0h(unsigned char*);

	//��ExFAT4���ֽڵõ�������ʱ�䡣��һ������Ϊ32λdosʱ�䣬�ڶ�������Ϊ10ms��
	static CString getExFATTime(unsigned char*,byte i_time);

	//��ExFAT�õ��ļ�������һ������Ϊ�ļ�������ʼ��ַ��
	static CString getExFATFileName(unsigned char*,byte );
};