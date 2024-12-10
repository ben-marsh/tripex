#pragma once
#include <math.h>
#include "ZColour.h"
#include "Misc.h"

/*---------------------------------
* ZColourHSV
-----------------------------------*/

class ZColourHSV
{
public:
	FLOAT32 m_fH, m_fS, m_fV;

	// Constructor:
	inline ZColourHSV();
	inline ZColourHSV( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV );

	// Set( ):
	inline void Set( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV );
	inline void Set( const ZColour &c, FLOAT32 fDefH = 0.0f, FLOAT32 fDefS = 0.0f );

	// CorrectRange( ):
	inline void CorrectRange( );
	
	// GetRGB( ):
	inline ZWideColour GetRGB( ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

ZColourHSV::ZColourHSV( )
{
}

ZColourHSV::ZColourHSV( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV )
{
	Set( fH, fS, fV );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void ZColourHSV::Set( FLOAT32 fH, FLOAT32 fS, FLOAT32 fV )
{
	m_fH = fH;
	m_fS = fS;
	m_fV = fV;
}

void ZColourHSV::Set( const ZColour &c, FLOAT32 fDefH, FLOAT32 fDefS )
{
	// sat = 0, hue doesn't matter (grey) (min = max)
	// light = 0, sat + hue doesn't matter (black) (min = max = 0)

	int nMin = min( c.m_nR, min( c.m_nG, c.m_nB ) );
	int nMax = max( c.m_nR, max( c.m_nG, c.m_nB ) );
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

ZWideColour ZColourHSV::GetRGB( ) const
{
	ZColourHSV c = *this;
	c.CorrectRange( );

	FLOAT32 fPos = ( c.m_fH + PI ) / ( PI / 3.0f );
	FLOAT32 fSpread = c.m_fS * c.m_fV * 255.0f;
	FLOAT32 fMin = c.m_fV * 255.0f - fSpread;

	FLOAT32 fR = fMin + Bound< FLOAT32 >( -1 + fabsf( fPos - 3.0f ), 0.0f, 1.0f ) * fSpread;
	FLOAT32 fG = fMin + Bound< FLOAT32 >( +2 - fabsf( fPos - 2.0f ), 0.0f, 1.0f ) * fSpread;
	FLOAT32 fB = fMin + Bound< FLOAT32 >( +2 - fabsf( fPos - 4.0f ), 0.0f, 1.0f ) * fSpread;
	return ZWideColour( ( SINT32 )fR, ( SINT32 )fG, ( SINT32 )fB );
}

/*---------------------------------
* CorrectRange( ):
-----------------------------------*/

void ZColourHSV::CorrectRange( )
{
	m_fH = Wrap( m_fH, -PI, +PI );
	m_fS = Bound< FLOAT32 >( m_fS, 0.0f, 1.0f );
	m_fV = Bound< FLOAT32 >( m_fV, 0.0f, 1.0f );
}
