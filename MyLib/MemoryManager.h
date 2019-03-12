#pragma once
#include <Windows.h>
#include <vector>
#include <list>

typedef struct _MEMORY_REGION
{
	DWORD_PTR dwBaseAddr;
	DWORD_PTR dwMemorySize;
}MEMORY_REGION;

class MemoryManager
{
public:
	MemoryManager(HANDLE hp);
	~MemoryManager();
	HANDLE hProcess;

	template<typename T> T RPM(DWORD_PTR address, DWORD bufSize)
	{
		T buff;
		ReadProcessMemory(hProcess, (LPCVOID)address, &buff, bufSize, NULL);
		return buff;
	}
	BOOL WPM(DWORD_PTR address, LPVOID buff, DWORD bufSize);

	DWORD_PTR GetModuleBase(LPCTSTR szModName);
	//iSearchMode 0:memfind 支持通配 1 sunday算法
	BOOL MemSearch(BYTE * bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR> &vRet);
};

