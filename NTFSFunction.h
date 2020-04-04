#pragma once
class NTFSFunction
{
public:
	unsigned int i_cu; //每簇扇区数
	unsigned int i_MFT_begin; //MFT的起始簇
	unsigned int i_PartitionBegin; //分区的起始扇区
public:
	NTFSFunction(unsigned int i_cu, unsigned int i_MFT_begin, unsigned int i_PartitionBegin);
	NTFSFunction();
	~NTFSFunction();
	unsigned _int64 getNTFSSectorNumber(DWORD32 i); //输入文件记录号得到扇区
	unsigned _int64 getNTFSSectorFromCu(DWORD32 i); //输入簇号得到扇区
};

