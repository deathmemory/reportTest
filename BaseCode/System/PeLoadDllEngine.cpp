#include "stdafx.h"
#include "PeLoadDllEngine.h"
#define ASSERT(i) if(!i)_asm int 3
JK_PeLoadDllEngine::JK_PeLoadDllEngine()
{
	isLoadOk = FALSE;
	pImageBase = NULL;
	pDllMain = NULL;
}
JK_PeLoadDllEngine::~JK_PeLoadDllEngine()
{
	if(isLoadOk)
	{
		ASSERT(pImageBase != NULL);
		ASSERT(pDllMain   != NULL);
		//�ѹ���׼��ж��dll
		pDllMain((HINSTANCE)&(pImageBase),DLL_PROCESS_DETACH,0);
		VirtualFree((LPVOID)&pImageBase, 0, MEM_RELEASE);
		isLoadOk=false;
	}
}
BYTE* JK_PeLoadDllEngine::FILELOADER::DecodeFile(BYTE *Src,DWORD decodeKey,DWORD len)//װ���ļ���ӳ��
{
	QUICK_CRYPT::QuickDecrypt(Src,decodeKey,len);
	return 0;
}
BOOL JK_PeLoadDllEngine::FILELOADER::LoadFile( TCHAR *lpFileName,DWORD decodeKey)//װ���ļ���ӳ��
{
	m_lpInImageBase = FILEIO::ReadFileToBuff(lpFileName,&fileSize);
	DecodeFile(m_lpInImageBase,decodeKey,fileSize);
	/*
	m_hInFile = CreateFile( lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( m_hInFile == INVALID_HANDLE_VALUE )				   
		return FALSE;
	fileSize = GetFileSize(m_hInFile,0);
	m_hInFileMapping = CreateFileMapping( m_hInFile, NULL, PAGE_READONLY, 0, 0, NULL );
	if( !m_hInFileMapping )
	{									
		CloseHandle( m_hInFile );
		m_hInFile = NULL ; return FALSE;
	}

	m_lpInImageBase = (BYTE*)MapViewOfFile( m_hInFileMapping, FILE_MAP_READ, 0, 0, 0 );
	if( !m_lpInImageBase )
	{									
		CloseHandle( m_hInFileMapping );
		CloseHandle( m_hInFile );
		m_hInFileMapping = m_hInFile = NULL; return FALSE;
	}
	*/
	IMAGE_NT_HEADERS *inh;
	IMAGE_DOS_HEADER *idh;
	*(DWORD*)&idh = *(DWORD*)&m_lpInImageBase;
	*(DWORD*)&inh = *(DWORD*)&m_lpInImageBase+idh->e_lfanew;
	ImageSize = inh->OptionalHeader.SizeOfImage;
	//CloseHandle( m_hInFile );
	return TRUE;
}
BOOL JK_PeLoadDllEngine::FILELOADER::LoadFile( TCHAR * lpFileName )
{
	m_hInFile = CreateFile( lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( m_hInFile == INVALID_HANDLE_VALUE )				   
		return FALSE;
	fileSize = GetFileSize(m_hInFile,0);
	m_hInFileMapping = CreateFileMapping( m_hInFile, NULL, PAGE_READONLY, 0, 0, NULL );
	if( !m_hInFileMapping )
	{									
		CloseHandle( m_hInFile );
		m_hInFile = NULL ; return FALSE;
	}

	m_lpInImageBase = (BYTE*)MapViewOfFile( m_hInFileMapping, FILE_MAP_READ, 0, 0, 0 );
	if( !m_lpInImageBase )
	{									
		CloseHandle( m_hInFileMapping );
		CloseHandle( m_hInFile );
		m_hInFileMapping = m_hInFile = NULL; return FALSE;
	}

	IMAGE_NT_HEADERS *inh;
	IMAGE_DOS_HEADER *idh;
	*(DWORD*)&idh = *(DWORD*)&m_lpInImageBase;
	*(DWORD*)&inh = *(DWORD*)&m_lpInImageBase+idh->e_lfanew;
	ImageSize = inh->OptionalHeader.SizeOfImage;
	CloseHandle( m_hInFile );
	return TRUE;
}
void JK_PeLoadDllEngine::FILELOADER::FreeFile()
{
	FILEIO::CloseHandleExt(m_lpInImageBase);
}
void JK_PeLoadDllEngine::FILELOADER::UnLoadFile( void )
{
	UnmapViewOfFile( m_lpInImageBase );
	CloseHandle( m_hInFileMapping );

}
void	JK_PeLoadDllEngine::MemFreeLibrary()
{
	if(isLoadOk)
	{
		ASSERT(pImageBase != NULL);
		ASSERT(pDllMain   != NULL);
		//�ѹ���׼��ж��dll
		pDllMain((HINSTANCE)&(pImageBase),DLL_PROCESS_DETACH,0);
		VirtualFree((LPVOID)&pImageBase, 0, MEM_RELEASE);
		isLoadOk=false;
	}
	return;
}
//MemLoadLibrary�������ڴ滺���������м���һ��dll����ǰ���̵ĵ�ַ�ռ䣬ȱʡλ��0x10000000
//����ֵ�� �ɹ�����TRUE , ʧ�ܷ���FALSE
//lpFileData: ���dll�ļ����ݵĻ�����
//DataLength: �����������ݵ��ܳ���
BOOL JK_PeLoadDllEngine::MemLoadLibrary(TCHAR *filename)
{
	FILELOADER loadDll;
	void* lpFileData;
	int DataLength;
	loadDll.LoadFile(filename);
	//char *DllNameBuffer = "../testDll/Release/netTest.exe";


	lpFileData = loadDll.m_lpInImageBase;
	DataLength = loadDll.fileSize;


	if(pImageBase != NULL)
	{
		return FALSE;  //�Ѿ�����һ��dll����û���ͷţ����ܼ����µ�dll
	}
	//���������Ч�ԣ�����ʼ��
	if(!CheckDataValide(lpFileData, DataLength))return FALSE;
	//��������ļ��ؿռ�
	int ImageSize = CalcTotalImageSize();
	if(ImageSize == 0) return FALSE;

	// ���������ڴ�
	void *pMemoryAddress = VirtualAlloc((LPVOID)0, ImageSize,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if(pMemoryAddress == NULL) return FALSE;
	else
	{
		CopyDllDatas(pMemoryAddress, lpFileData); //����dll���ݣ�������ÿ����
		//�ض�λ��Ϣ
		if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress >0
			&& pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size>0)
		{
			DoRelocation(pMemoryAddress);
		}
		//��������ַ��
		if(!FillRavAddress(pMemoryAddress)) //���������ַ��ʧ��
		{
			VirtualFree(pMemoryAddress,0,MEM_RELEASE);
			return FALSE;
		}
		//�޸�ҳ���ԡ�Ӧ�ø���ÿ��ҳ�����Ե����������Ӧ�ڴ�ҳ�����ԡ������һ�¡�
		//ͳһ���ó�һ������PAGE_EXECUTE_READWRITE
		unsigned long old;
		VirtualProtect(pMemoryAddress, ImageSize, PAGE_EXECUTE_READWRITE,&old);
	}
	//��������ַ
	pNTHeader->OptionalHeader.ImageBase = *(DWORD*)&pMemoryAddress;

	//������Ҫ����һ��dll����ں���������ʼ��������
	*(DWORD*)(&pDllMain) = (pNTHeader->OptionalHeader.AddressOfEntryPoint +*(DWORD*)&pMemoryAddress);

	BOOL InitResult = pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_ATTACH,0);
	if(!InitResult) //��ʼ��ʧ��
	{
		pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_DETACH,0);
		VirtualFree(pMemoryAddress,0,MEM_RELEASE);
		pDllMain = NULL;
		return FALSE;
	}

	isLoadOk = TRUE;
	pImageBase = *(DWORD*)&pMemoryAddress;
	loadDll.UnLoadFile();
	return TRUE;
}
BOOL JK_PeLoadDllEngine::MemLoadLibrary(TCHAR *filename,DWORD decodeKey)
{
	FILELOADER loadDll;
	void* lpFileData;
	DWORD old;
	int DataLength;
	int ImageSize;
	BOOL InitResult;
	loadDll.LoadFile(filename,decodeKey);
	//char *DllNameBuffer = "../testDll/Release/netTest.exe";

	
	lpFileData = loadDll.m_lpInImageBase;
	DataLength = loadDll.fileSize;


	if(pImageBase != NULL)
	{
		return FALSE;  //�Ѿ�����һ��dll����û���ͷţ����ܼ����µ�dll
	}
	//���������Ч�ԣ�����ʼ��
	if(!CheckDataValide(lpFileData, DataLength))return FALSE;
	//��������ļ��ؿռ�
	ImageSize = CalcTotalImageSize();
	if(ImageSize == 0) return FALSE;

	// ���������ڴ�
	void *pMemoryAddress = VirtualAlloc((LPVOID)0, ImageSize,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if(pMemoryAddress == NULL) return FALSE;
	else
	{
		CopyDllDatas(pMemoryAddress, lpFileData); //����dll���ݣ�������ÿ����
		//�ض�λ��Ϣ
		if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress >0
			&& pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size>0)
		{
			DoRelocation(pMemoryAddress);
		}
		//��������ַ��
		if(!FillRavAddress(pMemoryAddress)) //���������ַ��ʧ��
		{
			VirtualFree(pMemoryAddress,0,MEM_RELEASE);
			return FALSE;
		}
		//�޸�ҳ���ԡ�Ӧ�ø���ÿ��ҳ�����Ե����������Ӧ�ڴ�ҳ�����ԡ������һ�¡�
		//ͳһ���ó�һ������PAGE_EXECUTE_READWRITE
		
		VirtualProtect(pMemoryAddress, ImageSize, PAGE_EXECUTE_READWRITE,&old);
	}
	//��������ַ
	pNTHeader->OptionalHeader.ImageBase = *(DWORD*)&pMemoryAddress;

	//������Ҫ����һ��dll����ں���������ʼ��������
	*(DWORD*)(&pDllMain) = (pNTHeader->OptionalHeader.AddressOfEntryPoint +*(DWORD*)&pMemoryAddress);
	
	InitResult = pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_ATTACH,0);
	if(!InitResult) //��ʼ��ʧ��
	{
		pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_DETACH,0);
		VirtualFree(pMemoryAddress,0,MEM_RELEASE);
		pDllMain = NULL;
		return FALSE;
	}

	isLoadOk = TRUE;
	pImageBase = *(DWORD*)&pMemoryAddress;
	loadDll.FreeFile();
	return TRUE;
}
/////
////////��õ������������ӳ���ַ��ƫ��
//////
DWORD JK_PeLoadDllEngine::MemOffsetProcAddr(CHAR *lpProcName)
{
	DWORD offset;
	offset = (DWORD)MemGetProcAddress(lpProcName);
	offset = offset - pNTHeader->OptionalHeader.ImageBase;
	return offset;

}
//MemGetProcAddress������dll�л�ȡָ�������ĵ�ַ
//����ֵ�� �ɹ����غ�����ַ , ʧ�ܷ���NULL
//lpProcName: Ҫ���Һ��������ֻ������
FARPROC  JK_PeLoadDllEngine::MemGetProcAddress(CHAR *lpProcName)
{
	CHAR* pName=0;
	LPWORD  pAddressOfOrdinals =0;
	LPDWORD pAddressOfNames  = 0,pAddressOfFunctions=0;
	PIMAGE_EXPORT_DIRECTORY pExport=0;
	DWORD OffsetStart,Size,pFunctionOffset;
	FARPROC retfunc;
	int iBase,iNumberOfFunctions ,iNumberOfNames,iOrdinal = -1,iFound = -1;

	if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
		return NULL;
	if(!isLoadOk) return NULL;

	OffsetStart = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	Size = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	
	*(DWORD*)(&pExport) = ((DWORD)pImageBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	iBase = pExport->Base;
	iNumberOfFunctions = pExport->NumberOfFunctions;
	iNumberOfNames = pExport->NumberOfNames; //<= iNumberOfFunctions


	*(DWORD*)(& pAddressOfFunctions) = (pExport->AddressOfFunctions + pImageBase);
	*(DWORD*)(&pAddressOfOrdinals) = (pExport->AddressOfNameOrdinals + pImageBase);
	*(DWORD*)(& pAddressOfNames)  = (pExport->AddressOfNames + pImageBase);

	

	if(((*(DWORD*)(&lpProcName)) & 0xFFFF0000) == 0) //IT IS A ORDINAL!
	{
		iOrdinal = (*(DWORD*)(&lpProcName)) & 0x0000FFFF - iBase;
	}
	else  //use name
	{
		for(int i=0;i<iNumberOfNames;i++)
		{
			
			*(DWORD*)(&pName)= (pAddressOfNames[i] + pImageBase);
			if(lstrcmpA(pName, lpProcName) == 0)
			{
				iFound = i; break;
			}
		}
		if(iFound >= 0)
		{
			iOrdinal = (int)(pAddressOfOrdinals[iFound]);
		}
	}

	if(iOrdinal < 0 || iOrdinal >= iNumberOfFunctions ) return NULL;
	else
	{
		pFunctionOffset = pAddressOfFunctions[iOrdinal];
		if(pFunctionOffset > OffsetStart && pFunctionOffset < (OffsetStart+Size))//maybe Export Forwarding
			return NULL;
		else 
		{
			
			*(DWORD*)(&retfunc)=(pFunctionOffset + pImageBase);
			return retfunc;
		}
	}

}


// �ض���PE�õ��ĵ�ַ
void JK_PeLoadDllEngine::DoRelocation( void *NewBase)
{
	/* �ض�λ��Ľṹ��
	// DWORD sectionAddress, DWORD size (����������Ҫ�ض�λ������)
	// ���� 1000����Ҫ����5���ض�λ���ݵĻ����ض�λ���������
	// 00 10 00 00   14 00 00 00      xxxx xxxx xxxx xxxx xxxx 0000
	// -----------   -----------      ----
	// �����ڵ�ƫ��  �ܳߴ�=8+6*2     ��Ҫ�����ĵ�ַ           ���ڶ���4�ֽ�
	// �ض�λ�������ɸ����������address �� size����0 ��ʾ����
	// ��Ҫ�����ĵ�ַ��12λ�ģ���4λ����̬�֣�intel cpu����3
	*/
	//����NewBase��0x600000,���ļ������õ�ȱʡImageBase��0x400000,������ƫ��������0x200000
	DWORD Delta = (DWORD)(*(DWORD*)&NewBase - pNTHeader->OptionalHeader.ImageBase);

	//ע���ض�λ���λ�ÿ��ܺ�Ӳ���ļ��е�ƫ�Ƶ�ַ��ͬ��Ӧ��ʹ�ü��غ�ĵ�ַ
	PIMAGE_BASE_RELOCATION pLoc =0;
	*(DWORD*)(&pLoc)= (*(DWORD*)&NewBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	while((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0) //��ʼɨ���ض�λ��
	{
		WORD *pLocData = 0;
		*(int*)(&pLocData)=(*(int*)&pLoc + (int)sizeof(IMAGE_BASE_RELOCATION));
		//���㱾����Ҫ�������ض�λ���ַ������Ŀ
		int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
		for( int i=0 ; i < NumberOfReloc; i++)
		{
			if( (DWORD)(pLocData[i] & 0xF000) == 0x00003000) //����һ����Ҫ�����ĵ�ַ
			{
				// ������ 
				// pLoc->VirtualAddress = 0x1000; 
				// pLocData[i] = 0x313E; ��ʾ����ƫ�Ƶ�ַ0x13E����Ҫ����
				// ��� pAddress = ����ַ + 0x113E
				// ����������� A1 ( 0c d4 02 10)  �������ǣ� mov eax , [1002d40c]
				// ��Ҫ����1002d40c�����ַ
				DWORD * pAddress = 0;
				*(DWORD*)&pAddress = (*(DWORD*)&NewBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				*pAddress += Delta;
			}
		}
		//ת�Ƶ���һ���ڽ��д���
		*(DWORD*)(&pLoc)=(*(DWORD*)&pLoc + pLoc->SizeOfBlock);
	}
}
//���������ַ��
BOOL ChangeRavAddress(void* pImageBase,CHAR *funcName,CHAR *dllname,DWORD newAddr)
{
	/*
	int i=0;
	// �����ʵ������һ�� IMAGE_IMPORT_DESCRIPTOR �ṹ���飬ȫ����0��ʾ����
	// ���鶨�����£�
	// 
	// DWORD   OriginalFirstThunk;         // 0��ʾ����������ָ��δ�󶨵�IAT�ṹ����
	// DWORD   TimeDateStamp; 
	// DWORD   ForwarderChain;             // -1 if no forwarders
	// DWORD   Name;                       // ����dll������
	// DWORD   FirstThunk;                 // ָ��IAT�ṹ����ĵ�ַ(�󶨺���ЩIAT�������ʵ�ʵĺ�����ַ)
	PIMAGE_NT_HEADERS lpNTHeader;
	PIMAGE_DOS_HEADER lpDosHeader;
	PIMAGE_SECTION_HEADER lpSectionHeader;
	lpDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	//�µ�peͷ��ַ
	*(DWORD*)&lpNTHeader = (*(DWORD*)&pImageBase + (lpDosHeader->e_lfanew));
	//�µĽڱ��ַ
	*(DWORD*)&lpSectionHeader = (*(DWORD*)&lpNTHeader + sizeof(IMAGE_NT_HEADERS));


	DWORD Offset = lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ;
	if(Offset == 0) return TRUE; //No Import Table
	PIMAGE_IMPORT_DESCRIPTOR pID =0;
	*(DWORD*)(&pID)= (*(DWORD*) &pImageBase + Offset);
	//while(pID->Characteristics != 0)
		//int cnt=0;
	for(int cnt=0;cnt<lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size/sizeof(PIMAGE_IMPORT_DESCRIPTOR);cnt++)
	{
		PIMAGE_THUNK_DATA pRealIAT =0;
		*(DWORD*)(&pRealIAT)=(*(DWORD*)&pImageBase + pID->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalIAT =0;
		*(DWORD*)(&pOriginalIAT) = (*(DWORD*)&pImageBase + pID->OriginalFirstThunk);
		//��ȡdll������
		CHAR buf[MAX_PATH]; //dll name;
		BYTE* pName = 0;
		*(DWORD*)(&pName)=(*(DWORD*)&pImageBase + pID->Name);
		for(i=0;i<sizeof(buf);i++)
		{
			if(pName[i] == 0)break;
			buf[i] = pName[i];
		}
		if(i>=sizeof(buf)) return FALSE;  // bad dll name
		else buf[i] = 0;
	
		//if(lstrcmpiA(dllname,buf))
		//{
		//	*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
		//	continue;
		//}

		
		HMODULE hDll = GetModuleHandleA(buf); 
		if(hDll == NULL) { 
			hDll = LoadLibraryA(buf); 
			if (hDll == NULL) 
				return FALSE; //NOT FOUND DLL 
		} 
		
		for(i=0; ;i++)
		{
			if(pOriginalIAT[i].u1.Function == 0)
				break;
			FARPROC lpFunction = NULL;
			if(pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG) //�����ֵ�������ǵ������
			{
				lpFunction = GetProcAddress((HMODULE)hDll, (CHAR*)(pOriginalIAT[i].u1.Ordinal & 0x0000FFFF));
			}
			else //�������ֵ���
			{
				//��ȡ��IAT���������ĺ�������
				PIMAGE_IMPORT_BY_NAME pByName = 0;
				*(DWORD*)(&pByName) = (*(DWORD*)&pImageBase + (DWORD)(pOriginalIAT[i].u1.AddressOfData));
				//    if(pByName->Hint !=0)
				//     lpFunction = GetProcAddress(hDll, (LPCSTR)pByName->Hint);
				//    else
				lpFunction = GetProcAddress((HMODULE)hDll, (char *)pByName->Name);
			}
			if(lpFunction != NULL)   //�ҵ��ˣ�
			{
				//pRealIAT[i].u1.Function = *(DWORD*)&lpFunction;
				__asm MOV EAX,lpFunction
			}
			else return FALSE;
		}

		//move to next 
		*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	*/
	return TRUE;
}
//��������ַ��
BOOL JK_PeLoadDllEngine::FillRavAddress(void *pImageBase)
{
	int i=0;
	// �����ʵ������һ�� IMAGE_IMPORT_DESCRIPTOR �ṹ���飬ȫ����0��ʾ����
	// ���鶨�����£�
	// 
	// DWORD   OriginalFirstThunk;         // 0��ʾ����������ָ��δ�󶨵�IAT�ṹ����
	// DWORD   TimeDateStamp; 
	// DWORD   ForwarderChain;             // -1 if no forwarders
	// DWORD   Name;                       // ����dll������
	// DWORD   FirstThunk;                 // ָ��IAT�ṹ����ĵ�ַ(�󶨺���ЩIAT�������ʵ�ʵĺ�����ַ)
	unsigned long Offset = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ;
	if(Offset == 0) return TRUE; //No Import Table
	PIMAGE_IMPORT_DESCRIPTOR pID =0;
	*(DWORD*)(&pID)= (*(DWORD*) &pImageBase + Offset);
	while(pID->Characteristics != 0)
		//int cnt=0;
		//for(cnt=0;cnt<pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size/sizeof(PIMAGE_IMPORT_DESCRIPTOR);cnt++)
	{
		PIMAGE_THUNK_DATA pRealIAT =0;
		*(DWORD*)(&pRealIAT)=(*(DWORD*)&pImageBase + pID->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalIAT =0;
		*(DWORD*)(&pOriginalIAT) = (*(DWORD*)&pImageBase + pID->OriginalFirstThunk);
		//��ȡdll������
		CHAR buf[MAX_PATH]; //dll name;
		BYTE* pName = 0;
		*(DWORD*)(&pName)=(*(DWORD*)&pImageBase + pID->Name);
		for(i=0;i<sizeof(buf);i++)
		{
			if(pName[i] == 0)break;
			buf[i] = pName[i];
		}
		if(i>=sizeof(buf)) return FALSE;  // bad dll name
		else buf[i] = 0;

		HMODULE hDll = GetModuleHandleA(buf); 
		if(hDll == NULL) { 
			hDll = LoadLibraryA(buf); 
			if (hDll == NULL) 
				return FALSE; //NOT FOUND DLL 
		} 
		//��ȡDLL��ÿ�����������ĵ�ַ������IAT
		//ÿ��IAT�ṹ�� ��
		// union { PBYTE  ForwarderString;
		//   PDWORD Function;
		//   DWORD Ordinal;
		//   PIMAGE_IMPORT_BY_NAME  AddressOfData;
		// } u1;
		// ������һ��DWORD ����������һ����ַ��
		for(i=0; ;i++)
		{
			if(pOriginalIAT[i].u1.Function == 0)
				break;
			FARPROC lpFunction = NULL;
			if(pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG) //�����ֵ�������ǵ������
			{
				lpFunction = GetProcAddress(hDll, (CHAR*)(pOriginalIAT[i].u1.Ordinal & 0x0000FFFF));
			}
			else //�������ֵ���
			{
				//��ȡ��IAT���������ĺ�������
				PIMAGE_IMPORT_BY_NAME pByName = 0;
				*(DWORD*)(&pByName) = (*(DWORD*)&pImageBase + (DWORD)(pOriginalIAT[i].u1.AddressOfData));
				//    if(pByName->Hint !=0)
				//     lpFunction = GetProcAddress(hDll, (LPCSTR)pByName->Hint);
				//    else
				lpFunction = GetProcAddress(hDll, (char *)pByName->Name);
			}
			if(lpFunction != NULL)   //�ҵ��ˣ�
			{
				pRealIAT[i].u1.Function = *(DWORD*)&lpFunction;
			}
			else return FALSE;
		}

		//move to next 
		*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	return TRUE;
}

//CheckDataValide�������ڼ�黺�����е������Ƿ���Ч��dll�ļ�
//����ֵ�� ��һ����ִ�е�dll�򷵻�TRUE�����򷵻�FALSE��
//lpFileData: ���dll���ݵ��ڴ滺����
//DataLength: dll�ļ��ĳ���
BOOL JK_PeLoadDllEngine::CheckDataValide(void* lpFileData, int DataLength)
{
	//��鳤��
	if(DataLength < sizeof(IMAGE_DOS_HEADER)) return FALSE;
	pDosHeader = (PIMAGE_DOS_HEADER)lpFileData;  // DOSͷ
	//���dosͷ�ı��
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;  //0x5A4D : MZ

	//��鳤��
	if((DWORD)DataLength < (pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS)) ) return FALSE;
	//ȡ��peͷ
	*(DWORD*)&pNTHeader = ( *(DWORD*)&lpFileData + pDosHeader->e_lfanew); // PEͷ
	//���peͷ�ĺϷ���
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) return FALSE;  //0x00004550 : PE00
	if((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) //0x2000  : File is a DLL
		return FALSE;  
	if((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) //0x0002 : ָ���ļ���������
		return FALSE;
	if(pNTHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)) return FALSE;


	//ȡ�ýڱ��α�
	*(DWORD*)&pSectionHeader = (*(DWORD*)&pNTHeader + sizeof(IMAGE_NT_HEADERS));
	//��֤ÿ���ڱ�Ŀռ�
	for(int i=0; i< pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if((pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData) > (DWORD)DataLength)return FALSE;
	}
	return TRUE;
}

//�������߽�
int JK_PeLoadDllEngine::GetAlignedSize(int Origin, int Alignment)
{
	return (Origin + Alignment - 1) / Alignment * Alignment;
}
//��������dllӳ���ļ��ĳߴ�
int JK_PeLoadDllEngine::CalcTotalImageSize()
{
	int Size;
	if(pNTHeader == NULL)return 0;
	int nAlign = pNTHeader->OptionalHeader.SectionAlignment; //�ζ����ֽ���

	// ��������ͷ�ĳߴ硣����dos, coff, peͷ �� �α�Ĵ�С
	Size = GetAlignedSize(pNTHeader->OptionalHeader.SizeOfHeaders, nAlign);
	// �������нڵĴ�С
	for(int i=0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
	{
		//�õ��ýڵĴ�С
		int CodeSize = pSectionHeader[i].Misc.VirtualSize ;
		int LoadSize = pSectionHeader[i].SizeOfRawData;
		int MaxSize = (LoadSize > CodeSize)?(LoadSize):(CodeSize);

		int SectionSize = GetAlignedSize(pSectionHeader[i].VirtualAddress + MaxSize, nAlign);
		if(Size < SectionSize) 
			Size = SectionSize;  //Use the Max;
	}
	return Size;
}
//CopyDllDatas������dll���ݸ��Ƶ�ָ���ڴ����򣬲��������н�
//pSrc: ���dll���ݵ�ԭʼ������
//pDest:Ŀ���ڴ��ַ
void JK_PeLoadDllEngine::CopyDllDatas(void* pDest, void* pSrc)
{
	// ������Ҫ���Ƶ�PEͷ+�α��ֽ���
	int  HeaderSize = pNTHeader->OptionalHeader.SizeOfHeaders;
	int  SectionSize = pNTHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	int  MoveSize = HeaderSize + SectionSize;
	//����ͷ�Ͷ���Ϣ
	memmove(pDest, pSrc, MoveSize);

	//����ÿ����
	for(int i=0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
	{
		if(pSectionHeader[i].VirtualAddress == 0 || pSectionHeader[i].SizeOfRawData == 0)continue;
		// ��λ�ý����ڴ��е�λ��
		void *pSectionAddress =0;
		*(DWORD*)&pSectionAddress=(*(DWORD*)&pDest + pSectionHeader[i].VirtualAddress);
		// ���ƶ����ݵ������ڴ�
		memmove((void *)pSectionAddress,(void *)(*(DWORD*)&pSrc + pSectionHeader[i].PointerToRawData),pSectionHeader[i].SizeOfRawData);
	}

	//����ָ�룬ָ���·�����ڴ�
	//�µ�dosͷ
	pDosHeader = (PIMAGE_DOS_HEADER)pDest;
	//�µ�peͷ��ַ
	*(DWORD*)&pNTHeader = (*(DWORD*)&pDest + (pDosHeader->e_lfanew));
	//�µĽڱ��ַ
	*(DWORD*)&pSectionHeader = (*(DWORD*)&pNTHeader + sizeof(IMAGE_NT_HEADERS));
	return ;
}
