// airlogcpy.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma once

#include "stdafx.h"
#include "../publicdef/publicdef.h"

#include "airlogcpy.h"
#include "Airlog.h"
#include "AirLogEx.h"
#include "../BaseCode/hook/inline/HookInternal.h"
#include "../BaseCode/DEBUG_CODE.h"
#include "../BaseCode/File/CFileIO.hpp"
#include "../Config/Config.h"
#include "../LogRecorder/LogRecorder.h"
#include "../MD5Lib/md5.h"
#include "../ZipArchive/ZipArchive.h"
#include "../SharedMemory/SharedMemroy.h"
#include "../ZipArchive/zlib/GZipHelper.h"

#include <windows.h>
#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")

#define UPDATE_BASE_URL			_T("http://data.lolbf.178.com:8000/set_new_history.json")
#define UPLOAD_SITE				_T("data.lolbf.178.com")	//_T("10.3.3.114")
#define UPLOAD_PORT				8000		
#define BLUE_TEAM_NUM			100
#define PURPLE_TEAM_NUM			200

const int nmemcount = 1024*9;
typedef VOID(CALLBACK* lpGFCallback)();
lpGFCallback g_callback = NULL;
namespace AIRLOG_CONVERT
{
	HOOK_INTERNAL hookinbase;
	CAirLog Airlog;
	static DWORD g_dwMsg = 0;
	static DWORD g_dwParam = 0;
	VOID Init(DWORD dwMessage, DWORD dwParam)
	{
		g_dwMsg = dwMessage, g_dwParam = dwParam;
	}

	BOOL GetStrForRirCmd(BYTE* baseadd,CHAR* str)
	{
		int i=0;
		while((BYTE)*baseadd != 0x25)
		{
			*str = *baseadd;
			str++;
			baseadd++;
			i++;
			if (i>0x50)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	DWORD WINAPI LogWorkThread(LPVOID lpMemory)
	{
		CString strBfPath = CConfig::GetLocalDir();

		SYSTEMTIME  Time={0};
		TCHAR tszFileName[MAX_PATH*5];
		GetLocalTime(&Time);//获得当前本地时间
		_stprintf(tszFileName,_T("%d-%d-%d%02d%02d%02d.log"),
			Time.wYear, Time.wMonth, Time.wDay,
			Time.wHour, Time.wMinute, Time.wSecond);
		CString strSavePath = strBfPath + tszFileName;
		FILEIO::SaveBuffToFile( strSavePath.GetBuffer(strSavePath.GetLength()), (BYTE*)lpMemory, nmemcount);
		strSavePath.ReleaseBuffer();

		if (!Airlog.TeamScan((char*)lpMemory))
			RECORD_STATIC_ERR( _T("TeamScan error!"), strBfPath );
		else			// sucessed ...
		{
			CSharedMemroy sm;
			PostMessage(sm.GetMainWindow(), g_dwMsg, g_dwParam, 0);
			AIRLOG_CONVERT::LogConvertAndCommit(Airlog, (char*)lpMemory);
		}
		

// 		HANDLE hThread = CreateThread(NULL, 0, CommitSrcLogThread, (LPVOID)tszFileName, 0, NULL);
// 		if (hThread)
// 		{
// 			WaitForSingleObject(hThread, 20*60*1000);
// 			CloseHandle(hThread);
// 		}

		delete lpMemory;
		return 0;
	}
	
	DWORD WINAPI CommitSrcLogThread(LPVOID lpfilename)
	{
		CString strBfPath = CConfig::GetLocalDir() ;
		CString strZipPath = strBfPath + ZIPPACK_FILENAME;
		if (PackupSourceLog(strZipPath, (TCHAR*)lpfilename))
		{
			RECORD_STATIC_INFO(_T("path : ")+strZipPath, strBfPath);
			AIRLOG_CONVERT::CommitSourcesLog(strZipPath);
		}
		else
			RECORD_STATIC_ERR(_T("packup sl failed !"), strBfPath);

		return 0;
	}
	

	BOOL GetLoLLog(DWORD baseaddr)
	{
		if (IsBadReadPtr((DWORD*)*(DWORD*)baseaddr,4))
		{
			return FALSE;
		}
		BYTE* logaddr = (BYTE*)*(DWORD*)baseaddr;
		BYTE* temp = logaddr;
		CHAR  str[MAX_PATH*2] = {0};
		CHAR  cmp[] = "EndOfGameStat\0";
		temp+=0x2a;
		CHAR* tempstr = str;
		DWORD dwOldFlag = 0, dwTempFlag = 0;
		if (VirtualProtect(temp, nmemcount, PAGE_READWRITE, &dwOldFlag))
		{
			if (GetStrForRirCmd(temp,tempstr))
			{
				if (StrStrIA(str,cmp))
				{
					if ( g_callback )
						g_callback();
					CHAR *tempmem = new CHAR[nmemcount];
					ZeroMemory(tempmem, nmemcount);
					CopyMemory(tempmem, logaddr, nmemcount);
					CloseHandle( CreateThread(NULL, 0, LogWorkThread, tempmem, 0, NULL) );
					RECORD_STATIC_INFO(_T("working ..."), CConfig::GetLocalDir());
				}
			}
			VirtualProtect(temp, nmemcount, dwOldFlag, &dwTempFlag);
			return TRUE;
		}
		
		return FALSE;
	}
	VOID __declspec(naked) _stdcall HookIn()
	{
		__asm
		{		
			PUSH EBP
			MOV EBP,ESP
			SUB ESP,200h
			PUSHAD
		}
		DWORD baseaddr;
		__asm
		{
			lea eax,DWORD PTR[ebp+2ch]
			mov baseaddr,eax
		}
		GetLoLLog(baseaddr);
		__asm 
		{
			POPAD
			LEAVE
			PUSH EAX
			LEA EAX,hookinbase.RestoreCodeMem.JmpBack
			PUSH EAX
			MOV EAX,[ESP+4]
			RET 4
		}
	}

	BOOL HookProgm(void* callback)
	{
		DWORD HookAdd;
		/*
		102397D0    57              push    edi
		102397D1    57              push    edi
		102397D2    8D4D B4         lea     ecx, dword ptr [ebp-4C]
		102397D5    51              push    ecx
		102397D6    8D4E 40         lea     ecx, dword ptr [esi+40]
		102397D9    51              push    ecx
		102397DA    FF50 68         call    dword ptr [eax+68]
		102397DD >  3D 18030980     cmp     eax, 80090318                    ; lolbox
		
		57578d????518d??4051ff????3d		v1.0.0.1411
		57578d****518d**4051ff****3d

		0x054cfd64 "51FF????3D????????75??01????83??????8D????E8????????8B"
		*/
		CHAR AIR_BASE[0x40];
		VMBEGIN
		__asm
		{
			MOV DWORD PTR [AIR_BASE+00000h],'7575'
			MOV DWORD PTR [AIR_BASE+00004h],'**d8'
			MOV DWORD PTR [AIR_BASE+00008h],'15**'
			MOV DWORD PTR [AIR_BASE+0000Ch],'**d8'
			MOV DWORD PTR [AIR_BASE+00010h],'1504'
			MOV DWORD PTR [AIR_BASE+00014h],'**ff'
			MOV DWORD PTR [AIR_BASE+00018h],'d3**'
			MOV BYTE PTR [AIR_BASE+0001Ch],0
		}

		
		HookAdd = (DWORD)HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),(DWORD)GetModuleHandle(0),AIR_BASE,0);
		if (!HookAdd)
		{
			ERRBOX("01W - 01-0")
			//MessageBoxA(0,"hook false","error",MB_OK);
			return FALSE;
		}
		HookAdd+=13;
		hookinbase.InitHookCurrentProcess(0,(VOID*)HookAdd,(VOID*)&HookIn,TRUE);
		//MessageBoxA(0,"hook ok","ok",MB_OK);
		g_callback = (lpGFCallback)callback;
		return TRUE;
	}
	
#define MAX_ONETEAM					5
	VOID LogConvertAndCommit(CAirLog& Airlog, char* lpMemory)
	{
		if (memicmp(Airlog.LoLlog.queueType, "NORMAL", lstrlenA("NORMAL")) !=0 
			&& memicmp(Airlog.LoLlog.queueType, "BOT", lstrlenA("BOT")) !=0 
			&& memicmp(Airlog.LoLlog.queueType, "RANKED_SOLO_5x5", lstrlenA("RANKED_SOLO_5x5")) !=0 
			&& memicmp(Airlog.LoLlog.queueType, "RANKED_PREMADE_5x5", lstrlenA("RANKED_PREMADE_5x5")) !=0 
			)
		{	// 没有符合上传的游戏类型
			CString strqueueType;
			strqueueType.Format(_T("queueType : %s"), Airlog.LoLlog.queueType);
			RECORD_STATIC_WARN(strqueueType, CConfig::GetLocalDir());
			return;
		}
		if (memicmp(Airlog.LoLlog.GameType, "CUSTOM_GAME", lstrlenA("CUSTOM_GAME")) == 0
			|| Airlog.LoLlog.GameType[0] == '\0' 
			)
		{
			CString strGameType;
			strGameType.Format(_T("GameType : %s"), Airlog.LoLlog.GameType);
			RECORD_STATIC_WARN(strGameType, CConfig::GetLocalDir());
			return;
		}
// 		SYSTEMTIME st = {0};
// 		GetSystemTime(&st);
// 		CHAR szLocalTime[MAX_PATH] = {0};
// 		sprintf(szLocalTime, "%04d-%02d-%02d %02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

		// Airlog.LoLlog;
		Json::Value root; // 表示整个 json 对象
		root["gameid"] = Json::Value(Airlog.LoLlog.play[0].gameid);
		//Json["gametype"] = Json::Value(Airlog.LoLlog);
		root["gametype"] = Json::Value(Airlog.LoLlog.GameType);
		root["gamelen"] = Json::Value(Airlog.LoLlog.gamelen);
		root["gameMode"] = Json::Value(Airlog.LoLlog.gameMode);
		root["gamestarttime"] = Json::Value(Airlog.LoLlog.starttime);
		root["queuetype"] = Json::Value(Airlog.LoLlog.queueType);
		root["hash"] = Json::Value(Airlog.LoLlog.hash);

		root["selfteamplayers"];
		for (int i = 0 ; i < MAX_ONETEAM; ++ i )
		{
			Json::Value otherteamplayer;
			otherteamplayer["summonername"] = Json::Value(Airlog.LoLlog.play[i].summoner);
			otherteamplayer["champion"] = Json::Value(Airlog.LoLlog.play[i].heroname);
			otherteamplayer["userid"] = Json::Value(Airlog.LoLlog.play[i].userid);
			otherteamplayer["teamid"] = Json::Value(Airlog.LoLlog.play[i].teamid);
			otherteamplayer["basePoints"] = Json::Value(Airlog.LoLlog.play[i].basePoints);
			if(Airlog.LoLlog.play[i].type[0].logtype[0] == 0 && Airlog.LoLlog.play[i].type[1].logtype[0] == 0 && Airlog.LoLlog.play[0].type[MAX_PLAYERDATA-1].logtype[0] == 0)
			{	// 如果有两个或两个以上 Type 为空返回Error
#ifdef _LOG_DEBUG
				MessageBoxA(0, "type is null , please attach process and debuging . when the dialog destroyed , the next step is \'TeamScan\'", 0, 0);
				Airlog.TeamScan(lpMemory);
#endif
				RECORD_STATIC_ERR(_T("type is null"), CConfig::GetLocalDir());
				return;					
			}
			Json::Value maindata;
			for (int j = 0; j < MAX_PLAYERDATA; ++ j )
				maindata[Airlog.LoLlog.play[i].type[j].logtype] = Airlog.LoLlog.play[i].type[j].logvalue;
			otherteamplayer["maindata"].append(maindata);

			root["selfteamplayers"].append(otherteamplayer);
		}

		root["otherteamplayers"];
		for (int i = 5 ; i < MAX_ONETEAM+MAX_ONETEAM; ++ i )
		{
			Json::Value otherteamplayer;
			otherteamplayer["summonername"] = Json::Value(Airlog.LoLlog.play[i].summoner);
			otherteamplayer["champion"] = Json::Value(Airlog.LoLlog.play[i].heroname);
			otherteamplayer["userid"] = Json::Value(Airlog.LoLlog.play[i].userid);
			otherteamplayer["teamid"] = Json::Value(Airlog.LoLlog.play[i].teamid);
			otherteamplayer["basePoints"] = Json::Value(Airlog.LoLlog.play[i].basePoints);
			if(Airlog.LoLlog.play[i].type[0].logtype[0] == 0 && Airlog.LoLlog.play[i].type[1].logtype[0] == 0 && Airlog.LoLlog.play[0].type[MAX_PLAYERDATA-1].logtype[0] == 0)
			{	// 如果有两个或两个以上 Type 为空返回Error
#ifdef _LOG_DEBUG
				MessageBoxA(0, "type is null , please attach process and debuging . when the dialog destroyed , the next step is \'TeamScan\'", 0, 0);
				Airlog.TeamScan(lpMemory);
#endif
				RECORD_STATIC_ERR(_T("type is null"), CConfig::GetLocalDir());
				return;					
			}
			Json::Value maindata;
			for (int j = 0; j < MAX_PLAYERDATA; ++ j )
				maindata[Airlog.LoLlog.play[i].type[j].logtype] = Airlog.LoLlog.play[i].type[j].logvalue;
			otherteamplayer["maindata"].append(maindata);

			root["otherteamplayers"].append(otherteamplayer);
		}

		Json::FastWriter fast_writer;
		std::string outstring = fast_writer.write(root);
		if ( ! CommintAirLog(Airlog, outstring) )
			RECORD_STATIC_ERR(_T("_CommintAirLog err"), CConfig::GetLocalDir());
		if ( ! PostPersonLog(Airlog) )
			RECORD_STATIC_ERR(_T("_CommintAirLog err"), CConfig::GetLocalDir());
		
		//HttpUpload(NULL, NULL, outstring);
	}

