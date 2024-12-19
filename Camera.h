#pragma once

#include "Vector3.h"
#include <bitset>

class Camera
{
public:
	enum
	{
		F_SCREEN_TRANSFORM,
		F_LAST,
	};

	std::bitset< F_LAST > m_bsFlag;

	FLOAT32 m_fScreenX, m_fScreenY;
	FLOAT32 m_fScale;

	FLOAT32 m_fPerspective;
	FLOAT32 m_fTightness;

	Vector3 m_vPosition;
	FLOAT32 m_fPitch, m_fYaw, m_fRoll;

	// Constructor:
	Camera( );

	// SetDir( ):
	void SetDir( const Vector3 &vDir );

	// SetTarget( ):
	void SetTarget( const Vector3 &vPosition );

	// GetTransform( ):
	Matrix44 GetTransform( ) const;
};
