#pragma once

#define __LOLAIR_EXENAME				"LolClient.exe"
#define __LOLLEAGUE_DLLNAME				"launcher.maestro.dll"
#define __LOLLEAGUE_PROCNAME			"League of Legends.exe"
#define __RENDERCALLBACK				"RenderCallback.dll"
#define __INFOTRANSMISSION				"InfoTransmission.dll"
#define __GAMEMONITOR					"GameMonitor.dll"
#define __TIMELINE_FILENAME				"tl-report.dat"

#define TIMELINE_FILENAMEW				_T(__TIMELINE_FILENAME)
#define TIMELINE_FILENAMEA				__TIMELINE_FILENAME
#define LOLAIR_EXENAMEW					_T(__LOLAIR_EXENAME)
#define LOLAIR_EXENAMEA					__LOLAIR_EXENAME
#define LOLLEAGUE_PROCNAMEW				_T(__LOLLEAGUE_PROCNAME)
#define LOLLEAGUE_PROCNAMEA				__LOLLEAGUE_PROCNAME
#define LOLLEAGUE_DLLNAMEW				_T(__LOLLEAGUE_DLLNAME)
#define LOLLEAGUE_DLLNAMEA				__LOLLEAGUE_DLLNAME
#define RENDERCALLBACKW					_T(__RENDERCALLBACK)
#define RENDERCALLBACKA					__RENDERCALLBACK
#define INFOTRANSMISSIONW				_T(__INFOTRANSMISSION)
#define INFOTRANSMISSIONA				__INFOTRANSMISSION
#define GAMEMONITORW					_T(__GAMEMONITOR)
#define GAMEMONITORA					__GAMEMONITOR

#ifdef _UNICODE
#define TIMELINE_FILENAME				TIMELINE_FILENAMEW
#define LOLAIR_EXENAME					LOLAIR_EXENAMEW
#define LOLLEAGUE_DLLNAME				LOLLEAGUE_DLLNAMEW
#define LOLLEAGUE_PROCNAME				LOLLEAGUE_PROCNAMEW
#define RENDERCALLBACK					RENDERCALLBACKW
#define INFOTRANSMISSION				INFOTRANSMISSIONW
#define GAMEMONITOR						GAMEMONITORW
#else
#define TIMELINE_FILENAME				TIMELINE_FILENAMEA
#define LOLAIR_EXENAME					LOLAIR_EXENAMEA
#define LOLLEAGUE_DLLNAME				LOLLEAGUE_DLLNAMEA
#define LOLLEAGUE_PROCNAME				LOLLEAGUE_PROCNAMEA
#define RENDERCALLBACK					RENDERCALLBACKA
#define INFOTRANSMISSION				INFOTRANSMISSIONA
#define GAMEMONITOR						GAMEMONITORA
#endif

#define CSTRCAT(ctstr1, cstr2)			ctstr1 L##cstr2		
#define GARENA_APP_INI					_T("\\GarenaMessenger\\app.ini\0")
//////////////////////////////////////////////////////////////////////////
// Air ��ݽ��� 
#define AIR_FUNC
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ���뷨ע�뿪��
// #define __IME_INSERT
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Loading ս����忪��
// #define __DM_LAODING_PC
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Air ս������
#define _COMMIT_LOG
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// �Զ���Ǳ�׼ Debug ��ϸ��Ϣ�������
// #define _LOG_DEBUG
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////