#include "stdafx.h"


int WriteFileFromMsg( const char * fileName, const std::string & content )
{
	HANDLE hFile = CreateFileA( fileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, 0, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		printf( "create file:%s fails.\r\n", fileName );
		return -1;
	}

	DWORD dwWriteNum = 0;

	if( ! WriteFile( hFile, content.data(), (DWORD)content.size(), &dwWriteNum, NULL ) )
	{
		printf( "write file fails.\r\n" );
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
		printf( "open file fails\r\n" );
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

		if( stricmp( data.cFileName + strlen(data.cFileName) - 2, "rc" ) != 0 )		//根据文件扩展名来识别
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