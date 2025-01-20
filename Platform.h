#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

#pragma warning(disable:4100) // unreferenced formal parameter
#pragma warning(disable:4458) // declaration of X hides class member

#define  _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4244)

#define IMPLEMENT_ENUM_FLAGS(FlagType) \
	inline FlagType operator~(FlagType a){ return (FlagType)~(uint32)a; } \
	inline FlagType operator|(FlagType a, FlagType b){ return (FlagType)((uint32)a | (uint32)b); } \
	inline FlagType operator&(FlagType a, FlagType b){ return (FlagType)((uint32)a & (uint32)b); }
