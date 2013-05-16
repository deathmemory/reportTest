#include "stdafx.h"
#include "DMHttp.h"
#include "../ZipArchive/zlib/GZipHelper.h"

#pragma comment(lib, "Wininet.lib")
#define HTTP_MARK				TEXT("DMHTTP")

CDMHttp::CDMHttp(void)
{
	m_hInternet = NULL;
	m_hConnection = NULL;
	m_hRequest = NULL;
	ZeroMemory(&m_uc, sizeof(URL_COMPONENTS));
}

CDMHttp::~CDMHttp(void)
{
	if ( m_hInternet )
		InternetCloseHandle(m_hInternet);
	if ( m_hConnection )
		InternetCloseHandle(m_hConnection);
	if ( m_hRequest )
		InternetCloseHandle(m_hRequest);
}

// BOOL CDMHttp::UploadData()
// {
// 	
// }

// std::string CDMHttp::ConvertForm(char* name, char* value)
// {
	
/*}*/
BOOL CDMHttp::SaveStrToFile(__in LPVOID lpBuff, __in DWORD dwSize, __in LPCTSTR lpszSavePath, BOOL bCover /*= TRUE*/)
{
	TCHAR* pflag = NULL;
	if (bCover)
		pflag = _T("wb+");
	else
		pflag = _T("wb");
	FILE * hFile = _tfopen( lpszSavePath, pflag );
	if (hFile)
	{
		fwrite((CHAR*)lpBuff , dwSize , 1, hFile);
		fclose(hFile);
	}
	return TRUE;
}

BOOL CDMHttp::DownLoadFile( __in LPCTSTR lpszUrl, __out LPCTSTR lpszSavePath, __out DWORD *lpdwBytes /*= NULL*/, BOOL bCover /*= TRUE*/)
{
	CHAR* lpszRet = NULL;
	DWORD size = 0;
	BOOL bRet = FALSE;
	if ( (lpszRet = DownLoadFile(lpszUrl, lpdwBytes)) )
	{
		if ( lpdwBytes )
			size = *lpdwBytes;
		else
			size = lstrlenA(lpszRet);
		bRet = SaveStrToFile(lpszRet, size, lpszSavePath, bCover);
	}
	return bRet;
}

CHAR* CDMHttp::DownLoadFile( LPCTSTR lpUrl, __out DWORD *lpdwBytes )
{
	CHAR* lpszRetBuf = NULL;
	if (OpenHttp())
	{
		if (CrackUrl(lpUrl, (DWORD)_tcslen(lpUrl)))
		{
			if (Connect())
			{
				INTERNET_BUFFERS bufferIn = {0};
				bufferIn.dwStructSize = sizeof(bufferIn);
				if (this->HttpOpenRequest() )
				{
					if (this->HttpSendRequestEx(&bufferIn))
					{
						this->HttpEndRequest();
						DWORD dwbuf = 0;
						DWORD dwlen = sizeof(dwbuf);
						this->HttpQueryInfo(HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE, (LPVOID)&dwbuf, &dwlen);
						if (HTTP_STATUS_OK == dwbuf)
						{
							CHAR szRecvbuf[MAX_PATH*10] = {0};
							DWORD nRead = 0;
							DWORD dwTotalRead = 0;
							BOOL bRet = FALSE;
							while (this->InternetReadFile(szRecvbuf, MAX_PATH*10, &nRead) && nRead > 0)
							{
								dwTotalRead += nRead;
								lpszRetBuf = (CHAR*)realloc(lpszRetBuf, dwTotalRead+1);
								if (lpszRetBuf)
									memcpy(lpszRetBuf+dwTotalRead-nRead, szRecvbuf, nRead);
							}
							lpszRetBuf[dwTotalRead] = '\0';
							if (lpdwBytes)
								*lpdwBytes = dwTotalRead;
						}
					}
				}
			}
		}

		CloseHttp();
	}
	return lpszRetBuf;
}

BOOL CDMHttp::InternetWriteFile(
					   __in          LPCVOID lpBuffer,
					   __in          DWORD dwNumberOfBytesToWrite,
					   __out         LPDWORD lpdwNumberOfBytesWritten
					   )
{
	return ::InternetWriteFile(m_hRequest, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten);
}

