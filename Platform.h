#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

static const float PI = 3.1415926535897932384626433832795f;//3.141592f;
static const float PI2 = 6.283185307179586476925286766559f;//2.0f * PI;
static const float g_fDegToRad = 0.017453292519943295769236907684886f;//PI / 180.0f;
static const float g_fRadToDeg = 57.295779513082320876798154814105f;//180.0f / PI;

#pragma warning(disable:4201)
#pragma warning(disable:4275)
#pragma warning(disable:4100) // unreferenced formal parameter
#pragma warning(disable:4458) // declaration of X hides class member
#pragma warning(disable:4456) // declaration of X hides previous local declaration

#define  _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4244)

#define timeGetTime() ((DWORD)GetTickCount64())

#define IMPLEMENT_ENUM_FLAGS(FlagType) \
	inline FlagType operator~(FlagType a){ return (FlagType)~(uint32)a; } \
	inline FlagType operator|(FlagType a, FlagType b){ return (FlagType)((uint32)a | (uint32)b); } \
	inline FlagType operator&(FlagType a, FlagType b){ return (FlagType)((uint32)a & (uint32)b); }
