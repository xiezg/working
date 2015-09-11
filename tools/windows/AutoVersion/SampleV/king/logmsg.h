#pragma once

#ifndef __LOG_MESSAGE_H__
#define __LOG_MESSAGE_H__

#include <time.h>
#include <tchar.h>
#include <yvals.h >
#include <direct.h>
#include <stdio.h>
#include <conio.h> 

#include ".\Lock.h"

#include <Windows.h>
#include <Shlwapi.h>

#pragma comment( lib, "shlwapi.lib" )

#if defined(_UNICODE) || defined(UNICODE)
#define _T_FUNCTION__ __FUNCTIONW__
#else
#define _T_FUNCTION__ __FUNCTION__
#endif //__FUNCTIONW__

#define LOG_MSG_BUFFER_SIZE 4096

static HANDLE OpenLogFile( HMODULE hLogModule )
{
	TCHAR filePath[MAX_PATH] = {0};
	::GetModuleFileName( hLogModule, filePath, MAX_PATH );

	TCHAR logFilePath[MAX_PATH] = {0};
	::GetModuleFileName( NULL, logFilePath, MAX_PATH );
	::PathRemoveFileSpec( logFilePath );
	::PathAppend( logFilePath, _T("log") );
	_tmkdir( logFilePath );
	::PathAppend( logFilePath, ::PathFindFileName( filePath ) );
	_tmkdir( logFilePath );

	::PathAppend( logFilePath, ::PathFindFileName( filePath ) );
	::PathRenameExtension( logFilePath, _T(".log") );

	HANDLE hLogFile = ::CreateFile( logFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_WRITE_THROUGH, NULL );

	::SetFilePointer( hLogFile, 0, NULL, FILE_END );

	return hLogFile;
}

static void CloseLogFile( HANDLE hLogFile )
{
	if( hLogFile == INVALID_HANDLE_VALUE )
	{
		return;
	}

	::CloseHandle( hLogFile );
	hLogFile = INVALID_HANDLE_VALUE;

	return ;
}

static void WriteLogMsgA( HANDLE hLogFile, const char * lpMsg, ... )
{
	if( hLogFile == INVALID_HANDLE_VALUE || lpMsg == NULL )
	{
		return;
	}

	char msgBuf[LOG_MSG_BUFFER_SIZE] = {0};
	va_list arglist;

	va_start(arglist, lpMsg);
	vsprintf_s( msgBuf, LOG_MSG_BUFFER_SIZE, lpMsg, arglist);
	va_end(arglist);

	DWORD dwWriteBytes = 0;
	::WriteFile( hLogFile, msgBuf, static_cast<DWORD>( strlen( msgBuf ) ), &dwWriteBytes, NULL );
}

static void WriteLogMsgW( HANDLE hLogFile, const wchar_t * lpMsg, ... )
{
	if( hLogFile == INVALID_HANDLE_VALUE || lpMsg == NULL )
	{
		return;
	}

	wchar_t msgBuf[LOG_MSG_BUFFER_SIZE] = {0};
	va_list arglist;

	va_start(arglist, lpMsg);
	vswprintf_s( msgBuf, LOG_MSG_BUFFER_SIZE, lpMsg, arglist );
	va_end(arglist);

	char msgBufANSI[LOG_MSG_BUFFER_SIZE] = {0};
	::WideCharToMultiByte( CP_ACP, 0, msgBuf, -1, msgBufANSI, LOG_MSG_BUFFER_SIZE, NULL, NULL );

	DWORD dwWriteBytes = 0;
	::WriteFile( hLogFile, msgBufANSI, static_cast<DWORD>( strlen( msgBufANSI ) ), &dwWriteBytes, NULL );
}

#ifdef RELEASE_DEBUG
#if defined(_UNICODE) || defined(UNICODE)
#define WriteLogMsg WriteLogMsgW
#else
#define WriteLogMsg WriteLogMsgA
#endif
#define HLOG	g_hLogFile,
#else
#define HLOG	
#define WriteLogMsg _tcprintf
#endif //RELEASE_DEBUG

extern HANDLE g_hLogFile;

#define PRINT_DEBUG_MSG( MSG, ... ){\
	SYSTEMTIME sysTm;\
	GetLocalTime ( &sysTm );\
	TCHAR tmMsg[64] = {0};\
	_stprintf_s( tmMsg, 64, _T("%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d.%0.3d"), sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds );\
	WriteLogMsg( HLOG _T("%s  [0x%0.4x]  %s    ")##MSG##_T("\r\n"), tmMsg,  ::GetCurrentThreadId(), _T_FUNCTION__, __VA_ARGS__ );\
	}

#endif //__LOG_MESSAGE_H__