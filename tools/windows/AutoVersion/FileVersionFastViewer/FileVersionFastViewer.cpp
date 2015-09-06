// FileVersionFastViewer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

#include "king\logmsg.h"

#pragma comment( lib, "Version.lib" )

#define TMP_BUF_SIZE 1024

HANDLE g_hLogFile = NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	FreeConsole();

	g_hLogFile = OpenLogFile( NULL );

	char buf[TMP_BUF_SIZE] = {0};

	if( GetFileVersionInfo( argv[1], 0, TMP_BUF_SIZE, buf ) == FALSE )
	{
		PRINT_DEBUG_MSG( _T("%s get file version info fails. errNum[%d]"), argv[1], GetLastError() );

		return -1;
	}

	LPVOID lpItem = NULL;
	UINT itemSize = 0;

	if( VerQueryValue( buf, _T("\\"), &lpItem, &itemSize ) == FALSE )
	{
		PRINT_DEBUG_MSG( _T("%s query file version info fails. errNum:%d"), argv[1], GetLastError() );

		return -1;
	}

	VS_FIXEDFILEINFO * lpInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpItem);
	
	char buf2[1024] = {0};

	sprintf_s( buf2,  "文件版本：%d.%d.%d.%d\r\n", 
		HIWORD( lpInfo->dwFileVersionMS ), 
		LOWORD( lpInfo->dwFileVersionMS ),
		HIWORD( lpInfo->dwFileVersionLS ),
		LOWORD( lpInfo->dwFileVersionLS ) );

	sprintf_s( buf2 + strlen(buf2), 1024 - strlen(buf2), "产品版本号：%d.%d.%d.%d",
		HIWORD( lpInfo->dwProductVersionMS ), 
		LOWORD( lpInfo->dwProductVersionMS ),
		HIWORD( lpInfo->dwProductVersionLS ),
		LOWORD( lpInfo->dwProductVersionLS ) );

	MessageBoxA( NULL, buf2, "FileVersionFastViewer", MB_OK );

	return 0;
}

