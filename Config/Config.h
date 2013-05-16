#pragma once
#include "ColorConfig.h"

#define BF_BUG_COMMIT				_T("BF_Bug_Commit")

typedef struct tagSyncConfigList
{
	BOOL bRec_Item;
	BOOL bHero_Skin;
	BOOL bChat_MSG;
	BOOL bFun_Config;
	BOOL bGame_Font;
	BOOL bCursor;
	BOOL bRec_UI;
	BOOL bHealthBar;
}SYNC_CONFIG;

typedef struct ServerConfig
{
	BOOL bTimerBar;
	BOOL bChatMsg;
	BOOL bCrystalTimer;
	BOOL bEyeTimer;
	BOOL bMFalsh;

	BOOL bNotice;
	CString strNotice;
	CString strVer;
	CString DeployUrl;
	ULONGLONG nDeploySize;
	ServerConfig()
	{
		bTimerBar = TRUE;
		bChatMsg = TRUE;
		bCrystalTimer = TRUE;
		bEyeTimer = TRUE;
		bMFalsh = TRUE;
		bNotice = FALSE;
	}
}SERVER_CFG;

typedef struct tagRec_UI
{
	CString sGroup;
	int     xOffSet;
	int     yOffSet;
	CString sFileName;
}REC_UI;

class CConfig
{
public:
	static HWND GetCurrentWindow(BOOL bVisible = TRUE);

	static CString GetConfigPath();  //获得配置文件路径

	static void SetSelectChampion(LPCTSTR lpszChampion);
	static CString GetSelectChampion();

	static void SetSummonerName(LPCTSTR lpszGamePath);
	static CString GetSummonerName();

	static CString GetCurrentServer();
	static void SetCurrentServer(CString);

	static CString GetServerID();
	static void SetServerID(CString);

	static CString GetLocalCFGPath();
	static void SetGamePath(LPCTSTR lpszGamePath);
	static CString GetGamePath();

	static void SetHealthbarIndex(int nIndex);
	static int  GetHealthbarIndex();
	static CString GetHealthbarCFGPath();

	static void SetScreenShotPath(LPCTSTR lpszScreenShotPath);
	static CString GetScreenShotPath();

	static void SetScreenShotType(int type);
	static int GetScreenShotType();

	static void EnableScreenShot(BOOL bEnable);
	static BOOL IsScreenShotEnabled();

	static void EnableAutoShot(BOOL bEnable);
	static BOOL IsAutoShotEnabled();

	static void EnableChatMessage(BOOL bEnable);
	static BOOL IsChatMessageEnabled();

	static void EnablePlayerStats(BOOL bEnable);
	static BOOL IsPlayerStatsEnabled();

	static void EnableInfiniteRange(BOOL bEnable);		//是否开启无限视距
	static BOOL IsInfiniteRangeEnabled();	

	static void EnbleWardState(BOOL bEnable);		//是否开启插眼状态辅助
	static BOOL IsWardStateEnabled();

	static void EnableCrystalTimer(BOOL bEnable);		//是否开启水晶计时
	static BOOL IsCrystalTimerEnabled();

	static BOOL IsMonsterTimerEnbaled();				//是否开启打野计时
	static void EnableMonsterTimer(BOOL bEnable);

	static BOOL IsMTBarEnbaled();
	static void EnableMTBar(BOOL bEnable);		//计时条是否开启

	static BOOL IsMTNumWndEnbaled();
	static void EnableMTNumWnd(BOOL bEnable);	//小地图显示是否开启

	static BOOL IsTimerBarNoticeEnbaled();
	static void EnableTimerBarNotice(BOOL bEnable);	//屏幕中间提醒

	static BOOL IsTimerMiniMapFlashBarEnbaled();
	static void EnableTimerBarMiniMapFlash(BOOL bEnable);	//小地图闪烁

	static BOOL IsTimerBarAutoNoticeEnbaled();
	static void EnableTimerBarAutoNotice(BOOL bEnable);	//自动小队通知

	static BOOL IsNumOfWardEnabled();
	static void EnableNumOfWard(BOOL bEnable);		//是否开启插眼统计
	
	static BOOL IsSPanelEnabled();
	static void EnableSPanel(BOOL bEnable);		//是否开启水晶计时面板

	static BOOL IsAttackFlashEnbaled();
	static void EnableAttackFlash(BOOL bEnable);	//敌方闪耀

	static BOOL IsBaseFuncEnabled();
	static void EnableBaseFunc(BOOL bEnable);		//是否开启基础功能（超远视角、一键喊话、截图）

	static BOOL IsLolIFFuncEnabled();
	static void EnableLolIFFunc(BOOL bEnable);		//是否开启LOLInterface功能（略）

	static void EnableReplay(BOOL bEnable);
	static BOOL IsReplayEnabled();

	static CString GetReplayPath();
	static void SetReplayPath(LPCTSTR lpszReplayPath);

	static CString GetLocalDir();
	static BOOL IfWindows7();	//是否为Win7系统

	static void SetColorWeaknessMod(BOOL bEnble);
	static BOOL IsColorWeaknessMod();

	static void EnableNoNextTime(BOOL bEnable);
	static BOOL IsNoNextTimeEnbaled();

	static BOOL IsFirstRun();
	static void SetFirstRun(BOOL bFirst);

	static BOOL IsAutoLogin();
	static void EnableAutoLogin(BOOL bEnable);

    static BOOL IsCloseToMini();
	static void EnableCloseToMini(BOOL bEnable);

	static void GetSyncConfigList(SYNC_CONFIG &cSync);
	static void SetSyncConfigList(SYNC_CONFIG cSync);

	static CString GetSyncCFGPath();

	static BOOL IsQuitComfirm();
	static void EnableQuitComfirm(BOOL bEnable);	//退出提示


	static BOOL GetServerCfg(CString cfg);
	static void SetServerCfg(CString cfg, BOOL bEnable);	//服务器功能开关

	static int GetCurrentCursor();
	static void SetCurrentCursor(int);	//当前鼠标样式编号
	static CString GetCursorCFGPath();

	static BOOL GetRec_UICfg(REC_UI &cfg);
	static void SetRec_UICfg(REC_UI cfg);
	static void SetRec_UISkin(CString );
	static CString GetRec_UISkin( );
	static CString GetRec_UICFGPath();

	static int GetSkinIndex(CString strHeroName);
	static void SetSkinIndex(CString strHeroName,int nIndex);

	static BOOL IsShowSkinOnlyMe();
	static void EnableShowSkinOnlyMe(BOOL bEnable);		//是否只显示自己修改的皮肤

	static CString GetSkinCFGPath();

	static BOOL GetOnceConfig(UINT ID);

	static void EnablePraiseNotice(BOOL bEnable);
	static BOOL IsPraiseNoticeEnbaled();

	static void EnablePowerShow(BOOL bEnable);
	static BOOL IsPowerShowEnbaled();

	static void EnableLoadingPowerShow(BOOL bEnable);
	static BOOL IsLoadingPowerShow();

private:
	CConfig(void);
	~CConfig(void);
};

