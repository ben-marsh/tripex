#pragma once

#include <math.h>
#include "ColorRgb.h"
#include "Misc.h"

/*---------------------------------
* ZColourHSV
-----------------------------------*/

class ColorHsv
{
public:
	FLOAT32 m_fH, m_fS, m_fV;

	// Constructor:
	inline ColorHsv();
	inline ColorHsv( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV );

	// Set( ):
	inline void Set( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV );
	inline void Set( const ColorRgb &c, FLOAT32 fDefH = 0.0f, FLOAT32 fDefS = 0.0f );

	// CorrectRange( ):
	inline void CorrectRange( );
	
	// GetRGB( ):
	inline WideColorRgb GetRGB( ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

ColorHsv::ColorHsv( )
{
}

ColorHsv::ColorHsv( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV )
{
	Set( fH, fS, fV );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void ColorHsv::Set( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV )
{
	m_fH = fH;
	m_fS = fS;
	m_fV = fV;
}

void ColorHsv::Set( const ColorRgb &c, FLOAT32 fDefH, FLOAT32 fDefS )
{
	// sat = 0, hue doesn't matter (grey) (min = max)
	// light = 0, sat + hue doesn't matter (black) (min = max = 0)

	int nMin = std::min( c.m_nR, std::min( c.m_nG, c.m_nB ) );
	int nMax = std::max( c.m_nR, std::max( c.m_nG, c.m_nB ) );
	FLOAT32 fRange = nMax - nMin;

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
		FLOAT32 fRangeRecip = 1.0f / fRange;

		FLOAT32 fDiffR = ( nMax - c.m_nR ) * fRangeRecip;
		FLOAT32 fDiffG = ( nMax - c.m_nG ) * fRangeRecip;
		FLOAT32 fDiffB = ( nMax - c.m_nB ) * fRangeRecip;

		if( nMin == c.m_nB )		m_fH = 1.0f - fDiffG + fDiffR;
		else if( nMin == c.m_nR )	m_fH = 3.0f - fDiffB + fDiffG;
		else						m_fH = 5.0f - fDiffR + fDiffB;

		m_fH = -PI + ( m_fH * ( PI / 3.0f ) );
	}
}

/*---------------------------------
* ToRGB( ):
-----------------------------------*/

WideColorRgb ColorHsv::GetRGB( ) const
{
	ColorHsv c = *this;
	c.CorrectRange( );

	FLOAT32 fPos = ( c.m_fH + PI ) / ( PI / 3.0f );
	FLOAT32 fSpread = c.m_fS * c.m_fV * 255.0f;
	FLOAT32 fMin = c.m_fV * 255.0f - fSpread;

	FLOAT32 fR = fMin + Bound< FLOAT32 >( -1 + fabsf( fPos - 3.0f ), 0.0f, 1.0f ) * fSpread;
	FLOAT32 fG = fMin + Bound< FLOAT32 >( +2 - fabsf( fPos - 2.0f ), 0.0f, 1.0f ) * fSpread;
	FLOAT32 fB = fMin + Bound< FLOAT32 >( +2 - fabsf( fPos - 4.0f ), 0.0f, 1.0f ) * fSpread;
	return WideColorRgb( ( SINT32 )fR, ( SINT32 )fG, ( SINT32 )fB );
}

/*---------------------------------
* CorrectRange( ):
-----------------------------------*/

void ColorHsv::CorrectRange( )
{
	m_fH = Wrap( m_fH, -PI, +PI );
	m_fS = Bound< FLOAT32 >( m_fS, 0.0f, 1.0f );
	m_fV = Bound< FLOAT32 >( m_fV, 0.0f, 1.0f );
}
