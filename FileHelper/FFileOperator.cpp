#include "stdafx.h"
#include "FFileOperator.h"

#include <Shlwapi.h>
#include <shlobj.h>
#include <io.h>

#pragma comment( lib, "Shlwapi.lib" )
#pragma comment( lib, "mpr.lib" )

#include "atlconv.h"


bool CFFileOperator::CreateEmptyFile( const CString& rFilePath )
{
	//try {
	//	CFile file( rFilePath, CFile::modeWrite | CFile::modeCreate );
	//	file.Close();
	//	return true;
	//} catch (...) {
		return false;
	//}
}

bool CFFileOperator::DeleteFile( const CString& rFilePath )
{
	CString	strFilePath = rFilePath + _T('\0');
	SHFILEOPSTRUCT stFileOp;
	stFileOp.hwnd					= NULL;
	stFileOp.wFunc					= FO_DELETE;
	stFileOp.pFrom					= strFilePath;
	stFileOp.pTo					= NULL;
	stFileOp.fFlags					= FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;
	stFileOp.fAnyOperationsAborted	= FALSE;
	stFileOp.hNameMappings			= NULL;
	stFileOp.lpszProgressTitle		= NULL;
	return ::SHFileOperation( &stFileOp ) == 0;
}

bool CFFileOperator::CopyFile( const CString& rSrcFilePath, const CString& rDstFilePath )
{
	CString	strSrcFilePath = rSrcFilePath + _T('\0');
	CString	strDstFilePath = rDstFilePath + _T('\0');
	SHFILEOPSTRUCT stFileOp;
	stFileOp.hwnd					= NULL;
	stFileOp.wFunc					= FO_COPY;
	stFileOp.pFrom					= strSrcFilePath;
	stFileOp.pTo					= strDstFilePath;
	stFileOp.fFlags					= FOF_MULTIDESTFILES | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;
	stFileOp.fAnyOperationsAborted	= FALSE;
	stFileOp.hNameMappings			= NULL;
	stFileOp.lpszProgressTitle		= NULL;
	return ::SHFileOperation( &stFileOp ) == 0;
}

bool CFFileOperator::MoveFile( const CString& rSrcFilePath, const CString& rDstFilePath )
{
	CString	strSrcFilePath = rSrcFilePath + _T('\0');
	CString	strDstFilePath = rDstFilePath + _T('\0');
	SHFILEOPSTRUCT stFileOp;
	stFileOp.hwnd					= NULL;
	stFileOp.wFunc					= FO_MOVE;
	stFileOp.pFrom					= strSrcFilePath;
	stFileOp.pTo					= strDstFilePath;
	stFileOp.fFlags					= FOF_MULTIDESTFILES | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;
	stFileOp.fAnyOperationsAborted	= FALSE;
	stFileOp.hNameMappings			= NULL;
	stFileOp.lpszProgressTitle		= NULL;
	return ::SHFileOperation( &stFileOp ) == 0;
}

bool CFFileOperator::CopyFile_Confirm( const CString& rSrcFilePath, const CString& rDstFilePath )
{
	CString	strSrcFilePath = rSrcFilePath + _T('\0');
	CString	strDstFilePath = rDstFilePath + _T('\0');
	SHFILEOPSTRUCT stFileOp;
	stFileOp.hwnd					= NULL;
	stFileOp.wFunc					= FO_COPY;
	stFileOp.pFrom					= strSrcFilePath;
	stFileOp.pTo					= strDstFilePath;
	stFileOp.fFlags					= FOF_MULTIDESTFILES | FOF_SILENT | FOF_NOERRORUI;
	stFileOp.fAnyOperationsAborted	= FALSE;
	stFileOp.hNameMappings			= NULL;
	stFileOp.lpszProgressTitle		= NULL;
	return ::SHFileOperation( &stFileOp ) == 0;
}

bool CFFileOperator::MoveFile_Confirm( const CString& rSrcFilePath, const CString& rDstFilePath )
{
	CString	strSrcFilePath = rSrcFilePath + _T('\0');
	CString	strDstFilePath = rDstFilePath + _T('\0');
	SHFILEOPSTRUCT stFileOp;
	stFileOp.hwnd					= NULL;
	stFileOp.wFunc					= FO_MOVE;
	stFileOp.pFrom					= strSrcFilePath;
	stFileOp.pTo					= strDstFilePath;
	stFileOp.fFlags					= FOF_MULTIDESTFILES | FOF_SILENT | FOF_NOERRORUI;
	stFileOp.fAnyOperationsAborted	= FALSE;
	stFileOp.hNameMappings			= NULL;
	stFileOp.lpszProgressTitle		= NULL;
	return ::SHFileOperation( &stFileOp ) == 0;
}

