#include "StdAfx.h"
#include "ZVector.h"
#include "ZMatrix.h"

/*---------------------------------------------
* Identity( ):
---------------------------------------------*/

ZMatrix ZMatrix::Identity()
{
	ZMatrix m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* Translation( ):
---------------------------------------------*/

ZMatrix ZMatrix::Translation( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ )
{
	ZMatrix m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, fX,   fY,   fZ,   1.0f );
	return m;
}

/*---------------------------------------------
* Scaling( ):
---------------------------------------------*/

ZMatrix ZMatrix::Scaling( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ )
{
	ZMatrix m;
	m.SetRow( 0, fX,   0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, fY,   0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, fZ,   0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationX( ):
---------------------------------------------*/

ZMatrix ZMatrix::RotationX( FLOAT32 fPitch )
{
	FLOAT32 fCos = cosf( fPitch ), fSin = sinf( fPitch );

	ZMatrix m;
	m.SetRow( 0, 1.0f, 0.0f, 0.0f, 0.0f );
	m.SetRow( 1, 0.0f, fCos, fSin, 0.0f );
	m.SetRow( 2, 0.0f,-fSin, fCos, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationY( ):
---------------------------------------------*/

ZMatrix ZMatrix::RotationY( FLOAT32 fYaw )
{
	FLOAT32 fCos = cosf( fYaw ), fSin = sinf( fYaw );

	ZMatrix m;
	m.SetRow( 0, fCos, 0.0f,-fSin, 0.0f ); 
	m.SetRow( 1, 0.0f, 1.0f, 0.0f, 0.0f );
	m.SetRow( 2, fSin, 0.0f, fCos, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* RotationZ( ):
---------------------------------------------*/

ZMatrix ZMatrix::RotationZ( FLOAT32 fRoll )
{
	FLOAT32 fCos = cosf( fRoll ), fSin = sinf( fRoll );

	ZMatrix m;
	m.SetRow( 0, fCos, fSin, 0.0f, 0.0f );
	m.SetRow( 1,-fSin, fCos, 0.0f, 0.0f );
	m.SetRow( 2, 0.0f, 0.0f, 1.0f, 0.0f );
	m.SetRow( 3, 0.0f, 0.0f, 0.0f, 1.0f );
	return m;
}

/*---------------------------------------------
* Rotation( ):
---------------------------------------------*/

ZMatrix ZMatrix::Rotation( FLOAT32 fYaw, FLOAT32 fPitch )
{
	return RotationY( fYaw ) * RotationX( fPitch );
}
ZMatrix ZMatrix::Rotation( FLOAT32 fYaw, FLOAT32 fPitch, FLOAT32 fRoll)
{
	return Rotation( fYaw, fPitch ) * RotationZ( fRoll );
}
ZMatrix ZMatrix::Rotation( FLOAT32 fAng, const ZVector &v )
{
	FLOAT32 fYaw = v.GetYaw( ), fPitch = v.GetPitch( );
	return ( Rotation( -fPitch, -fYaw ) * RotationZ( fAng ) ) * Rotation( fPitch, fYaw );
}

/*---------------------------------------------
* operator*( ):
---------------------------------------------*/

ZMatrix ZMatrix::operator*( const ZMatrix &m ) const
{
	ZMatrix mRes;
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
 