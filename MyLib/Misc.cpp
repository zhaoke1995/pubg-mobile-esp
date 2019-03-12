#include "Misc.h"


#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>

#define MYPRINT

void MyOutputDebugStringA(const char * lpcszOutputString, ...)
{
#ifdef MYPRINT
	std::string strResult;
	if (NULL != lpcszOutputString)
	{
		va_list marker = NULL;
		va_start(marker, lpcszOutputString); //初始化变量参数
		size_t nLength = _vscprintf(lpcszOutputString, marker) + 1; //获取格式化字符串长度
		std::vector<char> vBuffer(nLength, '\0'); //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszOutputString, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker); //重置变量参数
	}
	if (!strResult.empty())
	{
		std::string strFormated = "[wwhtest] ";
		strFormated.append(strResult);
		OutputDebugStringA(strFormated.c_str());
	}
#endif
}

void MyOutputDebugStringW(const wchar_t * lpcwszOutputString, ...)
{
#ifdef MYPRINT
	std::wstring strResult;
	if (NULL != lpcwszOutputString)
	{
		va_list marker = NULL;
		va_start(marker, lpcwszOutputString); //初始化变量参数
		size_t nLength = _vscwprintf(lpcwszOutputString, marker) + 1; //获取格式化字符串长度
		std::vector<wchar_t> vBuffer(nLength, '\0'); //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcwszOutputString, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker); //重置变量参数
	}
	if (!strResult.empty())
	{
		std::wstring strFormated = L"[wwhtest] ";
		strFormated.append(strResult);
		OutputDebugStringW(strFormated.c_str());
	}
#endif
}