	VOID UnHookProgm()
	{
		hookinbase.UnhookAddress();
		
	}

	BOOL CommintAirLog(CAirLog& Airlog, std::string &strContext)
	{
		USES_CONVERSION;
		CString strBfPath = CConfig::GetLocalDir();
		RECORD_STATIC_INFO(_T("CommintAirLog start"), strBfPath);
		try
		{
			strContext = "var gameinfo=" + strContext;
			CString strBFPath = CConfig::GetLocalDir();
			HINTERNET hInternet = NULL;
			HINTERNET hConnection = NULL;
			TCHAR* pszHostName = NULL;
			TCHAR* pszUrl = NULL;
			URL_COMPONENTS uc;
			ZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);
			uc.dwHostNameLength = 1;
			uc.dwUrlPathLength = 1;
			hInternet = InternetOpen(TEXT("BigFoot-AirLog"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
			if ( hInternet == NULL )
				return FALSE;
			
			CString strServer = CConfig::GetCurrentServer();
			CHAR* pszServer = STRING_EX::WideCharToUtf8( strServer.GetBuffer(strServer.GetLength()) );
			int nLen = lstrlenA(pszServer);
			TCHAR szServerEncode[MAX_PATH]  = {0};

			for (int i = 0; i < nLen; ++i)
			{
				_stprintf(szServerEncode + i*3, _T("%%%02x"), ((BYTE*)pszServer)[i]&0xff );
			}
			STRING_EX::FreeString(pszServer);
			CString strOrd;
			strOrd.Format(_T("178bf_%d"), (DWORD)(Airlog.LoLlog.play[0].gameid));
			char* lpszOrdMd5 = MD5String(T2A(strOrd));
			CString strUrl;
	 		strUrl.Format(_T("http://%s:%d/upload_data.json?server=%s&gid=%d&ord=%s"), 
	 			UPLOAD_SITE , UPLOAD_PORT, szServerEncode, (DWORD)(Airlog.LoLlog.play[0].gameid), A2T(lpszOrdMd5) );
			if (lpszOrdMd5)	
				delete lpszOrdMd5;
			RECORD_STATIC_DEBUG(strUrl, strBFPath);
			InternetCrackUrl(strUrl, strUrl.GetLength(), 0, &uc);
			if (uc.lpszHostName && uc.dwHostNameLength > 0)
			{
				pszHostName = _tcsdup(uc.lpszHostName);
				pszHostName[uc.dwHostNameLength] = '\0';
			}
			if (uc.lpszUrlPath && uc.dwUrlPathLength > 0)
			{
				pszUrl = _tcsdup(uc.lpszUrlPath);
				pszUrl[uc.dwUrlPathLength] = '\0';
			}
			hConnection = InternetConnect(hInternet, pszHostName, uc.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hConnection)
				return FALSE;
			LPTSTR AcceptTypes[2]={0};
			AcceptTypes[0]=_T("*/*");
			HINTERNET hRequest = HttpOpenRequest(hConnection, _T("POST"), pszUrl, NULL, NULL, (LPCTSTR*)AcceptTypes, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hRequest )
				return FALSE;
			HttpAddRequestHeaders(hRequest,_T("Cache-Control: no-cache\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			HttpAddRequestHeaders(hRequest,_T("Accept: */*\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			HttpAddRequestHeaders(hRequest,_T("Content-Type: multipart/form-data; boundary=BigFootBoundary\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			
			CString strHeaders = _T("--BigFootBoundary\r\n");;
			strHeaders += _T("Content-Disposition: form-data; name=\"data\"; filename=\"data.dat\"\r\n");
			strHeaders += _T("Content-Type: application/octet-stream\r\n");
			strHeaders += _T("Content-Transfer-Encoding: binary\r\n\r\n");

			CString strFooters = _T("\r\n--BigFootBoundary--\r\n");

			DWORD dwTotalLength = strHeaders.GetLength() + strContext.length() + strFooters.GetLength();	 
			char* pszTotalBuff = new char[dwTotalLength+1];
			char* pstart = pszTotalBuff;
			ZeroMemory(pszTotalBuff, dwTotalLength+1);
			memcpy(pszTotalBuff, T2A(strHeaders), strHeaders.GetLength());
			pszTotalBuff += strHeaders.GetLength();
			memcpy(pszTotalBuff, strContext.c_str(), strContext.length());
			pszTotalBuff += strContext.length();
			memcpy(pszTotalBuff, T2A(strFooters), strFooters.GetLength());

			INTERNET_BUFFERS bufferIn = {0};
			bufferIn.dwStructSize = sizeof(bufferIn);
			bufferIn.dwBufferTotal = dwTotalLength;
			
			if (HttpSendRequestEx(hRequest, &bufferIn, NULL, HSR_INITIATE, 1) )
			{
				DWORD dwWritten = 0;
				int dwPart = 0;
				while (dwPart < dwTotalLength)
				{	
					DWORD dwWriten = 0;
					InternetWriteFile(hRequest,pstart+dwPart , dwTotalLength,&dwWriten);
					dwPart+=dwWriten;
				}
				HttpEndRequest(hRequest, NULL, 0, 0);
 				DWORD dwbuf = 0;
 				DWORD dwlen = sizeof(dwbuf);
 				HttpQueryInfo(hRequest,HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE,(LPVOID)&dwbuf,&dwlen,NULL);
 				if (dwbuf == HTTP_STATUS_OK)
 				{
 					CHAR szRecvbuf[MAX_PATH*10] = {0};
 					DWORD nRead = 0;
 					if ( InternetReadFile(hRequest, szRecvbuf, MAX_PATH*10, &nRead) ) 
 					{
						CHAR* pszStr = strstr(szRecvbuf, "success");
 						if (pszStr)
						{
							CString str;
							str.Format(_T("up v log success bytes:%d"), dwPart);
							RECORD_STATIC_INFO(str, strBFPath);
						}
						else
							RECORD_STATIC_ERR(A2T(szRecvbuf), strBFPath);
 					}
 				}
 				else
 				{
 					CString str;
 					str.Format(_T("query ser failed : %d"), dwbuf);
 					RECORD_STATIC_ERR(str, strBFPath);
 				}

			}
			else
				RECORD_STATIC_ERR(_T("QueryCapacity q failed"), strBFPath);
			free(pszHostName);
			free(pszUrl);
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInternet);
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("commit log crashed"), CConfig::GetLocalDir());
			return FALSE;
		}
		RECORD_STATIC_INFO(_T("CommintAirLog end"), strBfPath);
		return TRUE;
	}

	BOOL PostPersonLog(CAirLog& Airlog)
	{
		USES_CONVERSION;
		CString strBfPath = CConfig::GetLocalDir();
		RECORD_STATIC_INFO(_T("PostPersonLog start"), strBfPath);
		try
		{
			HINTERNET hInternet = NULL;
			HINTERNET hConnection = NULL;
			HINTERNET hSession = NULL;
			HINTERNET pHRequest = NULL;
			TCHAR* pszHostName = NULL;
			TCHAR* pszUrl = NULL;
			URL_COMPONENTS uc;
			ZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);
			uc.dwHostNameLength = 1;
			uc.dwUrlPathLength = 1;
			hInternet = InternetOpen(TEXT("BigFoot-POSTLOG"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
			if ( hInternet == NULL )
				return FALSE;
			
			std::string strData;
			ConverPostData(Airlog, strData);
			CString strUrl;
			strUrl.Format(_T("http://%s:%d/set_history.json"), UPLOAD_SITE , UPLOAD_PORT);
			InternetCrackUrl(strUrl, strUrl.GetLength(), 0, &uc);
			if (uc.lpszHostName && uc.dwHostNameLength > 0)
			{
				pszHostName = _tcsdup(uc.lpszHostName);
				pszHostName[uc.dwHostNameLength] = '\0';
			}
			if (uc.lpszUrlPath && uc.dwUrlPathLength > 0)
			{
				pszUrl = _tcsdup(uc.lpszUrlPath);
				pszUrl[uc.dwUrlPathLength] = '\0';
			}
			RECORD_STATIC_DEBUG(strUrl, strBfPath);
 			TCHAR* ptszData = A2T( (char*)strData.c_str() );
 			RECORD_STATIC_DEBUG(ptszData, strBfPath);
			hConnection = InternetConnect(hInternet, pszHostName, uc.nPort, _T(""), _T(""), INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hConnection)
				return FALSE;
			LPTSTR AcceptTypes[2]={0};
			AcceptTypes[0]=_T("*/*");
			hSession = HttpOpenRequest(hConnection, _T("POST"), pszUrl, NULL, NULL, (LPCTSTR*)AcceptTypes, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hSession )
				return FALSE;

			HttpAddRequestHeaders(hSession, _T("Cache-Control: no-cache\r\n"), -1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			if ( HttpSendRequestA(hSession,NULL, 0, (LPVOID)strData.c_str(), strData.length()) )
			{
				DWORD dwbuf = 0;
				DWORD dwlen = sizeof(dwbuf);
				::HttpQueryInfo(hSession,HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE,(LPVOID)&dwbuf,&dwlen,NULL);
				if (dwbuf == HTTP_STATUS_OK)
				{
					CHAR szRecvbuf[MAX_PATH*4] = {0};
					DWORD nRead = 0;
					::InternetReadFile(hSession, szRecvbuf, MAX_PATH*4, &nRead);
					CHAR* pszStr = strstr(szRecvbuf, "success");
					if (pszStr)
						RECORD_STATIC_INFO(_T("post log success"), strBfPath);
					else
						RECORD_STATIC_ERR(A2T(szRecvbuf), strBfPath);
				}
				else
				{
					CString str;
					str.Format(_T("query ser failed : %d"), dwbuf);
					RECORD_STATIC_ERR(str, CConfig::GetLocalDir());
				}
			}
			free(pszHostName);
			free(pszUrl);
			InternetCloseHandle(hSession);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInternet);
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("post log crashed"), strBfPath);
			return FALSE;
		}
		RECORD_STATIC_INFO(_T("PostPersonLog end"), strBfPath);
		return TRUE;
	}
	DWORD GetWinTeam(const CAirLogEx& Airlog)
	{
		DWORD dwWins = 0, dwLose = 0;
		for (int i = 0; i < MAXPNUM_ONETEAM; ++ i )
		{
			for (int j = 0 ; j < MAX_ITEMS_NUM; ++ j)
			{
				if ( memicmp(Airlog.EndofGameStats.teamPlayerParticipantStats[i]._gameItems[j].itemName, "WIN", lstrlenA("WIN")) == 0 )
				{
					++ dwWins;
					break;
				}
				else if ( memicmp(Airlog.EndofGameStats.teamPlayerParticipantStats[i]._gameItems[j].itemName, "LOSE", lstrlenA("LOSE")) == 0 )
				{
					++ dwLose;
					break;
				}
			}
		}
		
		if ( dwWins > dwLose )
			return Airlog.EndofGameStats.teamPlayerParticipantStats[0].teamId;
		else if ( dwWins < dwLose )
			return (Airlog.EndofGameStats.teamPlayerParticipantStats[0].teamId == BLUE_TEAM_NUM)?PURPLE_TEAM_NUM:BLUE_TEAM_NUM;
		else
			return 0;
	}
	DWORD GetWinTeam(const CAirLog& Airlog)
	{
		for (int i = 0; i < MAX_PLAYRCOUNT; ++ i )
		{
			for (int j = 0 ; j < MAX_PLAYERDATA; ++ j)
			{
				if ( memicmp(Airlog.LoLlog.play[i].type[j].logtype, "WIN", lstrlenA("WIN")) == 0 
					&& Airlog.LoLlog.play[i].type[j].logvalue)
				{
					return Airlog.LoLlog.play[i].teamid;
				}
			}
		}
		DWORD dwBlueLose = 0, dwPupleLose = 0;
		for (int i = 0; i < MAX_PLAYRCOUNT; ++ i )
		{
			for (int j = 0 ; j < MAX_PLAYERDATA; ++ j)
			{
				if ( memicmp(Airlog.LoLlog.play[i].type[j].logtype, "LOSE", lstrlenA("LOSE")) == 0 
					&& Airlog.LoLlog.play[i].type[j].logvalue)
				{
					if (Airlog.LoLlog.play[i].teamid == BLUE_TEAM_NUM)
						++ dwBlueLose;
					else
						++ dwPupleLose;
				}
			}
		}
		if ( dwBlueLose > dwPupleLose )
			return PURPLE_TEAM_NUM;
		else if ( dwBlueLose < dwPupleLose )
			return BLUE_TEAM_NUM;
		else
			return 0;
	}
	VOID ConverPostData( CAirLog& Airlog, std::string& strData)
	{
		USES_CONVERSION;

		CString strServer = CConfig::GetCurrentServer();
		CHAR* pszServer = STRING_EX::WideCharToUtf8( strServer.GetBuffer(strServer.GetLength()) );
		int nLen = lstrlenA(pszServer);
		CHAR szServerEncode[MAX_PATH]  = {0};

		for (int i = 0; i < nLen; ++i)
		{
			sprintf(szServerEncode + i*3, "%%%02x", ((BYTE*)pszServer)[i]&0xff );
		}
		STRING_EX::FreeString(pszServer);
		strServer.ReleaseBuffer();

		std::string strTmpData = "server=";
		strTmpData += szServerEncode;
		strTmpData += "&";

		CHAR szEncodeName[MAX_PATH] = {0};
		CHAR szData[MAX_PATH] = {0};
		CHAR szKey[20] = {0};
		// 获取胜者队伍
		DWORD dwWinnerTeam = GetWinTeam(Airlog);
		if ( 0 ==  dwWinnerTeam )
		{
			RECORD_STATIC_ERR(_T("winner unknow !"), CConfig::GetLocalDir());
			return;
		}

		for (int i = 0, nNameIdx = 0; i < MAX_PLAYRCOUNT; ++ i )
		{
			if ( Airlog.LoLlog.play[i].Bot )
			{	//如果是电脑，跳过
				continue;
			}
			ZeroMemory(szKey, 20);
			sprintf(szKey, "name%d=", ++nNameIdx);	// web index num. from 1 -> ...
			strTmpData += szKey;
			ZeroMemory(szEncodeName, MAX_PATH);
			int nLen = lstrlenA(Airlog.LoLlog.play[i].summoner);
			for (int idx = 0; idx < nLen; ++idx)
			{
				sprintf(szEncodeName + idx*3, "%%%02x", (Airlog.LoLlog.play[i].summoner)[idx]&0xff ); 
			}
			strTmpData += szEncodeName;
			strTmpData += "&";
			ZeroMemory(szData, MAX_PATH);
			sprintf(szData, "[\"%s\",\"%s\",%d,\"%d\",\"%d\"]",
				Airlog.LoLlog.play[i].heroname,								//英雄名字
				Airlog.LoLlog.queueType,									//比赛模式
				( dwWinnerTeam == Airlog.LoLlog.play[i].teamid ) ? 1 : 0,	//胜负结果
				(DWORD)Airlog.LoLlog.starttime,								//比赛时间
				(DWORD)Airlog.LoLlog.play[i].gameid);						//Game ID

			ZeroMemory(szKey, 20);
			sprintf(szKey, "data%d=", i+1);
			strTmpData += szKey;
			strTmpData += szData;
			strTmpData += "&";
		}

		strTmpData += "gameid=";
		ZeroMemory(szKey, 20);
		sprintf(szKey, "%d", (DWORD)(Airlog.LoLlog.play[0].gameid) );
		strTmpData += szKey;
		CString strOrd;
		strOrd.Format(_T("bf178_%d"), (DWORD)(Airlog.LoLlog.play[0].gameid));
		char* lpszOrdMd5 = MD5String(T2A(strOrd));
		strTmpData += "&ord=";
		strTmpData += lpszOrdMd5;
		if (lpszOrdMd5)	
			delete lpszOrdMd5;
		strData = strTmpData;
	}

	VOID ConverPostData2( CAirLogEx& Airlog, Json::Value& jsData)
	{
		USES_CONVERSION;

		CString strServer = CConfig::GetCurrentServer();
		CHAR* pszServer = STRING_EX::WideCharToUtf8( strServer.GetBuffer(strServer.GetLength()) );

		jsData["server"] = pszServer;
		STRING_EX::FreeString(pszServer);
		strServer.ReleaseBuffer();

		CHAR szEncodeName[MAX_PATH] = {0};
		CHAR szData[MAX_PATH*2] = {0};
		CHAR szKey[20] = {0};
		// 获取胜者队伍
		DWORD dwWinnerTeam = GetWinTeam(Airlog);
		if ( 0 ==  dwWinnerTeam )
		{
			RECORD_STATIC_ERR(_T("winner unknow !"), CConfig::GetLocalDir());
			return;
		}

		LPPPSTATSSUMMARY lpPPStatsSummary = Airlog.EndofGameStats.teamPlayerParticipantStats;
		for (int i = 0, nNameIdx = 0, idx = 0; i < MAX_PLAYRCOUNT; ++ i, ++ idx )
		{
			if ( idx >= MAXPNUM_ONETEAM )
				lpPPStatsSummary = Airlog.EndofGameStats.otherTeamPlayerParticipantStats, idx = 0;
			if ( lpPPStatsSummary[idx].botPlayer_c )
			{	//如果是电脑，跳过
				continue;
			}
			ZeroMemory(szKey, sizeof(szKey));
			sprintf(szKey, "name%d", ++nNameIdx);	// web index num. from 1 -> ...
			jsData[szKey] = lpPPStatsSummary[idx]._summonerName;

			ZeroMemory(szData, sizeof(szData));
			sprintf_s(szData, sizeof(szData)
				, "[\"%s\",\"%s\",%d,\"%.0lf\",\"%d\",%.02f, %.02f,%.02f,%.02f,%.02f,%.02f ,%.02f,%.02f,%.02f,%.02f]"
				, lpPPStatsSummary[idx].skinName									//英雄名字
				, Airlog.EndofGameStats.queueType									//比赛模式
				, ( dwWinnerTeam == lpPPStatsSummary[idx].teamId ) ? 1 : 0			//胜负结果
				, Airlog.EndofGameStats.timestamp_Ex_v								//比赛时间(毫秒)
				, (DWORD)Airlog.EndofGameStats.reportGameId_v						//game id
				, lpPPStatsSummary[idx].getItemValue(CHAMPIONS_KILLED)				//击杀
				, lpPPStatsSummary[idx].getItemValue(NUM_DEATHS)					//死亡
				, lpPPStatsSummary[idx].getItemValue(ASSISTS)						//助攻
				, lpPPStatsSummary[idx].getItemValue(MAGIC_DAMAGE_DEALT_PLAYER)		//魔法伤害
				, lpPPStatsSummary[idx].getItemValue(PHYSICAL_DAMAGE_DEALT_PLAYER)	//物理伤害
				, lpPPStatsSummary[idx].getItemValue(TOTAL_DAMAGE_TAKEN)			//承受总伤害
				, lpPPStatsSummary[idx].getItemValue(LARGEST_MULTI_KILL)			//最高连杀
				, lpPPStatsSummary[idx].getItemValue(MINIONS_KILLED)				//补兵数
				, lpPPStatsSummary[idx].getItemValue(NEUTRAL_MINIONS_KILLED)		//击杀野怪数
				, lpPPStatsSummary[idx].getItemValue(GOLD_EARNED)					//金币
				);
			ZeroMemory(szKey, sizeof(szKey) );
			sprintf_s(szKey, sizeof(szKey), "data%d", nNameIdx);
			jsData[szKey] = szData;

			ZeroMemory(szData, sizeof(szData));
			sprintf_s(szData, sizeof(szData), "[%d,\"%s\",%d,%d,%d,%d,%d]",
				(DWORD)(lpPPStatsSummary[idx].level),						//人物等级
				Airlog.EndofGameStats.queueType,							//战斗类型
				(DWORD)(lpPPStatsSummary[idx].wins),						//胜场
				(DWORD)(lpPPStatsSummary[idx].losses),						//负场
				(DWORD)(lpPPStatsSummary[idx].leaves),						//逃跑
				(DWORD)(lpPPStatsSummary[idx]._profileIconId),				//头像Id
				0);															//积分
			ZeroMemory(szKey, sizeof(szKey) );
			sprintf_s(szKey, sizeof(szKey), "sum%d", nNameIdx);
			jsData[szKey] = szData;
// 			OutputDebugStringA(szData);
// 			OutputDebugStringA("\r\n");

		}

		ZeroMemory(szKey, 20);
		sprintf(szKey, "%d", (DWORD)(Airlog.EndofGameStats.reportGameId_v) );

		jsData["gameid"] = szKey;

		CString strOrd;
		strOrd.Format(_T("bf178_%d"), (DWORD)(Airlog.EndofGameStats.reportGameId_v));
		char* lpszOrdMd5 = MD5String(T2A(strOrd));

		jsData["ord"] = lpszOrdMd5;

		if (lpszOrdMd5)	
			delete lpszOrdMd5;
		
	}

	BOOL CommitSourcesLog(CString strFilePath)
	{
		USES_CONVERSION;
		CString strBfPath = CConfig::GetLocalDir();
		RECORD_STATIC_INFO(_T("CommitSourcesLog start"), strBfPath);
		try
		{
			DWORD dwSize = 0;
			BYTE* lpZipContext = FILEIO::ReadFileToBuff(strFilePath.GetBuffer(strFilePath.GetLength()), &dwSize);
			strFilePath.ReleaseBuffer();
			HINTERNET hInternet = NULL;
			HINTERNET hConnection = NULL;
			TCHAR* pszHostName = NULL;
			TCHAR* pszUrl = NULL;
			URL_COMPONENTS uc;
			ZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);
			uc.dwHostNameLength = 1;
			uc.dwUrlPathLength = 1;
			hInternet = InternetOpen(TEXT("BigFoot-AirLog-sc"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
			if ( hInternet == NULL )
				return FALSE;
			
			CString strServer = CConfig::GetCurrentServer();
			CHAR* pszServer = STRING_EX::WideCharToUtf8( strServer.GetBuffer(strServer.GetLength()) );
			int nLen = lstrlenA(pszServer);
			TCHAR szServerEncode[MAX_PATH]  = {0};

			for (int i = 0; i < nLen; ++i)
			{
				_stprintf(szServerEncode + i*3, _T("%%%02x"), ((BYTE*)pszServer)[i]&0xff );
			}
			STRING_EX::FreeString(pszServer);
			CString strOrd;
			DWORD dwGameid = Airlog.LoLlog.play[0].gameid;
			if ( 0 == dwGameid)
				dwGameid = 1;
			strOrd.Format(_T("178bf_%d"), dwGameid);
			char* lpszOrdMd5 = MD5String(T2A(strOrd));
			CString strUrl;
			strUrl.Format(_T("http://%s:%d/upload_source.json?server=%s&gid=%d&ord=%s"), 
				UPLOAD_SITE , UPLOAD_PORT, szServerEncode, dwGameid, A2T(lpszOrdMd5) );
			if (lpszOrdMd5)	
				delete lpszOrdMd5;
			RECORD_STATIC_DEBUG(strUrl, strBfPath);

			CString strHeaders = _T("--BigFootBoundary\r\n");;
			strHeaders += _T("Content-Disposition: form-data; name=\"data\"; filename=\"data.dat\"\r\n");
			strHeaders += _T("Content-Type: application/octet-stream\r\n");
			strHeaders += _T("Content-Transfer-Encoding: binary\r\n\r\n");

			CString strFooters = _T("\r\n--BigFootBoundary--\r\n");

			DWORD dwTotalLength = strHeaders.GetLength() + dwSize + strFooters.GetLength();	 
			char* pszTotalBuff = new char[dwTotalLength+1];
			char* pstart = pszTotalBuff;
			ZeroMemory(pszTotalBuff, dwTotalLength+1);
			memcpy(pszTotalBuff, T2A(strHeaders), strHeaders.GetLength());
			pszTotalBuff += strHeaders.GetLength();
			memcpy(pszTotalBuff, lpZipContext, dwSize);
			pszTotalBuff += dwSize;
			memcpy(pszTotalBuff, T2A(strFooters), strFooters.GetLength());
			GlobalFree(lpZipContext);

			InternetCrackUrl(strUrl, strUrl.GetLength(), 0, &uc);
			if (uc.lpszHostName && uc.dwHostNameLength > 0)
			{
				pszHostName = _tcsdup(uc.lpszHostName);
				pszHostName[uc.dwHostNameLength] = '\0';
			}
			if (uc.lpszUrlPath && uc.dwUrlPathLength > 0)
			{
				pszUrl = _tcsdup(uc.lpszUrlPath);
				pszUrl[uc.dwUrlPathLength] = '\0';
			}
			hConnection = InternetConnect(hInternet, pszHostName, uc.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hConnection)
				return FALSE;
			LPTSTR AcceptTypes[2]={0};
			AcceptTypes[0]=_T("*/*");
			HINTERNET hRequest = HttpOpenRequest(hConnection, _T("POST"), pszUrl, NULL, NULL, (LPCTSTR*)AcceptTypes, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hRequest )
				return FALSE;
			HttpAddRequestHeaders(hRequest,_T("Cache-Control: no-cache\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			HttpAddRequestHeaders(hRequest,_T("Accept: */*\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			HttpAddRequestHeaders(hRequest,_T("Content-Type: multipart/form-data; boundary=BigFootBoundary\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			
			INTERNET_BUFFERS bufferIn = {0};
			bufferIn.dwStructSize = sizeof(bufferIn);
			bufferIn.dwBufferTotal = dwTotalLength;
			
			if (HttpSendRequestEx(hRequest, &bufferIn, NULL, HSR_INITIATE, 1) )
			{
				DWORD dwWriten = 0;
				int dwPart = 0;
				while (dwPart < dwTotalLength)
				{	
					dwWriten = 0;
					InternetWriteFile(hRequest,pstart+dwPart , dwTotalLength,&dwWriten);
					dwPart+=dwWriten;
				}
				dwWriten = 0;
				CHAR chEOF = EOF;
				InternetWriteFile(hRequest, &chEOF, 1, &dwWriten);
				HttpEndRequest(hRequest, NULL, 0, 0);
 				DWORD dwbuf = 0;
 				DWORD dwlen = sizeof(dwbuf);
 				HttpQueryInfo(hRequest,HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE,(LPVOID)&dwbuf,&dwlen,NULL);
 				if (dwbuf == HTTP_STATUS_OK)
 				{
 					CHAR szRecvbuf[MAX_PATH*10] = {0};
 					DWORD nRead = 0;
 					if ( InternetReadFile(hRequest, szRecvbuf, MAX_PATH*10, &nRead) ) 
 					{
						CHAR* pszStr = strstr(szRecvbuf, "success");
 						if (pszStr)
							RECORD_STATIC_INFO(_T("up src log success"), strBfPath);
						else
							RECORD_STATIC_ERR(A2T(szRecvbuf), strBfPath);
 					}
 				}
 				else
 				{
 					CString str;
 					str.Format(_T("query ser failed : %d"), dwbuf);
 					RECORD_STATIC_ERR(str, strBfPath);
 				}

			}
			else
				RECORD_STATIC_ERR(_T("QueryCapacity q failed"), strBfPath);
			free(pszHostName);
			free(pszUrl);
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInternet);
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("commit log crashed"), strBfPath);
			return FALSE;
		}
		RECORD_STATIC_INFO(_T("CommitSourcesLog end"), strBfPath);
		return TRUE;
	}

	BOOL PackupSourceLog( CString strZipPath , CString strSourceName)
	{
		try
		{
			CString strBbPath = CConfig::GetLocalDir();
			strBbPath.TrimRight(_T("\\"));
			strBbPath += _T("\\");
			CZipArchive ar;
			ar.SetCompressionMethod();
			if ( ar.Open(strZipPath, CZipArchive::zipCreate) )
			{
				ar.AddNewFile( strBbPath + strSourceName, strSourceName, -1, false );

				CString strLogName;
				CLogRecorder::GetCurrentLogDefaultName(strLogName);
				ar.AddNewFile( strBbPath + strLogName, strLogName, -1, false );

				ar.Close();
			}
			else
				return FALSE;
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("packup crashed !"), CConfig::GetLocalDir());
			return FALSE;
		}

		
		return TRUE;
	}

	BOOL CommintTimeLine(CString strFilePath)
	{
		USES_CONVERSION;
		CString strBfPath = CConfig::GetLocalDir();
		RECORD_STATIC_INFO(_T("CommitSourcesLog start"), strBfPath);
		try
		{
			DWORD dwSize = 0;
			BYTE* lpZipContext = FILEIO::ReadFileToBuff(strFilePath.GetBuffer(strFilePath.GetLength()), &dwSize);
			DWORD dwZlen = dwSize;
			BYTE* zContext = new BYTE[dwSize+1];
			ZeroMemory(zContext, dwSize+1);
			compress(zContext, &dwZlen ,lpZipContext, dwSize);
			GlobalFree(lpZipContext);
			strFilePath.ReleaseBuffer();
			HINTERNET hInternet = NULL;
			HINTERNET hConnection = NULL;
			TCHAR* pszHostName = NULL;
			TCHAR* pszUrl = NULL;
			URL_COMPONENTS uc;
			ZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);
			uc.dwHostNameLength = 1;
			uc.dwUrlPathLength = 1;
			hInternet = InternetOpen(TEXT("BigFoot-timeline"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
			if ( hInternet == NULL )
				return FALSE;
			
			CString strServer = CConfig::GetCurrentServer();
			CHAR* pszServer = STRING_EX::WideCharToUtf8( strServer.GetBuffer(strServer.GetLength()) );
			int nLen = lstrlenA(pszServer);
			TCHAR szServerEncode[MAX_PATH]  = {0};

			for (int i = 0; i < nLen; ++i)
			{
				_stprintf(szServerEncode + i*3, _T("%%%02x"), ((BYTE*)pszServer)[i]&0xff );
			}
			STRING_EX::FreeString(pszServer);
			CString strOrd;
			DWORD dwGameid = 0;	///////
			if ( 0 == dwGameid)
				dwGameid = 1;
			strOrd.Format(_T("bftl_%d"), dwGameid);
			char* lpszOrdMd5 = MD5String(T2A(strOrd));
			CString strUrl;
			strUrl.Format(_T("http://%s:%d/upload_timeline.json?server=%s&gid=%d&ord=%s"), 
				UPLOAD_SITE , UPLOAD_PORT, szServerEncode, dwGameid, A2T(lpszOrdMd5) );
			if (lpszOrdMd5)	
				delete lpszOrdMd5;
			RECORD_STATIC_DEBUG(strUrl, strBfPath);

			CString strHeaders = _T("--BigFootBoundary\r\n");;
			strHeaders += _T("Content-Disposition: form-data; name=\"data\"; filename=\"data.dat\"\r\n");
			strHeaders += _T("Content-Type: application/octet-stream\r\n");
			strHeaders += _T("Content-Transfer-Encoding: binary\r\n\r\n");
			strHeaders += _T("Connection: keep-alive\r\n\r\n");
			CString strFooters = _T("\r\n--BigFootBoundary--\r\n");

			DWORD dwTotalLength = strHeaders.GetLength() + dwZlen + strFooters.GetLength();	 
			char* pszTotalBuff = new char[dwTotalLength+1];
			char* pstart = pszTotalBuff;
			ZeroMemory(pszTotalBuff, dwTotalLength+1);
			memcpy(pszTotalBuff, T2A(strHeaders), strHeaders.GetLength());
			pszTotalBuff += strHeaders.GetLength();
			memcpy(pszTotalBuff, zContext, dwZlen);
			pszTotalBuff += dwZlen;
			memcpy(pszTotalBuff, T2A(strFooters), strFooters.GetLength());

			InternetCrackUrl(strUrl, strUrl.GetLength(), 0, &uc);
			if (uc.lpszHostName && uc.dwHostNameLength > 0)
			{
				pszHostName = _tcsdup(uc.lpszHostName);
				pszHostName[uc.dwHostNameLength] = '\0';
			}
			if (uc.lpszUrlPath && uc.dwUrlPathLength > 0)
			{
				pszUrl = _tcsdup(uc.lpszUrlPath);
				pszUrl[uc.dwUrlPathLength] = '\0';
			}
			hConnection = InternetConnect(hInternet, pszHostName, uc.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hConnection)
				return FALSE;
			LPTSTR AcceptTypes[2]={0};
			AcceptTypes[0]=_T("*/*");
			HINTERNET hRequest = HttpOpenRequest(hConnection, _T("POST"), pszUrl, NULL, NULL, (LPCTSTR*)AcceptTypes, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hRequest )
				return FALSE;
			HttpAddRequestHeaders(hRequest,_T("Cache-Control: no-cache\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			//HttpAddRequestHeaders(hRequest,_T("Accept: */*\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			HttpAddRequestHeaders(hRequest,_T("Content-Type: multipart/form-data; boundary=BigFootBoundary\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			
			INTERNET_BUFFERS bufferIn = {0};
			bufferIn.dwStructSize = sizeof(bufferIn);
			bufferIn.dwBufferTotal = dwTotalLength;
			
			if (HttpSendRequestEx(hRequest, &bufferIn, NULL, HSR_INITIATE, 1) )
			{
				DWORD dwWriten = 0;
				int dwPart = 0;
				while (dwPart < dwTotalLength)
				{	
					dwWriten = 0;
					InternetWriteFile(hRequest,pstart+dwPart , dwTotalLength,&dwWriten);
					dwPart+=dwWriten;
				}
				dwWriten = 0;
//  				CHAR chEOF = EOF;
//  				InternetWriteFile(hRequest, &chEOF, 1, &dwWriten);
 				HttpEndRequest(hRequest, NULL, 0, 0);
 				DWORD dwbuf = 0;
 				DWORD dwlen = sizeof(dwbuf);
 				if( HttpQueryInfo(hRequest,HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE, &dwbuf,&dwlen,NULL) )
 					if (dwbuf == HTTP_STATUS_OK)
 					{
 						CHAR szRecvbuf[MAX_PATH*10] = {0};
 						DWORD nRead = 0;
 						if ( InternetReadFile(hRequest, szRecvbuf, MAX_PATH*10, &nRead) ) 
 						{
							CHAR* pszStr = strstr(szRecvbuf, "success");
 							if (pszStr)
								RECORD_STATIC_INFO(_T("up src log success"), strBfPath);
							else
								RECORD_STATIC_ERR(A2T(szRecvbuf), strBfPath);
 						}
 					}
 					else
 					{
 						CString str;
 						str.Format(_T("query ser failed : %d"), dwbuf);
 						RECORD_STATIC_ERR(str, strBfPath);
 					}
				else
					__asm nop

			}
			else
				RECORD_STATIC_ERR(_T("QueryCapacity q failed"), strBfPath);
			free(pszHostName);
			free(pszUrl);
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInternet);
			if ( zContext )
				delete zContext, zContext = NULL;
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("commit log crashed"), strBfPath);
			return FALSE;
		}
		RECORD_STATIC_INFO(_T("CommitSourcesLog end"), strBfPath);
		return TRUE;
	}


	BOOL fnConvertLog2Json(Json::Value& root, const CAirLogEx& Airlog)
	{
		if (memicmp(Airlog.EndofGameStats.queueType, "NORMAL", lstrlenA("NORMAL")) !=0 
			&& memicmp(Airlog.EndofGameStats.queueType, "BOT", lstrlenA("BOT")) !=0 
			&& memicmp(Airlog.EndofGameStats.queueType, "RANKED_SOLO_5x5", lstrlenA("RANKED_SOLO_5x5")) !=0 
			&& memicmp(Airlog.EndofGameStats.queueType, "RANKED_PREMADE_5x5", lstrlenA("RANKED_PREMADE_5x5")) !=0 
			)
		{	// 没有符合上传的游戏类型
			CString strqueueType;
			strqueueType.Format(_T("queueType : %s"), Airlog.EndofGameStats.queueType);
			RECORD_STATIC_WARN(strqueueType, CConfig::GetLocalDir());
			return FALSE;
		}
		if (memicmp(Airlog.EndofGameStats.gameType, "CUSTOM_GAME", lstrlenA("CUSTOM_GAME")) == 0
			|| Airlog.EndofGameStats.gameType[0] == '\0' 
			)
		{
			CString strGameType;
			strGameType.Format(_T("GameType : %s"), Airlog.EndofGameStats.gameType);
			RECORD_STATIC_WARN(strGameType, CConfig::GetLocalDir());
			return FALSE;
		}
		// 		SYSTEMTIME st = {0};
		// 		GetSystemTime(&st);
		// 		CHAR szLocalTime[MAX_PATH] = {0};
		// 		sprintf(szLocalTime, "%04d-%02d-%02d %02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

		// Airlog.LoLlog;
		// Json::Value root; // 表示整个 json 对象
		root["gameid"] = Json::Value(Airlog.EndofGameStats.reportGameId_v);
		root["gametype"] = Json::Value(Airlog.EndofGameStats.gameType);
		root["gamelen"] = Json::Value(Airlog.EndofGameStats.gameLength_v);
		root["gameMode"] = Json::Value(Airlog.EndofGameStats.gameMode);
		root["gamestarttime"] = Json::Value(Airlog.EndofGameStats.timestamp_Ex_v);
		root["queuetype"] = Json::Value(Airlog.EndofGameStats.queueType);
		//root["hash"] = Json::Value(Airlog.EndofGameStats.roomPassword);

		root["selfteamplayers"];
		for (int i = 0 ; i < MAXPNUM_ONETEAM; ++ i )
		{
			Json::Value otherteamplayer;
			otherteamplayer["summonername"] = Json::Value(Airlog.EndofGameStats.teamPlayerParticipantStats[i]._summonerName);
			otherteamplayer["champion"] = Json::Value(Airlog.EndofGameStats.teamPlayerParticipantStats[i].skinName);
			otherteamplayer["userid"] = Json::Value(Airlog.EndofGameStats.teamPlayerParticipantStats[i].userId);
			otherteamplayer["teamid"] = Json::Value(Airlog.EndofGameStats.teamPlayerParticipantStats[i].teamId);
			otherteamplayer["profileIconId"] = Json::Value(Airlog.EndofGameStats.teamPlayerParticipantStats[i]._profileIconId);
			if( Airlog.EndofGameStats.teamPlayerParticipantStats[i]._gameItems[0].itemName[0] == 0 )
			{	// 如果有一个 Type 为空返回Error
				RECORD_STATIC_ERR(_T("type is null"), CConfig::GetLocalDir());
				return FALSE;					
			}
			Json::Value maindata;
			for (int j = 0; j < MAX_ITEMS_NUM; ++ j )
				maindata[Airlog.EndofGameStats.teamPlayerParticipantStats[i]._gameItems[j].itemName] = Airlog.EndofGameStats.teamPlayerParticipantStats[i]._gameItems[j].itemValue;
			otherteamplayer["maindata"].append(maindata);

			root["selfteamplayers"].append(otherteamplayer);
		}

		root["otherteamplayers"];
		for (int i = 0 ; i < MAXPNUM_ONETEAM; ++ i )
		{
			Json::Value otherteamplayer;
			otherteamplayer["summonername"] = Json::Value(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i]._summonerName);
			otherteamplayer["champion"] = Json::Value(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i].skinName);
			otherteamplayer["userid"] = Json::Value(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i].userId);
			otherteamplayer["teamid"] = Json::Value(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i].teamId);
			otherteamplayer["profileIconId"] = Json::Value(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i]._profileIconId);
			if(Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i]._gameItems[0].itemName[0] == 0)
			{	// 如果有一个 Type 为空返回Error
				RECORD_STATIC_ERR(_T("type is null"), CConfig::GetLocalDir());
				return FALSE;					
			}
			Json::Value maindata;
			for (int j = 0; j < MAX_ITEMS_NUM; ++ j )
				maindata[Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i]._gameItems[j].itemName] = Airlog.EndofGameStats.otherTeamPlayerParticipantStats[i]._gameItems[j].itemValue;
			otherteamplayer["maindata"].append(maindata);

			root["otherteamplayers"].append(otherteamplayer);
		}

		return TRUE;
	}



	VOID LogConvertAndCommit2(CAirLogEx& Airlog)
	{
		Json::Value root;
		std::string jsondata;
		Json::Value post10Data;
		if ( fnConvertLog2Json(root, Airlog) )
		{
			Json::FastWriter fwriter;
			jsondata = fwriter.write(root);
		}
		else
			RECORD_STATIC_ERR(_T("fnConvertLog2Json err"), CConfig::GetLocalDir());
		ConverPostData2(Airlog, post10Data);

		fnUploadLog(post10Data, jsondata);

	}

	BOOL fnUploadLog(Json::Value& postTdata, std::string& umcompPostBdata)
	{
		USES_CONVERSION;
		CString strBfPath = CConfig::GetLocalDir();
		RECORD_STATIC_INFO(_T("fnUploadLog start"), strBfPath);
		try
		{
			umcompPostBdata = "var gameinfo=" + umcompPostBdata;
			//CA2GZIP a2zip((char*)("test"));
			CA2GZIP a2zip((char*)(umcompPostBdata.c_str()));
			CHAR* postBdata = (CHAR*)(a2zip.pgzip);
			int npostBlen = a2zip.Length;
// 			CHAR* postBdata = (CHAR*)umcompPostBdata.c_str();
// 			int npostBlen = umcompPostBdata.length();
			CString strBFPath = CConfig::GetLocalDir();
			HINTERNET hInternet = NULL;
			HINTERNET hConnection = NULL;
			TCHAR* pszHostName = NULL;
			TCHAR* pszUrl = NULL;
			URL_COMPONENTS uc;
			ZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);
			uc.dwHostNameLength = 1;
			uc.dwUrlPathLength = 1;
			hInternet = InternetOpen(TEXT("BigFoot-AirLog"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
			if ( hInternet == NULL )
				return FALSE;

			CString strUrl = UPDATE_BASE_URL;
			RECORD_STATIC_DEBUG(strUrl, strBFPath);
			InternetCrackUrl(strUrl, strUrl.GetLength(), 0, &uc);
			if (uc.lpszHostName && uc.dwHostNameLength > 0)
			{
				pszHostName = _tcsdup(uc.lpszHostName);
				pszHostName[uc.dwHostNameLength] = '\0';
			}
			if (uc.lpszUrlPath && uc.dwUrlPathLength > 0)
			{
				pszUrl = _tcsdup(uc.lpszUrlPath);
				pszUrl[uc.dwUrlPathLength] = '\0';
			}
			hConnection = InternetConnect(hInternet, pszHostName, uc.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hConnection)
				return FALSE;
			LPTSTR AcceptTypes[2]={0};
			AcceptTypes[0]=_T("*/*");
			HINTERNET hRequest = HttpOpenRequest(hConnection, _T("POST"), pszUrl, NULL, NULL, (LPCTSTR*)AcceptTypes, INTERNET_FLAG_NO_CACHE_WRITE, 0);
			if ( ! hRequest )
				return FALSE;
			
			std::string mainstruction = "BigFootBoundary";
			std::string headerflag = "--" + mainstruction + "\r\n";
			std::string endflag = "--" + mainstruction + "--\r\n";	// "-----------------------------BigFootBoundary--\r\n";
			std::string strHeaders = headerflag;
			strHeaders += "Content-Disposition: form-data; name=\"file1\"; filename=\"test.tar.gz\"\r\n";	// name=file1
			strHeaders += "Content-Type: application/gzip\r\n";
			strHeaders += "Content-Transfer-Encoding: binary\r\n";
			strHeaders += "\r\n";
			std::string retflag = "\r\n";
			DWORD dwTotalLength = postTdata.size()*MAX_PATH
				+ retflag.length()
				+ strHeaders.length() 
				+ npostBlen //postBdata.length() 
				+ strHeaders.length()
				+ endflag.length();

// 			CString strDataLen;
// 			strDataLen.Format(_T("Content-Length: %d"), dwTotalLength);
// 			HttpAddRequestHeaders(hRequest,strDataLen,-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
//			HttpAddRequestHeaders(hRequest,_T("Cache-Control: no-cache\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
//			HttpAddRequestHeaders(hRequest,_T("Accept: */*\r\n"),-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			
			std::string requestHeader;
			requestHeader = "Content-Type: multipart/form-data; boundary=" + mainstruction + "\r\n";
			HttpAddRequestHeadersA(hRequest, requestHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);

			char* pszTotalBuff = new char[dwTotalLength+1];
			char* pstart = pszTotalBuff;
			ZeroMemory(pszTotalBuff, dwTotalLength+1);

			memcpy(pszTotalBuff, strHeaders.c_str(), strHeaders.length());
			pszTotalBuff += strHeaders.length();
			memcpy(pszTotalBuff, postBdata/*.c_str()*/, npostBlen/*postBdata.length()*/);
			pszTotalBuff += npostBlen/*postBdata.length()*/;
			memcpy(pszTotalBuff, "\r\n", strlen("\r\n"));
			pszTotalBuff += strlen("\r\n");

			Json::Value::Members member = postTdata.getMemberNames();//Members  这玩意就是vector<string>，typedef了而已
			for ( int i = 0, len = postTdata.size(); i < len; ++ i  )
			{
				/*
				-----------------------------114782935826962\r\n
				Content-Disposition: form-data; name="server"\r\n
				\r\n
				context
				\r\n
				-----------------------------114782935826962--\r\n
				*/
				std::string cell = headerflag;
				cell += "Content-Disposition: form-data; name=\"";
				cell += member[i];
				cell += "\"";
				cell += retflag;
				cell += retflag;
				cell += postTdata[member[i]].asString();
				cell += retflag;
				memcpy(pszTotalBuff, cell.c_str(), cell.length());
				pszTotalBuff += cell.length();
			}

// 			std::string cell = headerflag;
// 			cell += "Content-Disposition: form-data; name=\"";
// 			cell += "server";
// 			cell += "\"";
// 			cell += retflag;
// 			cell += retflag;
// 			cell += postTdata["server"].asString();
// 			cell += retflag;
// 			memcpy(pszTotalBuff, cell.c_str(), cell.length());
// 			pszTotalBuff += cell.length();

			memcpy(pszTotalBuff, endflag.c_str(), endflag.length());
			pszTotalBuff += endflag.length();

			INTERNET_BUFFERS bufferIn = {0};
			bufferIn.dwStructSize = sizeof(bufferIn);
			bufferIn.dwBufferTotal = dwTotalLength;
			
			if (HttpSendRequestEx(hRequest, &bufferIn, NULL, HSR_INITIATE, 1) )
			{
				DWORD dwWritten = 0;
				int dwPart = 0;
				while (dwPart < dwTotalLength)
				{	
					DWORD dwWriten = 0;
					InternetWriteFile(hRequest,pstart+dwPart , dwTotalLength,&dwWriten);
					dwPart+=dwWriten;
				}
				HttpEndRequest(hRequest, NULL, 0, 0);
 				DWORD dwbuf = 0;
 				DWORD dwlen = sizeof(dwbuf);
 				HttpQueryInfo(hRequest,HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE,(LPVOID)&dwbuf,&dwlen,NULL);
 				if (dwbuf == HTTP_STATUS_OK)
 				{
 					CHAR szRecvbuf[MAX_PATH*10] = {0};
 					DWORD nRead = 0;
 					if ( InternetReadFile(hRequest, szRecvbuf, MAX_PATH*10, &nRead) ) 
 					{
						CHAR* pszStr = strstr(szRecvbuf, "success");
 						if (pszStr)
							RECORD_STATIC_INFO(_T("up v log success"), strBFPath);
						else
							RECORD_STATIC_ERR(A2T(szRecvbuf), strBFPath);
						__asm nop
 					}
 				}
 				else
 				{
 					CString str;
 					str.Format(_T("query ser failed : %d"), dwbuf);
 					RECORD_STATIC_ERR(str, strBFPath);
 				}

			}
			else
				RECORD_STATIC_ERR(_T("QueryCapacity q failed"), strBFPath);
			free(pszHostName);
			free(pszUrl);
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInternet);
		}
		catch (...)
		{
			RECORD_STATIC_ERR(_T("fnUploadLog crashed"), CConfig::GetLocalDir());
			return FALSE;
		}
		RECORD_STATIC_INFO(_T("fnUploadLog end"), strBfPath);
		return TRUE;
	}
};

