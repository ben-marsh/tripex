#ifndef _ZTRIPEX_H
#define _ZTRIPEX_H

#include "main.h"
#include "ZTextureFont.h"

class ZTripex
{
public:
	bitset<TXS_LAST> txs;

	ZTextureFont tef;
	ZTexture *ptefTexture;
	vector< auto_ptr< ZTexture > > vpTexture;
	float fEffectFrames, fFadePos;
	char sStatusMsg[ 256 ];
	DWORD dwStatusTime;
	unsigned int id;
	int nNextEffect;
	auto_ptr< ZTexture > gui;
	DWORD dwLastTime;

	void ShowStatusMsg( const char *sFormat, ... );
	DWORD WINAPI InitialiseThread(void *pParam);
	HRESULT Startup( );
	HRESULT Render( );
	void Shutdown( );

};

#endif