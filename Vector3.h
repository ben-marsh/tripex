#pragma once

/*
#include <d3d.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
*/
#include "Misc.h"

class Matrix44;

class Vector3
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
	Vector3( );
	Vector3( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// Origin( ):
	static Vector3 Origin( );

	// Polar( ):
	static Vector3 Polar( FLOAT32 fLength, FLOAT32 fPitch, FLOAT32 fYaw );

	// Set( ):
	void Set( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// operator+( )/operator+=( ):
	Vector3 operator+( ) const;
	Vector3 operator+( const Vector3 &v ) const;
	Vector3 &operator+=( const Vector3 &v );

	// operator-( )/operator-=( ):
	Vector3 operator-( ) const;
	Vector3 operator-( const Vector3 &v ) const;
	Vector3 &operator-=( const Vector3 &v );

	// operator*( )/operator*=( ):
	Vector3 operator*( FLOAT32 fValue ) const;
	Vector3 operator*( const Matrix44 &m ) const;
	Vector3 &operator*=( FLOAT32 fValue );
	Vector3 &operator*=( const Matrix44 &m );

	// operator/( ):
	Vector3 operator/( FLOAT32 fValue ) const;
	Vector3 &operator/=( FLOAT32 fValue );

	// operator[ ]:
	FLOAT32 &operator[ ]( int nPos );
	const FLOAT32 &operator[ ]( int nPos ) const;

	// Dot( ):
	FLOAT32 Dot( const Vector3 &v ) const;

	// Length( ):
	FLOAT32 Length( ) const;

	// Normal( )/Normalize( ):
	Vector3 Normal( FLOAT32 fLength = 1.0f ) const;
	void Normalize( FLOAT32 fLength = 1.0f );

	// Cross( ):
	Vector3 Cross( const Vector3 &v ) const;

	// GetYaw( )/GetPitch( )/GetRoll( ):
	FLOAT32 GetYaw( ) const;
	FLOAT32 GetPitch( ) const;
	FLOAT32 GetRoll( FLOAT32 fTightness = 0.1f ) const;
};

#include "Vector3.inl"
