#pragma once

#ifndef _KC_DATA_BASE_MANAGER_H__
#define _KC_DATA_BASE_MANAGER_H__

#include <king\dllmanager.h>

#include <Windows.h>

typedef bool (*kcOpen)(void** bh ,LPCTSTR p);
typedef bool (*kcOpenRead)(void** bh ,LPCTSTR p);
typedef bool (*kcClose)(void** bh);
typedef bool (*kcSet)(void** bh, LPBYTE lpKey, size_t ksize, LPBYTE lpValue, size_t vsize);
typedef size_t (*kcGet)(void** bh, LPBYTE lpKey, size_t ksize, LPBYTE* pstrValue, size_t vsize);

#ifndef FILE_SIGN_SIZE
#define FILE_SIGN_SIZE 16
#endif

namespace King
{
	class localWhiteListCache
	{
	public:

		localWhiteListCache():m_lpKCDataCtx(NULL)
		{
			//
		}

		~localWhiteListCache()
		{
			Close();
		}

		int Open( LPCTSTR lpDataFileName, BOOL IsReadOnly, HMODULE hHostProc = NULL )
		{
			if( lpDataFileName == NULL )
			{
				return -1;
			}

			if( m_lpKCDataCtx != NULL )
			{
				return 0;
			}

			if( m_DllManager.Load( hHostProc, _T("kcdatabase.dll") ) == -1 )
			{
				return -1;
			}

			kcOpen lpKCOpen = reinterpret_cast<kcOpen>( IsReadOnly ? m_DllManager["kcdbOpenRead"] : m_DllManager["kcdbOpen"] );

			if( lpKCOpen == NULL )
			{
				return -1;
			}

			if( !lpKCOpen( &m_lpKCDataCtx, lpDataFileName ) || m_lpKCDataCtx == NULL )
			{
				return -1;
			}

			return 0;
		}

		int IsHit( unsigned char item[FILE_SIGN_SIZE] )
		{
			if( m_lpKCDataCtx == NULL )
			{
				return -1;
			}

			kcGet lpKCGet = reinterpret_cast<kcGet>( m_DllManager["kcdbGet"] );

			if( lpKCGet == NULL )
			{
				return -1;
			}

			int value = 0;
			int * lpValue = &value;
			size_t ValueSize = sizeof(int);

			//return the size of the value, or -1 on failure.
			if( lpKCGet( &m_lpKCDataCtx, reinterpret_cast<LPBYTE>( item ), FILE_SIGN_SIZE, reinterpret_cast<LPBYTE*>( &lpValue ), ValueSize ) == -1 )
			{
				return -1;
			}

			return value;
		}

		int InsertItem( unsigned char item[FILE_SIGN_SIZE] )
		{
			if( m_lpKCDataCtx == NULL )
			{
				return -1;
			}

			kcSet lpKCSet = reinterpret_cast<kcSet>( m_DllManager["kcdbSet"] );

			if( lpKCSet == NULL )
			{
				return -1;
			}

			int value = 0;

			return lpKCSet( &m_lpKCDataCtx, reinterpret_cast<LPBYTE>( item ), FILE_SIGN_SIZE, reinterpret_cast<LPBYTE>( &value ), sizeof(int) ) ? 0 : -1;
		}

		void Close()
		{
			if( m_lpKCDataCtx == NULL )
			{
				return;
			}

			kcClose lpKCClose = reinterpret_cast<kcClose>( m_DllManager["kcdbClose"] );

			if( lpKCClose == NULL )
			{
				return;
			}

			lpKCClose( &m_lpKCDataCtx );
			m_lpKCDataCtx = NULL;

			return;
		}

	private:

		void * m_lpKCDataCtx;

		DllManager m_DllManager;
	};

}

#endif //_KC_DATA_BASE_MANAGER_H__
