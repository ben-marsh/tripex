#include "StdAfx.h"

BOOL EntryPoint( HINSTANCE hInstance, DWORD dwReason, void *pReserved )
{
	return TRUE;
}

#ifndef _DEBUG

extern "C"
{ 
//	int _fltused = 0x9875;

	/*---------------------------------
	* CRT functions which can be used without initialisation:
	-----------------------------------*/
/*
	void abort( )
	{
	}
	void exit( int )
	{
	}
*/
//	long _ftol2( float ){ return 0; }
//	void *memmove( void*, const void*, size_t ){ return 0; }
//	double _CIexp( double a ){ return exp( a ); }
//	double _CIpow( double a, double b ){ 0.0; }
//	double _CIlog( double ){ return 0.0; }
//	double _CIcos( double a ){ return cos( a ); }
//	double _CIsin( double a ){ return sin( a ); }
//	double _CIatan2( double a, double b ){ return atan2(a,b); }
//	double _CIsqrt( double a ){ return sqrt(a); }
//	char *strrchr( const char*, int n ){ return 0; }
//	char *strncpy( char*, const char*, size_t ){ return 0; }
//	void _chkstk( ){ }
/*
	double _CIcos( double a )
	{
		double r;
		__asm
		{
			fld a
			fcos
			fstp &r
		}
		return r;
	}


	int *_errno( )
	{
		return 0;
	}
	int _vsnprintf( char *sBuf, size_t nSize, const char *, va_list )
	{
		if( nSize > 0 )
		{
			sBuf[ 0 ] = 0;
			return 1;
		}
		else
		{
			return -1;
		}
	}
	int puts( const char* )
	{
		return 1;
	}
	int atexit( void (*)(void) )
	{
		return 0;
	}
	void srand( unsigned int )
	{
	}
	int rand( )
	{
		return 0;
	}
	int isdigit( int n )
	{
		return ( n >= '0' && n <= '9' );
	}
	int isspace( int n )
	{
		return ( n >= 0x09 && n <= 0x0d ) || n == 0x20;
	}
	int isalpha( int n )
	{
		return ( n >= 'A' && n <= 'Z' ) || ( n >= 'a' && n <= 'z' );
	}
	int isalnum( int n )
	{
		return isdigit( n ) || isalpha( n );
	}
	int atoi( const char *sStr )
	{
		while(isspace(*sStr)) sStr++;

		int nRes = 0;
		int nSign = +1;
		if( *sStr == '-' )
		{
			sStr++;
			nSign = -nSign;
		}

		for( ; isdigit(*sStr); sStr++)
		{
			nRes = nRes * 10;
			nRes += ( *sStr ) - '0';
		}
        return nRes;
	}
	int sprintf( char *, const char*, ... )
	{
		return 0;
	}

	int tolower( int n )
	{
		if( !isalpha( n ) ) return n;
		else return n + ( 'a' - 'A' );
	}

	int _stricmp( const char *sA, const char *sB )
	{
		int nDiff;
		for(;;)
		{
			int nDiff = tolower( *( sB++ ) ) - tolower( *( sA++ ) );
			if( *sA == 0 || nDiff != 0 ) return nDiff;
		}
	}
	double _CIfmod( double x, double y )
	{
		double fDiv = x / y;
		if( fDiv < 0.0f ) fDiv -= 1.0f; // cast to int will round to zero
		return x - ( ( int )fDiv ) * y;
	}
	double _CIpow( double fBase, double fExp )
	{
		return exp( fExp * log( fBase ) );
	}
	int _purecall( )
	{
		return 0;
	}
	double _copysign( double x, double y )
	{
		if( y < 0.0 ) return -fabs( x );
		else return +fabs( x );
	}
	void abort( )
	{
	}
	void exit( int )
	{
	}
	int _CrtDbgReport( int nType, const char *sFilename, int nLine, const char *sModule, const char *sFormat, ... )
	{
		__asm int 0x3;
	}

//	void _assert( int n )
//	{
//		if( !n )
//		{
//			__asm int 0x03;
//		}
//	}
*/
};

#endif

// TODO: FORZA: SORT THIS OUT

void *operator new( size_t nSize )
{
	return malloc( nSize );
}
void *operator new[]( size_t nSize )
{
	return malloc( nSize );
}
//void *operator new( size_t nSize, void *pMem )
//{
//	return pMem;
//}
void operator delete( void *pData )
{
	free( pData );
}
void operator delete[]( void *pData )
{
	free( pData );
}
//void operator delete( void *, void * )
//{
//}