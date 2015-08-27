#pragma once

#ifndef _LOCK_H__
#define _LOCK_H__

#include <Windows.h>

namespace King
{
	class csLock
	{
		friend class AutoCSLock;

	public:

		csLock()
		{
			::InitializeCriticalSection( &m_cs );
		}

		~csLock()
		{
			::DeleteCriticalSection( &m_cs );
		}

	protected:

		CRITICAL_SECTION m_cs;
	};

	class AutoCSLock
	{
	public:

		AutoCSLock( csLock * lpCS ):m_lpCS(NULL)
		{
			m_lpCS = lpCS;

			EnterCriticalSection( &m_lpCS->m_cs );
		}

		~AutoCSLock()
		{
			if( m_lpCS != NULL )
			{
				LeaveCriticalSection( &m_lpCS->m_cs );
				m_lpCS = NULL;
			}
		}

	private:

		csLock * m_lpCS;
	};

}
#endif 