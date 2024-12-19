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
			float m_fX, m_fY, m_fZ;
		};
		float m_af[ 3 ];
	};

	// Constructors
	Vector3( );
	Vector3( float fX, float fY, float fZ );

	// Origin( ):
	static Vector3 Origin( );

	// Polar( ):
	static Vector3 Polar( float fLength, float fPitch, float fYaw );

	// Set( ):
	void Set( float fX, float fY, float fZ );

	// operator+( )/operator+=( ):
	Vector3 operator+( ) const;
	Vector3 operator+( const Vector3 &v ) const;
	Vector3 &operator+=( const Vector3 &v );

	// operator-( )/operator-=( ):
	Vector3 operator-( ) const;
	Vector3 operator-( const Vector3 &v ) const;
	Vector3 &operator-=( const Vector3 &v );

	// operator*( )/operator*=( ):
	Vector3 operator*( float fValue ) const;
	Vector3 operator*( const Matrix44 &m ) const;
	Vector3 &operator*=( float fValue );
	Vector3 &operator*=( const Matrix44 &m );

	// operator/( ):
	Vector3 operator/( float fValue ) const;
	Vector3 &operator/=( float fValue );

	// operator[ ]:
	float &operator[ ]( int nPos );
	const float &operator[ ]( int nPos ) const;

	// Dot( ):
	float Dot( const Vector3 &v ) const;

	// Length( ):
	float Length( ) const;

	// Normal( )/Normalize( ):
	Vector3 Normal( float fLength = 1.0f ) const;
	void Normalize( float fLength = 1.0f );

	// Cross( ):
	Vector3 Cross( const Vector3 &v ) const;

	// GetYaw( )/GetPitch( )/GetRoll( ):
	float GetYaw( ) const;
	float GetPitch( ) const;
	float GetRoll( float fTightness = 0.1f ) const;
};

#include "Vector3.inl"
