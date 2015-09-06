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



void CheckRcFile( const char * rootPath, lpTraverseFolderCallback cb )
{
	TraverseFolder( rootPath, cb );
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