// GetIdentifyingPicture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <Windows.h>

#include "curl/curl.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	HANDLE hFile = reinterpret_cast<HANDLE>( userdata );

	DWORD dwRemainDataSize = static_cast<DWORD>( size * nmemb );

	while( dwRemainDataSize > 0 )
	{
		DWORD dwNumberOfBytesWritten = 0;

		if( ! WriteFile( hFile, ptr, dwRemainDataSize, &dwNumberOfBytesWritten, NULL ) )
			return 0;

		ptr += dwNumberOfBytesWritten;
		dwRemainDataSize -= dwNumberOfBytesWritten;
	}

	return size * nmemb;
}

int GetPic()
{
	CURL * handle = curl_easy_init();

	HANDLE hFile = CreateFile( _T("File.jpg"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );

	curl_easy_setopt( handle, CURLOPT_WRITEDATA, hFile );
	curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, write_callback );
	curl_easy_setopt( handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt( handle, CURLOPT_URL, "https://kyfw.12306.cn/otn/passcodeNew/getPassCodeNew?module=login&rand=sjrand" );

	CURLcode retval = curl_easy_perform( handle );

	CloseHandle( hFile );

	curl_easy_cleanup( handle );

	return 0;
}

size_t write_callback_v2(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string msg;
	msg.append( ptr, size * nmemb );
	msg.append( 1, '\0' );

	printf( "%s", msg.c_str() );

	return size * nmemb;
}


int checkRandCodeAnsyn()
{
	CURL * handle = curl_easy_init();

	//curl_easy_setopt( handle, CURLOPT_VERBOSE, 1 ); 
	curl_easy_setopt( handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, write_callback_v2 );
	curl_easy_setopt( handle, CURLOPT_URL, "https://kyfw.12306.cn/otn/passcodeNew/checkRandCodeAnsyn" );
	curl_easy_setopt( handle, CURLOPT_POSTFIELDS, "randCode=178%2C46%2C117%2C54&rand=sjrand");

	CURLcode retval = curl_easy_perform( handle );

	long response_code = 0;
	curl_easy_getinfo( handle, CURLINFO_RESPONSE_CODE, &response_code );
	printf( "server response:%d\r\n", response_code );

	curl_easy_cleanup( handle );

	return 0;
}

int LogInAysnSuggest()
{
	CURL * handle = curl_easy_init();

	curl_easy_setopt( handle, CURLOPT_VERBOSE, 1 ); 
	curl_easy_setopt( handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, write_callback_v2 );
	curl_easy_setopt( handle, CURLOPT_URL, "https://kyfw.12306.cn//otn/login/loginAysnSuggest" );
	curl_easy_setopt( handle, CURLOPT_POSTFIELDS, "loginUserDTO.user_name=646639655%40qq.com&userDTO.password=heapfree096625&randCode=178%2C46%2C117%2C54");

	CURLcode retval = curl_easy_perform( handle );

	long response_code = 0;
	curl_easy_getinfo( handle, CURLINFO_RESPONSE_CODE, &response_code );
	printf( "server response:%d\r\n", response_code );

	curl_easy_cleanup( handle );

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	checkRandCodeAnsyn();

	return 0;
}

