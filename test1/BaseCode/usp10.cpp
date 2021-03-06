


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 头文件
#include "stdafx.h"
#include "usp10.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:LpkPresent=_usp10Lib_LpkPresent,@1")
#pragma comment(linker, "/EXPORT:ScriptApplyDigitSubstitution=_usp10Lib_ScriptApplyDigitSubstitution,@2")
#pragma comment(linker, "/EXPORT:ScriptApplyLogicalWidth=_usp10Lib_ScriptApplyLogicalWidth,@3")
#pragma comment(linker, "/EXPORT:ScriptBreak=_usp10Lib_ScriptBreak,@4")
#pragma comment(linker, "/EXPORT:ScriptCPtoX=_usp10Lib_ScriptCPtoX,@5")
#pragma comment(linker, "/EXPORT:ScriptCacheGetHeight=_usp10Lib_ScriptCacheGetHeight,@6")
#pragma comment(linker, "/EXPORT:ScriptFreeCache=_usp10Lib_ScriptFreeCache,@7")
#pragma comment(linker, "/EXPORT:ScriptGetCMap=_usp10Lib_ScriptGetCMap,@8")
#pragma comment(linker, "/EXPORT:ScriptGetFontProperties=_usp10Lib_ScriptGetFontProperties,@9")
#pragma comment(linker, "/EXPORT:ScriptGetGlyphABCWidth=_usp10Lib_ScriptGetGlyphABCWidth,@10")
#pragma comment(linker, "/EXPORT:ScriptGetLogicalWidths=_usp10Lib_ScriptGetLogicalWidths,@11")
#pragma comment(linker, "/EXPORT:ScriptGetProperties=_usp10Lib_ScriptGetProperties,@12")
#pragma comment(linker, "/EXPORT:ScriptIsComplex=_usp10Lib_ScriptIsComplex,@13")
#pragma comment(linker, "/EXPORT:ScriptItemize=_usp10Lib_ScriptItemize,@14")
#pragma comment(linker, "/EXPORT:ScriptJustify=_usp10Lib_ScriptJustify,@15")
#pragma comment(linker, "/EXPORT:ScriptLayout=_usp10Lib_ScriptLayout,@16")
#pragma comment(linker, "/EXPORT:ScriptPlace=_usp10Lib_ScriptPlace,@17")
#pragma comment(linker, "/EXPORT:ScriptRecordDigitSubstitution=_usp10Lib_ScriptRecordDigitSubstitution,@18")
#pragma comment(linker, "/EXPORT:ScriptShape=_usp10Lib_ScriptShape,@19")
#pragma comment(linker, "/EXPORT:ScriptStringAnalyse=_usp10Lib_ScriptStringAnalyse,@20")
#pragma comment(linker, "/EXPORT:ScriptStringCPtoX=_usp10Lib_ScriptStringCPtoX,@21")
#pragma comment(linker, "/EXPORT:ScriptStringFree=_usp10Lib_ScriptStringFree,@22")
#pragma comment(linker, "/EXPORT:ScriptStringGetLogicalWidths=_usp10Lib_ScriptStringGetLogicalWidths,@23")
#pragma comment(linker, "/EXPORT:ScriptStringGetOrder=_usp10Lib_ScriptStringGetOrder,@24")
#pragma comment(linker, "/EXPORT:ScriptStringOut=_usp10Lib_ScriptStringOut,@25")
#pragma comment(linker, "/EXPORT:ScriptStringValidate=_usp10Lib_ScriptStringValidate,@26")
#pragma comment(linker, "/EXPORT:ScriptStringXtoCP=_usp10Lib_ScriptStringXtoCP,@27")
#pragma comment(linker, "/EXPORT:ScriptString_pLogAttr=_usp10Lib_ScriptString_pLogAttr,@28")
#pragma comment(linker, "/EXPORT:ScriptString_pSize=_usp10Lib_ScriptString_pSize,@29")
#pragma comment(linker, "/EXPORT:ScriptString_pcOutChars=_usp10Lib_ScriptString_pcOutChars,@30")
#pragma comment(linker, "/EXPORT:ScriptTextOut=_usp10Lib_ScriptTextOut,@31")
#pragma comment(linker, "/EXPORT:ScriptXtoCP=_usp10Lib_ScriptXtoCP,@32")
#pragma comment(linker, "/EXPORT:UspAllocCache=_usp10Lib_UspAllocCache,@33")
#pragma comment(linker, "/EXPORT:UspAllocTemp=_usp10Lib_UspAllocTemp,@34")
#pragma comment(linker, "/EXPORT:UspFreeMem=_usp10Lib_UspFreeMem,@35")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// usp10Lib 命名空间
namespace usp10Lib
{
	static HMODULE m_hModule = NULL;	// 原始模块句柄
	static DWORD m_dwReturn[35] = {0};	// 原始函数返回地址


