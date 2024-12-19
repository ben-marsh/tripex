#include "Platform.h"
#include "Vector3.h"
#include "Matrix44.h"

/*---------------------------------------------
* Identity( ):
---------------------------------------------*/

Matrix44 Matrix44::Identity()
{
	Matrix44 m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* Translation( ):
---------------------------------------------*/

Matrix44 Matrix44::Translation( float fX, float fY, float fZ )
{
	Matrix44 m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, fX,   fY,   fZ,   1.0f );
	return m;
}

/*---------------------------------------------
* Scaling( ):
---------------------------------------------*/

Matrix44 Matrix44::Scaling( float fX, float fY, float fZ )
{
	Matrix44 m;
	m.SetRow( 0, fX,   0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, fY,   0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, fZ,   0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationX( ):
---------------------------------------------*/

Matrix44 Matrix44::RotationX( float fPitch )
{
	float fCos = cosf( fPitch ), fSin = sinf( fPitch );

	Matrix44 m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, fCos, fSin, 0.0f );
	m.SetRow( 2, 0.0f,-fSin, fCos, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationY( ):
---------------------------------------------*/

Matrix44 Matrix44::RotationY( float fYaw )
{
	float fCos = cosf( fYaw ), fSin = sinf( fYaw );

	Matrix44 m;
	m.SetRow( 0, fCos, 0.0f,-fSin, 0.0f ); 
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, fSin, 0.0f, fCos, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationZ( ):
---------------------------------------------*/

Matrix44 Matrix44::RotationZ( float fRoll )
{
	float fCos = cosf( fRoll ), fSin = sinf( fRoll );

	Matrix44 m;
	m.SetRow( 0, fCos, fSin, 0.0f, 0.0f );
	m.SetRow( 1,-fSin, fCos, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* Rotation( ):
---------------------------------------------*/

Matrix44 Matrix44::Rotation( float fYaw, float fPitch )
{
	return RotationY( fYaw ) * RotationX( fPitch );
}
Matrix44 Matrix44::Rotation( float fYaw, float fPitch, float fRoll)
{
	return Rotation( fYaw, fPitch ) * RotationZ( fRoll );
}
Matrix44 Matrix44::Rotation( float fAng, const Vector3 &v )
{
	float fYaw = v.GetYaw( ), fPitch = v.GetPitch( );
	return ( Rotation( -fPitch, -fYaw ) * RotationZ( fAng ) ) * Rotation( fPitch, fYaw );
}

/*---------------------------------------------
* operator*( ):
---------------------------------------------*/

Matrix44 Matrix44::operator*( const Matrix44 &m ) const
{
	Matrix44 mRes;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			mRes[i][j] = 0;
			for(int k = 0; k < 4; k++)
			{
//				check for 0s and 1s, because they are very common
//				mRes[i][j] += (*this)[k][j] * m[i][k];
				if(m[i][k] != 0.0f && (*this)[k][j] != 0.0f)
				{
					if(m[i][k] == 1.0f) mRes[i][j] += (*this)[k][j];
					else if((*this)[k][j] == 1.0f) mRes[i][j] += m[i][k];
					else mRes[i][j] += (*this)[k][j] * m[i][k];
				}
			}
		}
	}
	return mRes;
}
