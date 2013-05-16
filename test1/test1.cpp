// test1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Airlog.h"
#include "BaseCode/File/CFileIO.hpp"
#include "airlogcpy.h"
#include "../FileHelper/FileHelper.h"
#include <time.h>
#include "../ZipArchive/zlib/GZipHelper.h"
#include "../zlib/zlib.h"
#include "../zlib/zconf.h"
#include "../zlib/gzguts.h"
#include "AirLogEx.h"
#include "DMHttp.h"
#include "../publicdef/dmstring.hpp"
#include "../publicdef/processhelper.hpp"
#include "afmtest.h"

// DWORD GetTimeStamp(SYSTEMTIME* lpst)
// {
// 	DWORD dwTimeStamp = 0;
// 	DWORD dwPastDays = (lpst->wMonth - 1) * 30 + lpst->wDay;
// 	DWORD dwOutDays = 0;
// 
// 	switch (lpst->wMonth-1)
// 	{
// 	case 12:
// 		++ dwOutDays;
// 	case 11:
// 	case 10:
// 		++ dwOutDays;
// 	case 9:
// 	case 8:
// 		++ dwOutDays;
// 	case 7:
// 		++ dwOutDays;
// 	case 6:
// 	case 5:
// 		++ dwOutDays;
// 	case 4:
// 	case 3:
// 		++ dwOutDays;
// 	case 2:
// 		{
// 			if (lpst->wYear%4)
// 				dwOutDays -= 2;
// 			else
// 				dwOutDays -= 1;
// 		}
// 	case 1:
// 		++ dwOutDays;
// 		break;
// 	default:
// 		break;
// 	}
// 	dwPastDays += dwOutDays;
// 	dwPastDays -= 1;
// 	
// 	DWORD dwTYears = (lpst->wYear - 1970);
// 	dwTimeStamp =  
// 		dwTYears * 3600 * 24 * 365 + dwTYears / 4 * 3600 * 24 
// 		+ dwPastDays * 3600 * 24 
// 		+ lpst->wHour * 3600
// 		+ lpst->wMinute * 60
// 		+ lpst->wSecond
// 		;
// 
// 	return dwTimeStamp;
// }

#define FIX_MOD


CHAR* CharFilter(CHAR* lpSrc, DWORD dwSrcSize, const CHAR ch, DWORD* pdwOutSize = NULL)
{
	CHAR* lpszRes = NULL;
	if (lpSrc && dwSrcSize > 0)
	{
		lpszRes = new CHAR[dwSrcSize+1];
		ZeroMemory(lpszRes, dwSrcSize+1);
		CHAR* pTmp = lpszRes;
		DWORD dwMatchCount = 0;
		for (int idx = 0, last = 0; idx < dwSrcSize; ++ idx)
		{
			if ( ch == lpSrc[idx] )
			{
				DWORD dwLen = idx-last;
				memcpy(pTmp, lpSrc+last, dwLen );
				pTmp += dwLen;
				last = idx+1;
				++ dwMatchCount;
			}
		}
		if (pdwOutSize)
			*pdwOutSize = dwSrcSize - dwMatchCount;
	} 
	return lpszRes;
}

DWORD WINAPI testThread(LPVOID filePath)
{
	USES_CONVERSION;

	LPCTSTR lpFilePath = (LPCTSTR)filePath;
	CAirLog tmpAirLog;
	CHAR* ptrFile = NULL, *lpMemory = NULL;
	DWORD dwFileSize = 0;
	ptrFile = (CHAR*)FILEIO::ReadFileToBuff(T2A(lpFilePath), &dwFileSize);
	lpMemory = ptrFile;
	// 				DWORD dwResSize = 0;
	// 				lpMemory = CharFilter(ptrFile, dwFileSize, 0xC3, &dwResSize);
	if ( tmpAirLog.TeamScan(lpMemory) )
	{
		__asm nop
	}
	GlobalFree(ptrFile);
	return 0;
}

