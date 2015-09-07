#include "ToolBox.h"

#include <assert.h>
#include <string.h>

#include <tchar.h>

#include "auto_array.h"

/*
	功能：格式化路径（文件路径）到规范格式（目前是去掉路径最后的 "\" "/"，如果有）
	in @ lpFilePath
	out @ lpFilePath
	retval @ NULL
*/
void FormatFilePath( char * lpFilePath )
{
	size_t length = 0;

	if( lpFilePath == NULL || ( length = ::strlen( lpFilePath ) ) == 0 )
	{
		return ;
	}

	do
	{
		if( lpFilePath[length-1] != '\\' && lpFilePath[length-1] != '/' )
		{
			return;
		}

		lpFilePath[length-1] = '\0';

	}while( ( length = strlen( lpFilePath ) ) != 0 );
}

/*
	功能：将指定长度的目标内存置0
	in @ dest 目标内存起始地址
	in @ size 目标内存长度
	retval @ The value of dest
*/
void* _bzero( void * dest, size_t size )
{
	return ::memset( dest, 0, size );
}

/*
	Maps a character string to a wide character (Unicode UTF-16) string
	
	For UTF-8, dwFlags must be set to either 0 or MB_ERR_INVALID_CHARS. Otherwise, the function fails with ERROR_INVALID_FLAGS
	
	MB_ERR_INVALID_CHARS:Fail if an invalid input character is encountered. If this flag is not set, the function silently drops illegal code points. 
	A call to GetLastError returns ERROR_NO_UNICODE_TRANSLATION.

	简化windows API MultiByteToWideChar 的内存分配

	in @ MultiByteStr
	out @ WideCharStr
	retval @ 成功返回 true, 失败返回 false
*/
bool MultiByteToWideChar( UINT CodePage, DWORD dwFlags, const std::string & MultiByteStr, std::wstring & WideCharStr )
{
	if( CodePage == CP_UTF8 )
	{
		dwFlags = ( dwFlags == 0 ) ? 0 : MB_ERR_INVALID_CHARS;
	}

	int cchWideChar = 0;

	if( ( cchWideChar = ::MultiByteToWideChar( CodePage, dwFlags, MultiByteStr.c_str(), static_cast<int>( MultiByteStr.size() ), NULL, 0 ) ) == 0 )
	{
		return false;
	}

	auto_array<wchar_t> tempUnicode( cchWideChar + 1, true );	//windows的wchar_t应该是UTF-16编码

	if( static_cast<wchar_t*>( tempUnicode ) == NULL )
	{
		return false;
	}

	if( ( cchWideChar = ::MultiByteToWideChar( CodePage, dwFlags, MultiByteStr.c_str(), static_cast<int>( MultiByteStr.size() ), static_cast<wchar_t*>( tempUnicode ), cchWideChar ) ) == 0 )
	{
		assert( ::GetLastError() != ERROR_INSUFFICIENT_BUFFER );

		return false;
	}

	WideCharStr = static_cast<wchar_t*>( tempUnicode );

	return true;
}

/*
	Maps a wide character string to a new character string

	UTF-8. With this value set, lpDefaultChar and lpUsedDefaultChar must be set to null pointers, dwFlags must be set to either 0 or WC_ERR_INVALID_CHARS.

	WC_ERR_INVALID_CHARS: Fail if an invalid input character is encountered. If this flag is not set, the function silently drops illegal code points. A call to GetLastError returns ERROR_NO_UNICODE_TRANSLATION. 
	Note that this flag only applies when CodePage is specified as CP_UTF8. It cannot be used with other code page values.

	简化 windows API WideCharToMultiByte 内存分配函数
	in @ WideCharStr 
	out @ MultiByteStr
	retval @ 成功返回 true, 失败返回 false
*/
bool WideCharToMultiByte( UINT CodePage, DWORD dwFlags, const std::wstring & WideCharStr, std::string & MultiByteStr, LPCSTR lpDefaultChar,  LPBOOL lpUsedDefaultChar )
{
	if( CodePage == CP_UTF8 )	//提高函数的容错性
	{
		dwFlags = ( dwFlags == 0 ) ? 0 : WC_ERR_INVALID_CHARS;

		lpDefaultChar = NULL;

		lpUsedDefaultChar = NULL;
	}

	int cbMultiByte = 0;

	if( ( cbMultiByte = ::WideCharToMultiByte( CodePage, dwFlags, WideCharStr.c_str(), static_cast<int>( WideCharStr.size() ), NULL, 0, lpDefaultChar, lpUsedDefaultChar ) ) == 0 )
	{
		return false;
	}

	auto_array<char> tempMultiByte( cbMultiByte + 1, true );

	if( static_cast<char*>( tempMultiByte ) == NULL )
	{
		return false;
	}

	if( ( cbMultiByte = ::WideCharToMultiByte( CodePage, dwFlags, WideCharStr.c_str(), static_cast<int>( WideCharStr.size() ), static_cast<char*>( tempMultiByte ), cbMultiByte, lpDefaultChar, lpUsedDefaultChar ) ) == 0 )
	{
		assert( ::GetLastError() != ERROR_INSUFFICIENT_BUFFER );

		return false;
	}

	MultiByteStr = static_cast<char*>( tempMultiByte );

	return true;
}

