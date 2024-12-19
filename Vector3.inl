#include "Matrix44.h"

/*---------------------------------------------
* ZVector methods
---------------------------------------------*/

/*---------------------------------------------
* Constructor:
---------------------------------------------*/

inline Vector3::Vector3( )
{
}

inline Vector3::Vector3( float fX, float fY, float fZ )
{
	Set( fX, fY, fZ );
}

/*---------------------------------------------
* Origin( ):
---------------------------------------------*/

inline Vector3 Vector3::Origin( )
{
	return Vector3( 0.0f, 0.0f, 0.0f );
}

/*---------------------------------------------
* Polar( ):
---------------------------------------------*/

inline Vector3 Polar( float fLength, float fPitch, float fYaw )
{
	return Vector3( 0, 0, fLength ) * Matrix44::Rotation( fPitch, fYaw );
}

/*---------------------------------------------
* Set( ):
---------------------------------------------*/

inline void Vector3::Set( float fX, float fY, float fZ )
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
}

/*---------------------------------------------
* operator+( ):
---------------------------------------------*/

inline Vector3 Vector3::operator+( ) const
{
	return Vector3( +m_fX, +m_fY, +m_fZ );
}

inline Vector3 Vector3::operator+( const Vector3 &v ) const
{
	return Vector3( m_fX + v.m_fX, m_fY + v.m_fY, m_fZ + v.m_fZ );
}

/*---------------------------------------------
* operator-( ):
---------------------------------------------*/

inline Vector3 Vector3::operator-( ) const
{
	return Vector3( -m_fX, -m_fY, -m_fZ );
}

inline Vector3 Vector3::operator-( const Vector3 &v ) const
{
	return Vector3( m_fX - v.m_fX, m_fY - v.m_fY, m_fZ - v.m_fZ );
}

/*---------------------------------------------
* operator*( ):
---------------------------------------------*/

inline Vector3 Vector3::operator*( float fValue ) const
{
	return Vector3( m_fX * fValue, m_fY * fValue, m_fZ * fValue );
}

inline Vector3 operator*( float fValue, const Vector3 &v )
{
	return v * fValue;
}

/*---------------------------------------------
* operator/( ):
---------------------------------------------*/

inline Vector3 Vector3::operator/( float fValue ) const
{
	return operator*( 1.0f / fValue );
}

/*---------------------------------------------
* operator+=( ):
---------------------------------------------*/

inline Vector3 &Vector3::operator+=( const Vector3 &v )
{
	m_fX += v.m_fX;
	m_fY += v.m_fY;
	m_fZ += v.m_fZ;
	return *this;
}

/*---------------------------------------------
* operator-=( ):
---------------------------------------------*/

inline Vector3 &Vector3::operator-=( const Vector3 &v )
{
	m_fX -= v.m_fX;
	m_fY -= v.m_fY;
	m_fZ -= v.m_fZ;
	return *this;
}

/*---------------------------------------------
* operator*=( ):
---------------------------------------------*/

inline Vector3 &Vector3::operator*=( float fValue )
{
	m_fX *= fValue;
	m_fY *= fValue;
	m_fZ *= fValue;
	return *this;
}

/*---------------------------------------------
* operator/=( ):
---------------------------------------------*/

inline Vector3 &Vector3::operator/=( float fValue )
{
	return operator*=( 1.0f / fValue );
}

/*---------------------------------------------
* operator[ ]:
---------------------------------------------*/

inline float &Vector3::operator[ ]( int nPos )
{
	_ASSERT( nPos >= 0 && nPos < 3 );
	return m_af[ nPos ];
}

inline const float &Vector3::operator[ ]( int nPos ) const
{
	_ASSERT( nPos >= 0 && nPos < 3 );
	return m_af[ nPos ];
}

/*---------------------------------------------
* Dot( ):
---------------------------------------------*/

inline float Vector3::Dot( const Vector3 &v ) const
{
	return ( m_fX * v.m_fX ) + ( m_fY * v.m_fY ) + ( m_fZ * v.m_fZ );
}

/*---------------------------------------------
* Length( ):
---------------------------------------------*/

inline float Vector3::Length( ) const
{
	return sqrtf( Dot( *this ) );
}

/*---------------------------------------------
* Normal( )/Normalize( ):
---------------------------------------------*/

inline Vector3 Vector3::Normal( float fLength ) const
{
	return operator*( fLength / Length( ) );
}

inline void Vector3::Normalize( float fLength )
{
	operator*=( fLength / Length( ) );
}

/*---------------------------------------------
* GetYaw( )/GetPitch( )/GetRoll( ):
---------------------------------------------*/

inline float Vector3::GetYaw( ) const
{
	return atan2f( m_fX, m_fZ );
}

inline float Vector3::GetPitch( ) const
{
	return -atan2f( m_fY, sqrtf( m_fX * m_fX + m_fZ * m_fZ ) ); 
}

