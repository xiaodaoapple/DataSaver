#pragma once
class ExFATFunction
{
public:
	ExFATFunction(ExFatBPB);
	~ExFATFunction();
private:
	ExFatBPB bpb;
public:
	//����أ��õ�������
	UINT64 getSectorFromCu(UINT cu);
	vector <UINT32> getDiretoryCuVector(UINT32 cu);
};

