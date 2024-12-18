#pragma once

#include "ZFft.h"
#include "ZSpriteBuffer.h"

#define FREQ_HISTORY_SIZE 20
#define FREQ_BANDS 256
#define FREQ_BAND_POWER 2.0f
#define BEATHISTORY 5

class ZAudio
{
protected:
	short int *m_anMono;
	short int *m_aanStereo[ 2 ];
	float m_aafFreqHistory[ FREQ_HISTORY_SIZE ][ 16 ];
	ZFft m_Fft;

	int m_anBandIdx[ FREQ_BANDS ];
	float m_afBandMul[ FREQ_BANDS ];
	float m_afBand[ FREQ_BANDS ];

	float m_afBeatHistory[ BEATHISTORY ];

	float m_fElapsed;
	float m_fIntensity;
	float m_fBeat;
	float m_fIntensityBeatScale;
	bool m_bBeat;

	int m_nDataChannels;
	int m_nDataSampleSize;
	int m_nDataFreqShift;

	// AddConvertedData( ):
	void AddConvertedData( const short *pnSrc, int nSamples );

public:
	const int m_nSamples;

	// Constructor:
	ZAudio( int nSamples );

	// Destructor:
	~ZAudio( );

	// SetDataFormat( ):
	void SetDataFormat( int nChannels, int nFrequency, int nSampleBits );

	// AddData( ):
	void AddData( const void *pvData, int nSize );

	// Update( ):
	void Update( float fElapsed, float fSensitivity);
	
	// Render( ):
	void Render( ZSpriteBuffer &sb ) const;

	// GetIntensity( ):
	float GetIntensity( ) const;

	// GetBeat( ):
	float GetBeat( ) const;

	// IsBeat( ):
	bool IsBeat( ) const;

	// GetRandomSample( ):
	float GetRandomSample( ) const;

	// SetIntensityBeatScale( ):
	void SetIntensityBeatScale( float fScale );

	// GetDampenedBand( ):
	float GetDampenedBand( float fDampen, float fMin, float fMax ) const;

	// GetSample( ):
	float GetSample( int nIdx ) const;
	float GetSample( int nChannel, int nIdx ) const;

	// GetBand( ):
	float GetBand( int nIdx );
};
