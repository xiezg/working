#include "stdafx.h"

#include "ToolBox.h"
#include <shlobj.h>
#include "king/logmsg.h"

#pragma comment( lib, "shell32.lib" )

int WriteFileFromMsg( const char * fileName, const std::string & content )
{
	HANDLE hFile = CreateFileA( fileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, 0, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		PRINT_DEBUG_MSG( "create file:%s fails.", fileName );
		return -1;
	}

	DWORD dwWriteNum = 0;

	if( ! WriteFile( hFile, content.data(), (DWORD)content.size(), &dwWriteNum, NULL ) )
	{
		PRINT_DEBUG_MSG( "write file fails." );
	}

	CloseHandle(  hFile);

	return 0;
}

int MapFile( const char * fileName, std::string & fileContent )
{
	int retval = -1;
	LPVOID buf = NULL;
	HANDLE hFile = CreateFileA( fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		PRINT_DEBUG_MSG( "open file fails." );
		return -1;
	}

	HANDLE hMap = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 0, NULL );

	if( hMap == NULL )
		goto ERR_EXIT;

	buf = MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0 );

	if( buf == NULL )
		goto ERR_EXIT;

	fileContent.reserve( GetFileSize( hFile, NULL ) );

	fileContent.append( (const char*)buf, GetFileSize( hFile, NULL ) );

	retval = 0;

ERR_EXIT:

	if( buf != NULL )
		UnmapViewOfFile( buf );

	if( hMap != NULL )
		CloseHandle( hMap );

	if( hFile != INVALID_HANDLE_VALUE )
		CloseHandle( hFile );

	return retval;
}

static int nCount = 0;

#define DISCARD_FILE( fileName )\
	if( _stricmp( data.cFileName, (fileName) ) == 0 ){PRINT_DEBUG_MSG( "特殊文件[%s]，忽略", (fileName) );continue;}

void CheckRcFile( const char * rootPath, void (*lfn)( const std::string & ) )
{
	std::string searchPath = rootPath;
	
	if( searchPath.at( searchPath.size() -1 ) != '\\' )
		searchPath += "\\";

	searchPath += "*";

	WIN32_FIND_DATA data;

	HANDLE hFile = FindFirstFile( searchPath.c_str(), &data );

	do 
	{
		if( strcmp( data.cFileName, ".") == 0 || strcmp( data.cFileName, ".." ) == 0 )
			continue;

		if( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )	//目录
		{
			std::string newRootPath = searchPath.substr( 0, searchPath.size() - 1 );

			newRootPath += data.cFileName;

			CheckRcFile( newRootPath.c_str(), lfn );

			continue;
		}

		DISCARD_FILE( "zlib.rc" );
		DISCARD_FILE( "afxctl.rc" );
		DISCARD_FILE( "afxdb.rc" );
		DISCARD_FILE( "afxisapi.rc" );
		DISCARD_FILE( "afxolecl.rc" );
		DISCARD_FILE( "afxolesv.rc" );
		DISCARD_FILE( "afxprint.rc" );
		DISCARD_FILE( "afxres.rc" );
		DISCARD_FILE( "all.rc" );
		DISCARD_FILE( "app.rc" );
		DISCARD_FILE( "atl.rc" );
		DISCARD_FILE( "atlres.rc" );
		DISCARD_FILE( "atlsrv.rc" );


		if( _stricmp( data.cFileName + strlen(data.cFileName) - 2, "rc" ) != 0 )		//根据文件扩展名来识别
			continue;

		std::string filePath = rootPath;

		if( filePath.at( filePath.size() -1 ) != '\\' )
			filePath += "\\";

		filePath += data.cFileName;

		if( lfn )
			lfn( filePath );

	} while ( FindNextFile( hFile, &data ) );

	FindClose( hFile );
}

void BackupFile( const std::string & fileSrc, const std::string & backup_dir )
{
	std::string newFilePath = fileSrc;

	newFilePath.replace( 
		0, 
		newFilePath.find_first_of( '\\'), 
		backup_dir.substr( 0, backup_dir.at( backup_dir.size() - 1 ) == '\\' ? backup_dir.size() - 1 : std::string::npos ) );

	std::wstring newFilePathUnicode;

	MultiByteToWideChar( CP_ACP, 0, newFilePath, newFilePathUnicode );

	SHCreateDirectory( NULL, newFilePathUnicode.substr( 0 , newFilePathUnicode.rfind( L'\\' ) ).c_str() );

	CopyFile( fileSrc.c_str(), newFilePath.c_str(), FALSE );
}