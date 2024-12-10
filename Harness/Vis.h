#pragma once

struct VIS_INFO
{
	bool bWantsFreq;
	int iSyncDelay;
	// int iAudioDataLength;
	// int iFreqDataLength;
};

struct VISUALISATION
{
	void ( _cdecl* Create )( LPDIRECT3DDEVICE8 pd3dDevice );
	void ( _cdecl* Start )( int iChannels, int iSamplesPerSec, int iBitsPerSample );
	void ( _cdecl* AudioData )( short* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength );
	void ( _cdecl* Render )( );
	void ( _cdecl* Stop )( );
	void ( _cdecl* GetInfo )( VIS_INFO* pInfo );
};
