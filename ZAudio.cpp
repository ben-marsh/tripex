#include "StdAfx.h"
#include "ZAudio.h"
#include "Misc.h"


////////
#include "ZDirect3D.h"
#include "effect.h"
#include "main.h"

extern float fHUDTransparency;

void DrawLineBar(ZSpriteBuffer &sb, int x, int y, int h, float p)
{
	int n = (int)(h * Bound<float>(p, 0.0f, 1.0f));
	sb.AddSprite(ZPoint<int>(x, y), NULL, g_pD3D->LuminanceOpacity, ZRect<int>(0, 0, 1, n), 0.1f);
	sb.AddSprite(ZPoint<int>(x, y + n), NULL, g_pD3D->LuminanceOpacity, ZRect<int>(0, 0, 1, h - n), 1.0f);
}

void DrawHorizontalBar(ZSpriteBuffer &sb, int x, int y, int w, int h, float p)
{
	p = min(1.0f, max(0.0f, p));
	sb.AddSprite(ZPoint<int>(x, y), NULL, 0, ZRect<int>(0, 0, w, h), ZColour::Grey(20));
	sb.AddSprite(ZPoint<int>(x, y), NULL, 0, ZRect<int>(0, 0, (int)(w * p), h), ZColour::White());
}


extern bool bAvoidBigReactions;

/*---------------------------------------------
* ZAudio:
---------------------------------------------*/

/*---------------------------------------------
* Constructor:
---------------------------------------------*/

ZAudio::ZAudio( int nSamples ) : m_Fft( nSamples ), m_nSamples( nSamples )
{
	m_anMono = new short int[ nSamples ];
	m_aanStereo[ 0 ] = new short int[ nSamples ];
	m_aanStereo[ 1 ] = new short int[ nSamples ];

	memset( m_anMono, 0, sizeof( short int ) * nSamples );
	memset( m_aanStereo[ 0 ], 0, sizeof( short int ) * nSamples );
	memset( m_aanStereo[ 1 ], 0, sizeof( short int ) * nSamples );
	memset( m_aafFreqHistory, 0, sizeof( m_aafFreqHistory ) );

	m_fElapsed = 0.0f;
	m_fIntensity = 0.0f;
	m_fBeat = 0.0f;
	m_bBeat = FALSE;
	m_fIntensityBeatScale = 0.0f;

	static const float fPower = 2.0f;
	static const float fMul = 1.0f;

	float fMinT = 0.0f, fMidT, fMaxT;
	for( int i = 0; i < FREQ_BANDS; i++ )
	{
		fMidT = powf( ( i + 0.5f ) / FREQ_BANDS, fPower );
		fMaxT = powf( ( i + 1.0f ) / FREQ_BANDS, fPower );

		m_anBandIdx[ i ] = min( ( int )( ( m_nSamples / 2 ) * fMidT ), ( m_nSamples / 2 ) - 1 );
		m_afBandMul[ i ] = fMul * ( ( fMaxT - fMinT ) * ( m_nSamples / 2 ) );

		fMinT = fMaxT;
	}
}

/*---------------------------------------------
* Destructor:
---------------------------------------------*/

ZAudio::~ZAudio( )
{
	delete m_anMono;
	delete m_aanStereo[ 0 ];
	delete m_aanStereo[ 1 ];
}

/*---------------------------------------------
* SetDataFormat( ):
---------------------------------------------*/

void ZAudio::SetDataFormat( int nChannels, int nFrequency, int nSampleBits )
{
	m_nDataChannels = nChannels;
	m_nDataFreqShift = IntegerLog2( 44100 / nFrequency );
	m_nDataSampleSize = nSampleBits / 8;
}

/*---------------------------------------------
* AddData( ):
---------------------------------------------*/

