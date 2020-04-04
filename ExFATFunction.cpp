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
//��������ص�������
UINT64 ExFATFunction::getSectorFromCu(UINT cu)
{
	return (cu - 2)*bpb.i_cu + bpb.i_cuBegin;
}
vector <UINT32> ExFATFunction::getDiretoryCuVector(UINT32 cu)
{
	byte sector[512];
	DiskFunction df;
	vector<UINT32> cuVector;
	cuVector.push_back(cu); //����һ��ֵѹ��vector
	
	//��ʼ�жϴ�����λ��,ÿһ��fat�������Դ洢128����������һ��fat�������0��ʼ���洢0--127����
	UINT32 i_fromFat = cu / 128;
	UINT32 i_inFat = cu % 128;
	//��λ����i_inFat*4��λ�á�
	df.ReadDisk(bpb.s_driver, sector, i_fromFat + bpb.i_FATBegin);
	UINT32 i_nextCu = df.byte2Int(&sector[i_inFat*4], 4);
	while (i_nextCu != 0xFFFFFFFF && i_nextCu!=0) //��������ŵ�������δ���������������������
	{
		cuVector.push_back(i_nextCu);
		i_fromFat = i_nextCu / 128;
		i_inFat = i_nextCu % 128;
		df.ReadDisk(bpb.s_driver, sector, i_fromFat + bpb.i_FATBegin);
		i_nextCu = df.byte2Int(&sector[i_inFat*4], 4);
	}
	return cuVector;
}