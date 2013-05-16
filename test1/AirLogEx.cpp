#include "stdafx.h"
#include "AirLogEx.h"

CAirLogEx::CAirLogEx(void)
{
	ZeroMemory(&EndofGameStats, sizeof(ENDOFGAMESTATS));
	m_sblockSize = 0;
}

CAirLogEx::~CAirLogEx(void)
{
}

CHAR* CAirLogEx::CharFilter(CHAR* lpSrc, DWORD dwSrcSize, const CHAR ch, DWORD* pdwOutSize/* = NULL*/)
{
	CHAR* lpszRes = NULL;
	DWORD dwMatchCount = 0;
	if (lpSrc && dwSrcSize > 0)
	{
		lpszRes = new CHAR[dwSrcSize+1];
		ZeroMemory(lpszRes, dwSrcSize+1);
		CHAR* pTmp = lpszRes;
		int idx = 0, last = 0;
		for ( ; idx < dwSrcSize; ++ idx )
		{
			if ( ch == lpSrc[idx] )
			{
				DWORD dwLen = idx-last;
				memcpy(pTmp, lpSrc+last, dwLen );
				pTmp += dwLen;
				last = idx+1;
				++ dwMatchCount;
			}
		}
		memcpy(pTmp, lpSrc+last, dwSrcSize - last );
		if (pdwOutSize)
			*pdwOutSize = dwSrcSize - dwMatchCount;
	} 
	return lpszRes;
}

VOID CAirLogEx::FreeFilterStr(CHAR* lpstr)
{
	if (lpstr)
		delete [] lpstr;
}
// 粘贴内存解析战果
BOOL CAirLogEx::PasteMemory(CHAR* lpMemory, DWORD dwsize)
{
	DWORD dwOutsize = 0;
	CHAR* lpfltMemory = this->CharFilter(lpMemory, dwsize, FILTER_CHAR, &dwOutsize);
	BOOL bRet = FALSE;
	if (lpfltMemory)
	{
		CHAR* lpHead = memstr(lpfltMemory, BLOCK_ENDOFGAMESTATS, dwOutsize);
		if (lpHead)
		{
			lpHead += lstrlenA(BLOCK_ENDOFGAMESTATS);
			int nbodySize = dwOutsize - (lpHead - lpfltMemory);
			this->CutAllBlock(lpHead, nbodySize);
			if ( SWF_ERR_INT != this->GetFirstSummBlock(EndofGameStats.teamPlayerParticipantStats/*, lpfltMemory, dwsize*/) )
			{
				int airIdx = this->GetPointsPenaltyBlock(&EndofGameStats.PointsPenalty);
				if (SWF_ERR_INT != airIdx)
				{
					if ( SWF_ERR_INT != this->GetNextSummBlock(EndofGameStats.otherTeamPlayerParticipantStats, airIdx) )
					{
						if ( SWF_ERR_INT != this->GetEndOfGameBlock(&EndofGameStats) )
							bRet = TRUE;
					}
				}
			}
		}
		this->FreeFilterStr(lpfltMemory);
	}
	FreeAirMemory(m_airMemoryArr.begin(), m_airMemoryArr.size());
	return bRet;
}

#include "../lib_json/json.h"
VOID CAirLogEx::PrintfBlock(const std::vector<char*> &blockCells)
{
	Json::Value root;
	CHAR* lpstrformat = NULL;
	for (int i = 0, size = blockCells.size(); i < size; ++ i)
	{
		switch ( blockCells[i][0] )
		{
		case SWF_INT:
			{
				lpstrformat = "int : %d \r\n";
				printf(lpstrformat, getSWF_int(blockCells[i]));
			}
			break;
		case SWF_DOUBLE:
			{
				lpstrformat = "db : %lf \r\n";
				printf(lpstrformat, getSWF_double(blockCells[i]));
			}
			break;
		case SWF_STR:
			{
				lpstrformat = "str : %s \r\n";
				printf(lpstrformat, getSWF_str(blockCells[i]));
				root[blockCells[i]] = "1";
			}
			break;
		case SWF_SEG:
			{
				lpstrformat = "seg : %d \r\n";
				printf(lpstrformat, getSWF_int(blockCells[i]));
			}
			break;
			// 		case SWF_BOOLEAN:
			// 			{
			// 				lpstrformat = "bool : %s \r\n";
			// 				printf(lpstrformat, getSWF_int(blockCells[i]) ? "true" : "false");
			// 			}
			// 			break;
		}
	}
}

