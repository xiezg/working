#pragma once

#ifndef TOOL_BOX_H
#define TOOL_BOX_H

#include <string>

#include <Windows.h>

typedef int (*lpTraverseFolderCallback)( LPCTSTR lpFilePath, LPCTSTR lpFileName, void * ctx );

void FormatFilePath( char * lpFilePath );

void* _bzero( void * dest, size_t size );

bool MultiByteToWideChar( UINT CodePage, DWORD dwFlags, const std::string & MultiByteStr, std::wstring & WideCharStr );

bool WideCharToMultiByte( UINT CodePage, DWORD dwFlags, const std::wstring & WideCharStr, std::string & MultiByteStr, LPCSTR lpDefaultChar,  LPBOOL lpUsedDefaultChar );

HMODULE GetLocalModuleHandle( BOOL IsProtected );

ULONGLONG WINAPI Xiezg_InterlockedIncrement64( ULONGLONG volatile * Destination );

int TraverseFolder( LPCTSTR lpPath, lpTraverseFolderCallback, void * );

#endif //TOOL_BOX_H