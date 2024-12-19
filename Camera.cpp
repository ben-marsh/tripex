#include "StdAfx.h"
#include "Camera.h"

/*---------------------------------
* Constructor:
-----------------------------------*/

Camera::Camera( )
{
	m_bsFlag.set( F_SCREEN_TRANSFORM );

	m_vPosition = Vector3::Origin();
	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
	m_fRoll = 0.0f;
	m_fPerspective = 150.0f;
	m_fTightness = 0.1f;
}

/*---------------------------------
* GetTransform( ):
-----------------------------------*/

Matrix44 Camera::GetTransform( ) const
{
	return Matrix44::Translation( -m_vPosition ) * Matrix44::Rotation( -m_fYaw, -m_fPitch, -m_fRoll );
}

/*---------------------------------
* SetDir( ):
-----------------------------------*/

void Camera::SetDir( const Vector3 &vDir )
{
	m_fPitch = vDir.GetPitch( );
	m_fYaw = vDir.GetYaw( );
	m_fRoll = vDir.GetRoll( m_fTightness );
}

/*---------------------------------
* SetTarget( ):
-----------------------------------*/

void Camera::SetTarget( const Vector3 &vTarget )
{
	SetDir( vTarget - m_vPosition );
}