// 获取首个召唤师信息区块
// 06 05 05 
// CHAR* CAirLogEx::GetFirstSummBlock(LPPPSTATSSUMMARY lpPPSummary/*, CHAR* lpMemory, DWORD dwsize*/)
// {
// 	BOOL bRet = FALSE;
// 	std::vector<CHAR*> blockCells;
// 	CHAR* lpHead = NULL;
// 	CHAR* lpEnd = NULL;
// 	CHAR* lpGameIdflag = NULL;
// 	for (int i = 0; i < dwsize; ++ i)
// 	{
// 		if ( memicmp(lpMemory+i, BLOCK_STATRFLAG, lstrlenA(BLOCK_STATRFLAG)) == 0 )
// 		{
// 			lpHead = lpMemory + i + lstrlenA(BLOCK_STATRFLAG);
// 			bRet = TRUE;
// 			break;
// 		}
// 	}
// 	if ( ! bRet )
// 		return FALSE;
// 	
// 	for (int countIdx = 0; countIdx < MAXPNUM_ONETEAM; ++ countIdx)
// 	{
// 
// 		for (int i = 0 , last = -1; i < dwsize/2 ;( SWF_STR == lpHead[i])?i += 2 : ++ i	// 如果是 SWF_STR 紧跟着的下一个字符是长度, 直接忽略
// 			)
// 		{
// 				if ( SWF_INT == lpHead[i] 
// 				|| SWF_DOUBLE == lpHead[i]
// 				|| SWF_STR == lpHead[i]
// 				|| SWF_SEG == lpHead[i]
// 				//|| SWF_BOOLEAN == lpHead[i]
// 			 )
// 			 {
// 				 if (-1 == last)
// 				 {
// 					 last = i;
// 					 continue;
// 				 }
// 
// 				 int nlen = i - last;
// 				 CHAR* lpcell = new CHAR[nlen+1];
// 				 ZeroMemory(lpcell, nlen + 1);
// 
// 				 memcpy(lpcell, lpHead + last, nlen);
// 
// 				 if ( ! lpGameIdflag && SWF_DOUBLE == lpHead[last] && SWF_INT == lpHead[i] )
// 				 {
// 					 lpGameIdflag = new CHAR[SWF_DOUBLE_SIZE+1];
// 					 ZeroMemory(lpGameIdflag, SWF_DOUBLE_SIZE + 1);
// 					 memcpy(lpGameIdflag, lpHead + last + 1, SWF_DOUBLE_SIZE);
// 				 }
// 				 else if ( lpGameIdflag && memcmp(lpGameIdflag, lpcell+1, SWF_DOUBLE_SIZE) == 0 )
// 				 {
// 					 for (int i = 0; i < last; ++ i)
// 					 {
// 						 if ( SWF_STR == lpHead[last-i-1] )
// 						 {
// 							 if ( memcmp( lpHead + last - i + (lpHead[last-i] - 1) / 2 + 1, lpcell, nlen ) == 0 )	//计算偏移后面正好和GameId 地址内容吻合的话说明一组结束
// 								 lpEnd = lpHead + last - i - 1;
// 							 else
// 								 lpEnd = NULL;
// 							 break;
// 						 }
// 					 }
// 					 blockCells.pop_back();	// 下一组的英雄名 Pop back
// 					 break;
// 				 }
// 
// 				 blockCells.push_back(lpcell);
// 				 last = i;
// 			 }
// 		}
// 		m_sblockSize = blockCells.size();
// 
// 
// 		PrintfBlock(blockCells);
// 
// 		GetSummonerBlock(lpPPSummary+countIdx, blockCells);
// 		
// 		delete lpGameIdflag, lpGameIdflag = NULL;
// 		for (int i = 0, size = blockCells.size(); i < size; ++ i)
// 			delete blockCells[i];
// 		blockCells.clear();
// 		if (! lpEnd)	// 循环完跳出
// 			break;
// 		else
// 			lpHead = lpEnd;
// 	}
// 	
// }


