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

//ntfs文件标志
if (iOutcome == 0X5346544E)
{
	result = "NTFS";
}
//FAT32文件标志
if (iOutcome == 0X4F44534D)
{
	result = "FAT32";

}
//ExFAT定义
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
	FILETIME ft; //定义filetime结构，并将数组赋值给他
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
	//得到GBK编码的字符串
	char * getGBK = NULL;
	WORD wMFTHeadAttrLen = 0; //WORD 16位短整数
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	//wchar_t是Unicode用的宽字符类型。用双字表示的字符.汉字字符即为双字
	wchar_t fileName[256] = { 0 };
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//得到属性起始位置
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	tempBuff += wMFTHeadAttrLen; //得到第一个偏移量
	//跳过10属性
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
	}
	//跳过20属性
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
	}
	//解析30属性
	if (*(LPDWORD)tempBuff == 0x30)
	{
		DWORD i_temp = *(LPWORD)(tempBuff + 0x14); //得到属性体的开始地址
		byte i_judge = *(tempBuff + i_temp + 0x41); //得到命名空间，
		//如果命名空间是1或者3，说明对应的30h是长文件名
		if (i_judge == 1 || i_judge == 3 ||i_judge==0)
		{

		}
		else //命名空间是2，说明是短文件名，下个属性必然是长文件名
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
		}
		
		/* 实际证明，长文件30属性第二个不一定完整描述，只能通过命名空间进行判断。
		//如果下个属性不是30属性，说明是短文件，需要跳回去，长文件30属性永远是第二个
		if (*(LPDWORD)tempBuff != 0x30)
		{
			tempBuff -= dwMFT30AttrLen;
		}
		*/
		DWORD NeedBytes;
		DWORD AttrValOffset = *(LPWORD)(tempBuff + 0x14); //30属性的偏移
		tempBuff += AttrValOffset; //此时tempBuff的位置为30h属性体的地址

		mft30h.i_father = byte2Int(tempBuff, 6); //得到其父目录的文件号
		mft30h.st_fileTime = getNTFSDateTime(tempBuff + 0x20); //得到30h属性的最后访问时间

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
//解析runlist，结果放到vector中
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
	lowForBit = *(lpStrBuff) / 16;  //得到高位，占用的簇数
	hiForBit = *(lpStrBuff)& 0x0F; //得到低位，几个字节表示簇的开始
	i_cu = byte2Int(lpStrBuff + 1, hiForBit); //得到第一个数据列表的占用的簇数
	i_cu_begin = byte2Int(lpStrBuff + 1 + hiForBit, lowForBit);//得到第一个数据列表的簇的起始	
	//将第一个数据列表的解析放置到vector中
	rd.i_cuNumber = i_cu;
	rd.i_cuBegin = i_cu_begin;
	runlist.push_back(rd);

	i_cu_calc = i_cu_begin;
	lpStrBuff += 1 + hiForBit + lowForBit; //定位到下一个列表的开始
	while ((*(LPWORD)(lpStrBuff) != 0XFFFF) && (*lpStrBuff != 0))
	{
		runlistData rdTemp; //定义临时runlist单个解析变量

		DWORD64 d_temp=0;
		_int64 i_temp=0;
		lowForBit = *(lpStrBuff) / 16;  //得到高位，占用的簇数
		hiForBit = *(lpStrBuff)& 0x0F; //得到低位，几个字节表示簇的开始
		i_cu = byte2Int(lpStrBuff + 1, hiForBit); //得到数据列表的占用的簇数
		d_temp = byte2Int(lpStrBuff + 1 + hiForBit, lowForBit);//得到数据列表的簇的起始位置，第二个数据列表开始，为有符号值，需要对计算得到的值进行处理.
		//做高位字节如果小于128，证明其为正数，其簇相对于前一个簇位置为正
		if (*(lpStrBuff + hiForBit + lowForBit) < 128)
		{
			i_cu_calc += d_temp; //第二个列表解析开始，都是上一个列表的簇位置相加。
		}
		else //最高位字节大于127，说明其为负数。需要进行处理
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
		lpStrBuff += 1 + hiForBit + lowForBit; //定位到下一个列表的开始
	}
	return runlist;
}
MFT80h DiskFunction::get80h(unsigned char *lpStrBuff)
{
	MFT80h mft80h;
	WORD i_pass=0; //记录跳过的字节数
	WORD wMFTHeadAttrLen = 0; //WORD 16位短整数
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	DWORD32 i_temp;
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//得到属性起始位置
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	
	tempBuff += wMFTHeadAttrLen; //得到第一个偏移量
	i_pass += wMFTHeadAttrLen;
	//跳过10属性
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
		i_pass += dwMFT10AttrLen;
	}
	//跳过20属性
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
		i_pass += dwMFT20AttrLen;
	}
	//解析30属性
	if (*(LPDWORD)tempBuff == 0x30)
	{
		dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT30AttrLen;
		i_pass += dwMFT30AttrLen;
		//如果下个属性还是30属性，说明是长文件，需要再跳过去
		if (*(LPDWORD)tempBuff == 0x30)
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
			i_pass += dwMFT30AttrLen;
		}
	}
	//跳过40属性
	if (*(LPDWORD)tempBuff == 0x40)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//跳过50属性
	if (*(LPDWORD)tempBuff == 0x50)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//跳过60属性
	if (*(LPDWORD)tempBuff == 0x60)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//跳过70属性
	if (*(LPDWORD)tempBuff == 0x70)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
		i_pass += i_temp;
	}
	//开始处理80属性
	if (*(LPDWORD)tempBuff == 0x80)
	{
		//常驻属性,得到其文件大小
		if (*(tempBuff + 8) == 0)
		{
			mft80h.b_in = true;
			mft80h.b_start =i_pass+ 0x18; //80位置的指针减去初始指针，得到内容字节开始位置
			mft80h.i_size = byte2Int(tempBuff + 0x10, 4);
		}
		else //非常驻属性
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

	WORD wMFTHeadAttrLen = 0; //WORD 16位短整数
	DWORD32 dwMFT10AttrLen, dwMFT20AttrLen, dwMFT30AttrLen;
	DWORD32 i_temp;
	LPBYTE tempBuff = NULL;
	tempBuff = lpStrBuff;
	//得到属性起始位置
	wMFTHeadAttrLen = *(LPWORD)(lpStrBuff + 0x14);
	tempBuff += wMFTHeadAttrLen; //得到第一个偏移量
	//跳过10属性
	if (*(LPDWORD)tempBuff == 0x10)
	{
		dwMFT10AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT10AttrLen;
	}
	//跳过20属性
	if (*(LPDWORD)tempBuff == 0x20)
	{
		dwMFT20AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT20AttrLen;
	}
	//解析30属性
	if (*(LPDWORD)tempBuff == 0x30)
	{
		dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += dwMFT30AttrLen;
		//如果下个属性还是30属性，说明是长文件，需要再跳过去
		if (*(LPDWORD)tempBuff == 0x30)
		{
			dwMFT30AttrLen = *(LPDWORD)(tempBuff + 0x04);
			tempBuff += dwMFT30AttrLen;
		}
	}
	//跳过40属性
	if (*(LPDWORD)tempBuff == 0x40)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//跳过50属性
	if (*(LPDWORD)tempBuff == 0x50)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//跳过60属性
	if (*(LPDWORD)tempBuff == 0x60)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//跳过70属性
	if (*(LPDWORD)tempBuff == 0x80)
	{
		i_temp = *(LPDWORD)(tempBuff + 0x04);
		tempBuff += i_temp;
	}
	//开始处理B0属性
	if (*(LPDWORD)tempBuff == 0xB0)
	{
		//常驻属性,得到其文件大小
		if (*(tempBuff + 8) == 0)
		{
			mftB0h.b_in = true;
			//mft80h.i_size = byte2Int(tempBuff + 0x10, 4);
		}
		else //非常驻属性
		{
			mftB0h.b_in = false;
			//mftB0h.i_size = byte2Int(tempBuff + 0x30, 8);
			mftB0h.runlist = get80DataList(tempBuff + 0x40);
		}
	}
	return mftB0h;
}
//从连续的四个字节得到FAT的时间表示
CString DiskFunction::getExFATTime(unsigned char* str, byte i_time)
{
	WORD w_time = byte2Int(str, 2); //得到16位二进制值，
	WORD i_second = w_time & 0x1F; //得到秒,单位是2秒
	WORD i_minute = (w_time >>5)& 0X3F; //得到分
	WORD i_hour = (w_time >>11) & 0X1F; //得到小时
	i_second = i_second * 2 + i_time * 10 / 1000;
	
	w_time = byte2Int(str + 2, 2); //得到16位二进制值
	WORD i_day = w_time & 0x1F;
	WORD i_month = (w_time >>5) &0XF; //得到月
	WORD i_year = (w_time >>9) &0X7F; //得到年
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
	//得到GBK编码的字符串
	char * getGBK = NULL;
	//wchar_t是Unicode用的宽字符类型。用双字表示的字符.汉字字符即为双字
	wchar_t fileName[256] = { 0 };
	memcpy(fileName, file, i_length * 2);
	DWORD NeedBytes = WideCharToMultiByte(CP_ACP, NULL, fileName, wcslen(fileName), NULL, 0, NULL, NULL);
	USES_CONVERSION;
	getGBK = W2A(fileName);
	return getGBK;
}