void format_test(LPCTSTR lpszFormat, ...)
{
	//ATLASSERT(_IsValidString(lpszFormat));
	DWORD dwParamCount = 0;
	TCHAR *lptszHeader = NULL, *lptszLastHeader = NULL;
//	lptszHeader = _tcsstr(lpszFormat, _T("%"));
	va_list argList;
	va_start(argList, lpszFormat);
	//BOOL bRet = FormatV(lpszFormat, argList);
	DWORD dwWidth = va_arg(argList, int);
	va_end(argList);
}

int _tmain(int argc, _TCHAR* argv[])
{
//	format_test(1, 1, 2, 3, 4);
	USES_CONVERSION;
	DWORD dwFileSize = 0;
	CAirLog CAlog;
	TCHAR* ItemFileName = _T("D:\\0000000\\retest.log");
	TCHAR ItemFilePtr[MAX_PATH*2] = {0};
	CString strSourceName = _T("retest.log");
	TCHAR* lpZipPath = _T("d:\\zip.zip.dat");
	CHAR chType = 0;
	printf("ѭ��ɨ��[1],�ֹ��ύ[2],TimeLine�ύ[3]\n����ִ�з�ʽ�� ");
	scanf("%c", &chType);
	switch (chType)
	{
#pragma region case1
	case '1':
		{
			vector<CString> foundfiles;
			CFileHelper fh;
			fh.SearchFiles(_T("D:\\0000000"), _T("2012-*.log"), foundfiles);
			for (int i = 0; i < foundfiles.size(); ++ i)
			{
				CString strTmp = foundfiles[i];
				HANDLE hThread = CreateThread(NULL, 0, testThread, strTmp.GetBuffer(strTmp.GetLength()), 0, NULL);
				if (hThread)
				{
					WaitForSingleObject(hThread, INFINITE);
					CloseHandle(hThread);
				}
				strTmp.ReleaseBuffer();
			}
		}
		break;
#pragma endregion case1
#pragma region case2
	case '2':
		{
#ifdef FIX_MOD
			struct tm strtm;
			ZeroMemory(&strtm, sizeof(struct tm));
			DWORD dwTimeStamp = 0;

			printf("������ �� �� �� ʱ �� �� �� \n");
			scanf("%d %d %d %d %d %d", & strtm.tm_year, & strtm.tm_mon, & strtm.tm_mday, & strtm.tm_hour, & strtm.tm_min, & strtm.tm_sec );
			strtm.tm_year -= 1900;
			strtm.tm_mon -= 1;

			dwTimeStamp = mktime(&strtm);
			printf("��������� %d��%d��%d��%dʱ%d��%d��, ʱ���: %d\n",
				strtm.tm_year+1900, strtm.tm_mon+1, strtm.tm_mday, strtm.tm_hour, strtm.tm_min, strtm.tm_sec, dwTimeStamp);
			printf("ѡ�������:\n");
			printf("0 : ����֮�� 1 : ��ŷ���� 2 : �氲 3 : ŵ����˹\r\n"
				"4 : ��¶��� 5 : Ƥ�����ַ� 6 : ս��ѧԺ\r\n"
				"7 : ����� 8 : ��ɪ�ر� 9 : �þ�֮��\r\n"
				"10 : ��ɫõ�� 11 : ��Ӱ�� 12 : ��������\r\n"
				"13 : ˮ��֮�� 14 : ������� 15 : �ȶ�������\r\n"
				"16 : �������� 17 : ���׶�׿�� 18 : ��η�ȷ�\r\n"
				"19 : ŭ���� 20 : ������ר��\r\n");
			DWORD dwServer = 0;
			scanf("%d", &dwServer);
			switch(dwServer)
			{
			case 0:
				g_strServer = _T("����֮��");
				break;
			case 1:
				g_strServer = _T("��ŷ����");
				break;
			case 2:
				g_strServer = _T("�氲");
				break;
			case 3:
				g_strServer = _T("ŵ����˹");
				break;
			case 4:
				g_strServer = _T("��¶���");
				break;
			case 5:
				g_strServer = _T("Ƥ�����ַ�");
				break;
			case 6:
				g_strServer = _T("ս��ѧԺ");
				break;
			case 7:
				g_strServer = _T("�����");
				break;
			case 8:
				g_strServer = _T("��ɪ�ر�");
				break;
			case 9:
				g_strServer = _T("�þ�֮��");
				break;
			case 10:
				g_strServer = _T("��ɫõ��");
				break;
			case 11:
				g_strServer = _T("��Ӱ��");
				break;
			case 12:
				g_strServer = _T("��������");
				break;
			case 13:
				g_strServer = _T("ˮ��֮��");
				break;
			case 14:
				g_strServer = _T("�������");
				break;
			case 15:
				g_strServer = _T("�ȶ�������");
				break;
			case 16:
				g_strServer = _T("��������");
				break;
			case 17:
				g_strServer = _T("���׶�׿��");
				break;
			case 18:
				g_strServer = _T("��η�ȷ�");
				break;
			case 19:
				g_strServer = _T("ŭ����");
				break;
			case 20:
				g_strServer = _T("������ר��");
				break;
			}
			printf("��ѡ��ķ�������: %s\n", T2A(g_strServer));
#endif
			lstrcat(ItemFilePtr,ItemFileName);
			CHAR* ptrFile = NULL, *lpMemory = NULL;
			ptrFile = (CHAR*)FILEIO::ReadFileToBuff(ItemFilePtr,&dwFileSize);
			lpMemory = ptrFile;
			//DWORD dwResSize = 0;
			//lpMemory = CharFilter(ptrFile, dwFileSize, 0xC3, &dwResSize);

			if ( CAlog.TeamScan(lpMemory) )
			{
				printf("game id : %d\n", (DWORD)(CAlog.LoLlog.play[0].gameid));
#ifdef FIX_MOD
				CAlog.LoLlog.starttime = dwTimeStamp - CAlog.LoLlog.gamelen;
#endif
				//AIRLOG_CONVERT::LogConvertAndCommit(CAlog, (char*)lpMemory);
				AIRLOG_CONVERT::LogConvertAndCommit(CAlog, (char*)lpMemory);
				FILEIO::CloseHandleExt((BYTE*)ptrFile);
			}
// 			if (AIRLOG_CONVERT::PackupSourceLog(lpZipPath, strSourceName))
// 			{
// 				AIRLOG_CONVERT::CommitSourcesLog(lpZipPath);
// 			}
		}
		break;
#pragma endregion case2
	case '3':
		{
			printf("ѡ�������:\n");
			printf("0 : ����֮�� 1 : ��ŷ���� 2 : �氲 3 : ŵ����˹\r\n"
				"4 : ��¶��� 5 : Ƥ�����ַ� 6 : ս��ѧԺ\r\n"
				"7 : ����� 8 : ��ɪ�ر� 9 : �þ�֮��\r\n"
				"10 : ��ɫõ�� 11 : ��Ӱ�� 12 : ��������\r\n"
				"13 : ˮ��֮�� 14 : ������� 15 : �ȶ�������\r\n"
				"16 : �������� 17 : ���׶�׿�� 18 : ��η�ȷ�\r\n"
				"19 : ŭ���� 20 : ������ר��\r\n");
			DWORD dwServer = 0;
			scanf("%d", &dwServer);
			switch(dwServer)
			{
			case 0:
				g_strServer = _T("hn0");
				break;
			case 1:
				g_strServer = _T("��ŷ����");
				break;
			case 2:
				g_strServer = _T("�氲");
				break;
			case 3:
				g_strServer = _T("ŵ����˹");
				break;
			case 4:
				g_strServer = _T("��¶���");
				break;
			case 5:
				g_strServer = _T("Ƥ�����ַ�");
				break;
			case 6:
				g_strServer = _T("ս��ѧԺ");
				break;
			case 7:
				g_strServer = _T("�����");
				break;
			case 8:
				g_strServer = _T("��ɪ�ر�");
				break;
			case 9:
				g_strServer = _T("�þ�֮��");
				break;
			case 10:
				g_strServer = _T("��ɫõ��");
				break;
			case 11:
				g_strServer = _T("��Ӱ��");
				break;
			case 12:
				g_strServer = _T("��������");
				break;
			case 13:
				g_strServer = _T("ˮ��֮��");
				break;
			case 14:
				g_strServer = _T("�������");
				break;
			case 15:
				g_strServer = _T("�ȶ�������");
				break;
			case 16:
				g_strServer = _T("��������");
				break;
			case 17:
				g_strServer = _T("���׶�׿��");
				break;
			case 18:
				g_strServer = _T("��η�ȷ�");
				break;
			case 19:
				g_strServer = _T("ŭ����");
				break;
			case 20:
				g_strServer = _T("������ר��");
				break;
			}
			printf("��ѡ��ķ�������: %s\n", T2A(g_strServer));
			//AIRLOG_CONVERT::CommintTimeLineThread((LPVOID)_T("report.txt"));
		}
		break;
	case '5':
		{
			DWORD dwFileSize = 0;
			CHAR* ptrFile = (CHAR*)FILEIO::ReadFileToBuff("d:\\0000000\\memory.dat",&dwFileSize);
			CAirLogEx logEx;
			if ( logEx.PasteMemory(ptrFile, dwFileSize) )
			{
				printf("game id : %d\n", (DWORD)(logEx.EndofGameStats.reportGameId_v));
				//AIRLOG_CONVERT::LogConvertAndCommit(CAlog, (char*)lpMemory);
				AIRLOG_CONVERT::LogConvertAndCommit2(logEx);
				FILEIO::CloseHandleExt((BYTE*)ptrFile);
			}
		}
		break;
	case '6':
		{
			CDMHttp dmHttp;
			CHAR* lpszBuff = dmHttp.DownLoadFile(_T("http://www.178.com/index.html"));
			if (lpszBuff)
			{
				dmHttp.FreeDLStr(lpszBuff);
			}
		}
	case '7':
		{
			char* path = "c:\\report.txt";
			char* savepath = "c:\\report.gz";
			DWORD dwSize = 0;
			CHAR* report = (CHAR*)FILEIO::ReadFileToBuff( path, &dwSize);
			if (report)
			{
				CA2GZIP a2gzip(report);
				FILEIO::SaveBuffToFile(savepath, a2gzip.pgzip, a2gzip.Length);
				CDMHttp dmHttp;
				Json::Value postJson;
				dmHttp.ConvertFormData("server", "tw", postJson);
 				dmHttp.ConvertFormData("gameid", "227145799", postJson);
 				dmHttp.ConvertFormData("win", "1", postJson);
 				dmHttp.ConvertFormData("file1", "xxx.gz", postJson);
				BOOL bRet = dmHttp.CommitForm(_T("http://m.chilltouch.com/mupload"), postJson, (CHAR*)a2gzip.pgzip, a2gzip.Length, FALSE);
				GlobalFree(report);
			}
		}
	case '8':
		{
			std::string strTest = "kskksksksjfaaaaaaaaaaaaaaaaaieai";
			CA2GZIP a2gip((char*)strTest.c_str());
			char* ptest = (char*)a2gip.pgzip;
			DWORD dwlen = a2gip.Length;
			__asm nop
		}
		break;
	case '9':
		{
			CString str = _T("������");
			DMW2A w2a(L"�Ǻ�����");
			char* lpsz = w2a;
			DMA2W a2w("���������utf8", CP_ACP);
			w2a.reset(a2w);
			lpsz = w2a;
			a2w.reset(w2a);
			printf("%s\r\n", lpsz);
			wprintf(L"%s\r\n", (WCHAR*)a2w);
			__asm nop
		}
		break;
	case '0':
		{
			__asm
			{
				//movzx   eax, byte ptr [ebx+Fh]
				mov eax, 1
				neg     al
				sbb     eax, eax
				and     eax, 0xFFFFFF9C
				add     eax, 0C8h
				nop
			}
		}
		break;
	case 'a':
		{
			BOOL bRet = ProcessHelper::SupendProcess(_T("calc.exe"));
			__asm nop
				bRet = ProcessHelper::ResumeProcess(_T("calc.exe"));
			__asm nop
		}
		break;
	case 'b':
		{
			DWORD dwSize = 0;
			CHAR* lpBuf = (CHAR*)FILEIO::ReadFileToBuff( "D:\\unsealmsg3.txt", &dwSize);
			CAirLogEx airLogEx;
			CHAR *lpszFilt = airLogEx.CharFilter(lpBuf, dwSize, 0xc3, &dwSize);
			CHAR* lpstart = airLogEx.memstr(lpszFilt, BLOCK_SOCIALNETWORKUSERIDS, dwSize);
			airLogEx.CutAllBlock(lpstart, dwSize);
			std::vector<AIRMEMORY>::iterator itrBegin = airLogEx.m_airMemoryArr.begin();
			std::vector<AIRMEMORY>::iterator itr1 = airLogEx.getTypeIdx2(itrBegin, SWF_STR, (DWORD)2);
			if (itr1 != itrBegin)
			{
				std::vector<AIRMEMORY>::iterator itr2 = airLogEx.getTypeIdx2(itr1, SWF_STR, (DWORD)2+1);
				CHAR* lpNameNoCase = airLogEx.getSWF_str(itr1->lpMmeory);
				CHAR* lpName = airLogEx.getSWF_str(itr2->lpMmeory);
				if ( itr2 !=  itrBegin && stricmp(lpNameNoCase, lpName) == 0 )
				{
					MessageBoxA(0, lpName, 0, 0);
				}

			}
			airLogEx.FreeAirMemory(airLogEx.m_airMemoryArr.begin(), airLogEx.m_airMemoryArr.size());
			airLogEx.FreeFilterStr(lpszFilt);

			__asm nop
		}
		break;
	case 'c':
		{
			CAirLogEx airlog;
			char* pvbuf = "\x06\x11\x4F\x6C\xC3\xA9\x6F\x6C\xC3\xA9";
			char* lptarget = airlog.getSWF_strEx(pvbuf);
			DMA2W a2w(lptarget);
			DMW2A w2a(a2w.getBuffer(), CP_ACP);
			printf("w2a: %s\r\n", w2a.getBuffer());
			char* pvbuf2 = "\x06\x11\x4F\x6C\xA9\x6F\x6C\xA9";
			lptarget = airlog.getSWF_strEx(pvbuf2);
			a2w.reset(lptarget);
			w2a.reset(a2w.getBuffer(), CP_ACP);
			printf("w2a: %s\r\n", w2a.getBuffer());
			__asm nop
			
		}
		break;
	case 'd':
		{
			//{a:"123",b:[1,"2",0.3],c:"����"}
			const char* test = "0a0b010363060de6b189e5ad970361060731323303620907010401060332053fd333333333333301";

			AMF3::context ctx;
			AMF3::init_context(&ctx,read_data,write_data);

			//tagIoData* io = DecodeHex(test);
			tagIoData* io = new tagIoData();
			DWORD dwFileSize = 0;
			CHAR* ptrFile = (CHAR*)FILEIO::ReadFileToBuff("d:\\0000000\\memory.dat",&dwFileSize);
			CAirLogEx filter;
			CHAR* pnew = filter.CharFilter(ptrFile, dwFileSize, 0xC3, &dwFileSize);
			io->buf = (unsigned char*)pnew;
			io->buflen = dwFileSize;
			io->pos = 0;

			AMF3::amf_object_handle obj = AMF3::decode(&ctx,io);

			tagIoData* out = new tagIoData;
			out->buflen = 4096;
			out->buf = (unsigned char*)malloc(out->buflen);
			out->pos = 0;
			AMF3::encode(&ctx,out,obj);

			__asm nop
		}
		break;
	default:
		{
			TCHAR tszStr[20] = {0};
			UINT len = 0;
			while (TRUE)
			{
				ZeroMemory(tszStr, 20);
				len = DMSTR_EX::GetRandomStr(tszStr, 20);
				__asm nop
			}
		}
		break;

	}
	
	return 0;
}