int CAirLogEx::GetEndOfGameBlock(LPENDOFGAMESTATS lpEndOfGameStats)	//最后获取 EndOfGameBlock
{
	//int cellIdx = 0;
	std::vector<AIRMEMORY>::iterator itr;
	itr = m_airMemoryArr.begin();
	//itr += cellIdx + 1;			// +1 跳过标记, 这里没有标记可跳

	int tmpIdx = 0;
	int idx = 0;

 	idx = getTypeIdx(itr, SWF_STR, 0);
 	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
 	else	strcpy_s(lpEndOfGameStats->gameType, sizeof(lpEndOfGameStats->gameType), getSWF_str(itr[idx].lpMmeory));
 
 	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
 	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
 	else	lpEndOfGameStats->experienceEarned = getSWF_double(itr[idx].lpMmeory);
 
	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == tmpIdx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->reportGameId_v = getSWF_double(itr[idx].lpMmeory);


	tmpIdx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	
		if (tmpIdx-idx > 2)	strcpy_s(lpEndOfGameStats->difficulty, sizeof(lpEndOfGameStats->difficulty), SWF_ERR_STR);
		else	idx=tmpIdx, strcpy_s(lpEndOfGameStats->difficulty, sizeof(lpEndOfGameStats->difficulty), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->gameLength_v = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->boostXpEarned = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->roomName, sizeof(lpEndOfGameStats->roomName), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->userId_v = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->coOpVsAiMinutesLeftToday = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->loyaltyBoostIpEarned = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->rpEarned = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->completionBonusPoints = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->coOpVsAiMsecsUntilReset = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->gameId5 = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->timeUntilNextFirstWinBonus_v = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->loyaltyBoostXpEarned = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->roomPassword, sizeof(lpEndOfGameStats->roomPassword), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_INT, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->elo = getSWF_int(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->ipEarned_v = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->firstWinBonus_v = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_INT, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->eloChange = getSWF_int(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->gameMode, sizeof(lpEndOfGameStats->gameMode), getSWF_str(itr[idx].lpMmeory));


	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->queueType, sizeof(lpEndOfGameStats->queueType), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->clientId_Ex, sizeof(lpEndOfGameStats->clientId_Ex), getSWF_str(itr[idx].lpMmeory));
	if ( ! strstr(lpEndOfGameStats->clientId_Ex, "gn-") )
		return SWF_ERR_INT;	//验证结构体准确性

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->destination_Ex, sizeof(lpEndOfGameStats->destination_Ex), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->DSSubtopic_value_Ex, sizeof(lpEndOfGameStats->DSSubtopic_value_Ex), getSWF_str(itr[idx].lpMmeory));
	if ( memicmp(lpEndOfGameStats->DSSubtopic_value_Ex, SWF_ERR_STR, lstrlenA(SWF_ERR_STR)) == 0 )
		strcpy_s(lpEndOfGameStats->DSSubtopic_value_Ex, sizeof(lpEndOfGameStats->DSSubtopic_value_Ex), lpEndOfGameStats->clientId_Ex);

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpEndOfGameStats->messageId_Ex, sizeof(lpEndOfGameStats->messageId_Ex), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpEndOfGameStats->timestamp_Ex_v = getSWF_double(itr[idx].lpMmeory);


	++idx;

	FreeAirMemory(itr, idx);
	/*itr = m_airMemoryArr.begin(), itr += cellIdx + 1;
	return itr->idx;*/
	return 0;
}

