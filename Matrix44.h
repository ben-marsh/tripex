#pragma once

class Matrix44
{
public:
	// matrix values at (x,y) positions, not (r,c)
	float m_aafValue[ 4 ][ 4 ];

	// Identity( ): 
 	static Matrix44 Identity();

	// Translation( ): 
	static Matrix44 Translation( const Vector3 &v );
	static Matrix44 Translation( float fX, float fY, float fZ );

	// Scaling( ):
	static Matrix44 Scaling( float fX, float fY, float fZ );

	// Rotation/X/Y/Z( ):
	static Matrix44 Rotation( float fYaw, float fPitch);
	static Matrix44 Rotation( float fYaw, float fPitch, float fRoll);
	static Matrix44 Rotation( float fAng, const Vector3 &v );
	static Matrix44 RotationX( float fPitch );
	static Matrix44 RotationY( float fYaw );
	static Matrix44 RotationZ( float fRoll );

	// SetRow( ):
	void SetRow( int nRow, float fVal0, float fVal1, float fVal2, float fVal3 );

	// operator[ ]( ):
	float *operator[ ]( int nPos );
	const float *operator[ ]( int nPos ) const;

	// operator*( )/operator*=( ):
	Matrix44 operator*( const Matrix44 &m ) const;
	Matrix44 &operator*=( const Matrix44 &m );


//	inline ZMatrix(){}
//	inline ZMatrix(float v00, float v01, float v02, float v03, float v10, float v11, float v12, float v13, float v20, float v21, float v22, float v23, float v30, float v31, float v32, float v33);

//	inline float *operator[](int nPos);
//	inline const float *operator[](int nPos) const;
//	inline ZMatrix operator*(ZMatrix &m);
//	inline ZMatrix &operator=(ZMatrix &m);
//	inline ZMatrix &operator*=(ZMatrix &m);
//	inline ZVector operator*(ZVector &v);
};

#include "Matrix44.inl"
