// LogRecorder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LogRecorder.h"
//#include "LogRecorderApp.h"

static TCHAR g_szModuleName[MAX_PATH] = {0};

void CLogRecorder::InitModuleInSide(LPTSTR lpstrName/* = NULL*/)
{
	if (lpstrName)
	{
		_tcscpy_s(g_szModuleName, MAX_PATH, lpstrName);
	} 
	else
	{
		TCHAR* ptszLevel = NULL;
		TCHAR tstrName[T_MAX_PATH] = {0};
		GetModuleFileName(NULL, tstrName, T_MAX_PATH);
		TCHAR* lptstrDirRoot = _tcsrchr(tstrName, _T('\\'));
		if (lptstrDirRoot)
			_tcscpy_s(g_szModuleName, MAX_PATH, lptstrDirRoot+1);
	}

}

void CLogRecorder::GetCurrentLogDefaultName(CString& strLogName)
{
	SYSTEMTIME st = {0};
	GetSystemTime(&st);
	strLogName.Format(_T("%s%d%02d%02d%s"), LOGRECORD_DEFAULT_NAME, st.wYear, st.wMonth, st.wDay, LOGRECORD_DEFAULT_EXT);
}

void CLogRecorder::Record_std(LPTSTR lpstrContext, LPTSTR lpstrPath/* = NULL*/, LPTSTR lpstrFileName/* = NULL*/, LPTSTR lpstrExt/* = NULL*/, RECORD_LEVEL rLevel/* = RECORD_LEVEL_INFO*/)
{
	DWORD dwErr = GetLastError();
	SYSTEMTIME st;
	TCHAR tstrCurrentDir[T_MAX_PATH] = {0};
	TCHAR* ptszLevel = NULL;
	int nPathLen = lstrlen(lpstrPath);
	if ( ! nPathLen )
	{
		GetModuleFileName(NULL, tstrCurrentDir, T_MAX_PATH);
		TCHAR* lptstrDirRoot = _tcsrchr(tstrCurrentDir, _T('\\'));
		if (lptstrDirRoot)
		{
			nPathLen = lptstrDirRoot-tstrCurrentDir;
			tstrCurrentDir[nPathLen+1] = 0x00;
		}
		else	//not a valid path
			return;
	}
	else
		_tcscpy_s(tstrCurrentDir, nPathLen*sizeof(TCHAR), lpstrPath);

	ZeroMemory(&st, sizeof(SYSTEMTIME));
	GetLocalTime(&st);

	switch (rLevel)
	{
	case RECORD_LEVEL_INFO:
		ptszLevel = _T("INFO");
		break;
	case RECORD_LEVEL_WARN:
		ptszLevel = _T("WARN");
		break;
	case RECORD_LEVEL_ERROR:
		ptszLevel = _T("ERROR");
		break;
	default:
		ptszLevel = _T("NULL");
		break;
	}
	if ( ! lstrlen(lpstrFileName) )	lpstrFileName = LOGRECORD_DEFAULT_NAME;
	if ( ! lstrlen(lpstrExt) )	lpstrExt = LOGRECORD_DEFAULT_EXT;
	TCHAR tszFullFileName[T_MAX_PATH] = {0};
	_stprintf(tszFullFileName, _T("%s%s%04d%02d%02d%s"), tstrCurrentDir, lpstrFileName, st.wYear, st.wMonth, st.wDay, lpstrExt);
	FILE * hFile = _tfopen( tszFullFileName, _T("ab+") );
	if (hFile)
	{
		int nLen = lstrlen(g_szModuleName)+lstrlen(lpstrContext)+1;
		TCHAR* pContext = new TCHAR[nLen*sizeof(TCHAR)];
		ZeroMemory(pContext, nLen*sizeof(TCHAR));
		_stprintf(pContext, _T("%s %s"), g_szModuleName, lpstrContext);
		TCHAR* pRealContext = pContext;
		if ( g_szModuleName[0] == 0 )
			pRealContext += 1;	//跳过多余的空格
		TCHAR* lpszFormat = _T("[%s] %d-%02d-%02d %02d:%02d:%02d _std err:%d \t[%s]\r\n");
		int nFormatlen = (lstrlen(lpszFormat)+lstrlen(lpstrContext));
		TCHAR* lpszFormatContent = new TCHAR[nFormatlen*2];
		ZeroMemory(lpszFormatContent, nFormatlen*2);
		_stprintf(lpszFormatContent, lpszFormat,
			ptszLevel,
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond,
			dwErr,
			pRealContext);
#ifdef _UNICODE
		int nCount = WideCharToMultiByte(CP_ACP, 0, lpszFormatContent, -1, NULL, 0, NULL, NULL);
		CHAR* pMultiBytesChar = new CHAR[nCount + 1];
		ZeroMemory(pMultiBytesChar, nCount + 1);
		WideCharToMultiByte(CP_ACP, 0, lpszFormatContent, -1, pMultiBytesChar, nCount, NULL, NULL);
		fwrite(pMultiBytesChar, lstrlenA(pMultiBytesChar), 1, hFile);
		delete pMultiBytesChar;
#else
		fwrite(lpszFormatContent, lstrlen(lpszFormatContent)*sizeof(TCHAR), 1, hFile);
#endif

		fclose(hFile);
		delete lpszFormatContent;
		delete pContext;
	}
	SetLastError(dwErr);	//保证当时环境的 Error Num
}
void CLogRecorder::Record_Static(CString strContext, CString strPath/* = _T("")*/, CString strFileName/* = _T("")*/, CString strExt/* = _T("")*/, RECORD_LEVEL rLevel/* = RECORD_LEVEL_INFO*/)
{
	Record_std(strContext.GetBuffer(strContext.GetLength()),
		strPath.GetBuffer(strPath.GetLength()),
		strFileName.GetBuffer(strFileName.GetLength()),
		strExt.GetBuffer(strExt.GetLength()),
		rLevel);
	strContext.ReleaseBuffer();	strPath.ReleaseBuffer();	strFileName.ReleaseBuffer();	strExt.ReleaseBuffer();
}
void CLogRecorder::Record_Static_Debug(CString strContext, CString strPath/* = _T("")*/, CString strFileName/* = _T("")*/, CString strExt/* = _T("")*/)
{
	Record_Static(strContext, strPath, strFileName, strExt, RECORD_LEVEL_DEBUG);
}
void CLogRecorder::Record_Static_Info(CString strContext, CString strPath/* = _T("")*/, CString strFileName/* = _T("")*/, CString strExt /*= _T("")*/)
{
	Record_std(strContext.GetBuffer(strContext.GetLength()),
		strPath.GetBuffer(strPath.GetLength()),
		strFileName.GetBuffer(strFileName.GetLength()),
		strExt.GetBuffer(strExt.GetLength()),
		RECORD_LEVEL_INFO);
	strContext.ReleaseBuffer();	strPath.ReleaseBuffer();	strFileName.ReleaseBuffer();	strExt.ReleaseBuffer();
}
void CLogRecorder::Record_Static_Warn(CString strContext, CString strPath/* = _T("")*/, CString strFileName/* = _T("")*/, CString strExt/* = _T("")*/)
{
	Record_std(strContext.GetBuffer(strContext.GetLength()),
		strPath.GetBuffer(strPath.GetLength()),
		strFileName.GetBuffer(strFileName.GetLength()),
		strExt.GetBuffer(strExt.GetLength()),
		RECORD_LEVEL_WARN);
	strContext.ReleaseBuffer();	strPath.ReleaseBuffer();	strFileName.ReleaseBuffer();	strExt.ReleaseBuffer();
}
void CLogRecorder::Record_Static_Err(CString strContext, CString strPath/* = _T("")*/, CString strFileName/* = _T("")*/, CString strExt /*= _T("")*/)
{
	Record_std(strContext.GetBuffer(strContext.GetLength()),
		strPath.GetBuffer(strPath.GetLength()),
		strFileName.GetBuffer(strFileName.GetLength()),
		strExt.GetBuffer(strExt.GetLength()),
		RECORD_LEVEL_ERROR);
	strContext.ReleaseBuffer();	strPath.ReleaseBuffer();	strFileName.ReleaseBuffer();	strExt.ReleaseBuffer();
}