bool CFFileOperator::CreateDirectories( const CString& rDirPath )
{
	if ( rDirPath.IsEmpty() ) {
		return false;
	}
	CString strDirPath( RemoveBackslashPath( rDirPath ) );
	if ( IsFileExists( strDirPath ) ) {
		if ( IsDirectory( strDirPath ) ) {
			return true;
		}
		return false;
	}
	CString strCreateDirPath = GetDrive( strDirPath ) + GetDir( strDirPath );
	if ( !strCreateDirPath.IsEmpty() ) {

		if ( 0 == strCreateDirPath.CompareNoCase( strDirPath ) ) {
			return false;
		}

		if ( !CreateDirectories( strCreateDirPath ) ) {
			return false;
		}
	}
	if ( !::CreateDirectory( strDirPath, NULL ) ) {
		return false;
	}
	return true;
}


bool CFFileOperator::DeleteDirectories( const CString& rDirPath )
{
	return DeleteFile( rDirPath );
}


bool CFFileOperator::CopyDirectories( const CString& rSrcDirPath, const CString& rDstDirPath )
{
	return CopyFile( rSrcDirPath, rDstDirPath );
}


bool CFFileOperator::MoveDirectories( const CString& rSrcDirPath, const CString& rDstDirPath )
{
	return MoveFile( rSrcDirPath, rDstDirPath );
}


bool CFFileOperator::IsFileExists( const CString& rFilePath )
{
	return !!::PathFileExists( rFilePath );
}

bool CFFileOperator::IsDirectory( const CString& rDirPath )
{
	return !!::PathIsDirectory( rDirPath );
}


ULONGLONG CFFileOperator::GetFileSize( const CString& rFilePath )
{
	//CFileStatus cStatus;
	//if ( !CFile::GetStatus( rFilePath, cStatus ) ) {
	//	return 0;
	//}
	//return cStatus.m_size;
	HANDLE hFile = NULL;
	 hFile =CreateFile(rFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		DWORD dwFileHigh;
		DWORD dwFileSize = ::GetFileSize(hFile,&dwFileHigh);
		CloseHandle(hFile);
		return dwFileSize;
	}
	return 0;
}

DWORD CFFileOperator::GetFileAttributes( const CString& rFilePath )
{
	//CFileStatus cStatus;
	//if ( !CFile::GetStatus( rFilePath, cStatus ) ) {
	//	return 0;
	//}
	//return cStatus.m_attribute;
	return 0;
}

bool CFFileOperator::SetFileAttributes( const CString& rFilePath, DWORD dwFileAttributes )
{
	//CFileStatus cStatus;
	//if ( !CFile::GetStatus( rFilePath, cStatus ) ) {
	//	return false;
	//}
	//cStatus.m_attribute = (CFile::Attribute)dwFileAttributes;
	//CFile::SetStatus( rFilePath, cStatus );
	return true;
}


//CTime CFFileOperator::GetFileLastModifiedTime( const CString& rFilePath )
//{
//	//CFileStatus cStatus;
//	//if ( !CFile::GetStatus( rFilePath, cStatus ) ) {
//	//	return CTime();
//	//}
//	//return cStatus.m_mtime;
//	return 0;
//}


CString CFFileOperator::GetCurrentExeDir()
{
	CString strFilePath;
	DWORD dwRes = ::GetModuleFileName( NULL, strFilePath.GetBuffer( MAX_PATH ), MAX_PATH+1 );
	strFilePath.ReleaseBuffer();
	if ( dwRes == 0 ) {
		return _T("");
	}
	CString strDirPath( RemoveFileSpecPath( strFilePath ) );
	return strDirPath;
}


CString CFFileOperator::GetModuleCurrentDir( const CString& rModuleName )
{
	HMODULE hModule = ::GetModuleHandle( rModuleName );
	if ( hModule == NULL ) {
		return _T("");
	}
	CString strFilePath;
	DWORD dwRes = ::GetModuleFileName( hModule, strFilePath.GetBuffer( MAX_PATH ), MAX_PATH+1 );
	strFilePath.ReleaseBuffer();
	if ( dwRes == 0 ) {
		return _T("");
	}
	CString strDirPath( RemoveFileSpecPath( strFilePath ) );
	return strDirPath;
}


