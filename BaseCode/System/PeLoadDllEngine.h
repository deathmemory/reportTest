#pragma once
#include <windows.h>
#include "../../CodeLibs/File/CFileIO.hpp"
#include "../../CodeLibs/Crypt/QUICK_CRYPT/QUICK_CRYPT.h"
using namespace QUICK_CRYPT;
typedef   BOOL (__stdcall *ProcDllMain)(HINSTANCE, DWORD,  LPVOID );
BOOL ChangeRavAddress(void* pImageBase,CHAR *funcName,CHAR *dllname,DWORD newAddr);
class JK_PeLoadDllEngine
{
public:
	JK_PeLoadDllEngine();
	~JK_PeLoadDllEngine();
	BOOL    MemLoadLibrary(TCHAR *filename);  // Dll file data buffer
	BOOL    MemLoadLibrary(TCHAR *filename,DWORD decodeKey); 
	void	MemFreeLibrary();
	FARPROC MemGetProcAddress(CHAR *lpProcName);
	DWORD MemOffsetProcAddr(CHAR *lpProcName);
	
private:

	BOOL isLoadOk;
	BOOL CheckDataValide(void* lpFileData, int DataLength);
	int  CalcTotalImageSize();
	void CopyDllDatas(void* pDest, void* pSrc);
	
	BOOL FillRavAddress(void* pBase);
	void DoRelocation(void* pNewBase);
	int  GetAlignedSize(int Origin, int Alignment); 
private:
	ProcDllMain pDllMain;
	typedef struct tagFILELOADER
	{
		HANDLE m_hInFile,m_hInFileMapping;
		BYTE *m_lpInImageBase;
		DWORD fileSize;
		DWORD ImageSize;
		BOOL LoadFile( TCHAR *lpFileName );//װ���ļ���ӳ��
		BOOL LoadFile( TCHAR *lpFileName,DWORD decodeKey);//װ���ļ���ӳ��
		BYTE *DecodeFile(BYTE *Src,DWORD decodekey,DWORD len);
		void UnLoadFile( void );//ж��ӳ��
		void FreeFile();//�ͷŶ�ȡ���ļ�
		tagFILELOADER(){ZeroMemory(this,sizeof(tagFILELOADER));}
	}FILELOADER;

private:
	DWORD  pImageBase;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER pSectionHeader;
};
#include "PeLoadDllEngine.cpp"