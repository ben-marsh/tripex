#pragma once

#include "Platform.h"
#include <algorithm>
#include <assert.h>

extern const float PI;
extern const float PI2;
extern const float DEG_TO_RAD;
extern const float FLOAT_ZERO;

uint32 GetSystemTimestampMs();
float Wrap(float fValue, float fMin, float fMax);

template< class T > inline T Bound(T x, T a, T b)
{
	if (x < a)
	{
		return a;
	}
	if (x > b)
	{
		return b;
	}
	return x;
}

inline int IntegerLog2(int value)
{
	int log = 0;
	while (value > (1 << log)) log++;
	assert(1 << log);
	return log;
}

extern const unsigned char g_anBitReverse[256];

template < class T > T ReverseBitOrder(const T src)
{
	T dst;
	const uint8* src_bytes = (const uint8*)&src;
	uint8* dst_bytes = (uint8*)&dst;
	for (int i = 0; i < sizeof(T); i++)
	{
		dst_bytes[sizeof(T) - 1 - i] = g_anBitReverse[src_bytes[i]];
	}
	return dst;
}

template< class T > inline T StepTo(T value, T target, T nStep)
{
	if (value < target)
	{
		return std::min<T>(target, value + nStep);
	}
	else
	{
		return std::max<T>(target, value - nStep);
	}
}
