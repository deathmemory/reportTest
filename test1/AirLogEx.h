#pragma once

#include <vector>
//更新时易变的
#define MAX_ITEMS_NUM				(32+8-2)
#define	REOFFSET					0x16 
////
#define MAX_PLAYER_NUM				10
#define MAXPNUM_ONETEAM				5


#define FILTER_CHAR						0xC3

enum SWFTYPE
{
	SWF_UNKNOW,
	SWF_SEG				=				0x01,	// 分割符
	SWF_BOOLEAN			=				0x02,	//*
	SWF_INT				=				0x04,	//*
	SWF_DOUBLE			=				0x05,
	SWF_DOUBLE_SIZE		=				0X08,
	SWF_STR				=				0x06,
	SWF_RETURN			=				0x0A
};


#define SWF_ERR_STR						"(nil)"
#define SWF_ERR_INT						-1

#define BLOCK_SIZE						0x8F
#define BLOCK_STATRFLAG					"PlayerParticipantStatsSummary"
#define BLOCK_ENDOFGAMESTATS			"EndOfGameStats"
#define BLOCK_POINTSPENALTY				"PointsPenalty"
#define BLOCK_POINTSPENALTY_2			"endGame"
#define BLOCK_SOCIALNETWORKUSERIDS		"socialNetworkUserIds"
#define BLOCK_SERVERNAME_PENDINGCOUNTS	"pendingCounts"

#define CHAMPIONS_KILLED				"CHAMPIONS_KILLED"
#define NUM_DEATHS						"NUM_DEATHS"
#define ASSISTS							"ASSISTS"
#define MAGIC_DAMAGE_DEALT_PLAYER		"MAGIC_DAMAGE_DEALT_PLAYER"
#define PHYSICAL_DAMAGE_DEALT_PLAYER	"PHYSICAL_DAMAGE_DEALT_PLAYER"
#define TOTAL_DAMAGE_TAKEN				"TOTAL_DAMAGE_TAKEN"
#define LARGEST_MULTI_KILL				"LARGEST_MULTI_KILL"
#define MINIONS_KILLED					"MINIONS_KILLED"
#define NEUTRAL_MINIONS_KILLED			"NEUTRAL_MINIONS_KILLED"
#define GOLD_EARNED						"GOLD_EARNED"
#define WIN								"WIN"
#define LOSE							"LOSE"

typedef struct tagItems
{
	//int seg;
	int idx;
	double itemValue;
	char itemName[MAX_PATH];
}GAMEITEMS, *LPGAMEITEMS;
typedef struct tagPlayerParticipantStatsSummary
{
	char skinName[MAX_PATH];	//:String;
	double gameId;	//:Number;
	int _profileIconId;	//:int;	//头像ID
	bool leaver;	//:Boolean;
	double leaves;//:Number;	//逃跑
	double teamId;	//:Number;
	GAMEITEMS _gameItems[MAX_ITEMS_NUM];	//:ArrayCollection;
	double level;	//:Number;
	double userId;	//:Number;
	double spell2Id;	//:Number;
	double losses;	//:Number;
	char _summonerName[MAX_PATH];	//:String;
	double wins;	//:Number;
	double spell1Id;	//:Number;
	bool botPlayer_c;	//:Boolean;	//自己计算的, 是否是电脑

	//int elo;	//:int;
	//bool isMe;	//:Boolean = false;
	//bool inChat;	//:Boolean = false;
	//teamInfo:TeamInfo;
	//int eloChange;	//:int;
	//logger:ILogger;
	//bool reportEnabled;	//:Boolean = true;
	//_statistics:ArrayCollection;
	//_bindingEventDispatcher:EventDispatcher;
	//bool kudosEnabled;	//:Boolean = true;
	double getItemValue(char* itemName)
	{
		for ( int i = 0; i < MAX_ITEMS_NUM; ++ i )
			if ( memicmp(itemName, _gameItems[i].itemName, lstrlenA(itemName)) == 0 )
				return _gameItems[i].itemValue;
		return SWF_ERR_INT;
	}
}PPSTATSSUMMARY, *LPPPSTATSSUMMARY;
typedef struct tagPointsPenalty
{
	char type[MAX_PATH];	//:String;
	double penalty;	//:Number;
	int unKnowAddr; //private var _bindingEventDispatcher:EventDispatcher;
}POINTSPENALTY, *LPPOINTSPENALTY;
typedef struct tagEndOfGameStats
{
	double talentPointsGained;	//:Number;
	bool ranked;	//:Boolean;
	bool leveledUp;	//:Boolean;
	double skinIndex;	//:Number;
	double queueBonusEarned;	//:Number;
	char gameType[MAX_PATH];	//:String;
	double experienceEarned;	//:Number = 0;
	bool imbalancedTeamsNoPoints;	//:Boolean;	//队伍是否平衡
	tagPlayerParticipantStatsSummary teamPlayerParticipantStats[MAXPNUM_ONETEAM];	//_1093696278teamPlayerParticipantStats:ArrayCollection;
	double basePoints;	//:Number;
	double reportGameId_v;	//:Number;
	char difficulty[MAX_PATH];	//:String;
	double gameLength_v;	//:Number;
	double boostXpEarned;	//:Number;
	//bool invalid;	//:Boolean;
	//_1812690907otherTeamInfo:TeamInfo;
	char roomName[MAX_PATH];	//:String;
	//double customMinutesLeftToday;	//:Number;
	double userId_v;	//:Number;
	POINTSPENALTY PointsPenalty;	//_1303354692pointsPenalties:ArrayCollection;
	double coOpVsAiMinutesLeftToday;	//:Number;
	tagPlayerParticipantStatsSummary otherTeamPlayerParticipantStats[MAXPNUM_ONETEAM];	//_869788698otherTeamPlayerParticipantStats:ArrayCollection;
	double loyaltyBoostIpEarned;	//:Number = 0;
	double rpEarned;	//:Number;
	double completionBonusPoints;	//:Number;
	double coOpVsAiMsecsUntilReset;	//:Number;
	double boostIpEarned;	//:Number;
	//_599784949newSpells:ArrayCollection;
	double experienceTotal;	//:Number;
	double gameId5;	//:Number;
 	double timeUntilNextFirstWinBonus_v;	//:Number;	//下次首胜时间 value / 1000 = sec
 	double loyaltyBoostXpEarned;	//:Number = 0;
	char roomPassword[MAX_PATH];	//:String;
	int elo;	//:int;
	double ipEarned_v;	//:Number;	// 一共获取的金币
	double firstWinBonus_v;	// :Number;	首胜奖金
//	bool sendStatsToTournamentProvider;	//:Boolean = false;
	int eloChange;	//:int;
	char gameMode[MAX_PATH];	//:String;
	//_1208046985myTeamInfo:TeamInfo;
	char queueType[MAX_PATH];	//:String;
	double odinBonusIp;	//:Number;
	//_1000290437myTeamStatus:String;
	double ipTotal;	//:Number = 0;
	double customMsecsUntilReset;	//:Number;
	char clientId_Ex[MAX_PATH];	//:String;
	char destination_Ex[MAX_PATH];	//:String;
	//DSSubtopic_key_Ex;	//:seg;
	char DSSubtopic_value_Ex[MAX_PATH];	//:String; DSSubtopic_value = clientId_Ex
	char messageId_Ex[MAX_PATH];
	double timestamp_Ex_v;

// 	_bindingEventDispatcher:EventDispatcher;
// 	char skinName[MAX_PATH];	//:String;
// 	double locationBoostIpEarned;	//:Number = 0;
// 	double practiceMinutesPlayedToday;	//:Number;
// 	double expPointsToNextLevel;	//:Number = 0;
// 	double locationBoostXpEarned;	//:Number = 0;
} ENDOFGAMESTATS, *LPENDOFGAMESTATS;

