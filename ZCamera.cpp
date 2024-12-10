#include "StdAfx.h"
#include "ZCamera.h"

/*---------------------------------
* Constructor:
-----------------------------------*/

ZCamera::ZCamera( )
{
	m_bsFlag.set( F_SCREEN_TRANSFORM );

	m_vPosition = ZVector::Origin();
	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
	m_fRoll = 0.0f;
	m_fPerspective = 150.0f;
	m_fTightness = 0.1f;
}

/*---------------------------------
* GetTransform( ):
-----------------------------------*/

ZMatrix ZCamera::GetTransform( ) const
{
	return ZMatrix::Translation( -m_vPosition ) * ZMatrix::Rotation( -m_fYaw, -m_fPitch, -m_fRoll );
}

/*---------------------------------
* SetDir( ):
-----------------------------------*/

void ZCamera::SetDir( const ZVector &vDir )
{
	m_fPitch = vDir.GetPitch( );
	m_fYaw = vDir.GetYaw( );
	m_fRoll = vDir.GetRoll( m_fTightness );
}

/*---------------------------------
* SetTarget( ):
-----------------------------------*/

void ZCamera::SetTarget( const ZVector &vTarget )
{
	SetDir( vTarget - m_vPosition );
}
