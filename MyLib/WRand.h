#pragma once

#include <Windows.h>
#include <string>

class WRand
{
public:
	WRand();
	~WRand();

	std::string Generate(int iLong, BOOL bUpLow, BOOL bNum, BOOL bNumOnly);
	char GetChar(int Flag);
};

