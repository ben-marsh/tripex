#ifndef _ZVECTOR_H
#define _ZVECTOR_H
/*
#include <d3d.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
*/
#include "Misc.h"

class ZMatrix;

class ZVector
{
public:
	union
	{
		struct
		{
			FLOAT32 m_fX, m_fY, m_fZ;
		};
		FLOAT32 m_af[ 3 ];
	};

	// Constructors
	ZVector( );
	ZVector( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// Origin( ):
	static ZVector Origin( );

	// Polar( ):
	static ZVector Polar( FLOAT32 fLength, FLOAT32 fPitch, FLOAT32 fYaw );

	// Set( ):
	void Set( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// operator+( )/operator+=( ):
	ZVector operator+( ) const;
	ZVector operator+( const ZVector &v ) const;
	ZVector &operator+=( const ZVector &v );

	// operator-( )/operator-=( ):
	ZVector operator-( ) const;
	ZVector operator-( const ZVector &v ) const;
	ZVector &operator-=( const ZVector &v );

	// operator*( )/operator*=( ):
	ZVector operator*( FLOAT32 fValue ) const;
	ZVector operator*( const ZMatrix &m ) const;
	ZVector &operator*=( FLOAT32 fValue );
	ZVector &operator*=( const ZMatrix &m );

	// operator/( ):
	ZVector operator/( FLOAT32 fValue ) const;
	ZVector &operator/=( FLOAT32 fValue );

	// operator[ ]:
	FLOAT32 &operator[ ]( int nPos );
	const FLOAT32 &operator[ ]( int nPos ) const;

	// Dot( ):
	FLOAT32 Dot( const ZVector &v ) const;

	// Length( ):
	FLOAT32 Length( ) const;

	// Normal( )/Normalize( ):
	ZVector Normal( FLOAT32 fLength = 1.0f ) const;
	void Normalize( FLOAT32 fLength = 1.0f );

	// Cross( ):
	ZVector Cross( const ZVector &v ) const;

	// GetYaw( )/GetPitch( )/GetRoll( ):
	FLOAT32 GetYaw( ) const;
	FLOAT32 GetPitch( ) const;
	FLOAT32 GetRoll( FLOAT32 fTightness = 0.1f ) const;
};

#include "ZVector.inl"

#endif