void ZAudio::AddData( const void *pvSrc, int nSize )
{
	const char *pnSrc = ( const char* )pvSrc;

	int nDstIdx = 0;
	int nMaxSize = ( m_nSamples * m_nDataSampleSize * m_nDataChannels ) >> m_nDataFreqShift;
	if( nSize > nMaxSize )
	{
		pnSrc += nSize - nMaxSize;
	}
	else if( nSize < nMaxSize )
	{
		nDstIdx = m_nSamples - ( nSize << m_nDataFreqShift ) / ( m_nDataChannels * m_nDataSampleSize );
		memmove( m_aanStereo[ 0 ], m_aanStereo[ 0 ] + ( m_nSamples - nDstIdx ), nDstIdx * sizeof( short int ) );
		memmove( m_aanStereo[ 1 ], m_aanStereo[ 1 ] + ( m_nSamples - nDstIdx ), nDstIdx * sizeof( short int ) );
	}

	int nIdx = nDstIdx;
	int nStep = 1 << m_nDataFreqShift;

//	int nDstIdx = 0;
//	int nDstSize = &m_anStereo[ m_nSamples * 2 ] - pnDst;
//	int nDstStep = ( 2 * ( 1 << m_nDataFreqShift ) );
	switch( m_nDataSampleSize )
	{
	case 1:
		for( ; nIdx < m_nSamples; nIdx += nStep )
		{
			m_aanStereo[ 0 ][ nIdx ] = ( ( int )*pnSrc ) << 8;
			pnSrc += m_nDataChannels - 1;
			m_aanStereo[ 1 ][ nIdx ] = ( ( int )*pnSrc ) << 8;
			pnSrc++;
		}
		break;
	case 2:
		for( ; nIdx < m_nSamples; nIdx += nStep )
		{
			m_aanStereo[ 0 ][ nIdx ] = *( ( short int* )pnSrc );
			pnSrc += ( m_nDataChannels - 1 ) * 2;
			m_aanStereo[ 1 ][ nIdx ] = *( ( short int* )pnSrc );
			pnSrc += 2;
		}
		break;
	default:
		_ASSERT( false );
		break;
	}

//	short int *pnMonoDst = &m_anMono[ nDstIdx / 2 ];
//	for( int i = nDstIdx; i < m_nSamples * 2; i += 2 )
//	{
//		*( pnMonoDst++ ) = ( ( int )( m_anStereo[ i + 0 ] ) + ( int )( m_anStereo[ i + 1 ] ) ) >> 1;
//	}
}

/*---------------------------------------------
* Update( ):
---------------------------------------------*/

void ZAudio::Update( float fElapsed, float fSensitivity )
{
	for( int i = 0; i < m_nSamples; i++ )
	{
		m_anMono[ i ] = ( ( int )m_aanStereo[ 0 ][ i ] + ( int )m_aanStereo[ 1 ][ i ] ) / 2;
	}

	m_Fft.Update( m_anMono );

	for( int i = 0; i < FREQ_BANDS; i++ )
	{
		m_afBand[ i ] = m_afBandMul[ i ] * m_Fft.GetAmplitude( m_anBandIdx[ i ] );
	}

	m_fElapsed = min( m_fElapsed + fElapsed, 1.0f );
	for( ; m_fElapsed > 0.5f; m_fElapsed -= 0.5f )
	{
		memmove( m_aafFreqHistory[ 1 ], m_aafFreqHistory[ 0 ], sizeof( m_aafFreqHistory[ 0 ] ) * ( FREQ_HISTORY_SIZE - 1 ) );
		memset( m_aafFreqHistory[ 0 ], 0, sizeof( m_aafFreqHistory[ 0 ] ) );

		for( int i = 0; i < 256; i++ )
		{
			m_aafFreqHistory[ 0 ][ i >> 4 ] += min( 1.0f, m_afBand[ i ] * 4.0f ) / 16.0f;
		}

		float fSum = 0.0f;
		for( int i = 0; i < m_nSamples; i++ )
		{
			fSum += abs( m_anMono[ i ] );
		}

		m_fIntensity = ( 576.0f / 512.0f ) * fSum / ( /*10.0f **/ 10.0f * m_nSamples * 256.0f );//* 256.0f );// / ( 256.0f * 10.0f ); 
//		pastaverage[0] /= 10.0 * 2 * 576.0;//10.0 * 288.0;
///		pastaverage[0] *= (1 / (pastaverage[0] + 0.6));
//		pastaverage[0] = min(max(pastaverage[0], 0), 1);


//		m_fIntensity /= ( 1 << 16 ) * m_nSamples;
//		m_fIntensity /= 10.0f * 2.0f * 2.0f /*512.0f */* 2.0f;
		m_fIntensity *= 1.0f / ( m_fIntensity + 0.6f );
		m_fIntensity = min( max( m_fIntensity, 0.0f ), 1.0f );

		if( bAvoidBigReactions )
		{
			m_fBeat = 0.0f;
		}
		else
		{
			float fVal = ( m_Fft.GetAmplitude( 0 ) + m_Fft.GetAmplitude( 1 ) ) * 1.5f;

			float fSize = 0.0f;
			for( int i = 0; i < BEATHISTORY; i++ )
			{
				fSize = max( fSize, fVal - m_afBeatHistory[ i ] );
			}

			float fLimit = 500.0f - 4.8f * fSensitivity;
			if( fSize > fLimit )
			{
				m_bBeat = TRUE;
				m_fBeat += 0.5f * fSize / fLimit;
			}
			else
			{
				m_bBeat = FALSE;
			}

			memmove( &m_afBeatHistory[ 1 ], &m_afBeatHistory[ 0 ], sizeof( m_afBeatHistory[ 0 ] ) * ( BEATHISTORY - 1 ) );
			m_afBeatHistory[ 0 ] = m_fBeat;

			m_fBeat = min( m_fBeat, 1.0f );
			m_fBeat *= 0.6f;
		}
	}
}

