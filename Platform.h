#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed char SINT8;
typedef signed short SINT16;
typedef signed int SINT32;

static const float PI = 3.1415926535897932384626433832795f;//3.141592f;
static const float PI2 = 6.283185307179586476925286766559f;//2.0f * PI;
static const float g_fDegToRad = 0.017453292519943295769236907684886f;//PI / 180.0f;
static const float g_fRadToDeg = 57.295779513082320876798154814105f;//180.0f / PI;

#pragma warning(disable:4201)
#pragma warning(disable:4275)

#define  _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4244)

#define timeGetTime() ((DWORD)GetTickCount64())
