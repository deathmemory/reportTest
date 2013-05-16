#include "Airlog.h"
#include <time.h>
#define MAX_KEY_NUM				31+RESAVE_DATA
#define AIR_STAR_OFFSET 0x314 //日志内起始偏移
#define AIR_MODE_OFFSET 0x59  //日志gamemode偏移
#define AIR_FILL    0xc3  //air内填充的字节
#define AIR_VALUE   0x05  //value的开始标志符
#define AIR_VALUE_1 0x04  //value的开始标志
#define AIR_VALUE_2 0x01  //value的开始标志
#define AIR_TYPE    0x06  //type的开始标志符
#define AIR_NEXT_TEAM  0x0d0a  //队伍分隔符
#define TEAM_PLAYER1      0x00  //上面队伍第一个玩家的英雄名
#define TEAM_PLAYER2      (0x42+OFFSET_XIANG)//上面队伍第二个玩家的英雄名
#define TEAM_PLAYER3	  (0x46+OFFSET_XIANG)//上面队伍第三个玩家的英雄名
#define TEAM_PLAYER4	  (0x4a+OFFSET_XIANG)//上面队伍第四个玩家的英雄名
#define TEAM_PLAYER5	  (0x4e+OFFSET_XIANG)//上面队伍第五个玩家的英雄名
#define AIR_FILL_NEXT_DOUBLE 0x81//下一个c3出现的偏移
#define LOG_LOGTYPE1    0x4957//日志详细数据的分割特征标志
#define LOG_LOGTYPE2    0x4f4c//日志详细数据的分割特征标志
#define HERO_NAME_TYPE	0x0d0a//玩家英雄名的开始标志
#define HERO_NAME_TYPE_FILL_1 0xc30a//带有c3的标志
#define HERO_NAME_TYPE_FILL_2 0x0dc3
BOOL IsFirstUser = TRUE;
CHAR* ScanForData(char* ptr)
{
	if(IsBadReadPtr(ptr,1))
		return 0;
	while((BYTE)*ptr !=(BYTE)AIR_VALUE)
	{
		ptr++;
		if(IsBadReadPtr(ptr,1))
			return 0;
	}
	if ((BYTE)*ptr == (BYTE)AIR_VALUE)
	{
		return ptr;
	}
	else
		return 0;
}
CHAR* GetValueForDouble(char* ptr,double* datavalue)
{
	//CHAR* temp = ptr;
	if(IsBadReadPtr(ptr,1))
		return 0;
	CHAR* tempvalue = (CHAR*)datavalue;
	int i = 0;
	if ((BYTE)*ptr == (BYTE)AIR_FILL)
	{
		ptr++;
	}
	ptr++;
	if ((BYTE)*ptr == (BYTE)AIR_FILL)
	{
		ptr++;
	}
	//ptr++;
	while((BYTE)*ptr != (BYTE)AIR_TYPE )
	{

		i++;
		ptr++;
		while((BYTE)*ptr ==0)
		{
			i++;
			ptr++;
			if(IsBadReadPtr(ptr,1))
				return 0;
			if (i > MAX_OFFSETCOUNT)
			{
				return 0;
			}
		}
		if ((BYTE)*ptr ==AIR_TYPE && i<6)
		{
			i++;
			ptr++;
		}
		if (i>7)
		{
			break;
		}
		if(IsBadReadPtr(ptr,1))
			return 0;
	}
	CHAR* temp = ptr;
	//temp--;
	tempvalue += 7;
	temp -= i;
	while(i)
	{

		//if ((BYTE)*temp == (BYTE)AIR_FILL)
		if ((BYTE)*(temp+AIR_FILL_NEXT_DOUBLE) == (BYTE)AIR_FILL && (BYTE)*temp == (BYTE)AIR_FILL)
		{
			temp++;
			i--;
			continue;
		}
		if ((BYTE)*temp == (BYTE)0x00)
		{
			temp++;
			i--;
			continue;
		}
		*tempvalue = *temp;
		tempvalue--;
		temp++;
		i--;
		if(IsBadReadPtr(temp,1))
			return 0;
	}
	return ptr;
}
CHAR* GetGameLen(CHAR *ptr,double *gamelen)
{
	if(IsBadReadPtr(ptr,1))
		return 0;
	int i = 0;
	while (i<3)
	{

		if ((BYTE)*ptr == AIR_VALUE)
		{
			i++;
		}
		if (i == 3)
		{
			break;
		}
		ptr++;
		if(IsBadReadPtr(ptr,1))
			return 0;
	}
	if ((BYTE)*ptr == AIR_VALUE)
	{
		ptr = GetValueForDouble(ptr,gamelen);
	}
	else
		return 0;
	return ptr;
}
CHAR* GetDateForStr(CHAR *ptr,CHAR *str)
{
	if(IsBadReadPtr(ptr,1))
		return 0;
	//CHAR *temp = ptr;
	if ((BYTE)*ptr == (BYTE)AIR_FILL)
	{
		ptr++;
	}
	ptr++;
	if ((BYTE)*ptr == (BYTE)AIR_FILL)
	{
		ptr++;
	}
	ptr++;
	while((BYTE)*ptr != (BYTE)AIR_VALUE_2 && (BYTE)*ptr != (BYTE)AIR_VALUE_1 && (BYTE)*ptr != (BYTE)AIR_VALUE)
	{
		if ((BYTE)*ptr == (BYTE)AIR_FILL)
		{
			ptr++;
			continue;
		}
		*str = *ptr;
		str++;
		ptr++;
		if(IsBadReadPtr(ptr,1))
			return 0;
	}
	return ptr;
}
char* GetDateForType(CHAR *ptr,CHAR *str)
{
	if(IsBadReadPtr(ptr,1))
		return 0;
	//CHAR *temp = ptr;
	while((BYTE)*ptr != (BYTE)AIR_VALUE_2 && (BYTE)*ptr != (BYTE)AIR_VALUE_1 && (BYTE)*ptr != (BYTE)AIR_VALUE)
	{
		if ((BYTE)*ptr == (BYTE)(BYTE)AIR_FILL)
		{
			ptr++;
			continue;
		}
		*str = *ptr;
		str++;
		ptr++;
		if(IsBadReadPtr(ptr,1))
			return 0;
	}
	return ptr;
}
CHAR* GetNextTeam(CHAR* ptrTeam)
{
	if(IsBadReadPtr(ptrTeam,1))
		return 0;
	CHAR* temp = ptrTeam;
	int i = 0;
	int x =0;
	while(*(WORD*)temp != AIR_NEXT_TEAM && (*(WORD*)(temp+1) != HERO_NAME_TYPE_FILL_2 || *(WORD*)temp != HERO_NAME_TYPE_FILL_1))
	{
		i++;
		temp++;
		if(IsBadReadPtr(temp,1))
			return 0;
		/*if (*temp == AIR_TYPE)
		{
			if (*(WORD*)temp == 0x1706)
			{
				temp++;
				x-=1;
			}
			if (x >1)
			{
				break;
			}
			x++;
		}
		if (i>(BYTE)0xf0)
		{
			return 0;
		}*/
	}
	while((BYTE)*temp != (BYTE)AIR_VALUE)
	{
		temp--;
		if(IsBadReadPtr(temp,1))
			return 0;
	}
	return temp;
}
BOOL CAirLog::TeamScan(char *ptrLog)
{
	int i = 0;
	CHAR *ptrTemp = ptrLog;
	ZeroMemory((void*)&LoLlog,sizeof(Logs));
	ptrTemp +=AIR_STAR_OFFSET;
	char *strtemp = LoLlog.GameType;
	//ZeroMemory(LoLlog.GameType,MAX_PATH);
	CHAR* ptrTempForType = GetDateForType(ptrTemp,strtemp);
	if (IsBadReadPtr(ptrTempForType,4))
	{
		return FALSE;
	}
	IsFirstUser = TRUE;
	CHAR *ptrForlog = SummonerScan(ptrTempForType);
	if (IsBadReadPtr(ptrForlog,4))
	{
		return FALSE;
	}
	ptrForlog += (BYTE)AIR_MODE_OFFSET;
	strtemp = LoLlog.hash;
	ptrForlog = GetDateForStr(ptrForlog,strtemp);
	if (IsBadReadPtr(ptrForlog,4))
	{
		return FALSE;
	}
	while((BYTE)*ptrForlog != AIR_VALUE)
	{
		ptrForlog++;
	}
	while((BYTE)*ptrForlog != AIR_TYPE)
	{
		ptrForlog++;
	}
	strtemp = LoLlog.gameMode;
	ptrForlog = GetDateForStr(ptrForlog,strtemp);
	if (IsBadReadPtr(ptrForlog,4))
	{
		return FALSE;
	}
	strtemp = LoLlog.queueType;
	ptrForlog++;
	ptrForlog = GetDateForStr(ptrForlog,strtemp);
	if (IsBadReadPtr(ptrForlog,4))
	{
		return FALSE;
	}
	time_t ltime = 0;
	time( &ltime );
	LoLlog.starttime = ltime - LoLlog.gamelen;
	return TRUE;
}
CHAR* CAirLog::SummonerScan(CHAR *ptrTeam)
{
	CHAR* ptrTemp = ptrTeam;
	CHAR* ptrForTeam = ptrTemp;
	for(int i = 0 ;i < 5 ; i++)
	{
		LPUserLog tempLog = &LoLlog.play[i];
		ptrForTeam = DataScan(ptrForTeam,tempLog);
		if (IsBadReadPtr(ptrForTeam,4))
		{
			return 0;
		}
	}
	ptrForTeam = GetGameLen(ptrForTeam,&LoLlog.gamelen);
	if (ptrForTeam = GetNextTeam(ptrForTeam))
	{
		for(int i = 5;i < MAX_PLAYRCOUNT; i++)
		{
			LPUserLog tempLog = &LoLlog.play[i];
			ptrForTeam = DataScan(ptrForTeam,tempLog);
			if (IsBadReadPtr(ptrForTeam,4))
			{
				return 0;
			}
		}
	}
	return ptrForTeam;
}
char* CAirLog::DataScan(CHAR *ptrSummoner,LPUserLog ptrLog)
{
	if(IsBadReadPtr(ptrSummoner, 4))
		return NULL;
	CHAR* ptrTemp = ptrSummoner;
	int i = 0; 
	//CHAR* ptrTempForBuff;
	/*if (*ptrTemp == (BYTE)AIR_VALUE)
	{
		double basePoints;
		GetValueForDouble(ptrTemp,&basePoints);
		ptrLog->basePoints = basePoints;
		while(*ptrTemp != (BYTE)AIR_TYPE)
		{
			ptrTemp++;
			if (*ptrTemp == (BYTE)AIR_TYPE)
			{
				CHAR str[MAX_PATH*2] = {0};
				CHAR *strtemp = str;
				GetDateForStr(GameType,strtemp);
				lstrcpyA(ptrLog->heroname,strtemp);
				break;
			}
		}
	}
	while(i == 27)
	{
		
	}*/
	{
		if ((BYTE)*ptrTemp == (BYTE)AIR_VALUE|| (BYTE)*ptrTemp == (BYTE)AIR_FILL)
		{
			if (i == 0)
			{
				if (IsFirstUser)
				{
					double basePoints = 0;
					if (ptrTemp = GetValueForDouble(ptrTemp,&basePoints))
					{
						ptrLog->basePoints = basePoints;
					}
					else
						return 0;
					while((BYTE)*ptrTemp != (BYTE)AIR_TYPE)
					{
						ptrTemp++;
						if ((BYTE)*ptrTemp == (BYTE)AIR_TYPE)
						{
							CHAR str[MAX_PATH*2] = {0};
							CHAR *strtemp = str;
							ptrTemp = GetDateForStr(ptrTemp,strtemp);
							CHAR* strhero = ptrLog->heroname;
							lstrcpyA(strhero,strtemp);
							i++;
							break;
						}
					}
				}
				else
				{
					if (ptrTemp = ScanForData(ptrTemp))
					{
						double basePoints = 0;
						if (ptrTemp = GetValueForDouble(ptrTemp,&basePoints))
						{
							ptrLog->basePoints = basePoints;
						}
						else
							return 0;
						while(*(WORD*)ptrTemp != HERO_NAME_TYPE && (*(WORD*)(ptrTemp+1) != HERO_NAME_TYPE_FILL_2|| *(WORD*)ptrTemp != HERO_NAME_TYPE_FILL_1 ))
						{
							ptrTemp++;
						}
						ptrTemp+=2;
						if (*(ptrTemp+1) == AIR_TYPE)
						{
							ptrTemp++;
						}
						if ((BYTE)*ptrTemp == (BYTE)AIR_TYPE|| (BYTE)*ptrTemp == (BYTE)AIR_FILL)
						{
							CHAR str[MAX_PATH*2] = {0};
							CHAR *strtemp = str;
							ptrTemp = GetDateForStr(ptrTemp,strtemp);
							if ((DWORD)*(DWORD*)strtemp == TEAM_PLAYER1)//上面队伍第一个玩家的英雄名
							{						
								CHAR* strhero = ptrLog->heroname;
								CHAR* ptrHero =  LoLlog.play[0].heroname;
								lstrcpyA(strhero,ptrHero);
								i++;
							}
							else
							if ((DWORD)*(DWORD*)strtemp == TEAM_PLAYER2)//上面队伍第二个玩家的英雄名
							{
								CHAR* strhero = ptrLog->heroname;
								CHAR* ptrHero =  LoLlog.play[1].heroname;
								lstrcpyA(strhero,ptrHero);
								i++;
							}
							else
							if ((DWORD)*(DWORD*)strtemp == TEAM_PLAYER3)//上面队伍第三个玩家的英雄名
							{
								CHAR* strhero = ptrLog->heroname;
								CHAR* ptrHero =  LoLlog.play[2].heroname;
								lstrcpyA(strhero,ptrHero);
								i++;
							}
							else
							if ((DWORD)*(DWORD*)strtemp == TEAM_PLAYER4)//上面队伍第四个玩家的英雄名
							{
								CHAR* strhero = ptrLog->heroname;
								CHAR* ptrHero =  LoLlog.play[3].heroname;
								lstrcpyA(strhero,ptrHero);
								i++;
							}
							else
							if ((DWORD)*(DWORD*)strtemp == TEAM_PLAYER5)//上面队伍第五个玩家的英雄名
							{
								CHAR* strhero = ptrLog->heroname;
								CHAR* ptrHero =  LoLlog.play[4].heroname;
								lstrcpyA(strhero,ptrHero);
								i++;
							}
							else
							{
								CHAR* strhero = ptrLog->heroname;
								lstrcpyA(strhero,strtemp);
								i++;
							}
						}
					}
				}
			}
			if (i == 1)
			{
				double gameid = 0;
				if (ptrTemp = GetValueForDouble(ptrTemp,&gameid))
				{
					ptrLog->gameid = gameid;
				}
				else
					return 0;
				i++;
			}
			ptrTemp+=4;
			if (i == 2)
			{
				double unknow1 = 0;
				while((BYTE)*ptrTemp !=AIR_VALUE|| (BYTE)*ptrTemp == (BYTE)AIR_FILL)
				{
					ptrTemp++;
				}
				if (ptrTemp = GetValueForDouble(ptrTemp,&unknow1))
				{
					ptrLog->unknow1 = unknow1;
				}
				else
					return 0;
				i++;
			}

			if (i == 3)
			{
				double teamid = 0;
				if (ptrTemp = GetValueForDouble(ptrTemp,&teamid))
				{
					ptrLog->teamid = teamid;
				}
				else
					return 0;
				i++;
			}
			int num = 0;
			for(;i<MAX_KEY_NUM;i++)
			{

				if (IsFirstUser)
				{
					if (ptrTemp = ScanForData(ptrTemp))
					{
						ptrLog->type[num].cnm = i;
						double datavalue = 0;
						if (ptrTemp = GetValueForDouble(ptrTemp,&datavalue))
						{
							ptrLog->type[num].logvalue = datavalue;
						}
						else
							return 0;
						if ((BYTE)*ptrTemp == (BYTE)AIR_TYPE || (BYTE)*ptrTemp == (BYTE)AIR_FILL)
						{
							CHAR str[MAX_PATH*2] = {0};
							CHAR *strtemp = str;
							ptrTemp = GetDateForStr(ptrTemp,strtemp);
							CHAR* strlogtype = ptrLog->type[num].logtype;
							lstrcpyA(strlogtype,strtemp);
						}
						else
							return 0;
					}
					else
						return 0;
				}
				else
				{
					while((BYTE)*ptrTemp != AIR_VALUE|| (BYTE)*ptrTemp == (BYTE)AIR_FILL)
					{
						ptrTemp++;
					}
					if (ptrTemp = ScanForData(ptrTemp))
					{
						ptrLog->type[num].cnm = i;
						double datavalue = 0;
						if (ptrTemp = GetValueForDouble(ptrTemp,&datavalue))
						{
							ptrLog->type[num].logvalue = datavalue;
						}
						else
							return 0;
						if ((BYTE)*ptrTemp == (BYTE)AIR_TYPE || (BYTE)*ptrTemp == (BYTE)AIR_FILL)
						{
							CHAR str[MAX_PATH*2] = {0};
							CHAR *strtemp = str;
							ptrTemp = GetDateForStr(ptrTemp,strtemp);
							if (*(WORD*)strtemp == LOG_LOGTYPE1 || *(WORD*)strtemp == LOG_LOGTYPE2)
							{
								CHAR* strlogtype = ptrLog->type[num].logtype;
								lstrcpyA(strlogtype,strtemp);
							} 
							else
							{
								int Index = (BYTE)str[0]/2-5;
								CHAR* strlogtype1 = ptrLog->type[num].logtype;
								CHAR* strlogtype2 = LoLlog.play[0].type[Index].logtype;
								lstrcpyA(strlogtype1,strlogtype2);
							}
						}
						else
							return 0;
					}
					else
						return 0;
				}
				num++;
			}
			if (IsFirstUser)
			{
				IsFirstUser = FALSE;
			}
			if (i == 31+RESAVE_DATA)
			{
				double unknow2 = 0;
				ptrTemp++;
				if (ptrTemp = GetValueForDouble(ptrTemp,&unknow2))
				{
					ptrLog->unknow2 = unknow2;
				}
				else
					return 0;
				i++;
			}
			if (i == 32+RESAVE_DATA)
			{
				double userid = 0;
				ptrTemp++;
				if (ptrTemp = GetValueForDouble(ptrTemp,&userid))
				{
					ptrLog->userid = userid;
				}
				else
					return 0;
				i++;
			}
			if (i == 33+RESAVE_DATA)
			{
				double unknow3 = 0;
				if ((BYTE)*ptrTemp == (BYTE)AIR_VALUE|| (BYTE)*ptrTemp == (BYTE)AIR_FILL)
				{
					if (ptrTemp = GetValueForDouble(ptrTemp,&unknow3))
					{
						ptrLog->unknow3 = unknow3;
					}
					else
						return 0;
				}					
				i++;
			}
			if (i == 34+RESAVE_DATA)
			{
				CHAR str[MAX_PATH*2] = {0};
				double user = 0;
				CHAR *strtemp = str;
				if (ptrTemp = GetValueForDouble(ptrTemp,&user))
				{
					/*if (user < 1)
					{
						ptrTemp = GetDateForStr(ptrTemp,strtemp);
						CHAR* ptrhero1 = ptrLog->summoner;
						CHAR* ptrhero2 = ptrLog->heroname;
						lstrcpyA(ptrhero1,ptrhero2);
					}
					else
					{
						ptrTemp = GetDateForStr(ptrTemp,strtemp);
						CHAR* ptrhero1 = ptrLog->summoner;
						lstrcpyA(ptrhero1,strtemp);
					}*/
					ptrTemp = GetDateForStr(ptrTemp,strtemp);
					if (strlen(strtemp)>2)
					{
						CHAR* ptrhero1 = ptrLog->summoner;
						lstrcpyA(ptrhero1,strtemp);
						ptrLog->Bot = FALSE;
					}
					else
					{
						CHAR* ptrhero1 = ptrLog->summoner;
						CHAR* ptrhero2 = ptrLog->heroname;
						lstrcpyA(ptrhero1,ptrhero2);
						ptrLog->Bot = TRUE;
					}
				}
				else
					return 0;
				i++;
			}
			if (i == 35+RESAVE_DATA)
			{
				double unknow4 = 0;
				if (ptrTemp = GetValueForDouble(ptrTemp,&unknow4))
				{
					ptrLog->unknow4 = unknow4;
				}
				else
					return 0;
				i++;
			}
		}
	}
	return ptrTemp;
}

CAirLog::CAirLog()
{
	memset( &LoLlog, 0, sizeof(Logs) );
}