BOOL CDMHttp::InternetReadFile(
					  __out_bcount(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
					  __in DWORD dwNumberOfBytesToRead,
					  __out LPDWORD lpdwNumberOfBytesRead
					  )
{
	return ::InternetReadFile(m_hRequest, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
}

BOOL CDMHttp::HttpEndRequest(
					__out         LPINTERNET_BUFFERS lpBuffersOut/* = NULL*/,
					__in          DWORD dwFlags/* = 0*/,
					__in          DWORD_PTR dwContext/* = 0*/
					)
{
	return ::HttpEndRequest(m_hRequest, lpBuffersOut, dwFlags, dwContext);
}

HINTERNET CDMHttp::OpenHttp(
				   __in DWORD dwAccessType /*= INTERNET_OPEN_TYPE_DIRECT*/,
				   __in_opt LPCWSTR lpszProxy /*= NULL*/,
				   __in_opt LPCWSTR lpszProxyBypass /*= NULL*/,
				   __in DWORD dwFlags /*= 0*/
				   )
{
	return (m_hInternet = ::InternetOpen(HTTP_MARK,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0));
}

HINTERNET CDMHttp::HttpOpenRequest(
		__in_opt LPCTSTR lpszVerb,
		__in_opt LPCTSTR lpszObjectName,
		__in_opt LPCTSTR lpszVersion,
		__in_opt LPCTSTR lpszReferrer,
		__in_z_opt LPCTSTR FAR * lplpszAcceptTypes,
		__in DWORD dwFlags,
		__in_opt DWORD_PTR dwContext
		)
{
	TCHAR* pszUrl = NULL;
	LPCTSTR lpszObjName = NULL;
	if (! lpszObjectName)
	{
		if (m_uc.lpszUrlPath && m_uc.dwUrlPathLength > 0)
		{
			pszUrl = _tcsdup(m_uc.lpszUrlPath);
			pszUrl[m_uc.dwUrlPathLength] = '\0';
		}
		lpszObjName = pszUrl;
	}
	else
		lpszObjName = lpszObjectName;
	m_hRequest = ::HttpOpenRequest(m_hConnection, lpszVerb, lpszObjName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
	if ( pszUrl )
		free(pszUrl);
	return  m_hRequest;
}

HINTERNET CDMHttp::Connect(
		__in_opt LPCTSTR lpszUserName,
		__in_opt LPCTSTR lpszPassword,
		__in DWORD dwService,
		__in DWORD dwFlags,
		__in_opt DWORD_PTR dwContext
		)
{
	TCHAR* pszHostName = NULL;
	if (m_uc.lpszHostName && m_uc.dwHostNameLength > 0)
	{
		pszHostName = _tcsdup(m_uc.lpszHostName);
		pszHostName[m_uc.dwHostNameLength] = '\0';
	}
	m_hConnection = ::InternetConnect(m_hInternet, pszHostName, m_uc.nPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
	if ( pszHostName )
		free( pszHostName );
	return m_hConnection;
}

VOID CDMHttp::CloseHttp()
{
	if ( m_hRequest )
		::InternetCloseHandle(m_hRequest);
	if ( m_hConnection )
		::InternetCloseHandle(m_hConnection);
	if ( m_hInternet )
		::InternetCloseHandle(m_hInternet);
}

BOOL CDMHttp::CrackUrl( __in LPCTSTR lpszUrl, __in DWORD dwUrlLength, __in DWORD dwFlags)
{
	// 得到url
	BOOL bRet = FALSE;
	TCHAR* pszHostName = NULL;
	TCHAR* pszUrl = NULL;
	ZeroMemory(&m_uc, sizeof(URL_COMPONENTS));
	m_uc.dwStructSize = sizeof(URL_COMPONENTS);
	m_uc.dwHostNameLength = 1;
	m_uc.dwUrlPathLength = 1;
	bRet = InternetCrackUrl(lpszUrl, dwUrlLength, dwFlags, &m_uc);
// 	if (bRet)
// 	{
// 		if (m_uc.lpszHostName && m_uc.dwHostNameLength > 0)
// 		{
// 			pszHostName = _tcsdup(uc.lpszHostName);
// 			pszHostName[m_uc.dwHostNameLength] = '\0';
// 		}
// 		if (m_uc.lpszUrlPath && m_uc.dwUrlPathLength > 0)
// 		{
// 			pszUrl = _tcsdup(uc.lpszUrlPath);
// 			pszUrl[m_uc.dwUrlPathLength] = '\0';
// 		}
// 	}
	return bRet;
}

BOOL CDMHttp::HttpAddRequestHeaders(
							  __in LPCTSTR lpszHeaders,
							  __in DWORD dwHeadersLength,
							  __in DWORD dwModifiers
							  )
{
	return ::HttpAddRequestHeaders(m_hRequest, lpszHeaders, dwHeadersLength, dwModifiers);
}

BOOL CDMHttp::HttpSendRequestEx(
					   __in_opt LPINTERNET_BUFFERS lpBuffersIn,
					   __out_opt LPINTERNET_BUFFERS lpBuffersOut/* = NULL*/,
					   __in DWORD dwFlags/* = HSR_INITIATE*/,
					   __in_opt DWORD_PTR dwContext /*= 1*/
					   )
{
	return ::HttpSendRequestEx(m_hRequest, lpBuffersIn, lpBuffersOut, dwFlags, dwContext);
}

BOOL CDMHttp::HttpQueryInfo(
				   __in DWORD dwInfoLevel,
				   __inout_bcount_opt(*lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
				   __inout LPDWORD lpdwBufferLength,
				   __inout_opt LPDWORD lpdwIndex
				   )
{
	return ::HttpQueryInfo(m_hRequest,dwInfoLevel/*HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE*/,lpBuffer,lpdwBufferLength,lpdwIndex);
}

VOID CDMHttp::FreeDLStr( CHAR* lpszStr )
{
	if ( lpszStr )
		free(lpszStr);
}

BOOL CDMHttp::CommitForm( LPCTSTR lpszUrl, Json::Value& postTdata, char* lpPostBdata, int bdatalen, BOOL bcompress/* = FALSE*/)
{
	USES_CONVERSION;
	try
	{
		if ( this->OpenHttp() )
			if ( this->CrackUrl(lpszUrl, lstrlen(lpszUrl)) )
				if ( this->Connect() )
					if ( this->HttpOpenRequest(_T("POST")) )
					{
						CHAR* postBdata = NULL;
						int npostBlen = 0;
						if (bcompress)
						{
							CA2GZIP a2zip(lpPostBdata);
							postBdata = (CHAR*)(a2zip.pgzip);
							npostBlen = a2zip.Length;
						}
						else
						{
							postBdata = lpPostBdata;
							npostBlen = bdatalen;
						}
						
						std::string mainstruction = "BigFootBoundary";
						std::string headerflag = "--" + mainstruction + "\r\n";
						std::string endflag = "--" + mainstruction + "--\r\n";	// "-----------------------------BigFootBoundary--\r\n";
						std::string retflag = "\r\n";
						std::string strHeaders = headerflag;
						strHeaders += "Content-Disposition: form-data; name=\"file1\"; filename=\"test.tar.gz\"\r\n";	// name=file1
						strHeaders += "Content-Type: application/gzip\r\n";
						strHeaders += "Content-Transfer-Encoding: binary\r\n";
						strHeaders += "\r\n";
						DWORD dwTotalLength = postTdata.size()*MAX_PATH
							+ retflag.length()
							+ strHeaders.length() 
							+ npostBlen //postBdata.length() 
							+ strHeaders.length()
							+ endflag.length();
						char* pszTotalBuff = new char[dwTotalLength+1];
						char* pstart = pszTotalBuff;
						ZeroMemory(pszTotalBuff, dwTotalLength+1);
						if (postBdata && bdatalen > 0)
						{
							memcpy(pszTotalBuff, strHeaders.c_str(), strHeaders.length());
							pszTotalBuff += strHeaders.length();
							memcpy(pszTotalBuff, postBdata/*.c_str()*/, npostBlen/*postBdata.length()*/);
							pszTotalBuff += npostBlen/*postBdata.length()*/;
							memcpy(pszTotalBuff, "\r\n", strlen("\r\n"));
							pszTotalBuff += strlen("\r\n");
						}	

						std::string requestHeader;
						requestHeader = "Content-Type: multipart/form-data; boundary=" + mainstruction + "\r\n";
						this->HttpAddRequestHeaders(A2T(requestHeader.c_str()));
						//HttpAddRequestHeadersA(hRequest, requestHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
						
						Json::Value::Members member = postTdata.getMemberNames();//Members  这玩意就是vector<string>，typedef了而已
						for ( int i = 0, len = postTdata.size(); i < len; ++ i  )
						{
							/* 實例
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
						memcpy(pszTotalBuff, endflag.c_str(), endflag.length());
						pszTotalBuff += endflag.length();

						INTERNET_BUFFERS bufferIn = {0};
						bufferIn.dwStructSize = sizeof(bufferIn);
						bufferIn.dwBufferTotal = dwTotalLength;
						if (this->HttpSendRequestEx( &bufferIn ) )
						{
							DWORD dwWritten = 0;
							int dwPart = 0;
							while (dwPart < dwTotalLength)
							{	
								DWORD dwWriten = 0;
								this->InternetWriteFile(pstart+dwPart , dwTotalLength, &dwWriten);
								dwPart+=dwWriten;
							}
							this->HttpEndRequest();
							DWORD dwbuf = 0;
							DWORD dwlen = sizeof(dwbuf);
							this->HttpQueryInfo(HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE, (LPVOID)&dwbuf, &dwlen, NULL);
							if (dwbuf == HTTP_STATUS_OK)
							{
								CHAR szRecvbuf[MAX_PATH*10] = {0};
								DWORD nRead = 0;
								if ( this->InternetReadFile(szRecvbuf, MAX_PATH*10, &nRead) ) 
								{
									CHAR* pszStr = strstr(szRecvbuf, "success");
									if (pszStr)
										OutputDebugString(_T("up v log success\r\n"));
									else
										OutputDebugString(A2T(szRecvbuf));
									__asm nop
									return TRUE;
								}
							}
							else
							{
								CString str;
								str.Format(_T("query ser failed : %d\r\n"), dwbuf);
								OutputDebugString(str);
							}

						}
						else
							OutputDebugString(_T("QueryCapacity q failed\r\n"));
					}
		}
		catch (...)
		{
			OutputDebugString(_T("commit form crashed !\r\n"));
			return FALSE;
		}
		return FALSE;
}

VOID CDMHttp::ConvertFormData(char* lpszKey, char* lpszValue, Json::Value& outJson)
{
	outJson[lpszKey] = lpszValue;
}