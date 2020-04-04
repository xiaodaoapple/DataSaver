#include "stdafx.h"
#include "NTFSFunction.h"


NTFSFunction::NTFSFunction(unsigned int cu, unsigned int MFT_begin, unsigned int PartitionBegin)
{
	i_cu = cu;
	i_MFT_begin = MFT_begin;
	i_PartitionBegin = PartitionBegin;
}
NTFSFunction::NTFSFunction()
{
	i_cu = 0;
	i_MFT_begin = 0;
	i_PartitionBegin = 0;
}
unsigned _int64 NTFSFunction::getNTFSSectorNumber(DWORD32 i)
{
	return i_PartitionBegin + i_cu*i_MFT_begin + i * 2;
}
unsigned _int64 NTFSFunction::getNTFSSectorFromCu(DWORD32 i)
{
	return i_PartitionBegin + i_cu*i;
}
NTFSFunction::~NTFSFunction()
{
}
