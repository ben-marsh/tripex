#include "StdAfx.h"
#include "AudioFft.h"
#include "Misc.h"

/*---------------------------------
* ZFft:
-----------------------------------*/

/*---------------------------------------------
* Constructor:
---------------------------------------------*/

AudioFft::AudioFft( int nSamples ) : m_nSamples( nSamples ), m_nLogSamples( IntegerLog2( nSamples ) )
{
	_ASSERT( nSamples == ( 1 << m_nLogSamples ) );

	// precalc apodization function
	m_afScale = new float[ nSamples ];
	float fX = -1.0f;
	float fStepX = 2.0f / ( m_nSamples - 1.0f );
	for( int i = 0; i < m_nSamples; i++ )
	{
		m_afScale[ i ] = 1.0f - ( fX * fX );
		m_afScale[ i ] *= m_afScale[ i ];
		m_afScale[ i ] *= 1.0f / ( 1 << 16 );
		fX += fStepX;
	}

	m_afAmplitude = new float[ nSamples ];

	m_acTransform = new Complex[ nSamples ];

	m_acCoeff = new Complex[ m_nSamples + 1 ];
	for( int i = 0; i < m_nSamples; i++ )
	{
		float fAng = ( 2.0f * PI * i ) / m_nSamples;
		m_acCoeff[ i ] = Complex( cosf( fAng ), -sinf( fAng ) );
	}

	m_anIndex = new unsigned short[ nSamples ];
	for( int i = 0; i < nSamples; i++ )
	{
		m_anIndex[ i ] = ReverseBitOrder< short int >( i );
		m_anIndex[ i ] >>= ( ( sizeof( short int ) * 8 ) - m_nLogSamples );
	}
}

/*---------------------------------------------
* Destructor:
---------------------------------------------*/

AudioFft::~AudioFft( )
{
	if( m_afScale != NULL )
	{
		delete m_afScale;
	}
}

/*---------------------------------------------
* SetDecay( ):
---------------------------------------------*/

void AudioFft::SetDecay( float fDecay )
{
	m_fDecay = fDecay;
}

/*---------------------------------------------
* Update( ):
---------------------------------------------*/

void AudioFft::Update( const short int *anSample )
{
	for (int i = 0; i < m_nSamples; i++)
	{
		m_acTransform[ m_anIndex[ i ] ] = Complex( anSample[ i ] * m_afScale[ i ], 0.0f );
	}

	int nCoeffStep = m_nSamples >> 1;
	for( int nLevel = 0; nLevel < m_nLogSamples; nLevel++ )
	{
		int nStep = 1 << nLevel;
		int nInc = nStep << 1;

		const Complex *pcCoeff = &m_acCoeff[ 0 ];
		for( int j = 0; j < nStep; j++ )
		{
			for( int i = j; i < m_nSamples; i += nInc )
			{
				Complex cDelta = ( *pcCoeff ) * m_acTransform[ i + nStep ];
				m_acTransform[ i + nStep ] = m_acTransform[ i ] - cDelta;
				m_acTransform[ i ] = m_acTransform[ i ] + cDelta;
			}

			pcCoeff += nCoeffStep;
		}

		nStep <<= 1;
		nCoeffStep >>= 1;
	}

	m_fDecay = 0.8f;

	const Complex *pc = &m_acTransform[ 0 ];
	for( int i = 0; i < m_nSamples; i++ )
	{
		float fValue = pc->Length( );
		m_afAmplitude[ i ] *= m_fDecay;
		m_afAmplitude[ i ] = std::max( m_afAmplitude[ i ], fValue / sqrtf( 256.0f ) );
		pc++;
	}
}

/*---------------------------------------------
* GetAmplitude( ):
---------------------------------------------*/

float AudioFft::GetAmplitude( int nIdx ) const
{
	_ASSERT( nIdx >= 0 && nIdx < m_nSamples );
	return m_afAmplitude[ nIdx ];
}
