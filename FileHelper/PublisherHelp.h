#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <stdio.h>
#include <tchar.h>
#include "FileHelper.h"
#pragma comment(lib, "crypt32.lib")
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)


class CPublisherHelp
{
public:
	CPublisherHelp(void);
	~CPublisherHelp(void);
	static BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
		PSPROG_PUBLISHERINFO Info);


	static BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st);


	static BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext);

	static LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);
	static BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo,
		PCMSG_SIGNER_INFO *pCounterSignerInfo);

	static LPTSTR GetSigningName(const WCHAR* fileName);
};
