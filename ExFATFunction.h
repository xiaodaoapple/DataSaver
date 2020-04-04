#pragma once
class ExFATFunction
{
public:
	ExFATFunction(ExFatBPB);
	~ExFATFunction();
private:
	ExFatBPB bpb;
public:
	//输入簇，得到扇区数
	UINT64 getSectorFromCu(UINT cu);
	vector <UINT32> getDiretoryCuVector(UINT32 cu);
};

