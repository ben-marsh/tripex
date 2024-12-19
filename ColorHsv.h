#pragma once

#include <math.h>
#include "ColorRgb.h"
#include "Misc.h"

/*---------------------------------
* ZColourHSV
-----------------------------------*/

struct ColorHsv
{
	float m_fH, m_fS, m_fV;

	// Constructor:
	ColorHsv() = default;
	ColorHsv( float fH, float fS, float fV );

	// Set( ):
	void Set( float fH, float fS, float fV );
	void Set( const ColorRgb &c, float fDefH = 0.0f, float fDefS = 0.0f );

	// CorrectRange( ):
	void CorrectRange( );
	
	// GetRGB( ):
	WideColorRgb GetRGB( ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

inline ColorHsv::ColorHsv( float fH, float fS, float fV )
{
	Set( fH, fS, fV );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

inline void ColorHsv::Set( float fH, float fS, float fV )
{
	m_fH = fH;
	m_fS = fS;
	m_fV = fV;
}

inline void ColorHsv::Set( const ColorRgb &c, float fDefH, float fDefS )
{
	// sat = 0, hue doesn't matter (grey) (min = max)
	// light = 0, sat + hue doesn't matter (black) (min = max = 0)

	int nMin = std::min( c.m_nR, std::min( c.m_nG, c.m_nB ) );
	int nMax = std::max( c.m_nR, std::max( c.m_nG, c.m_nB ) );
	float fRange = nMax - nMin;

	// lightness
	m_fV = nMax / 255.0f;

	// saturation
	if(nMax == 0) m_fS = fDefS;
	else m_fS = fRange / nMax;

	// hue
	if(nMax == nMin)
	{
		m_fH = fDefH;
	}
	else
	{
		float fRangeRecip = 1.0f / fRange;

		float fDiffR = ( nMax - c.m_nR ) * fRangeRecip;
		float fDiffG = ( nMax - c.m_nG ) * fRangeRecip;
		float fDiffB = ( nMax - c.m_nB ) * fRangeRecip;

		if( nMin == c.m_nB )		m_fH = 1.0f - fDiffG + fDiffR;
		else if( nMin == c.m_nR )	m_fH = 3.0f - fDiffB + fDiffG;
		else						m_fH = 5.0f - fDiffR + fDiffB;

		m_fH = -PI + ( m_fH * ( PI / 3.0f ) );
	}
}

/*---------------------------------
* ToRGB( ):
-----------------------------------*/

inline WideColorRgb ColorHsv::GetRGB( ) const
{
	ColorHsv c = *this;
	c.CorrectRange( );

	float fPos = ( c.m_fH + PI ) / ( PI / 3.0f );
	float fSpread = c.m_fS * c.m_fV * 255.0f;
	float fMin = c.m_fV * 255.0f - fSpread;

	float fR = fMin + Bound< float >( -1 + fabsf( fPos - 3.0f ), 0.0f, 1.0f ) * fSpread;
	float fG = fMin + Bound< float >( +2 - fabsf( fPos - 2.0f ), 0.0f, 1.0f ) * fSpread;
	float fB = fMin + Bound< float >( +2 - fabsf( fPos - 4.0f ), 0.0f, 1.0f ) * fSpread;
	return WideColorRgb( ( SINT32 )fR, ( SINT32 )fG, ( SINT32 )fB );
}

/*---------------------------------
* CorrectRange( ):
-----------------------------------*/

inline void ColorHsv::CorrectRange( )
{
	m_fH = Wrap( m_fH, -PI, +PI );
	m_fS = Bound< float >( m_fS, 0.0f, 1.0f );
	m_fV = Bound< float >( m_fV, 0.0f, 1.0f );
}