typedef struct tagAirMemory
{
	int idx;
	SWFTYPE type;
	char* lpMmeory;
}AIRMEMORY, *LPAIRMEMORY;

class CAirLogEx
{
public:
	CAirLogEx(void);
	virtual ~CAirLogEx(void);
	ENDOFGAMESTATS EndofGameStats;
	std::vector<AIRMEMORY> m_airMemoryArr;
private:
	int m_sblockSize;
public:
	/*static */CHAR* CharFilter(CHAR* lpSrc, DWORD dwSrcSize, const CHAR ch, DWORD* pdwOutSize = NULL);
	char* connectRTMPPacks(CHAR* lpSrc, DWORD dwSrcSize, DWORD* pdwOutSize = NULL, const CHAR ch = 0xC3);
	/*static */VOID FreeFilterStr(CHAR* lpstr);
public:
	BOOL PasteMemory(CHAR* lpMemory, DWORD dwsize);	// 粘贴内存解析战果 
	double getSWF_double(CHAR* lpValAddr);
	int getSWF_int(CHAR* lpValAddr);
	char* getSWF_str(CHAR* lpValAddr);
	char* getSWF_strEx(CHAR* lpValAddr);	//带有补全过滤掉字符的函数
	std::vector<AIRMEMORY>::iterator getTypeIdx2(std::vector<AIRMEMORY>::iterator itr , DWORD dwSWFtype, DWORD dwCount);
	int getTypeIdx(std::vector<AIRMEMORY>::iterator itr , DWORD dwSWFtype, int startPos = 0);
	int getTypeIdx(const std::vector<char*> &vct , DWORD dwSWFtype, int startPos = 0);
	int getStrIdx(std::vector<AIRMEMORY>::iterator itr , CHAR* lpStr, int startPos = 0);
	void CutAllBlock(CHAR* lpMemory, DWORD dwsize);
	char* memstr(char* src, char* tag, int srcSize);
	void FreeAirMemory(__in const std::vector<AIRMEMORY>::iterator itr, __in int size);
protected:
	//CHAR* GetFirstSummBlock(LPPPSTATSSUMMARY lpPPSummary, CHAR* lpMemory, DWORD dwsize);	// 获取首个召唤师信息区块
	int GetFirstSummBlock(LPPPSTATSSUMMARY lpPPSummary/*, CHAR* lpMemory, DWORD dwsize*/);	// 获取首个召唤师信息区块
	int GetPointsPenaltyBlock(LPPOINTSPENALTY lpPointsPenalty);	// 获取PointsPenalty区块
	int GetNextSummBlock(LPPPSTATSSUMMARY lpPPSummary, int airIdx);	// 获取下个召唤师信息区块
	int GetEndOfGameBlock(LPENDOFGAMESTATS lpEndOfGameStats);	//最后获取 EndOfGameBlock
	//VOID GetSummonerBlock(__out LPPPSTATSSUMMARY lpPPSummary, __in const std::vector<char*> &blockCells );
	int GetSummonerBlock(__out LPPPSTATSSUMMARY lpPPSummary, __in const std::vector<AIRMEMORY>::iterator itr);
	VOID GetSummonerBlock(LPPPSTATSSUMMARY lpPPSummary, const std::vector<char*> &blockCells );
	VOID PrintfBlock(const std::vector<char*> &blockCells);
};
