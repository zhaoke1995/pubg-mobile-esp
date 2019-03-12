#include "WRand.h"
#include <time.h>


WRand::WRand()
{
	srand((unsigned)time(NULL));
}


WRand::~WRand()
{
}
char WRand::GetChar(int Flag)
{
	char m_sArray[] = ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	char m_sArrayNum[] = ("1234567890");
	char s;
	switch (Flag)
	{
	case 10:
	{
		int k = rand();
		k = k % 10;
		s = m_sArrayNum[k];
	}
	break;
	case 26:
	{
		int k = rand();
		k = k % 26;
		s = m_sArray[k];
	}
	break;
	case 52:
	{
		int k = rand();
		k = k % 52;
		s = m_sArray[k];
	}
	break;
	case 62:
	{
		int k = rand();
		k = k % 62;
		s = m_sArray[k];
	}
	break;
	default:
	{
		int k = rand();
		k = k % 26;
		s = m_sArray[k];
	}
	break;
	}

	return s;
}
std::string WRand::Generate(int iLong, BOOL bUpLow, BOOL bNum, BOOL bNumOnly)
{
	srand((unsigned)time(NULL));
	int iFlag = 26;
	std::string str;
	if (bUpLow == TRUE)
		iFlag = 52;
	if (bNum == TRUE)
		iFlag = 62;
	if (bNumOnly == TRUE)
		iFlag = 10;

	for (int i = 0; i < iLong; i++)
	{
		str += GetChar(iFlag);
	}
	return str;
}