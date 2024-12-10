#pragma once

#define WIN32_LEAN_AND_MEAN

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed char SINT8;
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;

static const FLOAT32 PI = 3.141592f;
static const FLOAT32 PI2 = 2.0f * PI;

/*
static const FLOAT32 PI = 3.141592f;
static const FLOAT32 PI2 = 2.0f * PI;
static const FLOAT32 g_fDegToRad = PI / 180.0f;
static const FLOAT32 g_fRadToDeg = 180.0f / PI;
*/
#pragma warning(disable:4201)
#pragma warning(disable:4275)
/*
#include <windows.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <conio.h>
#include <d3d8.h>
#include <crtdbg.h>
#include <string.h>
#include "malloc.h"

#include <string>
#include <set>
#include <map>
#include <vector>
#include <bitset>
#include <algorithm>
using namespace std;

#include "ZAudio.h"

extern ZAudio *g_pAudio;
*/