int CAirLogEx::GetPointsPenaltyBlock(LPPOINTSPENALTY lpPointsPenalty)	// 获取PointsPenalty区块
{
	BOOL bRet = FALSE;
	int cellIdx = 0;
	for (int i = 0, size = m_airMemoryArr.size(); i < size; ++ i)
	{
		for (int cidx = 0, csize = lstrlenA(m_airMemoryArr[i].lpMmeory)
			; cidx < csize
			; ++ cidx)
		{
			if ( memicmp(m_airMemoryArr[i].lpMmeory + cidx, BLOCK_POINTSPENALTY, lstrlenA(BLOCK_POINTSPENALTY)) == 0 )
			{
				cellIdx = i;
				bRet = TRUE;
				break;
			}
		}

	}
	if ( ! bRet )
	{	//如果没找到该区块，说明不是人机，消除 lpPointsPenalty 无用的多余部分
		for (int i = 0, size = m_airMemoryArr.size(); i < size; ++ i)
		{
			for (int cidx = 0, csize = lstrlenA(m_airMemoryArr[i].lpMmeory)
				; cidx < csize
				; ++ cidx)
			{
				if ( memicmp(m_airMemoryArr[i].lpMmeory + cidx, BLOCK_POINTSPENALTY_2, lstrlenA(BLOCK_POINTSPENALTY_2)) == 0 )
				{
					cellIdx = i;
					std::vector<AIRMEMORY>::iterator itr = m_airMemoryArr.begin();
					itr += cellIdx;	// itr 赋好值
					// 消除 lpPointsPenalty 无用的多余部分
					int tmpIdx = getTypeIdx(itr, SWF_DOUBLE, 0+1);
					cellIdx = getStrIdx(itr, "\n\r", tmpIdx+1);
					if (SWF_ERR_INT != cellIdx)
						FreeAirMemory(itr+tmpIdx+1, cellIdx-tmpIdx);

					cellIdx = getTypeIdx(itr, SWF_STR, 0+1);
					if ( SWF_ERR_INT == cellIdx )
						return SWF_ERR_INT;
					else
						return itr[cellIdx].idx;	// 返回 OtherTeam 开始地址索引
				}
			}

		}
		return SWF_ERR_INT;
	}
	else
		cellIdx += 1;	// +1 跳过标记
	std::vector<AIRMEMORY>::iterator itr;
	itr = m_airMemoryArr.begin();
	itr += cellIdx;

	int idx = 0;

	idx = getTypeIdx(itr, SWF_STR, 0);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpPointsPenalty->type, sizeof(lpPointsPenalty->type), getSWF_str(itr[idx].lpMmeory));

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPointsPenalty->penalty = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_INT, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPointsPenalty->unKnowAddr = getSWF_int(itr[idx].lpMmeory);

	if ( SWF_SEG == itr[idx+1].lpMmeory[0] )
		idx += 1;
	++idx;
	
	FreeAirMemory(itr, idx);
	itr = m_airMemoryArr.begin(), itr += cellIdx;
	return itr->idx;
}

int CAirLogEx::GetNextSummBlock(LPPPSTATSSUMMARY lpPPSummary, int airIdx)	// 获取下个召唤师信息区块
{
	int bRet = FALSE;
	int cellIdx = 0;
	for (int i = 0, size = m_airMemoryArr.size(); i < size; ++ i)
	{
		if ( m_airMemoryArr[i].idx == airIdx )
		{
			cellIdx = i;
			bRet = TRUE;
			break;
		}
	}
	if ( ! bRet )
		return SWF_ERR_INT;
	std::vector<AIRMEMORY>::iterator it;
	it = m_airMemoryArr.begin();
	it += cellIdx;

	for (int countIdx = 0; countIdx < MAXPNUM_ONETEAM; ++ countIdx, it = m_airMemoryArr.begin(), it += cellIdx)
	{
		int noffsetSize = GetSummonerBlock(lpPPSummary+countIdx, it);
		if ( SWF_ERR_INT != noffsetSize )
			FreeAirMemory(it, noffsetSize);
	}

	it = m_airMemoryArr.begin(), it += cellIdx;
	return it->idx;
}

int CAirLogEx::GetFirstSummBlock(LPPPSTATSSUMMARY lpPPSummary/*, CHAR* lpMemory, DWORD dwsize*/)
{
	BOOL bRet = FALSE;
	int cellIdx = 0;
	for (int i = 0, size = m_airMemoryArr.size(); i < size; ++ i)
	{
		for (int cidx = 0, csize = lstrlenA(m_airMemoryArr[i].lpMmeory)
			; cidx < csize
			; ++ cidx)
		{
			if ( memicmp(m_airMemoryArr[i].lpMmeory + cidx, BLOCK_STATRFLAG, lstrlenA(BLOCK_STATRFLAG)) == 0 )
			{
				cellIdx = i;
				bRet = TRUE;
				break;
			}
		}
		
	}
	if ( ! bRet )
		return SWF_ERR_INT;
	cellIdx += 1;
	std::vector<AIRMEMORY>::iterator it;
	it = m_airMemoryArr.begin();
	it += cellIdx;

	for (int countIdx = 0; countIdx < MAXPNUM_ONETEAM; ++ countIdx, it = m_airMemoryArr.begin(), it += cellIdx )
	{
		int noffsetSize = GetSummonerBlock(lpPPSummary+countIdx, it);
		if ( SWF_ERR_INT != noffsetSize )
			FreeAirMemory(it, noffsetSize);
		else
			return SWF_ERR_INT;
	}

	it = m_airMemoryArr.begin(), it += cellIdx;
	return it->idx;

}