CString CFFileOperator::GetUniqueFilePath( bool bFolder, const CString& rExt )
{
	CString strTempFolderPath;
	DWORD dwRes = ::GetTempPath( MAX_PATH+1, strTempFolderPath.GetBuffer( MAX_PATH ) );
	strTempFolderPath.ReleaseBuffer();
	if ( dwRes == 0 ) {
		return _T("");
	}
	if ( bFolder ) {
		return strTempFolderPath;
	}
	else {
		CString strTempFilePath;
		CString strUniqueFilePath;
		do {
			dwRes = ::GetTempFileName( strTempFolderPath, _T(""), 0, strTempFilePath.GetBuffer( MAX_PATH ) );
			strTempFilePath.ReleaseBuffer();
			if ( dwRes == 0 ) {
				return _T("");
			}
			strUniqueFilePath = RenameExtensionPath( strTempFilePath, rExt );
			if ( !IsFileExists( strUniqueFilePath ) ) {
				if ( !MoveFile( strTempFilePath, strUniqueFilePath ) ) {
					DeleteFile( strTempFilePath );
					return _T("");
				}
				break;
			}
			DeleteFile( strTempFilePath );
		} while ( 1 );
		return strUniqueFilePath;
	}
}


CString CFFileOperator::GetSpecialFolderPath( int nFolder, const CString& rRelativePath )
{
	CString strDirPath;
	BOOL bRes = ::SHGetSpecialFolderPath( NULL, strDirPath.GetBuffer( MAX_PATH ), nFolder, TRUE );
	strDirPath.ReleaseBuffer();
	if ( !bRes ) {
		return _T("");
	}
	return CombinePath( strDirPath, rRelativePath );
}


DWORD CFFileOperator::CheckSharedFolderConnection( const CString& rRemoteName, const CString& rAccount, const CString& rPassword )
{
	int	nState = ::_taccess( rRemoteName, 00 );
	DWORD dwState = 0;
	if ( nState != 0 ){
		CString strRemoteName( rRemoteName );
		NETRESOURCE stNetRes;
		ZeroMemory( &stNetRes, sizeof(NETRESOURCE) );
		stNetRes.dwType			= RESOURCETYPE_DISK;
		stNetRes.lpRemoteName	= strRemoteName.GetBuffer(0);
		stNetRes.lpLocalName	= NULL;
		stNetRes.lpProvider		= NULL;
		if( rAccount.IsEmpty() && rPassword.IsEmpty() ) {
			dwState = ::WNetAddConnection2( &stNetRes, NULL, NULL, CONNECT_UPDATE_PROFILE );
		} else {
			dwState = ::WNetAddConnection2( &stNetRes, rPassword, rAccount, CONNECT_UPDATE_PROFILE );
		}

		/*[1]<<<---Updated on 2008/04/28 SureKAM David Wong */
		/*
		  #NOTE:
			 In Windows NT, you can use the API with multiple sets of user credentials.
			 However, one major limitation applies, namely,
			 that connections to a given server or its shared resources have to be made within the context of a single set of credentials.
			 For an initial connection, if you use NULL for lpPassword or lpUsername, the interactive user's credentials are used by default.
			 If there has been a successful initial connection to the server,
			 subsequent attempts to open a connection using a different set of credentials results in error 1219 (ERROR_SESSION_CREDENTIAL_CONFLICT),
			 which indicates a conflict between the supplied set of credentials and the existing one.
			 To establish a connection using a different set of credentials,
			 first you have to cancel the existing connections to the server using WNetCancelConnection2().
			
			 See also:
					MSDN Oct-2000: INFO: WNetAddConnection2 and Multiple User Credentials		Article ID:	183366
 
		*/
		if (dwState == ERROR_SESSION_CREDENTIAL_CONFLICT) {
			dwState = ::WNetCancelConnection2 (
				rRemoteName,
				CONNECT_UPDATE_PROFILE, // The system updates the user profile with the information
										// that the connection is no longer a persistent one.
				TRUE
				);
			dwState = ::WNetAddConnection2 (
				&stNetRes,
				rPassword,
				rAccount,
				CONNECT_UPDATE_PROFILE
				);
		}
		/*[1]--->>>Updated on 2008/04/28 SureKAM David Wong */

		strRemoteName.ReleaseBuffer();
	}
	return dwState;
}


