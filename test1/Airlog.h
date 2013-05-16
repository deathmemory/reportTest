#pragma once

#include <windows.h>
#include "string.h"
typedef struct tagLogstruct
{
	int cnm;
	double logvalue;
	char logtype[MAX_PATH*2];
}Logstruct,*LPLogstruct;
#define OFFSET_XIANG				0x0a
#define RESAVE_DATA					5//2+3
#define MAX_PLAYERDATA				27+RESAVE_DATA
#define MAX_PLAYRCOUNT				10
#define MAX_OFFSETCOUNT				14
typedef struct tagUserLog
{
	double basePoints;
	CHAR heroname[MAX_PATH*2];
	double gameid;
	double unknow1;
	double teamid;
	Logstruct type[MAX_PLAYERDATA];
	double unknow2;
	double userid;
	double unknow3;
	CHAR summoner[MAX_PATH*2];
	BOOL Bot;
	double unknow4;
}UserLog,*LPUserLog;
typedef struct tagLogs
{
	CHAR GameType[MAX_PATH];
	double gamelen;
	int starttime;
	UserLog play[MAX_PLAYRCOUNT];
	CHAR hash[MAX_PATH];
	CHAR gameMode[MAX_PATH];
	CHAR queueType[MAX_PATH];
}Logs,*LPLogs;
class CAirLog
{
public:
	CAirLog(void);
	Logs LoLlog;
	LPUserLog ptrLogTemp;
	BOOL TeamScan(char *ptrLog);
	char* SummonerScan(CHAR *ptrTeam);
	char* DataScan(CHAR *ptrSummoner,LPUserLog ptrLog);
protected:
private:
};