/*---------------------------------------------
* Render( ):
---------------------------------------------*/

void ZAudio::Render( ZSpriteBuffer &sb ) const
{
	sb.AddSprite(ZPoint<int>(10, 30), NULL, g_pD3D->InverseMultiply, ZRect<int>(0, 0, 600, 400), 1.0f - fHUDTransparency);

	DrawHorizontalBar( sb, 20, 40, 256, 10, m_fIntensity );
	DrawHorizontalBar( sb, 20, 60, 256, 10, m_fBeat );

	for(int i = 0; i < 256; i++)
	{
		DrawLineBar( sb, 20 + i, 140, 200, 1 - m_afBand[ i ] );
	}

	for( int i = 0; i < 16; i++ )
	{
		DrawHorizontalBar( sb, 300, 280 + (i * 5), 256, 5, GetDampenedBand( 1.0f, i / 16.0f, ( i + 1.0f ) / 16.0f ) );
	}
}

/*---------------------------------------------
* GetRandomSample( ):
---------------------------------------------*/

float ZAudio::GetRandomSample( ) const
{
	float fRand = ( ( ( float )rand( ) ) * m_nSamples ) / RAND_MAX;
	return m_anMono[ ( rand( ) * m_nSamples ) / RAND_MAX ] * ( 1.0f / ( 1 << 16 ) );
}

/*---------------------------------------------
* GetDampenedBand( ):
---------------------------------------------*/

float ZAudio::GetDampenedBand( float fDampen, float fMin, float fMax ) const
{
	int nHistory = ( int )( fDampen * FREQ_HISTORY_SIZE );
	nHistory = min( max( nHistory, 1 ), FREQ_HISTORY_SIZE - 1 );

	float fPeak = 0.0f;

	int nMin = min( max( ( int )( fMin * 16.0f ), 0 ), 15 );
	int nMax = min( max( ( int )( fMax * 16.0f ), 0 ), 15 );
	for( int i = nMin; i < nMax; i++ )
	{
		float fSum = 0.0f;
		for( int j = 0; j < nHistory; j++ )
		{
			fSum += m_aafFreqHistory[ j ][ i ];
		}
		fPeak = max( fPeak, fSum );
	}
	return fPeak / nHistory;
}

/*---------------------------------------------
* GetIntensity( ):
---------------------------------------------*/

float ZAudio::GetIntensity( ) const
{
	return m_fIntensity + ( m_fBeat * m_fIntensityBeatScale );
}

