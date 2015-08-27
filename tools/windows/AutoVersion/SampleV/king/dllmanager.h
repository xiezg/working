#pragma once

#ifndef __DLL_MANAGER__H__
#define __DLL_MANAGER__H__

#include <Windows.h>
#include <Shlwapi.h>

#include <king\LogMsg.h>

#include <string>

#if _UNICODE || UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

namespace King
{
	class DllManager
	{
	public:

		DllManager():m_hModule(NULL)
		{
			//
		}

		DllManager( LPTSTR lpModulePath, LPTSTR lpModuleName ):m_hModule(NULL)
		{
			Load( lpModulePath, lpModuleName );
		}

		DllManager( HMODULE hLocalModule, LPTSTR lpModuleName ):m_hModule(NULL)
		{
			Load(  hLocalModule, lpModuleName );
		}

		~DllManager()
		{
#if 0
			UnLoad();
#endif
		}

	public:

		int Load( HMODULE hLocalModule, LPCTSTR lpModuleName )
		{
			if( lpModuleName == NULL )
			{
				return -1;
			}

			TCHAR moduleFileDir[MAX_PATH] = {0};

			::GetModuleFileName( hLocalModule, moduleFileDir, MAX_PATH );
			::PathRemoveFileSpec( moduleFileDir );

			return Load( moduleFileDir, lpModuleName );
		}

		int Load( LPCTSTR lpModulePath, LPCTSTR lpModuleName )
		{
			if( lpModulePath == NULL || lpModuleName == NULL )
			{
				return -1;
			}

			TCHAR moduleFilePath[MAX_PATH] = {0};

			if( PathCombine( moduleFilePath, lpModulePath, lpModuleName ) == NULL || !PathFileExists( moduleFilePath ) )
			{
				return -1;
			}

			return Load( moduleFilePath );
		}

		int Load( LPCTSTR lpModuleFilePath )
		{
			if( m_hModule != NULL )
			{
				return 0;	//�Ѿ��ɹ����أ�����Ҫ�ظ����أ�ֱ�ӷ���
			}

			if( lpModuleFilePath == NULL )
			{
				return -1;
			}

			if( !PathFileExists( lpModuleFilePath ) )
			{
				PRINT_DEBUG_MSG( _T("Ŀ���ļ�[%s]������"), lpModuleFilePath );

				return -1;
			}

			if( ( m_hModule = ::LoadLibrary( lpModuleFilePath ) ) == NULL )
			{
				PRINT_DEBUG_MSG( _T("Ŀ��ģ��[%s]����ʧ��. errNum[%d]"), lpModuleFilePath, GetLastError() );

				return -1;
			}

			return 0;
		}

		int UnLoad()
		{
			if( m_hModule == NULL )
			{
				return 0;
			}

			if( !::FreeLibrary( m_hModule ) )
			{
				PRINT_DEBUG_MSG( _T("�ͷ�ģ��[%s]ʧ��. errNum[%d]"), this->GetModuleFileName().c_str(), GetLastError() );

				return -1;
			}

			m_hModule = NULL;

			return 0;
		}

	public:

		FARPROC operator[]( LPCSTR lpProcName )
		{
			if( m_hModule == NULL )
			{
				return NULL;
			}

			return ::GetProcAddress( m_hModule, lpProcName );
		}

		_tstring GetModuleFileName()
		{
			if( m_hModule == NULL )
			{
				return _tstring( _T("") );
			}

			TCHAR moduleFilePath[MAX_PATH] = {0};

			::GetModuleFileName( m_hModule, moduleFilePath, MAX_PATH );

			return _tstring( moduleFilePath );
		}

	private:

		HMODULE m_hModule;
	};
}

#endif //__DLL_MANAGER__H__