double CAirLogEx::getSWF_double(CHAR* lpValAddr)
{
	double dbRet = 0;
	char* lpRet = (char*)(&dbRet);
	if ( SWF_DOUBLE != lpValAddr[0] )
		return 0;
	else
		lpValAddr += 1;
	for (int i = 0; i < SWF_DOUBLE_SIZE/2; ++ i)
	{	
		lpRet[i] = lpValAddr[SWF_DOUBLE_SIZE-i-1];
		lpRet[SWF_DOUBLE_SIZE-i-1] = lpValAddr[i];
	}
	return dbRet;
}

/*
0x841f => 508
1000001001111100	=> 0x841f
0000000111111100	=> 508

10000011
01111100

00000001
11111100
*/
int CAirLogEx::getSWF_int(CHAR* lpValAddr)
{
	if ( SWF_INT != lpValAddr[0] )
		return 0;
	else
		lpValAddr += 1;
	int size = lstrlenA(lpValAddr);
	if ( size > 2 )
		return SWF_ERR_INT;
	int nRet = 0;
	char* lpRet = (char*)&nRet;
	BOOL bCarry = FALSE;
	for (int i = 0; i < size; ++ i)
	{
		if ( 0 == i && (lpValAddr[i] & 0x80) != 0x00 )
		{
			lpRet[size-i-1] = (lpValAddr[i] & 0x7f);	// >> 1;
			if (lpRet[size-i-1] & 0x01)
				bCarry = TRUE;
			lpRet[size-i-1] >>= 1;
		}
		else
		{
			lpRet[size-i-1] = lpValAddr[i];
			if ( bCarry )
			{
				lpRet[size-i-1] |= 0x80;
				bCarry = FALSE;
			}
		}
	}

	return nRet;
}

char* CAirLogEx::getSWF_str(CHAR* lpValAddr)
{
	BOOL bret = BYTE(lpValAddr[1]) & 0x80;
	UINT nrealLen = lstrlenA(lpValAddr) - 2;
	UINT nlen = ( lpValAddr[1] - 1 ) / 2;
	if ( nlen == nrealLen )
		lpValAddr += 2;
	else
		return SWF_ERR_STR;

	return lpValAddr;
}

char* CAirLogEx::getSWF_strEx(CHAR* lpValAddr)		//带有补全过滤掉字符的函数
{
	BOOL bret = BYTE(lpValAddr[1]) & 0x80;
	UINT nrealLen = lstrlenA(lpValAddr) - 2;
	UINT nlen = ( lpValAddr[1] - 1 ) / 2;
	char* psTarget = NULL;
	if ( nlen == nrealLen )
	{
		psTarget = new char[nlen+1];
		memcpy(psTarget, lpValAddr+2, nlen);
		psTarget[nlen] = '\0';
	}
	else if ( ! bret ||  nlen > nrealLen )
	{
		int size = nlen - nrealLen;
		char* psStart = lpValAddr + 2;
		psTarget = new char[nrealLen*2+1];
		ZeroMemory(psTarget, nrealLen*2+1);
		for (int i = 0, j = 0; i < nrealLen; ++ i)
		{
			int mark1 = (psStart[i]) & 0x80;
			int mark2 = (psStart[i]) & 0x40;
			if (mark1 != 0 && mark2 == 0)
			{
				psTarget[i+(j++)] = FILTER_CHAR;
				psTarget[i+j] = psStart[i];
			}
			else
				psTarget[i+j] = psStart[i];
		}
		size = lstrlenA(psTarget);
		if ( size != nlen )
		{
			delete psTarget;
			return SWF_ERR_STR;
		}
	}
	else
		return SWF_ERR_STR;

	return psTarget;
}

std::vector<AIRMEMORY>::iterator CAirLogEx::getTypeIdx2(std::vector<AIRMEMORY>::iterator itr , DWORD dwSWFtype, DWORD dwCount)
{
	std::vector<AIRMEMORY>::iterator itrRes = m_airMemoryArr.begin();
	int i = 0, curCount = 0;
	for ( ; itr != m_airMemoryArr.end() && curCount < dwCount; ++i)
	{
		if ( itr[i].lpMmeory[0] == dwSWFtype )
		{
			++ curCount ;
		}
	}
	if ( curCount == dwCount )
		itrRes = itr + i - 1;	//如果找到了将返回搜索的结果, 如果没找到则返回 begin
	return itrRes;
}

int CAirLogEx::getTypeIdx(std::vector<AIRMEMORY>::iterator itr , DWORD dwSWFtype, int startPos/* = 0*/)
{
	int nRet = SWF_ERR_INT;
	itr+=startPos;
	for (int i = 0; itr != m_airMemoryArr.end() ; ++ i, ++ itr)
	{
		if ( itr->lpMmeory[0] == dwSWFtype )
		{
			nRet = i + startPos;
			break;
		}
	}
	return nRet;
}

