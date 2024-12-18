#pragma once

#include "ZVector.h"
#include <bitset>

class ZCamera
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

	ZVector m_vPosition;
	FLOAT32 m_fPitch, m_fYaw, m_fRoll;

	// Constructor:
	ZCamera( );

	// SetDir( ):
	void SetDir( const ZVector &vDir );

	// SetTarget( ):
	void SetTarget( const ZVector &vPosition );

	// GetTransform( ):
	ZMatrix GetTransform( ) const;
};
