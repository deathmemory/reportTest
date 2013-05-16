#include "StdAfx.h"
#include "Config.h"
//#include "BigFootMainFun.h"
#include "../SharedMemory/SharedMemroy.h"

#include <shlobj.h>

#define CONFIG_FILE_NAME				_T("Config\\Config.ini")
#define HEALTH_BAR_FILE_NAME		    _T("Config\\HbarConfig.ini")
#define CURSOR_FILE_NAME		        _T("Config\\CursorConfig.ini")
#define REC_UI_FILE_NAME    	        _T("Config\\UIConfig.ini")
#define SKIN_FILE_NAME    	            _T("Config\\HeroSkin.ini")
#define SYNC_FILE_NAME				    _T("SyncConfig.ini")
#define LOCAL_CONFIG				    _T("LocalInfo.ini")
#define MONSTERTIMER					_T("MonsterTimer")

static CString g_strBfCfgPath = _T("");
static CString g_strBfPath = _T("");

CString GetDocumentPath()
{
	TCHAR cPath[MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, cPath);
	CString strDocumentPath = cPath;
	strDocumentPath.Format(_T("%s\\BigFoot-LOL\\"),cPath);
	return strDocumentPath;
}

CConfig::CConfig(void)
{
}


CConfig::~CConfig(void)
{
}

 void CConfig::SetSummonerName(LPCTSTR lpszSummonerName)
 {
	WritePrivateProfileString(_T("Run"), _T("Summoner"), lpszSummonerName, CConfig::GetLocalCFGPath());
 }
 
 CString CConfig::GetSummonerName()
 {
	TCHAR tszName[MAX_PATH] = {0};
	DWORD dwLen = GetPrivateProfileString(_T("Run"), _T("Summoner"), _T(""), tszName, MAX_PATH, CConfig::GetLocalCFGPath());
	return CString(tszName);
 }
 CString CConfig::GetServerID()
 { 
	TCHAR tszName[MAX_PATH] = {0};
	DWORD dwLen = GetPrivateProfileString(_T("AirInfo"), _T("ServerID"), _T("BR"), tszName, MAX_PATH, CConfig::GetLocalCFGPath());
	return CString(tszName);
 }
 void CConfig::SetServerID(CString lpszSummonerName)
 {
	WritePrivateProfileString(_T("AirInfo"), _T("ServerID"), lpszSummonerName, CConfig::GetLocalCFGPath());
 }
 CString CConfig::GetCurrentServer()
 {
	 TCHAR tszServer[MAX_PATH] = {0};
	 DWORD dwLen = GetPrivateProfileString(_T("Run"), _T("Server"), _T("BR"), tszServer, MAX_PATH, CConfig::GetLocalCFGPath());
	 return CString(tszServer);
 }

 BOOL CConfig::GetOnceConfig(UINT ID)
 {
	 CString strKey;strKey.Format(_T("TIP%d"),ID);
	 return GetPrivateProfileInt(_T("ONCE"),strKey,0,GetLocalCFGPath());
 }

 void CConfig::SetCurrentServer(CString server)
 {
	 WritePrivateProfileString(_T("Run"), _T("Server"), server, CConfig::GetLocalCFGPath());
 }


BOOL CConfig::IsCloseToMini()
{
	//TCHAR tszEnable[2] = {0};
	//GetPrivateProfileString(_T("Misc"), _T("MinimizeOnClose"), _T("0"), tszEnable, 10, CConfig::GetConfigPath());
	//return tszEnable[0] == _T('0') ? FALSE : TRUE ;
	int nReturn = GetPrivateProfileInt(_T("Misc"), _T("MinimizeOnClose"), TRUE, CConfig::GetConfigPath());
	return nReturn;
}
void CConfig::EnableCloseToMini(BOOL bEnable)
{
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)bEnable + 0x30;
	WritePrivateProfileString(_T("Misc"), _T("MinimizeOnClose"), tszEnable, CConfig::GetConfigPath());
}


 void  CConfig::SetHealthbarIndex(int nIndex)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)nIndex + 0x30;
	 WritePrivateProfileString(_T("Color"), _T("nIndex"), tszEnable, CConfig::GetHealthbarCFGPath());
 }
 
 int CConfig::GetHealthbarIndex()
 {
	 int nReturn = GetPrivateProfileInt(_T("Color"), _T("nIndex"), 0, CConfig::GetHealthbarCFGPath());
	 return nReturn;
 }