int CAirLogEx::getStrIdx(std::vector<AIRMEMORY>::iterator itr , CHAR* lpStr, int startPos/* = 0*/)
{
	int nRet = SWF_ERR_INT;
	itr+=startPos;
	for (int i = 0; itr != m_airMemoryArr.end() ; ++ i, ++ itr)
	{
		for (int cidx = 0, csize = lstrlenA(itr->lpMmeory)
			; cidx < csize
			; ++ cidx)
		{
			if ( memicmp(itr->lpMmeory + cidx, lpStr, lstrlenA(lpStr)) == 0 )
			{
				nRet = i + startPos;
				return nRet;
			}
		}
	}
	return nRet;
}

int CAirLogEx::getTypeIdx(const std::vector<char*> &vct , DWORD dwSWFtype, int startPos /*= 0*/)
{
	int nRet = 0;
	for (int i = startPos, size = vct.size(); i < size; ++ i)
	{
		if ( vct[i][0] == dwSWFtype )
		{
			nRet = i;
			break;
		}
	}
	return nRet;
}

int CAirLogEx::GetSummonerBlock(__out LPPPSTATSSUMMARY lpPPSummary, __in const std::vector<AIRMEMORY>::iterator itr)
{
	int idx = 0;

	idx = getTypeIdx(itr, SWF_STR, 0);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	strcpy_s(lpPPSummary->skinName, sizeof(lpPPSummary->skinName), getSWF_str(itr[idx].lpMmeory));
	if ( memcmp(lpPPSummary->skinName, SWF_ERR_STR, lstrlenA(SWF_ERR_STR)) == 0 )
	{
		int strIdx = BYTE(itr[idx].lpMmeory[2]);
		if (0 != strIdx)
			strIdx = strIdx / 4 - REOFFSET;
		if ( strIdx >= 0 && strIdx < MAXPNUM_ONETEAM )
		{
			strcpy_s(lpPPSummary->skinName
				, sizeof(lpPPSummary->skinName)
				, EndofGameStats.teamPlayerParticipantStats[strIdx].skinName
				);
		}
	}

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->gameId = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_INT, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->_profileIconId = getSWF_int(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_INT, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->leaver = getSWF_int(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->leaves = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->teamId = getSWF_double(itr[idx].lpMmeory);

	for (int i = 0; i < MAX_ITEMS_NUM; ++ i)
	{
		/*idx = getTypeIdx(itr, SWF_SEG, idx+1);
		lpPPSummary->_gameItems[i].seg = getSWF_int(itr[idx].lpMmeory);*/
		idx = getTypeIdx(itr, SWF_INT, idx+1);
		if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
		else	lpPPSummary->_gameItems[i].idx = getSWF_int(itr[idx].lpMmeory);
		idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
		if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
		else	lpPPSummary->_gameItems[i].itemValue = getSWF_double(itr[idx].lpMmeory);
		idx = getTypeIdx(itr, SWF_STR, idx+1);
		if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
		else	strcpy_s(lpPPSummary->_gameItems[i].itemName, sizeof(lpPPSummary->_gameItems[i].itemName), getSWF_str(itr[idx].lpMmeory));
		if ( memcmp(lpPPSummary->_gameItems[i].itemName, SWF_ERR_STR, lstrlenA(SWF_ERR_STR)) == 0 )
		{
			int strIdx = BYTE(itr[idx].lpMmeory[2])/2-5;
			if ( strIdx < MAX_ITEMS_NUM)
			{
				strcpy_s(lpPPSummary->_gameItems[i].itemName
					, sizeof(lpPPSummary->_gameItems[i].itemName)
					, EndofGameStats.teamPlayerParticipantStats[0]._gameItems[strIdx].itemName
					);
			}
			else if ( (strIdx = (strIdx-MAX_ITEMS_NUM-8)) >= 0 && strIdx < MAX_ITEMS_NUM )
			{	// 如果本隊沒有勝利標記,則敵隊勝利
				if ( SWF_ERR_INT == EndofGameStats.teamPlayerParticipantStats[0].getItemValue(WIN) )
					strcpy_s(lpPPSummary->_gameItems[i].itemName
	 				, sizeof(lpPPSummary->_gameItems[i].itemName)
	 				, WIN
	 				);
				else
					strcpy_s(lpPPSummary->_gameItems[i].itemName
					, sizeof(lpPPSummary->_gameItems[i].itemName)
					, LOSE
					);
// 				strcpy_s(lpPPSummary->_gameItems[i].itemName
// 				, sizeof(lpPPSummary->_gameItems[i].itemName)
// 				, EndofGameStats.teamPlayerParticipantStats[0]._gameItems[strIdx].itemName
// 				);
			}
		}
	}

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->level = getSWF_double(itr[idx].lpMmeory);
	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else		lpPPSummary->userId = getSWF_double(itr[idx].lpMmeory);
	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->spell2Id = getSWF_double(itr[idx].lpMmeory);
	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->losses = getSWF_double(itr[idx].lpMmeory);

	idx = getTypeIdx(itr, SWF_STR, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else
	{
		char* lpName = getSWF_strEx(itr[idx].lpMmeory);
		if (  memicmp( lpName, SWF_ERR_STR, lstrlenA(SWF_ERR_STR) ) != 0 )
		{
			lpPPSummary->botPlayer_c = false;	// 判断是否为电脑
			strcpy_s(lpPPSummary->_summonerName, sizeof(lpPPSummary->_summonerName), lpName);
			delete lpName;
		}
		else
		{
			lpPPSummary->botPlayer_c = true;	// 判断是否为电脑
			strcpy_s(lpPPSummary->_summonerName, sizeof(lpPPSummary->_summonerName), lpPPSummary->skinName);
		}
	}

	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->wins = getSWF_double(itr[idx].lpMmeory);
	idx = getTypeIdx(itr, SWF_DOUBLE, idx+1);
	if (SWF_ERR_INT == idx)		return SWF_ERR_INT;
	else	lpPPSummary->spell1Id = getSWF_double(itr[idx].lpMmeory);

	return idx+1;
}

// VOID CAirLogEx::GetSummonerBlock(LPPPSTATSSUMMARY lpPPSummary, const std::vector<char*> &blockCells )
// {
// 	int idx = 0;
// 
// 	idx = getTypeIdx(blockCells, SWF_STR, 0);
// 	strcpy_s(lpPPSummary->skinName, sizeof(lpPPSummary->skinName), getSWF_str(blockCells[idx]));
// 
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->gameId = getSWF_double(blockCells[idx]);
// 
// 	idx = getTypeIdx(blockCells, SWF_INT, idx+1);
// 	lpPPSummary->_profileIconId = getSWF_int(blockCells[idx]);
// 
// 	idx = getTypeIdx(blockCells, SWF_INT, idx+1);
// 	lpPPSummary->leaver = getSWF_int(blockCells[idx]);
// 
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->leaves = getSWF_double(blockCells[idx]);
// 
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->teamId = getSWF_double(blockCells[idx]);
// 
// 	for (int i = 0; i < MAX_ITEMS_NUM; ++ i)
// 	{
// 		/*idx = getTypeIdx(blockCells, SWF_SEG, idx+1);
// 		lpPPSummary->_gameItems[i].seg = getSWF_int(blockCells[idx]);*/
// 		idx = getTypeIdx(blockCells, SWF_INT, idx+1);
// 		lpPPSummary->_gameItems[i].idx = getSWF_int(blockCells[idx]);
// 		idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 		lpPPSummary->_gameItems[i].itemValue = getSWF_double(blockCells[idx]);
// 		idx = getTypeIdx(blockCells, SWF_STR, idx+1);
// 		strcpy_s(lpPPSummary->_gameItems[i].itemName, sizeof(lpPPSummary->_gameItems[i].itemName), getSWF_str(blockCells[idx]));
// 		if ( memcmp(lpPPSummary->_gameItems[i].itemName, SWF_ERR_STR, lstrlenA(SWF_ERR_STR)) == 0 )
// 			strcpy_s(lpPPSummary->_gameItems[i].itemName
// 			, sizeof(lpPPSummary->_gameItems[i].itemName)
// 			, EndofGameStats.teamPlayerParticipantStats[0]._gameItems[BYTE(blockCells[idx][2])/2-5].itemName
// 			);
// 	}
// 
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->level = getSWF_double(blockCells[idx]);
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->userId = getSWF_double(blockCells[idx]);
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->spell2Id = getSWF_double(blockCells[idx]);
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->losses = getSWF_double(blockCells[idx]);
// 
// 	idx = getTypeIdx(blockCells, SWF_STR, idx+1);
// 	strcpy_s(lpPPSummary->_summonerName, sizeof(lpPPSummary->_summonerName), getSWF_str(blockCells[idx]));
// 
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->wins = getSWF_double(blockCells[idx]);
// 	idx = getTypeIdx(blockCells, SWF_DOUBLE, idx+1);
// 	lpPPSummary->spell1Id = getSWF_double(blockCells[idx]);
// 
// 	if (  memicmp( lpPPSummary->_summonerName, SWF_ERR_STR, lstrlenA(SWF_ERR_STR) ) == 0 )
// 	{
// 		lpPPSummary->botPlayer_c = true;	// 判断是否为电脑
// 		strcpy_s(lpPPSummary->_summonerName, sizeof(lpPPSummary->_summonerName), lpPPSummary->skinName);
// 	}
// 	else
// 		lpPPSummary->botPlayer_c = false;
// }

void CAirLogEx::CutAllBlock(CHAR* lpMemory, DWORD dwsize)
{
	BOOL bRet = FALSE;
	CHAR* lpEnd = NULL;
	CHAR* lpGameIdflag = NULL;
	CHAR* lpHead = lpMemory;
	AIRMEMORY airMemory = {0};
// 	for (int i = 0; i < dwsize; ++ i)
// 	{
// 		if ( memicmp(lpMemory+i, BLOCK_ENDOFGAMESTATS, lstrlenA(BLOCK_ENDOFGAMESTATS)) == 0 )
// 		{
// 			lpHead = lpMemory + i + lstrlenA(BLOCK_ENDOFGAMESTATS);
// 			bRet = TRUE;
// 			break;
// 		}
// 	}
// 	if ( ! bRet )
// 		return /*FALSE*/;

	int last = -1;
	int nlen = 0;
	CHAR* lpcell = NULL;
	for (int i = 0; i < dwsize ; ++ i )
	{
		if ( SWF_INT == lpHead[i] 
		|| SWF_DOUBLE == lpHead[i]
		|| SWF_STR == lpHead[i]
		|| SWF_SEG == lpHead[i]
		//|| SWF_BOOLEAN == lpHead[i]
		)
		{
			if (-1 == last)
			{
				last = i;
				continue;
			}

			nlen = i - last;
			lpcell = new CHAR[nlen+1];
			ZeroMemory(lpcell, nlen + 1);

			memcpy(lpcell, lpHead + last, nlen);

			airMemory.idx = last;
			airMemory.type = (SWFTYPE)lpcell[0];
			airMemory.lpMmeory = lpcell;

			m_airMemoryArr.push_back(airMemory);

			last = i;

			if ( SWF_STR == lpHead[i]	// 如果是 SWF_STR 紧跟着的下一个字符是长度, 直接忽略
			|| SWF_INT == lpHead[i]		// 如果是 SWF_INT 下一位字符一定是内容, 直接忽略
			)	 
			{
				i += 1;
				if ( SWF_INT == lpHead[i-1] && (lpHead[i] & 0x80) !=  0x00 )	// lpHead[i+1] 因为 i+=1 已经加过了, 所以这里直接用 lpHead[i]
					i += 1;				// 如果InT类型首字节是10000000(负数),说明是两个字节组成的, 还要再加 1
			}
			else if ( SWF_DOUBLE == lpHead[i] )
				i += SWF_DOUBLE_SIZE - 1;	// 如果是 SWF_DOUBLE 紧跟着一定是 SWF_DOUBLE_SIZE 长度, 直接忽略 多减 1 是排除多字节情况下 0xC3 对内存的干扰
		}
	}
	nlen = dwsize - last;
	lpcell = new CHAR[nlen+1];
	ZeroMemory(lpcell, nlen + 1);

	memcpy(lpcell, lpHead + last, nlen);

	airMemory.idx = last;
	airMemory.lpMmeory = lpcell;

	m_airMemoryArr.push_back(airMemory);
}

void CAirLogEx::FreeAirMemory(__in const std::vector<AIRMEMORY>::iterator itr, __in int size)
{
	for (int i = 0 ; i < size; ++ i)
	{
		if (itr[i].lpMmeory)
			delete itr[i].lpMmeory, itr[i].lpMmeory = NULL;
	}
	m_airMemoryArr.erase(itr, itr + size);
}

char* CAirLogEx::memstr(char* src, char* tag, int srcSize)
{
	for ( int i = 0, len = lstrlenA(tag); i < srcSize-len; ++ i)
		if ( memcmp(src + i, tag, len) == 0 )
			return src + i;
	return NULL;
}