	// 加载原始模块
	BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		GetSystemDirectory(tzPath, MAX_PATH);
		lstrcat(tzPath, TEXT("\\usp10.dll"));
		m_hModule = LoadLibrary(tzPath);
		if (m_hModule == NULL)
		{
			wsprintf(tzTemp, TEXT("无法加载 %s，程序无法正常运行。"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}

		return (m_hModule != NULL);	
	}
		
	// 释放原始模块
	VOID WINAPI Free()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}

	// 获取原始函数地址
	FARPROC WINAPI GetAddress(LPSTR pszProcName)
	{
		FARPROC fpAddress;
		CHAR szProcName[16];
		CHAR tzTemp[MAX_PATH];

		fpAddress = GetProcAddress(m_hModule, pszProcName);
		if (fpAddress == NULL)
		{
			if (HIWORD(pszProcName) == 0)
			{
				wsprintfA(szProcName, "%d", pszProcName);
				pszProcName = szProcName;
			}

			wsprintfA(tzTemp, "无法找到函数 %hs，程序无法正常运行。", pszProcName);
			MessageBoxA(NULL, tzTemp, "AheadLib", MB_ICONSTOP);
			ExitProcess(-2);
		}

		return fpAddress;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入口函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		for (INT i = 0; i < sizeof(m_dwReturn) / sizeof(DWORD); i++)
		{
			m_dwReturn[i] = TlsAlloc();
		}

		return Load();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		for (INT i = 0; i < sizeof(m_dwReturn) / sizeof(DWORD); i++)
		{
			TlsFree(m_dwReturn[i]);
		}

		Free();
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_LpkPresent(void)
{
	GetAddress("LpkPresent");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptApplyDigitSubstitution(void)
{
	GetAddress("ScriptApplyDigitSubstitution");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptApplyLogicalWidth(void)
{
	GetAddress("ScriptApplyLogicalWidth");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptBreak(void)
{
	GetAddress("ScriptBreak");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptCPtoX(void)
{
	GetAddress("ScriptCPtoX");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptCacheGetHeight(void)
{
	GetAddress("ScriptCacheGetHeight");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptFreeCache(void)
{
	GetAddress("ScriptFreeCache");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptGetCMap(void)
{
	GetAddress("ScriptGetCMap");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptGetFontProperties(void)
{
	GetAddress("ScriptGetFontProperties");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptGetGlyphABCWidth(void)
{
	GetAddress("ScriptGetGlyphABCWidth");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptGetLogicalWidths(void)
{
	GetAddress("ScriptGetLogicalWidths");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptGetProperties(void)
{
	GetAddress("ScriptGetProperties");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptIsComplex(void)
{
	GetAddress("ScriptIsComplex");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptItemize(void)
{
	GetAddress("ScriptItemize");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptJustify(void)
{
	GetAddress("ScriptJustify");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptLayout(void)
{
	GetAddress("ScriptLayout");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptPlace(void)
{
	GetAddress("ScriptPlace");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptRecordDigitSubstitution(void)
{
	GetAddress("ScriptRecordDigitSubstitution");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptShape(void)
{
	GetAddress("ScriptShape");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringAnalyse(void)
{
	GetAddress("ScriptStringAnalyse");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringCPtoX(void)
{
	GetAddress("ScriptStringCPtoX");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringFree(void)
{
	GetAddress("ScriptStringFree");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringGetLogicalWidths(void)
{
	GetAddress("ScriptStringGetLogicalWidths");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringGetOrder(void)
{
	GetAddress("ScriptStringGetOrder");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringOut(void)
{
	GetAddress("ScriptStringOut");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringValidate(void)
{
	GetAddress("ScriptStringValidate");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptStringXtoCP(void)
{
	GetAddress("ScriptStringXtoCP");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptString_pLogAttr(void)
{
	GetAddress("ScriptString_pLogAttr");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptString_pSize(void)
{
	GetAddress("ScriptString_pSize");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptString_pcOutChars(void)
{
	GetAddress("ScriptString_pcOutChars");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptTextOut(void)
{
	GetAddress("ScriptTextOut");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_ScriptXtoCP(void)
{
	GetAddress("ScriptXtoCP");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_UspAllocCache(void)
{
	GetAddress("UspAllocCache");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_UspAllocTemp(void)
{
	GetAddress("UspAllocTemp");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL usp10Lib_UspFreeMem(void)
{
	GetAddress("UspFreeMem");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
