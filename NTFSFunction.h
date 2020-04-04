#pragma once
class NTFSFunction
{
public:
	unsigned int i_cu; //ÿ��������
	unsigned int i_MFT_begin; //MFT����ʼ��
	unsigned int i_PartitionBegin; //��������ʼ����
public:
	NTFSFunction(unsigned int i_cu, unsigned int i_MFT_begin, unsigned int i_PartitionBegin);
	NTFSFunction();
	~NTFSFunction();
	unsigned _int64 getNTFSSectorNumber(DWORD32 i); //�����ļ���¼�ŵõ�����
	unsigned _int64 getNTFSSectorFromCu(DWORD32 i); //����غŵõ�����
};

