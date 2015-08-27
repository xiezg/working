// SampleV.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <iostream>
#include <fstream>

using namespace std;

extern int WriteFileFromMsg( const char * fileName, const std::string & content );
extern int MapFile( const char * fileName, std::string & fileContent );
extern void CheckRcFile( const char * rootPath, void (*lfn)( const std::string & ) );
extern void BackupFile( const std::string & fileSrc, const std::string & backup_dir );

#define PRE_FIND_VER_SEC( FILE_CONTENT, ERR_RETVAL )\
	if( nPosWithVersionSection == std::string::npos && ( nPosWithVersionSection = findRcVersionSection( (FILE_CONTENT) ) ) == std::string::npos )\
	{printf( "δ�ҵ�[VS_VERSION_INFO VERSIONINFO]��\r\n" );return (ERR_RETVAL);}

size_t nPosWithVersionSection = std::string::npos;

std::string g_SearchPath;
std::string g_BackupPath;

size_t findRcVersionSection( const std::string & fileContent )
{
	return fileContent.find( "VS_VERSION_INFO VERSIONINFO" ) == std::string::npos ? std::string::npos : std::string::npos + strlen( "VS_VERSION_INFO VERSIONINFO" );
}

std::string findAndReSetVersionSection( std::string & fileContent, const std::string & secName, const std::string & newValue  )
{
	PRE_FIND_VER_SEC( fileContent, "" );

	size_t nPosBegin = std::string::npos;

	std::string secNameEx = "\"" + secName + "\"";

	if( ( nPosBegin = fileContent.find( secNameEx, nPosWithVersionSection ) ) == std::string::npos )
	{
		printf( "δ�ҵ�VersionSection\r\n" );
		return "";
	}

	size_t nPosBeginTmp = nPosBegin += secNameEx.size();
	size_t nPosEndLineNewLine = fileContent.find( "\r\n", nPosBegin );
	size_t nPOsEndLineBacklash = fileContent.find( "\\", nPosBegin );

	if( nPOsEndLineBacklash == std::string::npos && nPosEndLineNewLine == std::string::npos )
	{
		printf( "����ĸ�ʽ���н��������[\\,\\r\\n]δ�ҵ�\r\n" );
		return "";
	}

	size_t nEndLine = -1;
	if( nPOsEndLineBacklash != std::string::npos && nPOsEndLineBacklash < nPosEndLineNewLine )
		nEndLine = nPOsEndLineBacklash;
	else
		nEndLine = nPosEndLineNewLine;

	if( ( nPosBegin = fileContent.find( ",", nPosBegin ) ) == std::string::npos )
		return "";

	if( nPosBegin >= nEndLine )	//���ҵ�����ֵ�����˱��еķ�Χ
	{
		printf( "δ��⵽�����[,]����֧�ֵ��滻[%s]\r\n", fileContent.substr( nPosBeginTmp, nEndLine - nPosBeginTmp ).c_str() );

		return "";
	}

	nPosBegin++;

	std::string oldValue = fileContent.substr( nPosBegin, nEndLine - nPosBegin );

	//�滻˫�����ڵ�ֵ
	fileContent.replace( nPosBegin, nEndLine - nPosBegin, " \"" + newValue + "\"" );

	return oldValue;
}

std::string findAndReSetFileVersion( std::string & fileContent, const std::string & fileNewVersion )
{
	return findAndReSetVersionSection( fileContent, "FileVersion", fileNewVersion );
}

std::string findAndReSetProductVersion( std::string & fileContent, const std::string & fileNewVersion )
{
	return findAndReSetVersionSection( fileContent, "ProductVersion", fileNewVersion );
}

void Update( const std::string & fileName )
{
	printf( "begin process -------[ %s ]\r\n", fileName.c_str() );

	std::string rcFileContent;

	MapFile( fileName.c_str(), rcFileContent );

	std::string oldValue = findAndReSetFileVersion( rcFileContent, "-.-.-.-" );
	printf( "[%s] [%s]\r\n", fileName.c_str(), oldValue.c_str() );
	
	oldValue = findAndReSetProductVersion( rcFileContent, "0.0.0.0" );

	BackupFile( fileName, g_BackupPath );
	WriteFileFromMsg( fileName.c_str(), rcFileContent );

	printf( "end process -------[ %s ]\r\n", fileName.c_str() );
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_SearchPath = argv[1];
	g_BackupPath = argv[2];

	CheckRcFile( g_SearchPath.c_str(), &Update );

	return 0;
}