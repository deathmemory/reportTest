#pragma once
#include "stdafx.h"
namespace usp10Lib
{
	BOOL WINAPI Load();
	VOID WINAPI Free();
	FARPROC WINAPI GetAddress(LPSTR pszProcName);
};
#include "usp10.cpp"
