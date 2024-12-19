#include "Platform.h"
#include "Vector3.h"

/*---------------------------------------------
* ZVector
---------------------------------------------*/

/*---------------------------------------------
* Cross( ):
---------------------------------------------*/

Vector3 Vector3::Cross( const Vector3 &v ) const
{
	float fX = ( m_fY * v.m_fZ ) - ( m_fZ * v.m_fY );
	float fY = ( m_fZ * v.m_fX ) - ( m_fX * v.m_fZ );
	float fZ = ( m_fX * v.m_fY ) - ( m_fY * v.m_fX );
	return Vector3( fX, fY, fZ );
}

/*---------------------------------------------
* GetRoll( ):
---------------------------------------------*/

float Vector3::GetRoll( float fTightness ) const
{
	float fPos = fabsf( GetPitch( ) / ( PI / 2.0f ) );
	float fRollT = ( fPos - ( 1.0f - fTightness ) ) / fTightness;
	float fRoll = Bound< float >( fRollT, 0.0f, 1.0f ) * ( PI / 2.0f );
	if( fabsf( GetYaw( ) ) > ( PI / 2.0f ) ) fRoll = -fRoll;
	return fRoll;
}

/*---------------------------------------------
* operator*( )/operator*=( ):
---------------------------------------------*/

Vector3 Vector3::operator*( const Matrix44 &m ) const
{
	Vector3 vRes;
	for( int i = 0; i < 3; i++ )
	{
		vRes[ i ] = ( m_fX * m[ i ][ 0 ] ) + ( m_fY * m[ i ][ 1 ] ) + ( m_fZ * m[ i ][ 2 ] ) + m[ i ][ 3 ];
	}
	return vRes; 
}

Vector3 &Vector3::operator*=( const Matrix44 &m )
{
	*this = *this * m;
	return *this;
}