/*
	The handle returned is not global or inheritable. It cannot be duplicated or used by another process.

	If dwFlags contains GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
	use care when passing the handle to the FreeLibrary function,because doing so can cause a DLL module to be unmapped prematurely.

	If dwFlags contains GET_MODULE_HANDLE_EX_UNCHANGED_REFCOUNT,this function must be used carefully in a multithreaded application.
	There is no guarantee that the module handle remains valid between the time this function returns the handle and the time it is used. 
	For example, a thread retrieves a module handle, but before it uses the handle, a second thread frees the module. 
	If the system loads another module, it could reuse the module handle that was recently freed. Therefore, first thread would have a handle to a module different than the one intended.

	@IsProtected FALSE: 返回的句柄引用计数值不递增，这样句柄使用完毕后不用FreeLibrary来递减该引用计数，但是该句柄有可能因为所指向的模块被卸载，而失效
				 TRUE: 返回的句柄引用计数值递增，在句柄使用完毕后需要调用FreeLibrary来递减该引用计数，但是可以保证该句柄在使用期间保持有效性。
*/
HMODULE GetLocalModuleHandle( BOOL IsProtected )
{
	HMODULE hModule = NULL;

	const DWORD dwFlags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | ( IsProtected == FALSE ? GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT : 0 );

	::GetModuleHandleEx( dwFlags, reinterpret_cast<LPCTSTR>( GetLocalModuleHandle ), &hModule );

	return hModule;
}

/*

The CMPXCHG8B instruction also requires three operands:

	[a 64-bit value in EDX:EAX]
	[a 64-bit value in ECX:EBX]
	[a destination operand in memory]

The instruction 
	compares the 64-bit value in the EDX:EAX registers with the destination	operand. 

	If they are equal, the 64-bit value in the ECX:EBX register is stored in the destination operand. 
	If the EDX:EAX register and the destination are not equal, the destination is loaded in the EDX:EAX register. 
	
The CMPXCHG8B instruction can be combined with the LOCK prefix to perform the operation atomically.
*/

ULONGLONG WINAPI Xiezg_InterlockedIncrement64( ULONGLONG volatile * Destination )
{
#ifdef _WIN64

	return static_cast<ULONGLONG>( ::InterlockedIncrement64( reinterpret_cast<LONGLONG volatile*>( Destination ) ) );

#else

	ULONGLONG retval = 0;

	__asm
	{
		push edi
		push eax
		push ebx
		push ecx
		push edx
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		mov edi, dword ptr [Destination]
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		mov eax, dword ptr [edi]
		mov edx, dword ptr [edi+4h]
retry:	add eax, 1
		adc edx, 0
		mov ebx, eax
		mov ecx, edx
		sub	eax, 1
		sbb edx, 0
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		lock cmpxchg8b qword ptr[edi]
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		jnz retry
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		mov dword ptr [retval], ebx
		mov dword ptr [retval+4h], ecx
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		pop edx
		pop ecx
		pop ebx
		pop eax
		pop edi
	}

	return retval;

#endif //_WIN64
}

int TraverseFolder( LPCTSTR lpPath, lpTraverseFolderCallback callbackFn, void * ctx )
{
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH] = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if( _tcsclen ( lpPath ) > ( MAX_PATH - 2 ) )
		return -1;

	_tcscpy_s( szDir, MAX_PATH, lpPath );
	_tcscat_s( szDir, _T("\\*.*") );

	if( INVALID_HANDLE_VALUE == ( hFind = FindFirstFile( szDir, &ffd ) ) ) 
	{
		return -1;
	}

	szDir[ _tcsclen( szDir ) - 3 ] = _T('\0');

	do
	{
		_tcscat_s( szDir, ffd.cFileName );

		if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( _tcscmp( ffd.cFileName, "." ) == 0 || _tcscmp( ffd.cFileName, ".." ) == 0 )
				continue;

			TraverseFolder( szDir, callbackFn, ctx );
		}
		else
		{
			callbackFn( szDir, ffd.cFileName, ctx );
		}
	}
	while( szDir[ _tcsclen( szDir ) - _tcsclen( ffd.cFileName ) ] = _T('\0'), FindNextFile( hFind, &ffd ) );

	int retval = ( GetLastError() == ERROR_NO_MORE_FILES ? 0 : -1 );

	FindClose(hFind);

	return retval;
}