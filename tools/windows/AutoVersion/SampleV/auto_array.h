#pragma once

#ifndef AUTO_ARRAY_H_
#define AUTO_ARRAY_H_

#include <new>

template<class T> class auto_array
{
public:

	auto_array():size( 0 ), lpArray ( NULL )
	{
		//
	}

	auto_array( size_t arraySize, bool IsZeroMem ):size( 0 ), lpArray ( NULL )
	{
		reset( arraySize, IsZeroMem );
	}

	~auto_array()
	{
		release();
	}

	void reset( size_t arraySize, bool IsZeroMem = false )
	{
		release();

		if( ( lpArray = new(std::nothrow) T[ arraySize ] ) != NULL )
		{
			size = arraySize;

			if( IsZeroMem )
			{
				::memset( lpArray, 0, arraySize * sizeof(T) );
			}
		}
	}

	void release()
	{
		if( lpArray != NULL )
		{
			delete[] lpArray;

			lpArray = NULL;

			size = 0;
		}
	}

	operator T * ()
	{
		return lpArray;
	}

	operator size_t ()
	{
		return size;
	}

private:

	T * lpArray;

	size_t size;
};

#endif //AUTO_ARRAY_H_