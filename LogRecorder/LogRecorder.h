#pragma once

#include <stdio.h>

#define T_MAX_PATH				MAX_PATH*sizeof(TCHAR)
#define LOGRECORD_DEFAULT_NAME				_T("LogRecord")
#define LOGRECORD_DEFAULT_EXT				_T(".log")

//extern TCHAR g_szModuleName[MAX_PATH];

enum RECORD_LEVEL 
{
	RECORD_LEVEL_NULL,
	RECORD_LEVEL_INFO,
	RECORD_LEVEL_WARN,
	RECORD_LEVEL_ERROR,
	RECORD_LEVEL_DEBUG
};

extern TCHAR g_szModuleName[MAX_PATH];
class CLogRecorder {
private:
	CLogRecorder(void);
public:
	static void InitModuleInSide(LPTSTR lpstrName = NULL);

	static void GetCurrentLogDefaultName(CString& strLogName);

	static void Record(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""), RECORD_LEVEL rLevel = RECORD_LEVEL_INFO);
	static void Record_Debug(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
 	static void Record_Info(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	static void Record_Warn(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	static void Record_Err(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));

	static void Record_std(LPTSTR lpstrContext, LPTSTR lpstrPath = NULL, LPTSTR lpstrFileName = NULL, LPTSTR lpstrExt = NULL, RECORD_LEVEL rLevel = RECORD_LEVEL_INFO);
	static void Record_Static(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""), RECORD_LEVEL rLevel = RECORD_LEVEL_INFO);
	static void Record_Static_Debug(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	static void Record_Static_Info(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	static void Record_Static_Warn(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	static void Record_Static_Err(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""));
	
};

#ifdef _LOG_DEBUG
	#define RECORD_STATIC_DEBUG(STRCONTEXT, STRPATH)	CLogRecorder::Record_Static_Debug(STRCONTEXT, STRPATH)
	#define __RECORD_STATIC_DEBUG(STRCONTEXT, STRPATH, STRFILENAME, STREXT)	CLogRecorder::Record_Static_Debug(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
#else
	#define RECORD_STATIC_DEBUG(STRCONTEXT, STRPATH)
	#define __RECORD_STATIC_DEBUG(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
#endif

#ifndef _DM_NO_RECORD
	#define RECORD_STATIC_INFO(STRCONTEXT, STRPATH)	CLogRecorder::Record_Static_Info(STRCONTEXT, STRPATH)
	#define RECORD_STATIC_WARN(STRCONTEXT, STRPATH)	CLogRecorder::Record_Static_Warn(STRCONTEXT, STRPATH)
	#define RECORD_STATIC_ERR(STRCONTEXT, STRPATH)		CLogRecorder::Record_Static_Err(STRCONTEXT, STRPATH)

	#define __RECORD_STATIC_INFO(STRCONTEXT, STRPATH, STRFILENAME, STREXT)	CLogRecorder::Record_Static_Info(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define __RECORD_STATIC_WARN(STRCONTEXT, STRPATH, STRFILENAME, STREXT)	CLogRecorder::Record_Static_Warn(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define __RECORD_STATIC_ERR(STRCONTEXT, STRPATH, STRFILENAME, STREXT)		CLogRecorder::Record_Static_Err(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
#else
	#define RECORD_STATIC_INFO(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define RECORD_STATIC_WARN(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define RECORD_STATIC_ERR(STRCONTEXT, STRPATH, STRFILENAME, STREXT)

	#define __RECORD_STATIC_INFO(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define __RECORD_STATIC_WARN(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
	#define __RECORD_STATIC_ERR(STRCONTEXT, STRPATH, STRFILENAME, STREXT)
#endif
