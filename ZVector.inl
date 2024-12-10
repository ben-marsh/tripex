#include "ZMatrix.h"

/*---------------------------------------------
* ZVector methods
---------------------------------------------*/

/*---------------------------------------------
* Constructor:
---------------------------------------------*/

inline ZVector::ZVector( )
{
}

inline ZVector::ZVector( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ )
{
	Set( fX, fY, fZ );
}

/*---------------------------------------------
* Origin( ):
---------------------------------------------*/

inline ZVector ZVector::Origin( )
{
	return ZVector( 0.0f, 0.0f, 0.0f );
}

/*---------------------------------------------
* Polar( ):
---------------------------------------------*/

inline ZVector Polar( FLOAT32 fLength, FLOAT32 fPitch, FLOAT32 fYaw )
{
	return ZVector( 0, 0, fLength ) * ZMatrix::Rotation( fPitch, fYaw );
}

/*---------------------------------------------
* Set( ):
---------------------------------------------*/

inline void ZVector::Set( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ )
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
}

/*---------------------------------------------
* operator+( ):
---------------------------------------------*/

inline ZVector ZVector::operator+( ) const
{
	return ZVector( +m_fX, +m_fY, +m_fZ );
}

inline ZVector ZVector::operator+( const ZVector &v ) const
{
	return ZVector( m_fX + v.m_fX, m_fY + v.m_fY, m_fZ + v.m_fZ );
}

/*---------------------------------------------
* operator-( ):
---------------------------------------------*/

inline ZVector ZVector::operator-( ) const
{
	return ZVector( -m_fX, -m_fY, -m_fZ );
}

inline ZVector ZVector::operator-( const ZVector &v ) const
{
	return ZVector( m_fX - v.m_fX, m_fY - v.m_fY, m_fZ - v.m_fZ );
}

/*---------------------------------------------
* operator*( ):
---------------------------------------------*/

inline ZVector ZVector::operator*( FLOAT32 fValue ) const
{
	return ZVector( m_fX * fValue, m_fY * fValue, m_fZ * fValue );
}

inline ZVector operator*( FLOAT32 fValue, const ZVector &v )
{
	return v * fValue;
}

/*---------------------------------------------
* operator/( ):
---------------------------------------------*/

inline ZVector ZVector::operator/( FLOAT32 fValue ) const
{
	return operator*( 1.0f / fValue );
}

/*---------------------------------------------
* operator+=( ):
---------------------------------------------*/

inline ZVector &ZVector::operator+=( const ZVector &v )
{
	m_fX += v.m_fX;
	m_fY += v.m_fY;
	m_fZ += v.m_fZ;
	return *this;
}

/*---------------------------------------------
* operator-=( ):
---------------------------------------------*/

inline ZVector &ZVector::operator-=( const ZVector &v )
{
	m_fX -= v.m_fX;
	m_fY -= v.m_fY;
	m_fZ -= v.m_fZ;
	return *this;
}

/*---------------------------------------------
* operator*=( ):
---------------------------------------------*/

inline ZVector &ZVector::operator*=( FLOAT32 fValue )
{
	m_fX *= fValue;
	m_fY *= fValue;
	m_fZ *= fValue;
	return *this;
}

/*---------------------------------------------
* operator/=( ):
---------------------------------------------*/

inline ZVector &ZVector::operator/=( FLOAT32 fValue )
{
	return operator*=( 1.0f / fValue );
}

/*---------------------------------------------
* operator[ ]:
---------------------------------------------*/

inline FLOAT32 &ZVector::operator[ ]( int nPos )
{
	_ASSERT( nPos >= 0 && nPos < 3 );
	return m_af[ nPos ];
}

inline const FLOAT32 &ZVector::operator[ ]( int nPos ) const
{
	_ASSERT( nPos >= 0 && nPos < 3 );
	return m_af[ nPos ];
}

/*---------------------------------------------
* Dot( ):
---------------------------------------------*/

inline FLOAT32 ZVector::Dot( const ZVector &v ) const
{
	return ( m_fX * v.m_fX ) + ( m_fY * v.m_fY ) + ( m_fZ * v.m_fZ );
}

/*---------------------------------------------
* Length( ):
---------------------------------------------*/

inline FLOAT32 ZVector::Length( ) const
{
	return sqrtf( Dot( *this ) );
}

/*---------------------------------------------
* Normal( )/Normalize( ):
---------------------------------------------*/

inline ZVector ZVector::Normal( FLOAT32 fLength ) const
{
	return operator*( fLength / Length( ) );
}

inline void ZVector::Normalize( FLOAT32 fLength )
{
	operator*=( fLength / Length( ) );
}

/*---------------------------------------------
* GetYaw( )/GetPitch( )/GetRoll( ):
---------------------------------------------*/

inline FLOAT32 ZVector::GetYaw( ) const
{
	return atan2f( m_fX, m_fZ );
}

inline FLOAT32 ZVector::GetPitch( ) const
{
	return -atan2f( m_fY, sqrtf( m_fX * m_fX + m_fZ * m_fZ ) ); 
}

