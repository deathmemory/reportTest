// airlogcpy.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma once
class CAirLog;
class CAirLogEx;
#include <string>
#include "../lib_json/json.h"

#define ZIPPACK_FILENAME						_T("SourceLog.dat")
#define ZIPPACK_FILENAME_TIMELINE				_T("timeline.dat")

namespace AIRLOG_CONVERT
{
	VOID Init(DWORD dwMessage, DWORD dwParam);

	BOOL GetStrForRirCmd(BYTE* baseadd,CHAR* str);
	BOOL GetLoLLog(DWORD baseaddr);
	BOOL HookProgm(void* callback);
	VOID UnHookProgm();
	VOID LogConvertAndCommit(CAirLog& Airlog, char* lpMemory);
	BOOL CommintAirLog(CAirLog& Airlog, std::string &strContext);
	BOOL PostPersonLog(CAirLog& Airlog);
	VOID GetAuthorCode(CAirLog& Airlog, std::string& strData );
	DWORD GetWinTeam(const CAirLog& Airlog);
	DWORD GetWinTeam(const CAirLogEx& Airlog);

	DWORD WINAPI LogWorkThread(LPVOID lpMemory);
	
	DWORD WINAPI CommitSrcLogThread(LPVOID lpfilename);
	BOOL PackupSourceLog(CString strZipPath , CString strSourceName);
	BOOL CommitSourcesLog(CString strFilePath);

	VOID ConverPostData(CAirLog& Airlog, std::string& strData);
	VOID ConverPostData2(CAirLogEx& Airlog, Json::Value& jsData);
	//BOOL PackupByZlib(CString strZipPath , CString strSourceName);
	DWORD WINAPI CommintTimeLineThread(LPVOID lpfilename);
	BOOL CommintTimeLine(CString strFilePath);

	VOID LogConvertAndCommit2(CAirLogEx& Airlog);
	BOOL fnConvertLog2Json(Json::Value& root, const CAirLogEx& Airlog);
	BOOL fnUploadLog(Json::Value& postTdata, std::string& postBdata);
};
