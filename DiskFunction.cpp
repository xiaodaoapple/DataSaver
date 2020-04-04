#include "stdafx.h"
#include "DiskFunction.h"


#define _LARGEFILE64_SOURCE 
DiskFunction::DiskFunction()
{

}
DiskFunction::~DiskFunction()
{

}

 _int64 DiskFunction::myFileSeek(HANDLE hf, unsigned __int64 distance, DWORD MoveMethod)
{
	LARGE_INTEGER li;
	li.QuadPart = distance;
	li.LowPart = SetFilePointer(hf, li.LowPart, &li.HighPart, MoveMethod);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		li.QuadPart = -1;
	}

	return li.QuadPart;
}
int DiskFunction::ReadDisk(CString driver, unsigned char *Buf, unsigned _int64 addr)
{
	HANDLE hDevice;
	BOOL bResult;
	DWORD bytesread;

	hDevice = CreateFile(driver, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{

		return 0;
	}

	if (addr != 0)
	{
		unsigned _int64 distance = (unsigned _int64)512 * addr;
		myFileSeek(hDevice, distance, FILE_BEGIN);
		//SetFilePointer(hDevice,512*addr,NULL,NULL);
	}

	bResult = ReadFile(hDevice, Buf, 512, &bytesread, NULL);

	if ((bResult == FALSE) || (bytesread<512))
	{

		return 0;
	}

	CloseHandle(hDevice);

	return 1;
}
int DiskFunction::ReadDiskByNumber(CString driver, unsigned char *Buf, unsigned _int64 addr, WORD i_number)
{
	HANDLE hDevice;
	BOOL bResult;
	DWORD bytesread;

	hDevice = CreateFile(driver, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{

		return 0;
	}

	if (addr != 0)
	{
		unsigned _int64 distance = (unsigned _int64)512 * addr;
		myFileSeek(hDevice, distance, FILE_BEGIN);
		//SetFilePointer(hDevice,512*addr,NULL,NULL);
	}

	bResult = ReadFile(hDevice, Buf, 512*i_number, &bytesread, NULL);

	if ((bResult == FALSE) || (bytesread<512))
	{

		return 0;
	}

	CloseHandle(hDevice);

	return 1;
}
int DiskFunction::WriteDisk(CString driver, unsigned char *Buf, unsigned _int64 addr)
{
	HANDLE hDevice;
	BOOL bResult;
	DWORD bytesread;

	hDevice = CreateFile(driver, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{

		return 0;
	}

	if (addr != 0)
	{
		unsigned _int64 distance = (unsigned _int64)512 * addr;
		myFileSeek(hDevice, distance, FILE_BEGIN);
		//SetFilePointer(hDevice,512*addr,NULL,NULL);
	}

	bResult = WriteFile(hDevice, Buf, 512, &bytesread, NULL);

	if ((bResult == FALSE) || (bytesread<512))
	{

		return 0;
	}

	CloseHandle(hDevice);

	return 1;
}
unsigned _int64 DiskFunction::byte2Int(unsigned char * bRefArr, int k)
{
	unsigned _int64 iOutcome = 0;
	unsigned _int64 temp;
	unsigned _int64 bLoop;
	for (int i = 0; i<k; i++) {
		bLoop = bRefArr[i];
		temp = (bLoop & 0xFF) << (8 * i);
		iOutcome = iOutcome + temp;
		//iOutcome += (bLoop & 0xFF) << (8 * i);
	}
	return iOutcome;
}
CString DiskFunction::judgeFileSystem(unsigned char * bRefArr)
{
	CString result = "";
	
	unsigned _int64 iOutcome = 0;
	unsigned _int64 temp;
	unsigned _int64 bLoop;
	for (int i = 0; i<4; i++) {
		bLoop = bRefArr[i];
		temp = (bLoop & 0xFF) << (8 * i);
		iOutcome = iOutcome + temp;
	}

//ntfs�ļ���־
if (iOutcome == 0X5346544E)
{
	result = "NTFS";
}
//FAT32�ļ���־
if (iOutcome == 0X4F44534D)
{
	result = "FAT32";

}
//ExFAT����
if (iOutcome == 0x41465845)
{
	result = "ExFAT";
}
return result;
}
SYSTEMTIME DiskFunction::getNTFSDateTime(unsigned char * bRefArr)
{
	SYSTEMTIME st;
	unsigned int bLoop = 0;
	unsigned _int64 iOutcome = 0;
	unsigned _int64 temp;
	FILETIME ft; //����filetime�ṹ���������鸳ֵ����
	for (int i = 0; i < 4; i++)
	{
		bLoop = bRefArr[i];
		temp = (bLoop & 0xFF) << (8 * i);
		iOutcome = iOutcome + temp;
	}
	ft.dwLowDateTime = iOutcome;
	iOutcome = 0;
	for (int i = 0; i < 4; i++)
	{
		bLoop = bRefArr[i + 4];
		temp = (bLoop & 0xFF) << (8 * i);
		iOutcome = iOutcome + temp;
	}
	ft.dwHighDateTime = iOutcome;
	FileTimeToSystemTime(&ft, &st);
	return st;
}
MFT30h DiskFunction::get30h(unsigned char *lpStrBuff)
{
	MFT30h mft30h;
	//�õ�GBK������ַ���
	char * getGBK = NULL;
	WORD wMFTHeadAttrLen = 0; //WORD 16λ������
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	//wchar_t��Unicode�õĿ��ַ����͡���˫�ֱ�ʾ���ַ�.�����ַ���Ϊ˫��
	wchar_t fileName[256] = { 0 };
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//�õ�������ʼλ��
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	tempBuff += wMFTHeadAttrLen; //�õ���һ��ƫ����
	//����10����
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
	}
	//����20����
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
	}
	//����30����
	if (*(LPDWORD)tempBuff == 0x30)
	{
		DWORD i_temp = *(LPWORD)(tempBuff + 0x14); //�õ�������Ŀ�ʼ��ַ
		byte i_judge = *(tempBuff + i_temp + 0x41); //�õ������ռ䣬
		//��������ռ���1����3��˵����Ӧ��30h�ǳ��ļ���
		if (i_judge == 1 || i_judge == 3 ||i_judge==0)
		{

		}
		else //�����ռ���2��˵���Ƕ��ļ������¸����Ա�Ȼ�ǳ��ļ���
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
		}
		
		/* ʵ��֤�������ļ�30���Եڶ�����һ������������ֻ��ͨ�������ռ�����жϡ�
		//����¸����Բ���30���ԣ�˵���Ƕ��ļ�����Ҫ����ȥ�����ļ�30������Զ�ǵڶ���
		if (*(LPDWORD)tempBuff != 0x30)
		{
			tempBuff -= dwMFT30AttrLen;
		}
		*/
		DWORD NeedBytes;
		DWORD AttrValOffset = *(LPWORD)(tempBuff + 0x14); //30���Ե�ƫ��
		tempBuff += AttrValOffset; //��ʱtempBuff��λ��Ϊ30h������ĵ�ַ

		mft30h.i_father = byte2Int(tempBuff, 6); //�õ��丸Ŀ¼���ļ���
		mft30h.st_fileTime = getNTFSDateTime(tempBuff + 0x20); //�õ�30h���Ե�������ʱ��

		DWORD fileNum = *(tempBuff + 0x40);
		if (fileNum >= 1)
		{

			memcpy(fileName, tempBuff + 0x42, fileNum * 2);
			NeedBytes = WideCharToMultiByte(CP_ACP, NULL, fileName, wcslen(fileName), NULL, 0, NULL, NULL);
			USES_CONVERSION;
			getGBK = W2A(fileName);
			mft30h.s_fileName = getGBK;
		}
	}
	mft30h.s_fileName = getGBK;
	return mft30h;
}
//����runlist������ŵ�vector��
vector<runlistData> DiskFunction::get80DataList(unsigned char *lpStrBuff)
{
	runlistData rd;
	vector<runlistData> runlist;
	DWORD dwRunBytes;
	WORD hiForBit;
	WORD lowForBit;
	CString temp, tempStr;
	long tempOffset = 0;
	int c_offsetBytes = 0;
	DWORD32 i_cu;
	DWORD64 i_cu_begin;
	DWORD64 i_cu_calc;
	lowForBit = *(lpStrBuff) / 16;  //�õ���λ��ռ�õĴ���
	hiForBit = *(lpStrBuff)& 0x0F; //�õ���λ�������ֽڱ�ʾ�صĿ�ʼ
	i_cu = byte2Int(lpStrBuff + 1, hiForBit); //�õ���һ�������б��ռ�õĴ���
	i_cu_begin = byte2Int(lpStrBuff + 1 + hiForBit, lowForBit);//�õ���һ�������б�Ĵص���ʼ	
	//����һ�������б�Ľ������õ�vector��
	rd.i_cuNumber = i_cu;
	rd.i_cuBegin = i_cu_begin;
	runlist.push_back(rd);

	i_cu_calc = i_cu_begin;
	lpStrBuff += 1 + hiForBit + lowForBit; //��λ����һ���б�Ŀ�ʼ
	while ((*(LPWORD)(lpStrBuff) != 0XFFFF) && (*lpStrBuff != 0))
	{
		runlistData rdTemp; //������ʱrunlist������������

		DWORD64 d_temp=0;
		_int64 i_temp=0;
		lowForBit = *(lpStrBuff) / 16;  //�õ���λ��ռ�õĴ���
		hiForBit = *(lpStrBuff)& 0x0F; //�õ���λ�������ֽڱ�ʾ�صĿ�ʼ
		i_cu = byte2Int(lpStrBuff + 1, hiForBit); //�õ������б��ռ�õĴ���
		d_temp = byte2Int(lpStrBuff + 1 + hiForBit, lowForBit);//�õ������б�Ĵص���ʼλ�ã��ڶ��������б�ʼ��Ϊ�з���ֵ����Ҫ�Լ���õ���ֵ���д���.
		//����λ�ֽ����С��128��֤����Ϊ��������������ǰһ����λ��Ϊ��
		if (*(lpStrBuff + hiForBit + lowForBit) < 128)
		{
			i_cu_calc += d_temp; //�ڶ����б������ʼ��������һ���б�Ĵ�λ����ӡ�
		}
		else //���λ�ֽڴ���127��˵����Ϊ��������Ҫ���д���
		{
			switch (lowForBit)
			{
				case 1:
					i_temp = d_temp - 0x100;
					break;
				case 2:
					i_temp = d_temp - 0x10000;
					break;
				case 3:
					i_temp = d_temp - 0x1000000;
					break;
				case 4:
					i_temp = d_temp - 0x100000000;
					break;
				default:
				break;
			}
			i_cu_calc += i_temp;
		}
		rdTemp.i_cuNumber = i_cu;
		rdTemp.i_cuBegin = i_cu_calc;
		runlist.push_back(rdTemp);
		lpStrBuff += 1 + hiForBit + lowForBit; //��λ����һ���б�Ŀ�ʼ
	}
	return runlist;
}
MFT80h DiskFunction::get80h(unsigned char *lpStrBuff)
{
	MFT80h mft80h;
	WORD i_pass=0; //��¼�������ֽ���
	WORD wMFTHeadAttrLen = 0; //WORD 16λ������
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	DWORD32 i_temp;
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//�õ�������ʼλ��
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	
	tempBuff += wMFTHeadAttrLen; //�õ���һ��ƫ����
	i_pass += wMFTHeadAttrLen;
	//����10����
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
		i_pass += dwMFT10AttrLen;
	}
	//����20����
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
		i_pass += dwMFT20AttrLen;
	}
	//����30����
	if (*(LPDWORD)tempBuff == 0x30)
	{
		dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT30AttrLen;
		i_pass += dwMFT30AttrLen;
		//����¸����Ի���30���ԣ�˵���ǳ��ļ�����Ҫ������ȥ
		if (*(LPDWORD)tempBuff == 0x30)
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
			i_pass += dwMFT30AttrLen;
		}
	}
	//����40����
	if (*(LPDWORD)tempBuff == 0x40)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//����50����
	if (*(LPDWORD)tempBuff == 0x50)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//����60����
	if (*(LPDWORD)tempBuff == 0x60)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//����70����
	if (*(LPDWORD)tempBuff == 0x70)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//��ʼ����80����
	if (*(LPDWORD)tempBuff == 0x80)
	{
		//��פ����,�õ����ļ���С
		if (*(tempBuff + 8) == 0)
		{
			mft80h.b_in = true;
			mft80h.b_start =i_pass+ 0x18; //80λ�õ�ָ���ȥ��ʼָ�룬�õ������ֽڿ�ʼλ��
			mft80h.i_size = byte2Int(tempBuff + 0x10, 4);
		}
		else //�ǳ�פ����
		{
			mft80h.b_in = false;
			mft80h.i_size = byte2Int(tempBuff + 0x30, 8);
			mft80h.runlist = get80DataList(tempBuff + 0x40);
		}
	}
	return mft80h;
}
MFTB0h DiskFunction::getB0h(unsigned char *lpStrBuff)
{
	MFTB0h mftB0h;

	WORD wMFTHeadAttrLen = 0; //WORD 16λ������
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	DWORD32 i_temp;
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//�õ�������ʼλ��
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	tempBuff += wMFTHeadAttrLen; //�õ���һ��ƫ����
	//����10����
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
	}
	//����20����
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
	}
	//����30����
	if (*(LPDWORD)tempBuff == 0x30)
	{
		dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT30AttrLen;
		//����¸����Ի���30���ԣ�˵���ǳ��ļ�����Ҫ������ȥ
		if (*(LPDWORD)tempBuff == 0x30)
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
		}
	}
	//����40����
	if (*(LPDWORD)tempBuff == 0x40)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//����50����
	if (*(LPDWORD)tempBuff == 0x50)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//����60����
	if (*(LPDWORD)tempBuff == 0x60)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//����70����
	if (*(LPDWORD)tempBuff == 0x80)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//��ʼ����B0����
	if (*(LPDWORD)tempBuff == 0xB0)
	{
		//��פ����,�õ����ļ���С
		if (*(tempBuff + 8) == 0)
		{
			mftB0h.b_in = true;
			//mft80h.i_size = byte2Int(tempBuff + 0x10, 4);
		}
		else //�ǳ�פ����
		{
			mftB0h.b_in = false;
			//mftB0h.i_size = byte2Int(tempBuff + 0x30, 8);
			mftB0h.runlist = get80DataList(tempBuff + 0x40);
		}
	}
	return mftB0h;
}
//���������ĸ��ֽڵõ�FAT��ʱ���ʾ
CString DiskFunction::getExFATTime(unsigned char* str, byte i_time)
{
	WORD w_time = byte2Int(str, 2); //�õ�16λ������ֵ��
	WORD i_second = w_time & 0x1F; //�õ���,��λ��2��
	WORD i_minute = (w_time >>5)& 0X3F; //�õ���
	WORD i_hour = (w_time >>11) & 0X1F; //�õ�Сʱ
	i_second = i_second * 2 + i_time * 10 / 1000;
	
	w_time = byte2Int(str + 2, 2); //�õ�16λ������ֵ
	WORD i_day = w_time & 0x1F;
	WORD i_month = (w_time >>5) &0XF; //�õ���
	WORD i_year = (w_time >>9) &0X7F; //�õ���
	i_year = i_year + 1980;
	CString s;
	s.Format("%d-%d-%d  %d:%d:%d", i_year, i_month, i_day, i_hour, i_minute, i_second);
	return s;
}
CString DiskFunction::getExFATFileName(unsigned char * str,byte i_length)
{
	int d = 0;
	unsigned char * file = new unsigned char[2*i_length];
	for (int i = 0; i < i_length / 15 + 1; i++)
	{
		for (int j = 2; j < 32; j++)
		{
			if (d < 2 * i_length)
			{
				file[d] = str[i * 32 + j];
				d++;
			}
			else
				break;
			
		}
	}
	//�õ�GBK������ַ���
	char * getGBK = NULL;
	//wchar_t��Unicode�õĿ��ַ����͡���˫�ֱ�ʾ���ַ�.�����ַ���Ϊ˫��
	wchar_t fileName[256] = { 0 };
	memcpy(fileName, file, i_length * 2);
	DWORD NeedBytes = WideCharToMultiByte(CP_ACP, NULL, fileName, wcslen(fileName), NULL, 0, NULL, NULL);
	USES_CONVERSION;
	getGBK = W2A(fileName);
	return getGBK;
}