#ifndef _ZMATRIX_H
#define _ZMATRIX_H

class ZMatrix
{
public:
	// matrix values at (x,y) positions, not (r,c)
	FLOAT32 m_aafValue[ 4 ][ 4 ];

	// Identity( ): 
 	static ZMatrix Identity();

	// Translation( ): 
	static ZMatrix Translation( const ZVector &v );
	static ZMatrix Translation( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// Scaling( ):
	static ZMatrix Scaling( FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ );

	// Rotation/X/Y/Z( ):
	static ZMatrix Rotation( FLOAT32 fYaw, FLOAT32 fPitch);
	static ZMatrix Rotation( FLOAT32 fYaw, FLOAT32 fPitch, FLOAT32 fRoll);
	static ZMatrix Rotation( FLOAT32 fAng, const ZVector &v );
	static ZMatrix RotationX( FLOAT32 fPitch );
	static ZMatrix RotationY( FLOAT32 fYaw );
	static ZMatrix RotationZ( FLOAT32 fRoll );

	// SetRow( ):
	void SetRow( int nRow, FLOAT32 fVal0, FLOAT32 fVal1, FLOAT32 fVal2, FLOAT32 fVal3 );

	// operator[ ]( ):
	FLOAT32 *operator[ ]( int nPos );
	const FLOAT32 *operator[ ]( int nPos ) const;

	// operator*( )/operator*=( ):
	ZMatrix operator*( const ZMatrix &m ) const;
	ZMatrix &operator*=( const ZMatrix &m );


//	inline ZMatrix(){}
//	inline ZMatrix(float v00, float v01, float v02, float v03, float v10, float v11, float v12, float v13, float v20, float v21, float v22, float v23, float v30, float v31, float v32, float v33);

//	inline float *operator[](int nPos);
//	inline const float *operator[](int nPos) const;
//	inline ZMatrix operator*(ZMatrix &m);
//	inline ZMatrix &operator=(ZMatrix &m);
//	inline ZMatrix &operator*=(ZMatrix &m);
//	inline ZVector operator*(ZVector &v);
};

#include "ZMatrix.inl"

#endif