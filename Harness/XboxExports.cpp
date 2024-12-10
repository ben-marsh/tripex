#include "StdAfx.h"
#include "Exp2Dll.h"
#include "XboxExports.h"

#define OPTIMIZED_BUILD

#define FPU_DOUBLE(x) double x; __asm { fstp x }
#define FPU_DOUBLES(x,y) double x,y; __asm { fstp y } __asm { fstp x }

#define EXPORT_FN( x, y, z ) printf("%s:%s\n", x, y ), new Exp2Dll( x, y, ( long )z )

#define EXPORT_CDECL( x, y ) EXPORT_FN( x, #y, ( long )&y )
#define EXPORT_STDCALL( x, y, z ) EXPORT_FN( x, "_" #y "@" #z, ( long )&y )

namespace XboxExports
{
#ifndef OPTIMIZED_BUILD
	double _CIatan2( )
	{
		FPU_DOUBLES(x,y);
		return atan2(x,y); 
	}
	double _CIcos( )
	{ 
		FPU_DOUBLE(x);
		return cos(x); 
	}
	double _CIexp( )
	{
		FPU_DOUBLE(x);
		return exp(x);
	}
	double _CIfmod( )
	{ 
		FPU_DOUBLES(x,y);
		return fmod(x,y);
	}
	double _CIsin( )
	{ 
		FPU_DOUBLE(x);
		return sin(x); 
	}
	double _CIsqrt( )
	{ 
		FPU_DOUBLE(x);
		return sqrt(x);
	}
#endif
	double _CIpow( )
	{
		FPU_DOUBLES(x,y);
		return pow(x,y);
	}

	/*---------------------------------
	* Init( ):
	-----------------------------------*/

	void Init( )
	{
#ifndef OPTIMIZED_BUILD
		EXPORT_CDECL( "msvcr71.dll", _CIatan2 );
		EXPORT_CDECL( "msvcr71.dll", _CIcos );
		EXPORT_CDECL( "msvcr71.dll", _CIexp );
		EXPORT_CDECL( "msvcr71.dll", _CIfmod );
		EXPORT_CDECL( "msvcr71.dll", _CIsin );
		EXPORT_CDECL( "msvcr71.dll", _CIsqrt );
#endif
		EXPORT_CDECL( "msvcr71.dll", _CIpow );
		EXPORT_CDECL( "msvcr71.dll", _copysign );
		EXPORT_CDECL( "msvcr71.dll", _errno );
		EXPORT_CDECL( "msvcr71.dll", _purecall );
		EXPORT_CDECL( "msvcr71.dll", _stricmp );
		EXPORT_CDECL( "msvcr71.dll", _vsnprintf );
		EXPORT_CDECL( "msvcr71.dll", abort );
		EXPORT_CDECL( "msvcr71.dll", atexit );
		EXPORT_CDECL( "msvcr71.dll", atoi );
		EXPORT_CDECL( "msvcr71.dll", isalnum );
		EXPORT_CDECL( "msvcr71.dll", isdigit );
		EXPORT_CDECL( "msvcr71.dll", isspace );
		EXPORT_CDECL( "msvcr71.dll", printf );
		EXPORT_CDECL( "msvcr71.dll", puts );
		EXPORT_CDECL( "msvcr71.dll", rand );
		EXPORT_CDECL( "msvcr71.dll", sprintf );
		EXPORT_CDECL( "msvcr71.dll", srand );
		EXPORT_CDECL( "msvcr71.dll", exit );
		EXPORT_CDECL( "msvcr71.dll", strncpy );
		EXPORT_CDECL( "winmm.dll", timeGetTime );
		EXPORT_STDCALL( "xbox-dx8.dll", D3DXCreateTexture, 32 );
		EXPORT_STDCALL( "xbox-dx8.dll", D3DXCreateTextureFromFileInMemory, 16 );
	}
};

/*
	new Exp2Dll( "winmm.dll", "timeGetTime", ( long )&timeGetTime );
	new Exp2Dll( "xbox-dx8.dll", "D3DXCreateTexture", ( long )&XBoxDX8::D3DXCreateTexture );
	new Exp2Dll( "xbox-dx8.dll", "D3DXCreateTextureFromFileInMemory", ( long )&XBoxDX8::D3DXCreateTextureFromFileInMemory );

*/