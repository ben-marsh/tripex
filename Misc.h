#ifndef __MISC_H
#define __MISC_H

#pragma warning(disable: 4786)
#pragma runtime_checks( "",off )

#define FLOAT_ZERO 0.00001

// FormatString( ):
//string FormatString( const char *sFormat, ... );

// FormatStringV( ):
//string FormatStringV( const char *sFormat, va_list pArg );

// Wrap( ):
FLOAT32 Wrap( FLOAT32 fValue, FLOAT32 fMin, FLOAT32 fMax );

/*---------------------------------------------
* Bound( ):
---------------------------------------------*/

template< class T > inline T Bound( T x, T a, T b )
{
	if( x < a )
	{
		return a;
	}
	if( x > b )
	{
		return b;
	}
	return x;
}

/*---------------------------------------------
* IntegerLog2( ):
---------------------------------------------*/

inline int IntegerLog2( int nValue )
{
	int nLog = 0;
	while( nValue > ( 1 << nLog ) ) nLog++;
	_ASSERT( 1 << nLog );
	return nLog;
}

/*---------------------------------------------
* ReverseBitOrder( ):
---------------------------------------------*/

extern const unsigned char g_anBitReverse[ 256 ];

template < class T > T ReverseBitOrder( const T tSrc )
{
	T tDst;
	const unsigned char *anSrc = ( const unsigned char* )&tSrc;
	unsigned char *anDst = ( unsigned char* )&tDst;
	for( int i = 0; i < sizeof( T ); i++ )
	{
		anDst[ sizeof( T ) - 1 - i ] = g_anBitReverse[ anSrc[ i ] ];
	}
	return tDst;
}

/*---------------------------------------------
* StepTo( ):
---------------------------------------------*/

template< class T > inline T StepTo( T nValue, T nTarget, T nStep )
{
	if( nValue < nTarget )
	{
		return min( nTarget, nValue + nStep );
	}
	else 
	{
		return max( nTarget, nValue - nStep );
	}
}

#endif