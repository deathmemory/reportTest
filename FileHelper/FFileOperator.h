
#pragma once
#include <shellapi.h>
#define CSIDL_PROGRAM_FILES		0x0026	// "Program Files"


class CFFileOperator  
{
public:
	static bool	CreateEmptyFile( const CString& rFilePath );							
	static bool	DeleteFile( const CString& rFilePath );									
	static bool	CopyFile( const CString& rSrcFilePath, const CString& rDstFilePath );	
	static bool	MoveFile( const CString& rSrcFilePath, const CString& rDstFilePath );

	static bool	CopyFile_Confirm( const CString& rSrcFilePath, const CString& rDstFilePath );	
	static bool	MoveFile_Confirm( const CString& rSrcFilePath, const CString& rDstFilePath );	
	
	static bool	CreateDirectories( const CString& rDirPath );								
	static bool	DeleteDirectories( const CString& rDirPath );								
	static bool	CopyDirectories( const CString& rSrcDirPath, const CString& rDstDirPath );	
	static bool	MoveDirectories( const CString& rSrcDirPath, const CString& rDstDirPath );	


	static bool			IsFileExists( const CString& rFilePath );								
	static bool			IsDirectory( const CString& rDirPath );									
	static ULONGLONG	GetFileSize( const CString& rFilePath );								
	static DWORD		GetFileAttributes( const CString& rFilePath );							
	static bool			SetFileAttributes( const CString& rFilePath, DWORD dwFileAttributes );	
//	static CTime		GetFileLastModifiedTime( const CString& rFilePath );					

	
	static CString	GetCurrentExeDir();													
	static CString	GetModuleCurrentDir( const CString& rModuleName );					
	static CString	GetUniqueFilePath( bool bFolder, const CString& rExt );				
	static CString	GetSpecialFolderPath( int nFolder, const CString& rRelativePath );	

	
	static DWORD	CheckSharedFolderConnection( const CString& rRemoteName, const CString& rAccount, const CString& rPassword );	

	
	static CString	CombinePath( const CString& rRootPath, const CString& rRelativePath );	
	static CString	RemoveFileSpecPath( const CString& rFilePath );							
	static CString	FindFileNamePath( const CString& rFilePath );							
	static CString	AddExtensionPath( const CString& rFilePath, const CString& rExt );		
	static CString	RenameExtensionPath( const CString& rFilePath, const CString& rExt );	
	static CString	AddBackslashPath( const CString& rDirPath );							
	static CString	RemoveBackslashPath( const CString& rDirPath );							
	static CString	MakePath( const CString& rDrive, const CString& rDir, const CString& rFileName, const CString& rExt );	
	static CString	StripToRootPath( const CString& rFilePath );							

	
	static CString	GetDrive( const CString& rFilePath );		
	static CString	GetDir( const CString& rFilePath );			
	static CString	GetFileName( const CString& rFilePath );
	static CString	GetFileTitle( const CString& rFilePath );
	static CString	GetExt( const CString& rFilePath );			
	static CString	GetFileExtName(const CString& rFilePath );	
	static CString	GetDirectory(const CString& rFilePath );	
	static bool		RenameDirectories( const CString &cDelDir, const CString &cNewDirName );

private:
	CFFileOperator();
	virtual ~CFFileOperator();

};

//#endif // !defined(AFX_FFILEOPERATOR_H__A11F0E7C_0CF4_4AEB_B360_CA4232546C6F__INCLUDED_)
