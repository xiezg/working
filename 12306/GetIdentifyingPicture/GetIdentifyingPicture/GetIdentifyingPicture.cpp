// GetIdentifyingPicture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

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

int _tmain(int argc, _TCHAR* argv[])
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

