#include "StdAfx.h"
#include "effect.h"
#include "Tripex.h"
#include "xbmc.h"

//IDirect3DDevice9 *g_pd3dDevice;
//unsigned char g_pcSpectrum[ 2 ][ 576 ];
//unsigned char g_pcWaveform[ 2 ][ 576 ];

BOOL g_bInit = FALSE;

Tripex *g_pTripex;

//void _cdecl Create( IDirect3DDevice8 *pd3dDevice )
void _cdecl Create( LPDIRECT3DDEVICE9 pd3dDevice, int iWidth, int iHeight, const char* szVisualisationName)
{
//	g_pd3dDevice = pd3dDevice;
	g_bInit = FALSE;
}

//void _cdecl Start( int iChannels, int iSamplesPerSec, int iBitsPerSample )
void _cdecl Start( int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
	g_pTripex = new Tripex();

	Error* error = g_pTripex->Startup( );
	if( error ) 
	{
		g_pTripex->Shutdown( );
		g_bInit = FALSE;
	}
	else
	{
		g_bInit = TRUE;
	}
}

void _cdecl AudioData( short* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength )
{
	if( g_pTripex->pAudio != NULL )
	{
		g_pTripex->pAudio->SetDataFormat( 2, 44100, 16 );
		g_pTripex->pAudio->AddData( pAudioData, iAudioDataLength );
	}

//	for( int i = 0; i < 2; i++ )
//	{
//		for( int j = 0; j < 576; j++ )
//		{
//			g_pcSpectrum[ i ][ j ] = 0.6f * rand( ) / powf(j, 1.4f);
//			//	powf( rand( ), 1.0f + ( 4.0f * -j / 576.0f ) );
//			g_pcWaveform[ i ][ j ] = rand( );
//		}
//	}
}

void _cdecl Render( )
{
	if( g_bInit )
	{
		Error* hRes = g_pTripex->Render( );
		if( hRes )
		{
			g_pTripex->Shutdown( );
			g_bInit = FALSE;
		}
	}
}

void _cdecl Stop( )
{
	if( g_bInit )
	{
		g_pTripex->Shutdown( );
		g_bInit = FALSE;
	}
}

void _cdecl GetInfo( VIS_INFO *pInfo )
{
	pInfo->bWantsFreq = TRUE;
	pInfo->iSyncDelay = 0;
}
