#pragma once
//#pragma comment (lib, "lib/xbox_dx8.lib" )

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

//#pragma warning(disable:C4786)

#define  _CRT_SECURE_NO_WARNINGS

// vs6 only
//#pragma warning(push,4)
//#pragma warning(disable:4710) // shit stl
//#pragma warning(disable:4512) // shit compiler
//#pragma warning(disable:4511) // shit compiler
//#pragma warning(disable:4100) // shit stl
//#pragma warning(disable:4663) // shit stl
//#pragma warning(disable:4201) // shit compiler

#pragma warning(disable:4244)

#include <crtdbg.h>
#include <windows.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <conio.h>
#include <d3d9.h>
#include <crtdbg.h>
#include <string.h>

//#include "malloc.h"

#define timeGetTime() ((DWORD)GetTickCount64())

#include <malloc.h>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include <bitset>

//#include <H:\Microsoft Xbox SDK\xbox\include\malloc.h>
using namespace std;

#include "ZAudio.h"

extern ZAudio *g_pAudio;