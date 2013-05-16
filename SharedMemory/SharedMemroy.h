#pragma once

class CSharedMemroy 
{
public:
	CSharedMemroy();
	~CSharedMemroy();

	BOOL Open();
	void Close();

	BOOL SetMainWindow(HWND hWnd);
	HWND GetMainWindow();

	BOOL SetClientPath(LPCTSTR lpszPath);
	LPCTSTR GetClientPath();

	BOOL Set_RecItem_Window(HWND hWnd);
	HWND Get_RecItem_Window();


	BOOL Set_RecUI_Window(HWND hWnd);
	HWND Get_RecUI_Window();


	BOOL Set_ChangeSkin_Window(HWND hWnd);
	HWND Get_ChangeSkin_Window();


	BOOL Set_PicMgr_Window(HWND hWnd);
	HWND Get_PicMgr_Window();

	
protected:
	struct SharedMemory
	{
		HWND hBigFootWnd;
		TCHAR cClientPath[MAX_PATH];
		HWND hBF_RecItem;
		HWND hBF_ChangeSkin;
		HWND hBF_RecUI;
		HWND hBF_PicMgr;
		
	};

	HANDLE m_hMutex;
	HANDLE m_hFileMapping;
	SharedMemory *m_pSharedMemory;
	CRITICAL_SECTION m_CriticalSection;
	bool m_bOpened;
};