/*---------------------------------------------
* GetBeat( ):
---------------------------------------------*/

float ZAudio::GetBeat( ) const
{
	return m_fBeat;
}

/*---------------------------------------------
* IsBeat( ):
---------------------------------------------*/

bool ZAudio::IsBeat( ) const
{
	return m_bBeat;
}

/*---------------------------------------------
* SetIntensityBeatScale( ):
---------------------------------------------*/

void ZAudio::SetIntensityBeatScale( float fScale )
{
	m_fIntensityBeatScale = fScale;
}

/*---------------------------------------------
* GetSample( ):
---------------------------------------------*/

float ZAudio::GetSample( int nIdx ) const
{
	_ASSERT( nIdx >= 0 && nIdx < m_nSamples );
	return ( ( float )m_anMono[ nIdx ] ) / ( 1 << 15 );
}

float ZAudio::GetSample( int nChannel, int nIdx ) const
{
	_ASSERT( nIdx >= 0 && nIdx < m_nSamples );
	_ASSERT( nChannel >= 0 && nChannel < 2 );
	return ( ( float )m_aanStereo[ nChannel ][ nIdx ] ) / ( 1 << 15 );
}

/*---------------------------------------------
* GetBand( ):
---------------------------------------------*/

float ZAudio::GetBand( int nIdx )
{
	_ASSERT( nIdx >= 0 && nIdx < m_nSamples );
	return m_afBand[ nIdx ];
}


