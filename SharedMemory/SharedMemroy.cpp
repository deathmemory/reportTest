// SharedMemroy.cpp : implementation file
//

#include "stdafx.h"
#include "SharedMemroy.h"

// CSharedMemroy

CSharedMemroy::CSharedMemroy()
{
	m_hMutex = NULL;
	m_hFileMapping = NULL;
	m_pSharedMemory = NULL;
	m_bOpened = false;
	Open();
}

CSharedMemroy::~CSharedMemroy()
{
	Close();
}

BOOL CSharedMemroy::Open()
{
	m_hMutex = CreateMutex(NULL, TRUE, _T("BigFoot-LOL IPC Mutex"));
	if (m_hFileMapping)
		return FALSE;

	m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory), _T("BigFoot-LOL IPC"));
	if (m_hFileMapping)
	{
		m_pSharedMemory  = (SharedMemory *)MapViewOfFile(m_hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (m_pSharedMemory)
		{
			m_bOpened = true;
			return TRUE;
		}
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}
	return FALSE;
}

void CSharedMemroy::Close()
{
	if (m_pSharedMemory)
	{
		UnmapViewOfFile(m_pSharedMemory);
		m_pSharedMemory = NULL;
	}
	if (m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_pSharedMemory = NULL;
	}
	if (m_hMutex)
		CloseHandle(m_hMutex);

	m_hMutex = NULL;
	m_bOpened = false;
}

BOOL CSharedMemroy::SetClientPath( LPCTSTR lpszPath )
{
	if (!m_bOpened)
		return FALSE;
	
	WaitForSingleObject(m_hMutex, INFINITE);
	_tcscpy(m_pSharedMemory->cClientPath, lpszPath);
	ReleaseMutex(m_hMutex);
	return TRUE;
}

LPCTSTR CSharedMemroy::GetClientPath()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->cClientPath;
}

BOOL CSharedMemroy::SetMainWindow( HWND hWnd )
{
	if (!m_bOpened)
		return FALSE;

	WaitForSingleObject(m_hMutex, INFINITE);
	m_pSharedMemory->hBigFootWnd = hWnd;
	ReleaseMutex(m_hMutex);
	return TRUE;
}

HWND CSharedMemroy::GetMainWindow()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->hBigFootWnd;
}

BOOL CSharedMemroy::Set_RecItem_Window(HWND hWnd)
{
	if (!m_bOpened)
		return FALSE;

	InitializeCriticalSection(&m_CriticalSection);
	m_pSharedMemory->hBF_RecItem = hWnd;
	DeleteCriticalSection(&m_CriticalSection);
	return TRUE;
}
HWND CSharedMemroy::Get_RecItem_Window()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->hBF_RecItem;
}

BOOL CSharedMemroy::Set_RecUI_Window(HWND hWnd)
{
	if (!m_bOpened)
		return FALSE;


	InitializeCriticalSection(&m_CriticalSection);
	m_pSharedMemory->hBF_RecUI = hWnd;
	DeleteCriticalSection(&m_CriticalSection);
	return TRUE;
}
HWND CSharedMemroy::Get_RecUI_Window()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->hBF_RecUI;
}

BOOL CSharedMemroy::Set_ChangeSkin_Window(HWND hWnd)
{
	if (!m_bOpened)
		return FALSE;

	InitializeCriticalSection(&m_CriticalSection);
	m_pSharedMemory->hBF_ChangeSkin = hWnd;
	DeleteCriticalSection(&m_CriticalSection);
	return TRUE;
}
HWND CSharedMemroy::Get_ChangeSkin_Window()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->hBF_ChangeSkin;
}

BOOL CSharedMemroy::Set_PicMgr_Window(HWND hWnd)
{
	if (!m_bOpened)
		return FALSE;

	InitializeCriticalSection(&m_CriticalSection);
	m_pSharedMemory->hBF_PicMgr = hWnd;
	DeleteCriticalSection(&m_CriticalSection);
	return TRUE;
}
HWND CSharedMemroy::Get_PicMgr_Window()
{
	if (!m_bOpened)
		return NULL;

	return m_pSharedMemory->hBF_PicMgr;
}