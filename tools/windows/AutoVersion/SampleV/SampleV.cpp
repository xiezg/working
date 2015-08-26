// SampleV.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <fstream>

using namespace std;

extern int WriteFileFromMsg( const char * fileName, const std::string & content );
extern int MapFile( const char * fileName, std::string & fileContent );
extern void CheckRcFile( const char * rootPath, void (*lfn)( const std::string & ) );

#define PRE_FIND_VER_SEC( FILE_CONTENT, ERR_RETVAL )\
	if( nPosWithVersionSection == std::string::npos && ( nPosWithVersionSection = findRcVersionSection( (FILE_CONTENT) ) ) == std::string::npos )\
		return (ERR_RETVAL)

size_t nPosWithVersionSection = std::string::npos;

size_t findRcVersionSection( const std::string & fileContent )
{
	return fileContent.find( "VS_VERSION_INFO VERSIONINFO" ) == std::string::npos ? std::string::npos : std::string::npos + strlen( "VS_VERSION_INFO VERSIONINFO" );
}

std::string findAndReSetVersionSection( std::string & fileContent, const std::string & secName, const std::string & newValue  )
{
	PRE_FIND_VER_SEC( fileContent, "" );

	size_t nPosBegin = -1;

	std::string secNameEx = "\"" + secName + "\"";

	if( ( nPosBegin = fileContent.find( secNameEx, nPosWithVersionSection ) ) == std::string::npos )
	{
		printf( "未找到VersionSection\r\n" );
		return "";
	}

	size_t nPosBeginTmp;
	nPosBeginTmp = nPosBegin += secNameEx.size();

	size_t nEndLine = fileContent.find( "\r\n", nPosBegin );

	//被替换的数值必须是 "value" 格式,即由双引号引起
	if( ( nPosBegin = fileContent.find( "\"", nPosBegin ) ) == std::string::npos )
		return "";

	if( nPosBegin >= nEndLine )	//查找到的数值超出了本行的范围
	{
		printf( "未检测到双引号，不支持的替换[%s]\r\n", fileContent.substr( nPosBeginTmp, nEndLine - nPosBeginTmp ).c_str() );

		return "";
	}
	nPosBegin++;

	size_t nPosEnd = -1;

	if( ( nPosEnd = fileContent.find( "\"", nPosBegin ) ) == std::string::npos )
		return "";

	std::string oldValue = fileContent.substr( nPosBegin, nPosEnd - nPosBegin );

	//替换双引号内的值
	fileContent.replace( nPosBegin, nPosEnd - nPosBegin, newValue );

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

	std::string oldValue = findAndReSetFileVersion( rcFileContent, "2.2.2.2" );
	printf( "[%s] [%s]\r\n", fileName.c_str(), oldValue.c_str() );
	
	oldValue = findAndReSetProductVersion( rcFileContent, "3.3.3.3" );

	WriteFileFromMsg( fileName.c_str(), rcFileContent );

	printf( "end process -------[ %s ]\r\n", fileName.c_str() );
}

int _tmain(int argc, _TCHAR* argv[])
{
	CheckRcFile( "C:\\Users\\Xiezg\\Desktop\\rc_test", &Update );

	return 0;
}