CString CFFileOperator::CombinePath( const CString& rRootPath, const CString& rRelativePath )
{
	CString strFilePath;
	::PathCombine( strFilePath.GetBuffer( MAX_PATH ), rRootPath, rRelativePath );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::RemoveFileSpecPath( const CString& rFilePath )
{
	CString strFilePath( rFilePath );
	::PathRemoveFileSpec( strFilePath.GetBuffer( MAX_PATH ) );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::FindFileNamePath( const CString& rFilePath )
{
	CString strFilePath( ::PathFindFileName( rFilePath ) );
	return strFilePath;
}

CString CFFileOperator::AddExtensionPath( const CString& rFilePath, const CString& rExt )
{
	CString strFilePath( rFilePath );
	::PathAddExtension( strFilePath.GetBuffer( MAX_PATH ), rExt );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::RenameExtensionPath( const CString& rFilePath, const CString& rExt )
{
	CString strFilePath( rFilePath );
	::PathRenameExtension( strFilePath.GetBuffer( MAX_PATH ), rExt );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::AddBackslashPath( const CString& rDirPath )
{
	CString strDirPath( rDirPath );
	::PathAddBackslash( strDirPath.GetBuffer( MAX_PATH ) );
	strDirPath.ReleaseBuffer();
	return strDirPath;
}

CString CFFileOperator::RemoveBackslashPath( const CString& rDirPath )
{
	CString strDirPath( rDirPath );
	::PathRemoveBackslash( strDirPath.GetBuffer( MAX_PATH ) );
	strDirPath.ReleaseBuffer();
	return strDirPath;
}

CString CFFileOperator::MakePath( const CString& rDrive, const CString& rDir, const CString& rFileName, const CString& rExt )
{
	CString strFilePath;
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, rDrive, rDir, rFileName, rExt );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::StripToRootPath( const CString& rFilePath )
{
	CString strFilePath( rFilePath );
	::PathStripToRoot( strFilePath.GetBuffer( MAX_PATH ) );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::GetDrive( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, strDrive, NULL, NULL, NULL );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::GetDir( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, NULL, strDir, NULL, NULL );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::GetFileName( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, NULL, NULL, strFileName, NULL );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString	CFFileOperator::GetFileTitle( const CString& rFilePath )
{
	CString strFileName = GetFileName(rFilePath);

	return strFileName.Left(strFileName.Find(_T(".")));

}

CString CFFileOperator::GetExt( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, NULL, NULL, NULL, strExt );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::GetFileExtName( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();
	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, NULL, NULL, strFileName, strExt );
	strFilePath.ReleaseBuffer();
	return strFilePath;
}

CString CFFileOperator::GetDirectory( const CString& rFilePath )
{
	CString strFilePath;
	CString strDrive;
	CString strDir;
	CString strFileName;
	CString strExt;
	_tsplitpath_s( rFilePath,
		strDrive.GetBuffer( _MAX_DRIVE ), _MAX_DRIVE, strDir.GetBuffer( _MAX_DIR ), _MAX_DIR,
		strFileName.GetBuffer( _MAX_FNAME ), _MAX_FNAME, strExt.GetBuffer( _MAX_EXT ), _MAX_EXT );
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strExt.ReleaseBuffer();

	TCHAR	cPath[_MAX_PATH];
	_tcscpy_s(cPath, (strDrive+strDir));

	PathRemoveBackslash(cPath);

	strFilePath = cPath;
//	_tmakepath_s( strFilePath.GetBuffer( MAX_PATH ), MAX_PATH, strDrive, strDir, NULL, NULL );
//	strFilePath.ReleaseBuffer();
	return strFilePath;
}

bool CFFileOperator::RenameDirectories( const CString &cDelDir, const CString &cNewDirName )
{
	CString	strNewName = CombinePath( CFFileOperator::GetDirectory(cDelDir), cNewDirName );
	//
	CString	cTargetDir = cDelDir + _T('\0');
	strNewName += _T('\0');
	//
	SHFILEOPSTRUCT	sFileOperation;
	sFileOperation.hwnd						= NULL;
    sFileOperation.wFunc					= FO_RENAME;
    sFileOperation.pFrom					= cTargetDir;
    sFileOperation.pTo						= strNewName;
    sFileOperation.fFlags					= FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	sFileOperation.fAnyOperationsAborted	= FALSE;
    sFileOperation.hNameMappings			= NULL; 
    sFileOperation.lpszProgressTitle		= NULL; 

	return (0 == ::SHFileOperation( &sFileOperation ));
}

CFFileOperator::CFFileOperator()
{

}

CFFileOperator::~CFFileOperator()
{

}