CString CConfig::GetLocalCFGPath()
{
	CString strLocalPath;
	CSharedMemroy sm;
	strLocalPath = sm.GetClientPath();
	strLocalPath.TrimRight(_T("\\"));
	strLocalPath += _T("\\");
	strLocalPath += LOCAL_CONFIG;
	return strLocalPath;
}

 CString CConfig::GetHealthbarCFGPath()
 {
	 CString strSyncPath;
	 CSharedMemroy sm;
	 strSyncPath = sm.GetClientPath();
	 strSyncPath.TrimRight(_T("\\"));
	 strSyncPath += _T("\\");
	 strSyncPath += HEALTH_BAR_FILE_NAME;
	 return strSyncPath;
 }
 
 void CConfig::SetGamePath( LPCTSTR lpszGamePath )
 {
	 WritePrivateProfileString(_T("Run"), _T("GamePath"), lpszGamePath, CConfig::GetLocalCFGPath());
 }

 CString CConfig::GetGamePath()
 {
	 TCHAR tszPath[MAX_PATH] = {0};
	 DWORD dwLen = GetPrivateProfileString(_T("Run"), _T("GamePath"), _T(""), tszPath, MAX_PATH, CConfig::GetLocalCFGPath());
	 if (dwLen!= 0 && tszPath[dwLen-1] != _T('\\')&&dwLen<MAX_PATH)
		 tszPath[dwLen] = _T('\\');
 	return CString(tszPath);
 }
 
 void CConfig::SetScreenShotPath( LPCTSTR lpszScreenShotPath )
 {
	 WritePrivateProfileString(_T("ScreenShot"), _T("Path"), lpszScreenShotPath, CConfig::GetConfigPath());
 }
 
 CString  CConfig::GetScreenShotPath()
 {
	TCHAR tszPath[MAX_PATH] = {0};
	DWORD dwLen = GetPrivateProfileString(_T("ScreenShot"), _T("Path"),
		GetDocumentPath() + _T("ScreenShots"), tszPath, MAX_PATH, CConfig::GetConfigPath());
	CString strPath = tszPath;
	strPath.TrimRight(_T('\\'));
	strPath+=_T("\\");
	SHCreateDirectoryEx(NULL, tszPath, NULL);
	return strPath;
 }

 void CConfig::SetScreenShotType( int type )
 {
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)type + 0x30;
	WritePrivateProfileString(_T("ScreenShot"), _T("Type"), tszEnable, CConfig::GetConfigPath());
 }
 
 int CConfig::GetScreenShotType()
 {
	 int nReturn = GetPrivateProfileInt(_T("ScreenShot"), _T("Type"), 2, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableScreenShot(BOOL bEnable)
 {
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)bEnable + 0x30;
	WritePrivateProfileString(_T("ScreenShot"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsScreenShotEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("ScreenShot"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableAutoShot(BOOL bEnable)
 {
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)bEnable + 0x30;
	WritePrivateProfileString(_T("ScreenShot"), _T("AUTOEnabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsAutoShotEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("ScreenShot"), _T("AUTOEnabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }

 BOOL CConfig::IsChatMessageEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("ChatMessage"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;// &&CConfig::GetServerCfg(_T("ChatMessage"));
 }

 void CConfig::EnableChatMessage( BOOL bEnable )
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("ChatMessage"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 BOOL CConfig::IsBaseFuncEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("BaseFunc"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }

  void CConfig::EnableBaseFunc(BOOL bEnable)		//是否开启基础功能（超远视角、一键喊话、截图）
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("BaseFunc"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 BOOL CConfig::IsLolIFFuncEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("LolIFFunc"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }

 void CConfig::EnableLolIFFunc(BOOL bEnable)		//是否开启LOLInterface功能（略）
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("LolIFFunc"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 void CConfig::EnablePlayerStats( BOOL bEnable )
 {
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)bEnable + 0x30;
	WritePrivateProfileString(_T("PlayerStats"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsPlayerStatsEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("PlayerStats"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 BOOL CConfig::IsSPanelEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("SPanel"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }

 void CConfig::EnableSPanel(BOOL bEnable)		//是否开启水晶计时面板
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("SPanel"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

  BOOL CConfig::IsAttackFlashEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(_T("AttackFlash"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
  void CConfig::EnableAttackFlash(BOOL bEnable)	//敌方闪耀
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("AttackFlash"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 BOOL CConfig::IsReplayEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("Replay"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 CString CConfig::GetReplayPath()
 {
	 TCHAR tszPath[MAX_PATH] = {0};
	 DWORD dwLen = GetPrivateProfileString(_T("Replay"), _T("Path"),
		 GetDocumentPath() + _T("Replays"), tszPath, MAX_PATH, CConfig::GetConfigPath());
	 if (dwLen!= 0 && tszPath[dwLen-1] != _T('\\'))
		 tszPath[dwLen-1] = _T('\\');
	 SHCreateDirectoryEx(NULL, tszPath, NULL);
	 return CString(tszPath);
 }
 
 void CConfig::EnableReplay( BOOL bEnable )
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("Replay"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 void CConfig::SetReplayPath( LPCTSTR lpszReplayPath )
 {
 	WritePrivateProfileString(_T("Replay"), _T("Path"), lpszReplayPath, CConfig::GetConfigPath());
 }

 CString CConfig::GetLocalDir()
 {
	 CString strBFPath ;
	 CSharedMemroy sm;
	 strBFPath = sm.GetClientPath();
	 if (!strBFPath.IsEmpty())
	 { 
		 strBFPath.TrimRight(_T("\\"));
		 strBFPath += _T("\\");
		 g_strBfPath = strBFPath;
	 }
 	 return g_strBfPath;
 }

 CString CConfig::GetConfigPath()
 {
	 if (_T("") == g_strBfCfgPath)
	 {
		 CSharedMemroy sm;
		 g_strBfCfgPath = sm.GetClientPath();
		 g_strBfCfgPath.TrimRight(_T("\\"));
		 g_strBfCfgPath += _T("\\");
		 g_strBfCfgPath += CONFIG_FILE_NAME;
	 }
	 CString strCfg = g_strBfCfgPath.Left(g_strBfCfgPath.ReverseFind('\\'));
	// DWORD s = GetFileAttributes(strCfg);
	 if (GetFileAttributes(strCfg)==-1)
	 {
		 CreateDirectory(strCfg,NULL);
	 }
	 return g_strBfCfgPath;
 }
 
 BOOL CConfig::IfWindows7()
 {
 	OSVERSIONINFO osvi;
 	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
 	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
 	GetVersionEx(&osvi);
 	if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 1)
 	{
 		return TRUE;
 	}
 	return FALSE;
 }
 
 BOOL CConfig::IsInfiniteRangeEnabled()	//是否开启无限视距
 {
	 int nReturn = GetPrivateProfileInt(_T("InfiniteRange"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;//&&CConfig::GetServerCfg(_T("InfiniteRange"));
 }

 void CConfig::EnableInfiniteRange(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("InfiniteRange"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsMonsterTimerEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(MONSTERTIMER, _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;//&&CConfig::GetServerCfg(_T("TimerBar"));
 }

 void CConfig::EnableMonsterTimer(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(MONSTERTIMER, _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 BOOL CConfig::IsTimerBarNoticeEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(MONSTERTIMER, _T("Notice"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableTimerBarNotice(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(MONSTERTIMER, _T("Notice"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsTimerMiniMapFlashBarEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(MONSTERTIMER, _T("MiniMapFlash"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableTimerBarMiniMapFlash(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(MONSTERTIMER, _T("MiniMapFlash"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsTimerBarAutoNoticeEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(MONSTERTIMER, _T("AutoNotice"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableTimerBarAutoNotice(BOOL bEnable)	//自动小队通知
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(MONSTERTIMER, _T("AutoNotice"), tszEnable, CConfig::GetConfigPath());
 }
 
 void CConfig::SetColorWeaknessMod(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("ColorWeakness"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 BOOL CConfig::IsColorWeaknessMod()
 {
	 int nReturn = GetPrivateProfileInt(_T("ColorWeakness"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void CConfig::EnableNoNextTime(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("NoConfigOut"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsNoNextTimeEnbaled()
 {
	 int nReturn = GetPrivateProfileInt(_T("NoConfigOut"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 
 BOOL CConfig::IsFirstRun() //第一次运行
 {
	 int nReturn = GetPrivateProfileInt(_T("Run"), _T("First"), TRUE, CConfig::GetLocalCFGPath());
	 return nReturn;
 }
 
 void CConfig::SetFirstRun(BOOL bFirst)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bFirst + 0x30;
	 WritePrivateProfileString(_T("Run"), _T("First"), tszEnable, CConfig::GetLocalCFGPath());
 }
 
 BOOL CConfig::IsWardStateEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("WardStat"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;//&&CConfig::GetServerCfg(_T("WardStat"));
 }

 void CConfig::EnbleWardState(BOOL bEnable)		//是否开启插眼状态辅助
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("WardStat"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

 void  CConfig::EnableNumOfWard(BOOL bEnable)		//是否开启插眼状态辅助
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("EyeStatics"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
	 //theApp.WriteProfileInt(_T("EyeStatics"), _T("Enabled"), bEnable);
 }
 BOOL  CConfig::IsNumOfWardEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("EyeStatics"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
	// return theApp.GetProfileInt(_T("EyeStatics"), _T("Enabled"), TRUE) ? TRUE : false;
 }

 
 BOOL  CConfig::IsAutoLogin()
 {
	 int nReturn = GetPrivateProfileInt(_T("run"), _T("Auto"), TRUE, CConfig::GetConfigPath());
	 return nReturn;
 }
 
 void  CConfig::EnableAutoLogin(BOOL bEnable)
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("run"), _T("Auto"), tszEnable, CConfig::GetConfigPath());
 }
 
 BOOL CConfig::IsCrystalTimerEnabled()
 {
	 int nReturn = GetPrivateProfileInt(_T("Crystal"), _T("Enabled"), TRUE, CConfig::GetConfigPath());
	 return nReturn;//&&CConfig::GetServerCfg(_T("Crystal"));
 }

 void CConfig::EnableCrystalTimer(BOOL bEnable)		//是否开启水晶计时
 {
	TCHAR tszEnable[2] = {0};
	tszEnable[0] = (int)bEnable + 0x30;
	WritePrivateProfileString(_T("Crystal"), _T("Enabled"), tszEnable, CConfig::GetConfigPath());
 }

CString CConfig::GetSyncCFGPath()
{
	CString strSyncPath;
	CSharedMemroy sm;
	strSyncPath = sm.GetClientPath();
	strSyncPath.TrimRight(_T("\\"));
	strSyncPath += _T("\\");
	strSyncPath += SYNC_FILE_NAME;
	return strSyncPath;
}

 void CConfig::GetSyncConfigList(SYNC_CONFIG &cSync)
 {
	 cSync.bRec_Item   = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Rec_Item"),   TRUE, CConfig::GetSyncCFGPath());
	 cSync.bHero_Skin  = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Hero_Skin"),  TRUE, CConfig::GetSyncCFGPath());
	 cSync.bChat_MSG   = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Chat_MSG"),   TRUE, CConfig::GetSyncCFGPath());
	 cSync.bFun_Config = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Fun_Config"), TRUE, CConfig::GetSyncCFGPath());
	 cSync.bGame_Font  = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Game_Font"),  TRUE, CConfig::GetSyncCFGPath());
	 cSync.bCursor     = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Cursor"),     TRUE, CConfig::GetSyncCFGPath());
	 cSync.bRec_UI     = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("Rec_UI"),     TRUE, CConfig::GetSyncCFGPath());
	 cSync.bHealthBar  = GetPrivateProfileInt(_T("SYNC_CONFIG"), _T("HealthBar"),  TRUE, CConfig::GetSyncCFGPath());
 }

 void CConfig::SetSyncConfigList(SYNC_CONFIG cSync)
 {
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Rec_Item"),   cSync.bRec_Item?_T("1"):_T("0"),   CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Hero_Skin"),  cSync.bHero_Skin?_T("1"):_T("0"),  CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Chat_MSG"),   cSync.bChat_MSG?_T("1"):_T("0"),   CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Fun_Config"), cSync.bFun_Config?_T("1"):_T("0"), CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Game_Font"),  cSync.bGame_Font?_T("1"):_T("0"),  CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Cursor"),     cSync.bCursor?_T("1"):_T("0"),     CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("Rec_UI"),     cSync.bRec_UI?_T("1"):_T("0"),     CConfig::GetSyncCFGPath());
	 WritePrivateProfileString(_T("SYNC_CONFIG"), _T("HealthBar"),  cSync.bHealthBar?_T("1"):_T("0"),  CConfig::GetSyncCFGPath());
 }

 BOOL CConfig::IsMTBarEnbaled()
 {
	 return GetPrivateProfileInt(_T("TimerBar"), _T("MTBar"), TRUE,CConfig::GetConfigPath());
 }
 void CConfig::EnableMTBar(BOOL bEnable)		//计时条是否开启
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("TimerBar"), _T("MTBar"), tszEnable, CConfig::GetConfigPath());
 }
 BOOL CConfig::IsMTNumWndEnbaled()
 {
	 return GetPrivateProfileInt(_T("TimerBar"), _T("MTNumWnd"), TRUE, CConfig::GetConfigPath());
 }
 void CConfig::EnableMTNumWnd(BOOL bEnable)	//小地图显示是否开启
 {
	 TCHAR tszEnable[2] = {0};
	 tszEnable[0] = (int)bEnable + 0x30;
	 WritePrivateProfileString(_T("TimerBar"), _T("MTNumWnd"), tszEnable, CConfig::GetConfigPath());
 }

 BOOL CConfig::IsQuitComfirm()
 {
	 return GetPrivateProfileInt(_T("Misc"), _T("ConfirmQuit"), TRUE,CConfig::GetConfigPath());
 }

 void CConfig::EnableQuitComfirm(BOOL bEnable)
 {
	 WritePrivateProfileString(_T("Misc"), _T("ConfirmQuit"), bEnable?_T("1"):_T("0"), CConfig::GetConfigPath());
 }

 BOOL CConfig::GetServerCfg(CString cfg)
 {
	 return GetPrivateProfileInt(_T("Server"), cfg, TRUE,CConfig::GetLocalDir() + _T("ServerCFG.ini"));
 }

 void CConfig::SetServerCfg(CString cfg, BOOL bEnable)	//服务器功能开关
 {
	 WritePrivateProfileString(_T("Server"), cfg, bEnable?_T("1"):_T("0"), CConfig::GetLocalDir() + _T("ServerCFG.ini"));
 }


 int CConfig::GetCurrentCursor()
 {
	 int nReturn = GetPrivateProfileInt(_T("Cursor"), _T("nIndex"), 0, CConfig::GetCursorCFGPath());
	 return nReturn;	
 }
 void CConfig::SetCurrentCursor(int nIndex)	//当前鼠标样式编号
 {
	 CString sValue;sValue.Format(_T("%d"),nIndex);
	 WritePrivateProfileString(_T("Cursor"), _T("nIndex"), sValue, CConfig::GetCursorCFGPath());
 }

  CString CConfig::GetCursorCFGPath()
  {
	  CString strSyncPath;
	  CSharedMemroy sm;
	  strSyncPath = sm.GetClientPath();
	  strSyncPath.TrimRight(_T("\\"));
	  strSyncPath += _T("\\");
	  strSyncPath += CURSOR_FILE_NAME;
	  return strSyncPath;
  }

  BOOL CConfig::GetRec_UICfg(REC_UI &cfg)
  {
	  USES_CONVERSION;
	  if (cfg.sFileName!=_T("")&&cfg.sGroup!=_T(""))
	  {
		  TCHAR sValue[20] = {0};
		  GetPrivateProfileString(cfg.sFileName, cfg.sGroup,_T(""),sValue,20,CConfig::GetRec_UICFGPath());
		  char*sv= T2A(sValue);
		  sscanf(sv,"%d,%d",&cfg.xOffSet,&cfg.yOffSet);
		  return TRUE;
	  }
	   return FALSE;
  }
  void CConfig::SetRec_UICfg(REC_UI cfg)
  {
	 CString sValue;sValue.Format(_T("%d,%d"),cfg.xOffSet,cfg.yOffSet);
	 WritePrivateProfileString(cfg.sFileName, cfg.sGroup, sValue, CConfig::GetRec_UICFGPath());
  }

  void CConfig::SetRec_UISkin(CString sValue)
  {
	//  CString sValue;sValue.Format(_T("%d"),nIndex);
	  WritePrivateProfileString(_T("UI_SKIN"), _T("Name"), sValue, CConfig::GetRec_UICFGPath());
  }
  CString CConfig::GetRec_UISkin()
  {
	  TCHAR res[30] = {0};
	  GetPrivateProfileString(_T("UI_SKIN"), _T("Name"), _T("Default"),res,30,CConfig::GetRec_UICFGPath());
	  return CString(res);
  }

  CString CConfig::GetRec_UICFGPath()
  {
	  CString strSyncPath;
	  CSharedMemroy sm;
	  strSyncPath = sm.GetClientPath();
	  strSyncPath.TrimRight(_T("\\"));
	  strSyncPath += _T("\\");
	  strSyncPath += REC_UI_FILE_NAME;
	  return strSyncPath;
  }

  void CConfig::SetSelectChampion(LPCTSTR lpszChampion)
  {
	  WritePrivateProfileString(_T("SELECT_STAT"), _T("CHAMPION"), lpszChampion, CConfig::GetLocalCFGPath());
  }

  CString CConfig::GetSelectChampion()
  {
	  TCHAR res[30] = {0};
	  GetPrivateProfileString(_T("SELECT_STAT"), _T("CHAMPION"), _T("NULL"),res,30,CConfig::GetLocalCFGPath());
	  return CString(res);
  }

  int CConfig::GetSkinIndex(CString strHeroName)
  {
		int nIndex = GetPrivateProfileInt(_T("Skin"), strHeroName, 0,GetSkinCFGPath());
		return nIndex;
  }
  void CConfig::SetSkinIndex(CString strHeroName,int nIndex)
  {
	  CString sValue;sValue.Format(_T("%d"),nIndex);
	  WritePrivateProfileString(_T("Skin"), strHeroName, sValue, CConfig::GetSkinCFGPath());
  }

  CString CConfig::GetSkinCFGPath()
  {
	  CString strSyncPath;
	  CSharedMemroy sm;
	  strSyncPath = sm.GetClientPath();
	  strSyncPath.TrimRight(_T("\\"));
	  strSyncPath += _T("\\");
	  strSyncPath += SKIN_FILE_NAME;
	  return strSyncPath;
  }

  BOOL CConfig::IsShowSkinOnlyMe()
  {
	  return GetPrivateProfileInt(_T("ShowSkin"), _T("ShowSkinOnlyMe"), TRUE,CConfig::GetConfigPath());
  }

  void CConfig::EnableShowSkinOnlyMe( BOOL bEnable )
  {
	  TCHAR tszEnable[2] = {0};
	  tszEnable[0] = (int)bEnable + 0x30;
	  WritePrivateProfileString(_T("ShowSkin"), _T("ShowSkinOnlyMe"), tszEnable, CConfig::GetConfigPath());
  }

  static BOOL g_bVisible = TRUE;
  BOOL CALLBACK GetCurrentWndProc(
	  HWND hwnd,
	  LPARAM lParam
	  )
  {
	  BOOL  Result=TRUE;
	  DWORD dwPid = 0;
	  if (GetParent(hwnd) == NULL && IsWindowVisible(hwnd) == g_bVisible)
	  {
		  GetWindowThreadProcessId(hwnd, &dwPid);
		  if (GetCurrentProcessId() == dwPid)
		  {
			  *(HWND*)lParam = hwnd;
			  Result = FALSE;
		  }
	  }
	  return Result;
  }
  HWND CConfig::GetCurrentWindow(BOOL bVisible /*= TRUE*/)
  {
	  HWND hWnd = NULL;
	  g_bVisible = bVisible;
	  EnumWindows(GetCurrentWndProc, (LPARAM)&hWnd);
	  return hWnd;
  }

  void CConfig::EnablePraiseNotice(BOOL bEnable)
  {
	  TCHAR tszEnable[2] = {0};
	  tszEnable[0] = (int)bEnable + 0x30;
	  WritePrivateProfileString(_T("PraiseNotice"), _T("Enable"), tszEnable, CConfig::GetConfigPath());
  }
  BOOL CConfig::IsPraiseNoticeEnbaled()
  {
	  return GetPrivateProfileInt(_T("PraiseNotice"), _T("Enable"), TRUE, CConfig::GetConfigPath());
  }
  void CConfig::EnablePowerShow(BOOL bEnable)
  {
	  TCHAR tszEnable[2] = {0};
	  tszEnable[0] = (int)bEnable + 0x30;
	  WritePrivateProfileString(_T("PowerShow"), _T("Enable"), tszEnable, CConfig::GetConfigPath());
  }
  BOOL CConfig::IsPowerShowEnbaled()
  {
	  return GetPrivateProfileInt(_T("PowerShow"), _T("Enable"), TRUE, CConfig::GetConfigPath());
  }

	
  void CConfig::EnableLoadingPowerShow(BOOL bEnable)
  {
	  TCHAR tszEnable[2] = {0};
	  tszEnable[0] = (int)bEnable + 0x30;
	  WritePrivateProfileString(_T("PowerShow"), _T("Enable"), tszEnable, CConfig::GetConfigPath());
  }
  BOOL CConfig::IsLoadingPowerShow()
  {
	  return GetPrivateProfileInt(_T("PowerShow"), _T("Enable"), FALSE, CConfig::GetConfigPath());
  }
