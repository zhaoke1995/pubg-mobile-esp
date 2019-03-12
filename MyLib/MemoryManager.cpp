#include "MemoryManager.h"
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")


MemoryManager::MemoryManager(HANDLE hp)
{
	hProcess = hp;
}


MemoryManager::~MemoryManager()
{
}



BOOL MemoryManager::WPM(DWORD_PTR address, LPVOID buff, DWORD bufSize)
{
	return WriteProcessMemory(hProcess, (LPVOID)address, buff, bufSize, NULL);
}

DWORD_PTR MemoryManager::GetModuleBase(LPCTSTR szModName)
{
	return 0;
}
//参数：buffer，要查找的字符串,buffer长度，字符串长度。返回索引
int MemFind(BYTE *buffer, int dwBufferSize, BYTE *bstr, DWORD dwStrLen)
{
	if (dwBufferSize <0)
	{
		return -1;
	}
	DWORD  i, j;
	for (i = 0; i < dwBufferSize; i++)
	{
		for (j = 0; j < dwStrLen; j++)
		{
			if (buffer[i + j] != bstr[j] && bstr[j] != '?')
				break;
		}
		if (j == dwStrLen)
			return i;
	}
	return -1;
}

//也是从前往后搜索
int SundaySearch(BYTE* bStartAddr, int dwSize, BYTE* bSearchData, DWORD dwSearchSize)
{
	if (dwSize < 0)
	{
		return -1;
	}
	int iIndex[256] = { 0 };
	int i, j;
	DWORD k;
	//初始化索引
	for (i = 0; i < 256; i++)
	{
		iIndex[i] = -1;
	}

	j = 0;
	for (i = dwSearchSize - 1; i >= 0; i--)
	{
		if (iIndex[bSearchData[i]] == -1)
		{
			iIndex[bSearchData[i]] = dwSearchSize - i;
			if (++j == 256)
				break;
		}
	}
	i = 0;
	BOOL bFind = FALSE;
	//j=dwSize-dwSearchSize+1;
	j = dwSize - dwSearchSize + 1;
	while (i < j)
	{
		for (k = 0; k < dwSearchSize; k++)
		{
			if (bStartAddr[i + k] != bSearchData[k])
				break;
		}
		if (k == dwSearchSize)
		{
			//ret=bStartAddr+i;
			bFind = TRUE;
			break;
		}
		if (i + dwSearchSize >= dwSize)
		{
			//防止内存访问错误
			return -1;
		}
		k = iIndex[bStartAddr[i + dwSearchSize]];
		if (k == -1)
			i = i + dwSearchSize + 1;
		else
			i = i + k;
	}
	if (bFind)
	{
		return i;
	}
	else
		return -1;

}

BOOL MemoryManager::MemSearch(BYTE * bSearchData,int nSearchSize,DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR> &vRet)
{
	BYTE* pCurrMemoryData = NULL;
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x1000;
	DWORD dwAddress = dwStartAddr;
	//搜索内存块,放到vector数组中
	while (VirtualQueryEx(hProcess, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress))
	{
		if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE))
		{
			MEMORY_REGION mData = {0};
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);
		}
		dwAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;
	}

	//查找内存块
	std::vector<MEMORY_REGION>::iterator it;
	for (it = m_vMemoryRegion.begin(); it != m_vMemoryRegion.end(); it++)
	{
		MEMORY_REGION mData = *it;

		//如果是当前进程，直接用指针
		DWORD_PTR dwNumberOfBytesRead = 0;
		if (bIsCurrProcess)
		{
			pCurrMemoryData = (BYTE*)mData.dwBaseAddr;
			dwNumberOfBytesRead = mData.dwMemorySize;
		}
		else
		{
			pCurrMemoryData = new BYTE[mData.dwMemorySize];//先枚举再搜索的，所以这块内存不会被搜索
			ZeroMemory(pCurrMemoryData, mData.dwMemorySize);
			ReadProcessMemory(hProcess, (LPCVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);
			if ((int)dwNumberOfBytesRead <= 0)
			{
				delete[] pCurrMemoryData;
				continue;
			}
		}
		if (iSearchMode == 0)
		{
			DWORD_PTR dwOffset = 0;
			int iOffset = MemFind(pCurrMemoryData, dwNumberOfBytesRead, bSearchData,nSearchSize);
			while (iOffset !=-1)
			{
				dwOffset += iOffset;
				vRet.push_back(dwOffset + mData.dwBaseAddr);
				dwOffset += nSearchSize;
				iOffset = MemFind(pCurrMemoryData+ dwOffset, dwNumberOfBytesRead- dwOffset -nSearchSize, bSearchData, nSearchSize);
			}
		}
		else if (iSearchMode == 1)
		{
			
			DWORD_PTR dwOffset = 0;
			int iOffset = SundaySearch(pCurrMemoryData, dwNumberOfBytesRead, bSearchData, nSearchSize);
			while (iOffset != -1)
			{
				dwOffset += iOffset;
				vRet.push_back(dwOffset + mData.dwBaseAddr);
				dwOffset += nSearchSize;
				iOffset = MemFind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
			
		}
		//如果不是当前进程，需要释放
		if (!bIsCurrProcess && (pCurrMemoryData != NULL))
		{
			delete[] pCurrMemoryData;
			pCurrMemoryData = NULL;
		}
		
	}
	return TRUE;
}