/*
#include "StdAfx.h"
#include "effect.h"
//#include "winamp.h"
#include "main.h"
#include "logspecanalyser.h"
#include "general.h"
#include "config.h"
#include "ZTextureFont.h"
#include "xbmc.h"

extern bool bAvoidBigReactions;

// spectrum analyser
#define WASA_STARTFREQ 200
#define WASA_ENDFREQ 20000
#define WASA_FREQRANGE (WASA_ENDFREQ - WASA_STARTFREQ)
#define SA_MULTIPLIER 2.0
#define SA_SIZE 256 //32

#define AVERAGES 1//20
#define BEATHISTORY 5

int beathistory[BEATHISTORY];
double pastaverage[AVERAGES] = { 0, };
double average;

#define NUMSPECS 20//50

double pdSpec[256], ppdLastSpec[NUMSPECS][16];

int waveform[128] = { 0, };
int waveformStereo[2][128] = { 0, };
bool fBigBeat = false;
double bigbeat = 0;

static double pdOrder[256];
static double pdLastSpec[256] = { 0, };
void InitBeats()
{
	InitLogSpec();

	for(int i = 0; i < NUMSPECS; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			ppdLastSpec[i][j] = 0;
		}
	}
}
void UpdateBeat(double elapsed)
{
	static double accum = 0;
	accum += elapsed;

	CalcLogSpec(g_pcSpectrum[0], g_pcSpectrum[1], pdSpec);

	int nItems = 0;
	for(int i = 0; i < 256; i++)
	{
		double ch = pdSpec[i] - pdLastSpec[i];
		for(int j = 0; j < nItems; j++)
		{
			if(pdOrder[j] > ch) break;
		}
		memmove(&pdOrder[j + 1], &pdOrder[j], (nItems - j) * sizeof(double));
		pdOrder[j] = ch;
		nItems++;

		pdLastSpec[i] = pdSpec[i];
	}

	while(accum > 0.5)
	{
		for(int i = NUMSPECS - 1; i > 0; i--)
		{
			memmove(ppdLastSpec[i], ppdLastSpec[i - 1], 16 * sizeof(double));
		}
		for(i = 0; i < 16; i++) ppdLastSpec[0][i] = 0;
		for(i = 0; i < 256; i++)
		{
			int n = i / 16;
			ppdLastSpec[0][n] += min(1.0, pdSpec[i] * 4.0) / 16.0;
		}

		memmove(&pastaverage[1], &pastaverage[0], (AVERAGES - 1) * sizeof(double));

		pastaverage[0] = 0;
		for(int j = 0; j < 2; j++)
		{
			int data, last;
			for(int i = 0; i < 576; i++)
			{
				data = g_pcWaveform[j][i];
				if(data > 128) data -= 256;

				if((i / 4) < 128)
				{
					if((i % 4) == 0)
					{
						waveform[i / 4] = 0;
						waveformStereo[j][i / 4] = 0;
					}
					waveform[i / 4] += data; 
					waveformStereo[j][i / 4] += data;
				}

				pastaverage[0] += abs(data);

				last = data;
			}
		}
		pastaverage[0] /= 10.0 * 2 * 576.0;//10.0 * 288.0;
		pastaverage[0] *= (1 / (pastaverage[0] + 0.6));
		pastaverage[0] = min(max(pastaverage[0], 0), 1);

		average = 0;
		for(i = 0; i < AVERAGES; i++) average += pastaverage[i];
		average /= AVERAGES;
//		average = pastaverage[0];

		if(bAvoidBigReactions)
		{
			bigbeat = 0;
			fBigBeat = false;
		}
		else
		{
			int thisBeat = 0;
			thisBeat += g_pcSpectrum[0][0] + g_pcSpectrum[1][0];// + tripex2.spectrumData[2][0];
			thisBeat += g_pcSpectrum[0][1] + g_pcSpectrum[1][1];// + tripex2.spectrumData[2][1];

			thisBeat *= 1.5;

			int limit = 500 - (4.8 * vpEffect[nEffect]->fSensitivity);
			int size = 0;

			for(i = 0; i < BEATHISTORY; i++)
			{
				if((thisBeat - beathistory[i]) > size) size = thisBeat - beathistory[i];
			}

			if(size > limit) //120)
			{
				fBigBeat = true;
				bigbeat += 0.5 * size / double(limit);
			}
			else fBigBeat = false;

			memmove(&beathistory[1], &beathistory[0], sizeof(beathistory[0]) * (BEATHISTORY - 1));
			beathistory[0] = thisBeat;
			//	lastBeat = thisBeat;

			bigbeat = Bound<float>(bigbeat, 0, 1);
//			average += bigbeat;//min(1.0f, average + bigbeat);
			bigbeat = bigbeat * 0.6;
		}

		accum-=0.5;
	}
}
void DrawLineBar(ZSpriteBuffer &sb, int x, int y, int h, double p)
{
	int n = h * Bound<double>(p, 0.0, 1.0);
	sb.AddSprite(ZPoint<int>(x, y), NULL, g_pD3D->LuminanceOpacity, ZRect<int>(0, 0, 1, n), 0.1f);
	sb.AddSprite(ZPoint<int>(x, y + n), NULL, g_pD3D->LuminanceOpacity, ZRect<int>(0, 0, 1, h - n), 1.0f);


//	DrawHorizontalLine(sb, x, y, x, ym, ZColour::Grey(0.1f * 256.0f));
//	DrawHorizontalLine(sb, x, ym, x, y + h, ZColour::Grey(0.8f * 256.0f));
}
void DrawHorizontalBar(ZSpriteBuffer &sb, int x, int y, int w, int h, double p)
{
	p = min(1, max(0, p));
	sb.AddSprite(ZPoint<int>(x, y), NULL, 0, ZRect<int>(0, 0, w, h), ZColour::Grey(20));
	sb.AddSprite(ZPoint<int>(x, y), NULL, 0, ZRect<int>(0, 0, w * p, h), ZColour::White());
//	sb.AddSprite(ZPoint<int>(x + (w * p), y), NULL, 0, ZRect<int>(0, 0, w * (1 - p), h), ZColour::Blue());
//	DrawHorizontalLine(sb, x, x + w, y, ZColour::White());
//	DrawHorizontalLine(sb, x, x + w, y+h, ZColour::White());
}
void DrawBeat(ZSpriteBuffer &sb)
{
//	d3d->SetTexture(0, NULL);

	sb.AddSprite(ZPoint<int>(10, 30), NULL, g_pD3D->InverseMultiply, ZRect<int>(0, 0, 600, 400), 1.0f - fHUDTransparency);

	bool bMipPoint = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) != 0;
	bool bMipLinear = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) != 0;
	
	bool bMagPoint = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) != 0;
	bool bMagLinear = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) != 0;

	bool bMinPoint = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) != 0;
	bool bMinLinear = (g_pD3D->g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) != 0;

	tef.Draw(&sb, MakeString("MipPoint: %s", (bMipPoint? "yes" : "no")).c_str(), ZPoint<int>(300, 145));
	tef.Draw(&sb, MakeString("MipLinear: %s", (bMipLinear? "yes" : "no")).c_str(), ZPoint<int>(300, 160));
	tef.Draw(&sb, MakeString("MagPoint: %s", (bMagPoint? "yes" : "no")).c_str(), ZPoint<int>(300, 175));
	tef.Draw(&sb, MakeString("MagLinear: %s", (bMagLinear? "yes" : "no")).c_str(), ZPoint<int>(300, 190));
	tef.Draw(&sb, MakeString("MinPoint: %s", (bMinPoint? "yes" : "no")).c_str(), ZPoint<int>(300, 205));
	tef.Draw(&sb, MakeString("MinLinear: %s", (bMinLinear? "yes" : "no")).c_str(), ZPoint<int>(300, 220));


//	d3d->SetState(g_pD3D->LuminanceOpacity);//Transparent);
	DrawHorizontalBar(sb, 20, 40, 256, 10, average);
	DrawHorizontalBar(sb, 20, 60, 256, 10, bigbeat);
	DrawHorizontalBar(sb, 20, 80, 256, 10, (4.0 * pdOrder[64]));//dBass);
	DrawHorizontalBar(sb, 20, 100, 256, 10, (4.0 * pdOrder[128]));//dMidRange);
	DrawHorizontalBar(sb, 20, 120, 256, 10, (4.0 * pdOrder[192]));//dTreble);

	for(int i = 0; i < 256; i++)
	{
		DrawLineBar(sb, 20 + i, 140, 200, 1 - pdSpec[i]);
	}
	for(i = 0; i < 16; i++)
	{
		DrawHorizontalBar(sb, 300, 280 + (i * 5), 256, 5, GetStrength(1.0, i, i + 1));
	}

//	char sBuf[200];
//	strcpy(sBuf, (active == 0)? "(no active)" : ppEffect[active]->name);
//	strlwr(sBuf);
//	tef.Draw(d3d, sBuf, 30, 350);
//	fSmall->Draw(d3d, 30, 350, D3DRGB(1,1,1), sBuf); 
//	strcpy(sBuf, (!bInFade || target == 0)? "(no target)" : ppEffect[target]->name);
//	strlwr(sBuf);
//	tef.Draw(d3d, sBuf, 30, 365);
//	fSmall->Draw(d3d, 30, 365, D3DRGB(1,1,1), sBuf); 

//	char sBuf[200];
//	sprintf(sBuf, "fps: %.1f (%.1f)", GetFPS(false), GetFPS(true));
//	tef.Draw(&sb, sBuf, ZPoint<int>(30, 380));
}
double MusicRandom()
{
	int i = 256 * rand() / RAND_MAX;
	
	int data1, data2;
	data1 = g_pcWaveform[0][i];
	if(data1 > 128) data1 = 256 - data1;
	data2 = g_pcWaveform[1][i];
	if(data2 > 128) data2 = 256 - data2;

	return (data1 + data2) / 256.0;
}
double GetStrength(double sensitivity, int nStart, int nEnd)
{
	double d = 0;
	sensitivity = min(1.0, max(0.0, sensitivity));
	int ns = NUMSPECS * sensitivity;
	ns = min(NUMSPECS - 1, max(ns, 1));
	nStart = min(max(nStart, 0), 15);
	nEnd = min(max(nEnd, 0), 15);
	for(int i = nStart; i < nEnd; i++)
	{
		double dt = 0;
		for(int j = 0; j < ns; j++) dt += ppdLastSpec[j][i];
		d = max(d, dt);
	}
	return d / ns;//(ns * (nEnd - nStart));
}
*/