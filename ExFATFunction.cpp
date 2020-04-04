#include "stdafx.h"
#include "ExFATFunction.h"
#include "DiskFunction.h"

ExFATFunction::ExFATFunction(ExFatBPB exfatbpb)
{
	bpb = exfatbpb;
}


ExFATFunction::~ExFATFunction()
{
}
//返回输入簇的扇区号
UINT64 ExFATFunction::getSectorFromCu(UINT cu)
{
	return (cu - 2)*bpb.i_cu + bpb.i_cuBegin;
}
vector <UINT32> ExFATFunction::getDiretoryCuVector(UINT32 cu)
{
	byte sector[512];
	DiskFunction df;
	vector<UINT32> cuVector;
	cuVector.push_back(cu); //将第一个值压入vector
	
	//开始判断簇链的位置,每一个fat扇区可以存储128个簇链，第一个fat扇区编号0开始，存储0--127簇链
	UINT32 i_fromFat = cu / 128;
	UINT32 i_inFat = cu % 128;
	//其位置在i_inFat*4的位置。
	df.ReadDisk(bpb.s_driver, sector, i_fromFat + bpb.i_FATBegin);
	UINT32 i_nextCu = df.byte2Int(&sector[i_inFat*4], 4);
	while (i_nextCu != 0xFFFFFFFF && i_nextCu!=0) //簇连续存放的问题尚未解决！！！！！！！！！
	{
		cuVector.push_back(i_nextCu);
		i_fromFat = i_nextCu / 128;
		i_inFat = i_nextCu % 128;
		df.ReadDisk(bpb.s_driver, sector, i_fromFat + bpb.i_FATBegin);
		i_nextCu = df.byte2Int(&sector[i_inFat*4], 4);
	}
	return cuVector;
}