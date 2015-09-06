// FileVersionFastViewer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

#include <string>

#include "ToolBox.h"
#include "king\logmsg.h"

#pragma comment( lib, "Version.lib" )

#define TMP_BUF_SIZE 1024

HANDLE g_hLogFile = NULL;

std::string g_expectFileVersion;
std::string g_expectFileProductVersion;

int QueryFileVersionInfo( const _TCHAR * fileName, std::string & fileVersion, std::string & fileProductVersion )
{
	char buf[TMP_BUF_SIZE] = {0};

	if( GetFileVersionInfo( fileName, 0, TMP_BUF_SIZE, buf ) == FALSE )
	{
		//PRINT_DEBUG_MSG( _T("%s get file version info fails. errNum[%d]"), fileName, GetLastError() );

		return -1;
	}

	LPVOID lpItem = NULL;
	UINT itemSize = 0;

	if( VerQueryValue( buf, _T("\\"), &lpItem, &itemSize ) == FALSE )
	{
		//PRINT_DEBUG_MSG( _T("%s query file version info fails. errNum:%d"), fileName, GetLastError() );

		return -1;
	}

	VS_FIXEDFILEINFO * lpInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpItem);

	char buf2[1024] = {0};
	sprintf_s( buf2,  "%d.%d.%d.%d", 
		HIWORD( lpInfo->dwFileVersionMS ), 
		LOWORD( lpInfo->dwFileVersionMS ), 
		HIWORD( lpInfo->dwFileVersionLS ), 
		LOWORD( lpInfo->dwFileVersionLS ) );
	fileVersion = buf2;

	memset( buf2, 0, 1024 );
	sprintf_s( buf2, "%d,%d,%d,%d",
		HIWORD( lpInfo->dwProductVersionMS ), 
		LOWORD( lpInfo->dwProductVersionMS ),
		HIWORD( lpInfo->dwProductVersionLS ),
		LOWORD( lpInfo->dwProductVersionLS ) );
	fileProductVersion = buf2;

	return 0;
}

int FastViewerFileVersinInfo( const _TCHAR * fileName )
{
	std::string fileVersion, fileProductVersion;

	if( QueryFileVersionInfo( fileName, fileVersion, fileProductVersion ) == -1 )
		return -1;

	char buf2[1024] = {0};
	sprintf_s( buf2, "文件版本号：%s\r\n文件产品号：%s", fileVersion.c_str(), fileProductVersion.c_str() );
	MessageBoxA( NULL, buf2, PathFindFileNameA( fileName ), MB_OK );

	return 0;
}

int TraverseFolderCb( LPCTSTR filePath, LPCTSTR fileName )
{
	LPCSTR lpFileExe = PathFindExtension( filePath );

	if( _tcscmp( lpFileExe, _T(".exe") ) != 0 && _tcscmp( lpFileExe, _T(".sys") ) != 0 && _tcscmp( lpFileExe, _T(".dll") ) != 0 )
		return 0;

	std::string fileVersion, fileProductVersion;

	if( QueryFileVersionInfo( filePath, fileVersion, fileProductVersion ) == -1 )
		return 0;

	if( fileVersion == g_expectFileVersion && fileProductVersion == g_expectFileProductVersion )
	{
		PRINT_DEBUG_MSG( _T("[%s][%s][%s]"), filePath, fileVersion.c_str(), fileProductVersion.c_str() );
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_hLogFile = OpenLogFile( NULL );

	if( _tcsicmp( argv[1], _T("/v") ) == 0 )
		return FastViewerFileVersinInfo( argv[2] );

	if( _tcsicmp( argv[1], _T("/c") ) == 0 )
	{
		g_expectFileVersion = argv[2];
		g_expectFileProductVersion  = argv[3];
		std::string searchFilePath = argv[4];

		TraverseFolder( argv[4], TraverseFolderCb );

		return 0;
	}

	return 0;
}