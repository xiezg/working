// SampleV.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ToolBox.h"

#include "king/logmsg.h"

#include <iostream>
#include <fstream>

using namespace std;

extern int WriteFileFromMsg( const char * fileName, const std::string & content );
extern int MapFile( const char * fileName, std::string & fileContent );
extern void CheckRcFile( const char * rootPath, lpTraverseFolderCallback cb );
extern void BackupFile( const std::string & fileSrc, const std::string & backup_dir );

#define PRE_FIND_VER_SEC( FILE_CONTENT, ERR_RETVAL )\
	if( nPosWithVersionSection == std::string::npos && ( nPosWithVersionSection = findRcVersionSection( (FILE_CONTENT) ) ) == std::string::npos )\
	{PRINT_DEBUG_MSG( "未找到[VS_VERSION_INFO VERSIONINFO]节." );return (ERR_RETVAL);}

size_t nPosWithVersionSection = std::string::npos;

std::string g_SearchPath;
std::string g_BackupPath;

std::string g_VersionNum = "1.0.0.1";
std::string g_ProductNum = "1.0.0.1";

std::string g_VersionNumV2 = "1,0,0,1";
std::string g_ProductNumV2 = "1,0,0,1";

int g_Revision = 0;
int g_Number = 0;

HANDLE g_hLogFile = NULL;

size_t findRcVersionSection( const std::string & fileContent )
{
	return fileContent.find( "VS_VERSION_INFO VERSIONINFO" ) == std::string::npos ? std::string::npos : std::string::npos + strlen( "VS_VERSION_INFO VERSIONINFO" );
}

std::string findAndReSetVersionSectionV2( std::string & fileContent, const std::string &secName, const std::string & newValue )
{
	PRE_FIND_VER_SEC( fileContent, "" );

	size_t nPosBegin = std::string::npos;

	if( ( nPosBegin = fileContent.find( secName, nPosWithVersionSection ) ) == std::string::npos )
	{
		PRINT_DEBUG_MSG( "未找到[%s]", secName.c_str() );
		return "";
	}

	nPosBegin += secName.size();

	size_t nPosEndLineNewLine = fileContent.find( "\r\n", nPosBegin );

	std::string oldValue = fileContent.substr( nPosBegin, nPosEndLineNewLine - nPosBegin );

	fileContent.replace( nPosBegin, nPosEndLineNewLine - nPosBegin, newValue );

	return oldValue;
}

std::string findAndReSetVersionSection( std::string & fileContent, const std::string & secName, const std::string & newValue  )
{
	PRE_FIND_VER_SEC( fileContent, "" );

	size_t nPosBegin = std::string::npos;

	std::string secNameEx = "\"" + secName + "\"";

	if( ( nPosBegin = fileContent.find( secNameEx, nPosWithVersionSection ) ) == std::string::npos )
	{
		PRINT_DEBUG_MSG( "未找到VersionSection." );
		return "";
	}

	size_t nPosBeginTmp = nPosBegin += secNameEx.size();
	size_t nPosEndLineNewLine = fileContent.find( "\r\n", nPosBegin );
	size_t nPOsEndLineBacklash = fileContent.find( "\\", nPosBegin );

	if( nPOsEndLineBacklash == std::string::npos && nPosEndLineNewLine == std::string::npos )
	{
		PRINT_DEBUG_MSG( "错误的格式：行结束定界符[\\,\\r\\n]未找到." );
		return "";
	}

	size_t nEndLine = -1;
	if( nPOsEndLineBacklash != std::string::npos && nPOsEndLineBacklash < nPosEndLineNewLine )
		nEndLine = nPOsEndLineBacklash;
	else
		nEndLine = nPosEndLineNewLine;

	if( ( nPosBegin = fileContent.find( ",", nPosBegin ) ) == std::string::npos )
		return "";

	if( nPosBegin >= nEndLine )	//查找到的数值超出了本行的范围
	{
		PRINT_DEBUG_MSG( "未检测到定界符[,]，不支持的替换[%s].", fileContent.substr( nPosBeginTmp, nEndLine - nPosBeginTmp ).c_str() );

		return "";
	}

	nPosBegin++;

	std::string oldValue = fileContent.substr( nPosBegin, nEndLine - nPosBegin );

	//替换双引号内的值
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

//#define DISCARD_FILE( fileName )\
//	if( _stricmp( data.cFileName, (fileName) ) == 0 ){PRINT_DEBUG_MSG( "特殊文件[%s]，忽略", (fileName) );continue;}

#define DISCARD_FILE( fileName )

std::string rcFileList;

int Update( LPCTSTR fileName, LPCSTR lpFileName )
{
	DISCARD_FILE( "zlib.rc" );
	DISCARD_FILE( "zlib1.rc" );
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
	DISCARD_FILE( "libcurl.rc" );
	DISCARD_FILE( "curl.rc" );

	//if( _stricmp( data.cFileName + strlen(data.cFileName) - 2, "rc" ) != 0 )		//根据文件扩展名来识别
	if( _stricmp( fileName, "versionno.rc2" ) != 0 )		//仅处理versionno.rc2文件
		return 0;

	PRINT_DEBUG_MSG( "begin process -------[ %s ].", fileName );

	std::string rcFileContent;

	MapFile( fileName, rcFileContent );

	std::string oldValue = findAndReSetFileVersion( rcFileContent, g_VersionNum );
	PRINT_DEBUG_MSG( "[%s] [%s].", fileName, oldValue.c_str() );

	oldValue = findAndReSetProductVersion( rcFileContent, g_ProductNum );
	PRINT_DEBUG_MSG( "[%s] [%s].", fileName, oldValue.c_str() );

	oldValue = findAndReSetVersionSectionV2( rcFileContent, "FILEVERSION ", g_VersionNumV2 );
	PRINT_DEBUG_MSG( "[%s] [%s].", fileName, oldValue.c_str() );

	oldValue = findAndReSetVersionSectionV2( rcFileContent, "PRODUCTVERSION ", g_ProductNumV2 );
	PRINT_DEBUG_MSG( "[%s] [%s].", fileName, oldValue.c_str() );

	BackupFile( fileName, g_BackupPath );
	WriteFileFromMsg( fileName, rcFileContent );

	PRINT_DEBUG_MSG( "end process -------[ %s ].", fileName );

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_SearchPath = argv[1];
	g_BackupPath = argv[2];

	g_Revision = atoi( argv[3] );

	g_hLogFile = OpenLogFile( NULL );

	PRINT_DEBUG_MSG( "SearchPath:[%s].", g_SearchPath.c_str() );

	char tmpbuf[1024] = {0};

	time_t t = time( NULL );
	struct tm t2;
	localtime_s( &t2, &t );

	sprintf_s( tmpbuf, "%d.%0.2d.%0.2d.%d", t2.tm_year + 1900 , t2.tm_mon + 1, t2.tm_mday, g_Number  );
	g_VersionNum = tmpbuf;
	sprintf_s( tmpbuf, "%d,%0.2d,%0.2d,%d", t2.tm_year + 1900 , t2.tm_mon + 1, t2.tm_mday, g_Number  );
	g_VersionNumV2 = tmpbuf;

	memset( tmpbuf, 0, 1024 );

	sprintf_s( tmpbuf, "1.0.%d.%d", g_Revision, g_Number  );
	g_ProductNum = tmpbuf;
	sprintf_s( tmpbuf, "1,0,%d,%d", g_Revision, g_Number  );
	g_ProductNumV2 = tmpbuf;

	CheckRcFile( g_SearchPath.c_str(), &Update );

	return 0;
}