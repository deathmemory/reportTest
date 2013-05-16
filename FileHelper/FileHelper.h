#pragma once
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>
#include <vector>
#include <wincrypt.h>	//crypt signal
using namespace std;

//////////////////////////////////////////////////////////////////////////
// PE ����ǩ��
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
typedef struct {
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, *PSPROG_PUBLISHERINFO;

typedef struct {
	WCHAR wszSerialNum[MAX_PATH];
	WCHAR wszIssuerName[MAX_PATH];
	WCHAR wszSubjectName[MAX_PATH];
} SSIGNER_INFO, *PSSIGNER_INFO;


class CSignerChecker
{
public:
	LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);
	BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,	PSPROG_PUBLISHERINFO Info);
	BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st);
	BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo);
	BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext, PSSIGNER_INFO psinfo);
};

#define FULLPATH		CString
#define FILENAME		CString
enum SEARCHLEVEL
{
	NO_LIMIT = -1,
	CURRENT_DIR	// �������¼ӱ�Ϊָ��Ŀ¼����
};

class CFileHelper : public CSignerChecker
{
public:
	CFileHelper(void);
	virtual ~CFileHelper(void);
	BOOL SearchFiles(const TCHAR* chFolderPath, const TCHAR* chFilter, vector<CString>& vcFound, BOOL bFindFolder = TRUE);
 	//BOOL FindFilesInDirecotry(CString fileName,CString directory, vector<pair<FILENAME,FULLPATH>>& vctOut );
 	//BOOL FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount, vector<pair<FILENAME,FULLPATH>>& vctOut);
// 	vector<pair<FILENAME,FULLPATH>> FindFilesInDirecotry(CString fileName,CString directory );
// 	vector<pair<FILENAME,FULLPATH>> FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount);
	BOOL GetAppProductVersion(WCHAR* pszPath, VS_FIXEDFILEINFO* pVS);
	int GetPhysicalMAC(char*  mac);	//��ȡ����MAC Buffer���� 12+1
	//bool IsPe(CString strFileName);
	BOOL GetPESections(CString strFileName, vector<CString>* pvcSectionNames,bool bAccessReadOnly = false);
	BOOL GetPESections(BYTE* lpbFileMap, vector<CString>* pvcSectionNames);
	bool GetCpuID(char *pszCpuID);
	
	PBYTE GetFileMapping(CString strFileName, bool bAccessReadOnly = false);
	BOOL ReleaseFileMapping(void* lpFMBase);
	//////////////////////////////////////////////////////////////////////////
	// PE ����ǩ��
	BOOL GetSignerInfoSimply(CString strFileName, PSSIGNER_INFO psinfo);
	static char* GetFileCRC32(CString strFileName);
	static int Get_CRC(LPVOID pdate, DWORD dwSize);//��ȡcrc32ֵ
	//////////////////////////////////////////////////////////////////////////
	PVOID FindExportFuncBase(PBYTE ImageBase, PCSTR Name);	// ͨ������ƫ�ƻ�ȡDLL����������ַ
	
};

class CProcessHelper
{
public:
	CProcessHelper(){}
	virtual ~CProcessHelper(){}
public:
	BOOL ScanDllInProcess(CString strProcName, CString strDllName);
	BOOL ScanDllInProcess(DWORD dwPID, CString strDllName);
	DWORD ScanProcess(CString strProcName);
	BOOL KillProcByName(CString strProcName);
};