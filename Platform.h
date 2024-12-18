#pragma once

#define WIN32_LEAN_AND_MEAN

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed char SINT8;
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;

static const FLOAT32 PI = 3.1415926535897932384626433832795f;//3.141592f;
static const FLOAT32 PI2 = 6.283185307179586476925286766559f;//2.0f * PI;
static const FLOAT32 g_fDegToRad = 0.017453292519943295769236907684886f;//PI / 180.0f;
static const FLOAT32 g_fRadToDeg = 57.295779513082320876798154814105f;//180.0f / PI;

#pragma warning(disable:4201)
#pragma warning(disable:4275)

#define  _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4244)

#include <crtdbg.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <conio.h>

#define timeGetTime() ((DWORD)GetTickCount64())

#include "ZColour.h"
#include "ZPoint.h"
#include "ZRect.h"
#include <vector>
