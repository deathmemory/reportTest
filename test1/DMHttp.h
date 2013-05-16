#pragma once
// #include <windows.h>
// #include <stdio.h>
// #include <string>
// #include <iostream>
#include <Wininet.h>
#include "../lib_json/json.h"

// #ifdef _UNICODE
// typedef dstring, std::wstring;
// #else
// typedef dstirng, std::string;
// #endif
// 
class CDMHttp
{
public:
	CDMHttp(void);
	virtual ~CDMHttp(void);
	BOOL SaveStrToFile(__in LPVOID lpBuff, __in DWORD dwSize, __in LPCTSTR lpszSavePath, BOOL bCover = TRUE);
	BOOL DownLoadFile( __in LPCTSTR lpszUrl, __out LPCTSTR lpszSavePath, __out DWORD *lpdwBytes = NULL, BOOL bCover = TRUE);
	CHAR* DownLoadFile( __in LPCTSTR lpUrl, __out DWORD *lpdwBytes = NULL);
	VOID FreeDLStr( CHAR* lpszStr );
	BOOL CommitForm( LPCTSTR lpszUrl, Json::Value& postTdata, char* lpPostBdata, int bdatalen, BOOL bcompress = FALSE);
	VOID ConvertFormData(char* lpszKey, char* lpszValue, Json::Value& outJson);

	HINTERNET OpenHttp(
		__in DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT,
		__in_opt LPCWSTR lpszProxy = NULL,
		__in_opt LPCWSTR lpszProxyBypass = NULL,
		__in DWORD dwFlags = 0
		);
	BOOL CrackUrl( __in LPCTSTR lpszUrl, __in DWORD dwUrlLength, __in DWORD dwFlags = 0);
	HINTERNET Connect(__in_opt LPCTSTR lpszUserName = NULL,
		__in_opt LPCTSTR lpszPassword = NULL,
		__in DWORD dwService = INTERNET_SERVICE_HTTP,
		__in DWORD dwFlags = INTERNET_FLAG_NO_CACHE_WRITE,
		__in_opt DWORD_PTR dwContext = 0
		);

	VOID CloseHttp();
	HINTERNET HttpOpenRequest(
		__in_opt LPCTSTR lpszVerb = _T("GET"),
		__in_opt LPCTSTR lpszObjectName = NULL,
		__in_opt LPCTSTR lpszVersion = NULL,
		__in_opt LPCTSTR lpszReferrer = NULL,
		__in_z_opt LPCTSTR FAR * lplpszAcceptTypes = NULL,
		__in DWORD dwFlags = INTERNET_FLAG_NO_CACHE_WRITE,
		__in_opt DWORD_PTR dwContext = 0
		);
	BOOL HttpAddRequestHeaders(
		__in LPCTSTR lpszHeaders,
		__in DWORD dwHeadersLength = -1,
		__in DWORD dwModifiers = HTTP_ADDREQ_FLAG_ADD_IF_NEW
		);
	BOOL HttpSendRequestEx(
		__in_opt LPINTERNET_BUFFERS lpBuffersIn,
		__out_opt LPINTERNET_BUFFERS lpBuffersOut = NULL,
		__in DWORD dwFlags = HSR_INITIATE,
		__in_opt DWORD_PTR dwContext = 1
		);
	BOOL HttpQueryInfo(
		__in DWORD dwInfoLevel,
		__inout_bcount_opt(*lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
		__inout LPDWORD lpdwBufferLength,
		__inout_opt LPDWORD lpdwIndex = NULL
		);
	BOOL InternetWriteFile(
		__in          LPCVOID lpBuffer,
		__in          DWORD dwNumberOfBytesToWrite,
		__out         LPDWORD lpdwNumberOfBytesWritten
		);
	BOOL InternetReadFile(
		__out_bcount(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
		__in DWORD dwNumberOfBytesToRead,
		__out LPDWORD lpdwNumberOfBytesRead
		);
	BOOL HttpEndRequest(
		__out         LPINTERNET_BUFFERS lpBuffersOut = NULL,
		__in          DWORD dwFlags = 0,
		__in          DWORD_PTR dwContext = 0
		);


//	static std::string ConvertForm(char* name, char* value);
//	BOOL CommitForm(char* lpszUrl, Json::Value& forms);
private:
	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;
	URL_COMPONENTS m_uc;
};
