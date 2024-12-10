#include "StdAfx.h"
#include "general.h"
#include "error.h"
#include "main.h"
#include "ZTexture.h"
#include "ZTextureFont.h"
#include "config.h"
#include "ZSpriteBuffer.h"
#include "CTextureItem.h"
#include "config-variables.h"
#include "config.h"
#include "TextureData.h"
#include "ZAudio.h"
//#include "mmsystem.h"
#include <H:\Microsoft Xbox SDK\xbox\include\algorithm>
#include "ZTripex.h"

/****** constants *****/

#define MSG_DISPLAY_TIME 4000
#define MSG_FADEOUT_TIME 1000

ZTexture *pBlankTexture;
int nEffect;
void InitObjectArrays( );
void InitObjectClipper( );


void ZTripex::ShowStatusMsg(const char *sFormat, ...)
{
	va_list pArg;
	va_start( pArg, sFormat );

	// TODO: FORZA: SORT THIS OUT
	//vsnprintf( sStatusMsg, sizeof( sStatusMsg ), sFormat, pArg );
	dwStatusTime = timeGetTime( );
}

DWORD WINAPI ZTripex::InitialiseThread(void *pParam)
{
	vector< CTextureItem* > ppItem;
	LoadTextureSettings(ppItem);

	for(int i = 1; i < (int)pvpEffectList->size(); i++)
	{
		if((*pvpEffectList)[i]->fPreference > FLOAT_ZERO)
		{
			pvpEffect->push_back((*pvpEffectList)[i]);
		}
	}

	vpTexture.clear();

	srand( timeGetTime( ) );

	_ASSERT(nIntTextures >= 1);

	int nTexture = 0;
	for(i = 0; i < (int)ppItem.size(); i++ )//nIntTextures + nDiskTextures; i++)
	{
		const UINT32 *pnTexData = g_apnIntTexture[ ppItem[ i ]->nInternalID ];

		ZTexture *pTexture = new ZTexture( );
		pTexture->SetFlags( ZTexture::F_FILTERING | ZTexture::F_MIP_CHAIN );
		pTexture->SetSource( pnTexData + 1, *pnTexData );
//		auto_ptr< ZTexture > pTexture( new ZTexture( ) );
//		pTexture->m_nFlags.set( ZTexture::F_SOURCE_FILE );
//		pTexture->m_nFlags.set( ZTexture::F_FILTERING );
//		pTexture->m_nFlags.set( ZTexture::F_MIP_CHAIN );
//		pTexture->m_pnData = ( BYTE* )( pnTexData + 1 );
//		pTexture->m_nDataSize = *pnTexData;

		ppItem[ i ]->pTexture = pTexture;

/*
		UINT32 *pnTex = UnpackJpeg( ( const UINT8* )( pnTexData + 1 ), pnTexData[ 0 ] );
		if( pnTex == NULL )
		{
			ppItem.erase(ppItem.begin() + i);
		}
		else 
		{
			ppItem[i]->pTexture = auto_ptr<ZTexture>(new ZTexture((ZColour*)pnTex));
			ppItem[i]->pTexture->m_nFlags.set(ZTexture::F_CREATE_MIPMAPS);
			i++;
		}
*/
/*

		HBITMAP hbmTexture = NULL;
		try
		{
			if(ppItem[i]->bInternal)//i < nIntTextures)//fInternal)
			{
				hbmTexture = LoadJPEG(hAppInstance, psIntTexture[ppItem[i]->nInternalID]);
				assert(hbmTexture != NULL);
			}
			else
			{
				int nIndex = ppItem[i]->sFilename.find_last_of('.');
				if(nIndex != -1)
				{
					string sExtension = ppItem[i]->sFilename.substr(nIndex);//Mid(nIndex);
					if(!_stricmp(sExtension.c_str(), ".jpeg") || !_stricmp(sExtension.c_str(), ".jpg") || !_stricmp(sExtension.c_str(), ".jif"))
					{
						hbmTexture = LoadJPEG(ppItem[i]->sFilename.c_str());
					}
					else if(!_stricmp(sExtension.c_str(), ".gif"))
					{
						hbmTexture = LoadGIF(ppItem[i]->sFilename.c_str());
					}
					else if(!_stricmp(sExtension.c_str(), ".bmp"))
					{
						hbmTexture = (HBITMAP)LoadImage(NULL, ppItem[i]->sFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
					}
				}
			}
		}
		catch(...)
		{
			hbmTexture = NULL;
		}

		if(hbmTexture == NULL)
		{
			ppItem.erase(ppItem.begin() + i);
		}
		else 
		{
			cTexture.Copy(hbmTexture);

			ppItem[i]->pTexture = auto_ptr<ZTexture>(new ZTexture(cTexture.GetPtr()));
			ppItem[i]->pTexture->nFlags.set(ZTexture::TF_CREATE_MIPMAPS);
			i++;
		}
*/		nTexture++;
	}

	pBlankTexture = NULL;
	for(i = 0; i < (int)ppItem.size(); i++)
	{
		if(ppItem[i]->bInternal && ppItem[i]->nInternalID == 1)
		{
			pBlankTexture = ppItem[i]->pTexture;//.release();
		}
		for(set<int>::iterator it = ppItem[i]->snClass.begin(); it != ppItem[i]->snClass.end(); it++)
		{
			ppItem[i]->pTexture->m_snType.insert(*it);
		}

		g_pD3D->AddTexture( ppItem[ i ]->pTexture );
//		vpTexture.push_back(ppItem[i]->pTexture);
	}

	for(i = 0; i < (int)pvpEffect->size(); i++)
	{
		(*pvpEffect)[i]->Create();
	}
	return 0;
}

HRESULT ZTripex::Startup()
{
	InitObjectArrays( );
	InitObjectClipper( );

	printf( "init arrays\n" );

	txs.reset();

	pBlankTexture = NULL;

	pvpEffect = new vector< ZEffectPtr* >;
	pvpEffect->push_back((*pvpEffectList)[0]);

	printf( "tex\n" );

	vpTexture.clear();
	fEffectFrames = 0.0f;
	fFadePos = 0.0f;
	sStatusMsg[ 0 ] = 0;
	dwStatusTime = 0;
	nEffect = 0;
	nNextEffect = 0; 

//	txs[TXS_RENDER_FIRST] = true;
	txs[TXS_STARTED] = true;

//	InitBeats();
	LoadCfgItems();
	UpdateCfgItems(true);

	printf( "cfg items\n" );

	g_pAudio = new ZAudio( 512 );
	g_pD3D = new ZDirect3D;

	printf( "d3d\n" );

	srand( timeGetTime( ) );

	printf( "srand\n" );

	PALETTEENTRY *ppe = new PALETTEENTRY[ 256 ];
	{
		for( int i = 0; i < 256; i++ )
		{
			ppe->peRed = ppe->peGreen = ppe->peBlue = i;
			ppe->peFlags = 0xff;
		}
	}


//	{for( int i = 0; i < 256 * 256; i++ )
//	{
//		unsigned char c = ((UINT8*)&g_anTexRawGUI[ 1 ])[ i ];
//		pc[ i ] = ZColour( c,c,c);
//	}
//	}

	gui = auto_ptr< ZTexture >( new ZTexture( ) );
	gui->SetSource( ppe, &g_anTexRawGUI[ 1 ], 256 * 256, 256 );

//	gui = auto_ptr< ZTexture >(new ZTexture(pc));//(ZColour*)g_anTexRawGUI ));//cGUI.GetPtr()));
//	gui->m_nFlags.set(ZTexture::F_POINT_FILTER);

	// TEXTURE
	g_pD3D->AddTexture( gui.get( ) );
//	g_pD3D->vpTexture.push_back(gui.get());//auto_ptr< ZTexture >(gui);

	printf( "gui tex\n" );

	tef.Add( ( UINT8* )&g_anTexRawFont[ 1 ] );
	tef.GetLetter(' ')->nEnd = 2;
	g_pD3D->AddTexture( tef.GetTexture( ) );
//	g_pD3D->vpTexture.push_back(tef.GetTexture());

//	pcBuffer.SetLength(1024 * 16);

//	pcHUD = new ZPaletteCanvas;
//	pcHUD->Create(true, 1024, 16);
//	ZColour c[256];
//	for(int i = 0; i < 256; i++) c[i] = ZColour::Grey(i);
//	pcHUD->SetPalette(c);

	srand( timeGetTime( ) );
	
	printf( "pre init thread\n" );
	InitialiseThread( NULL );
	printf( "post init thread\n" );

	for( int i = 0; i < (int)vpTexture.size(); i++)
	{
		g_pD3D->AddTexture( vpTexture[ i ].get( ) );
//		g_pD3D->vpTexture.push_back(vpTexture[i].get());
	}

	printf( "added textures\n" );

	id = (int)pvpEffect->size();

	txs.set(TXS_CHANGE_EFFECT);

	txs.set( TXS_RESET_TIMING );

	printf( "d3d open\n" );

	HRESULT hRes = g_pD3D->Open( );
	if( FAILED( hRes ) ) return TraceError( hRes );

	printf( "d3d opened\n" );
//	hRes = pcHUD->Initialise(d3d.get());
//	if(FAILED(hRes)) return TraceError(hRes);

	return D3D_OK;
}
#include "ZTripex.h"
extern ZTripex *g_pTripex;
HRESULT ZTripex::Render()
{
	UpdateCfgItems();

	DWORD dwTime = timeGetTime( );
	if(txs.test(TXS_RESET_TIMING))//bResetTiming)
	{
		dwLastTime = dwTime - 10000;
		txs.reset(TXS_RESET_TIMING);
//		bResetTiming = false;
	}

	static float fFrames = 0;
	DWORD dwTimeChange = dwTime - dwLastTime;

	fFrames += min(4.0f, dwTimeChange / (1000.0f / 15.0f));
	dwLastTime = dwTime;
//	AddFrameTime(false, dwTimeChange);

	printf( "Render Started\n" );

	fEffectFrames += fFrames;
	fFadePos += dwTimeChange; 

	ZEffectPtr *ppDrawEffect[2];
	if(txs.test(TXS_EFFECT_LEFT) || txs.test(TXS_EFFECT_RIGHT))//bEffectLeft || bEffectRight))
	{
		int nNewEffect = nEffect;
		if(txs.test(TXS_EFFECT_LEFT))
		{
			if(nEffect > 1) nNewEffect--;
			txs.reset(TXS_EFFECT_LEFT);
		}
		else
		{
			if(nEffect < ( int )pvpEffect->size() - 1) nNewEffect++;
			txs.reset(TXS_EFFECT_RIGHT);
		}

		if(nNewEffect != nEffect)
		{
			nEffect = nNewEffect;
			ShowStatusMsg("Current Effect: %s", (*pvpEffect)[nEffect]->sName.c_str());

//				if(bEffectLeft) nEffect--;
//				else if(bEffectRight) nEffect++;
//				bEffectLeft = bEffectRight = false;

			txs.reset(TXS_IN_FADE);
//				bInFade = false;
			fEffectFrames = 0;

			HRESULT hRes = (*pvpEffect)[nEffect]->Reconfigure( );
			if(FAILED(hRes)) return TraceError(hRes); 
		}
	}
	if(!txs.test(TXS_IN_FADE) && (!txs.test(TXS_HOLD) || txs.test(TXS_CHANGE_EFFECT)) && 
		(nEffect == 0 || 
		fEffectFrames > ((*pvpEffect)[nEffect]->fChange * EFFECT_CHANGE_FRAMES)) && 
		pvpEffect->size() > 1)
	{
		txs.reset(TXS_CHANGE_EFFECT);
//				bChangeEffect = false;
		nNextEffect = 0;

		txs.set(TXS_IN_FADE);
//				bInFade = true;
		fEffectFrames = 0;

		for(int i = 1; i < ( int )pvpEffect->size(); i++) 
		{
			(*pvpEffect)[i]->bValid = (i != nEffect && (*pvpEffect)[nEffect]->nDrawOrder != (*pvpEffect)[i]->nDrawOrder);
		}

		float pt = 0;
		// fairness = (random)0-1(ordered)
		float temperature = (1.0f / (fFairness)) - 1;
		// temperature = (random)+inf - 0(ordered)
		for(i = 1; i < ( int )pvpEffect->size(); i++)
		{
			float weight = (float(id - (*pvpEffect)[i]->nLastUsed) / (pvpEffect->size() - 1)) - 1;
			// weight = small (last used) - large (not used)

			if(!(*pvpEffect)[i]->bValid)
			{
				(*pvpEffect)[i]->fProb = 0.0f;
			}
			else if(temperature < FLOAT_ZERO)
			{
				(*pvpEffect)[i]->fProb = (weight >= 0)? 0.5f : 0.0f;
			}
			else 
			{
				(*pvpEffect)[i]->fProb = 1.0f / (1.0f + expf(-weight / temperature));
			}

			(*pvpEffect)[i]->fProb *= (*pvpEffect)[i]->fPreference * max(0.1f, 1.0f - fabs(g_pAudio->GetIntensity( ) - (*pvpEffect)[i]->fActivity));

			pt += (*pvpEffect)[i]->fProb;//vpEffect[i]->preference * p[i];
		}

		nNextEffect = 0;
		if(pt > FLOAT_ZERO)
		{
			for(i = 1; i < ( int )pvpEffect->size(); i++) (*pvpEffect)[i]->fProb /= pt;

			double r = (rand() % 1000);
			for(nNextEffect = 1; nNextEffect < ( int )pvpEffect->size() - 1; nNextEffect++) 
			{
				r -= (*pvpEffect)[nNextEffect]->fProb * 1000.0;
				if(r < 0) break;
			}
		}
		(*pvpEffect)[nNextEffect]->nLastUsed = id;

		fFadePos = 0;
		txs.reset(TXS_RESET_TARGET);
//				bResetTarget = false;
	}
	if(txs[TXS_RECONFIGURE])//bReconfigure)
	{
		HRESULT hRes = (*pvpEffect)[nEffect]->Reconfigure( );
		if(FAILED(hRes)) return TraceError(hRes);

		txs.reset(TXS_IN_FADE);
		txs.reset(TXS_RECONFIGURE);
	}

	if(txs.test(TXS_HOLD))//bEffectPref)
	{
		txs[TXS_IN_FADE] = false;
		if(nEffect == 0) nEffect = nNextEffect;
	}

	float fOut = (nEffect == 0)? 0 : (fFadeOut * 5000.0f);
	float fFadeLength = (fFadeIn * 5000.0f) + fOut - (fCrossfading * min((fFadeIn * 5000.0f), fOut));

	float fBr = txs.test(TXS_IN_FADE)? min(1, max(0, 1 - (fFadePos / fOut))) : 1;
	ppDrawEffect[0] = (fBr < FLOAT_ZERO)? pEffectBlank : (*pvpEffect)[nEffect];
	ppDrawEffect[0]->fBr = fBr;

	fBr = txs.test(TXS_IN_FADE)? min(1.0f, max(0.0f, 1.0f - ((fFadeLength - fFadePos) / (fFadeIn * 5000.0f)))) : 0;
	ppDrawEffect[1] = (fBr < FLOAT_ZERO)? pEffectBlank : (*pvpEffect)[nNextEffect];
	ppDrawEffect[1]->fBr = fBr;

	if(ppDrawEffect[1]->fBr > FLOAT_ZERO && !txs[TXS_RESET_TARGET])
	{
		HRESULT hRes = ppDrawEffect[1]->Reconfigure( );
		if(FAILED(hRes)) return TraceError(hRes);

		txs[TXS_RESET_TARGET] = true;
	}
	if(ppDrawEffect[1]->fBr >= 1 - FLOAT_ZERO)
	{
		nEffect = nNextEffect;
		ppDrawEffect[0] = ppDrawEffect[1];
		ppDrawEffect[1] = pEffectBlank;//&blank;
		txs.reset(TXS_IN_FADE);
	}

	if(ppDrawEffect[1]->nDrawOrder < ppDrawEffect[0]->nDrawOrder)
	{
		swap( ppDrawEffect[1], ppDrawEffect[0] );
	}
	if((!ppDrawEffect[0]->CanRender(fFrames) || ppDrawEffect[0] == pEffectBlank) && (!ppDrawEffect[1]->CanRender(fFrames) || ppDrawEffect[1] == pEffectBlank) && !(ppDrawEffect[0] == pEffectBlank && ppDrawEffect[1] == pEffectBlank && fFrames > 1.0f))
	{
		return D3D_OK;
	}

	printf( "Audio Update\n" );

	g_pAudio->Update( fFrames );
//	UpdateBeat(fFrames);

	for(int i = 0; i < 2; i++)
	{
		HRESULT hRes = ppDrawEffect[i]->Calculate( fFrames );
		if(FAILED(hRes)) return TraceError(hRes);
	}

	// lock the back

//	static unsigned int nc = 0x0000ffff;
//	nc ^= 0x00005553;

	printf( "Device Clear\n" );

	HRESULT hRes = g_pD3D->g_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	if(FAILED(hRes)) return TraceError(hRes);

	printf( "Begin scene\n" );

	hRes = g_pD3D->g_pDevice->BeginScene( );
	if( FAILED( hRes ) ) return TraceError( hRes );

	printf( "Set Vertex Shader\n" );

	hRes = g_pD3D->g_pDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );
	if( FAILED( hRes ) ) return TraceError( hRes );

	printf( "Set Texture\n" );

	g_pD3D->SetTexture(0, gui.get());
/*
	Transparent = (1 << 0L),
	Multiply = (1 << 1L),
	Perspective = (1 << 2L),
	Shade = (1 << 3L),
	ZBuffer = (1 << 4L),
	PerspectiveCorrect = (1 << 5L),
	LuminanceOpacity = (1 << 6L),
	InverseMultiply = (1 << 7L),
*/
	printf( "SetRenderState\n" );
	// D3DRS_CLIPPING is not supported in the XBox DX8 SDK ...
	//g_pD3D->SetRenderState(D3DRS_CLIPPING, false);
	// ... Forza.

		for(i = 0; i < 2; i++)
		{
			DWORD dwStartTick = timeGetTime( );

			printf( "Render Effect %d\n", i );
			ppDrawEffect[i]->Render( );
			printf( "Done\n" );
			if(FAILED(hRes)) return TraceError(hRes);
		}

	ZSpriteBuffer sb;

//	int nX = d3d->GetWidth() * 0.06;
//	int nY = d3d->GetHeight() - 63;

	printf( "Set Texture\n" );
	g_pD3D->SetTexture(0, g_pTripex->gui.get());

	string sMsg;
	float fMsgBr = 0.0f;

	static int n = 0;
	if(pvpEffect->size() <= 1 || TRUE )
	{
		n++;

		// TODO
		static char sBuf[ 200 ];
		sprintf(sBuf, "%d\n", n );

		sMsg = sBuf;//"No effects can be used" + sBuf;
		fMsgBr = 1.0f;
	}

	DWORD dwTick = timeGetTime( );
	if(dwTick >= dwStatusTime && dwTick <= dwStatusTime + (MSG_DISPLAY_TIME + MSG_FADEOUT_TIME) && sStatusMsg[ 0 ] != 0)
	{
		sMsg = sStatusMsg;
		if(dwTick < dwStatusTime + MSG_DISPLAY_TIME) fMsgBr = 1.0f;
		else
		{
			float fPos = (dwTick - (dwStatusTime + MSG_DISPLAY_TIME)) / (float)MSG_FADEOUT_TIME;
			float fV = 1.0f - cos(fPos * 3.14159f / 2.0f);
			fMsgBr = 1.0f - (fV * fV);//fPos;//fV;//1 - (fV * fV);//1.0f;
		}
	}
	if(sMsg.size() > 0 && bShowMessages)
	{
		printf( "Draw Message: %s\n", sMsg.c_str() );
		//DrawMessage(sb, &tef, 38, sMsg.c_str(), fMsgBr, 1 - fHUDTransparency);
	}

	if(txs.test(TXS_VISIBLE_BEATS))
	{
		printf( "Audio Render\n" );
		g_pAudio->Render( sb );
	}

//		ZSpriteBuffer sb;
//		sb.AddSprite(ZPoint<int>(10, 30), d3d->vpTexture[3], g_pD3D->Shade, ZRect<int>(0, 0, 600, 400), 1.0f );
//		sb.Flush( d3d.get( ) );

	printf( "Flush\n" );
	hRes = sb.Flush( );
	if(FAILED(hRes)) return TraceError(hRes);

	printf( "End Scene\n" );
	hRes = g_pD3D->g_pDevice->EndScene( );
	if( FAILED( hRes ) ) return TraceError( hRes );

	printf( "Present\n" );
	hRes = g_pD3D->g_pDevice->Present(NULL, NULL, NULL, NULL);
	if( FAILED( hRes ) ) return TraceError( hRes );

//	AddFrameTime(true, clock() - dwRenderStartClock);

	printf( "Finished Render\n" );
	fFrames = 0;
	return 0;
}
void ZTripex::Shutdown( )
{
	if(!txs.test(TXS_STARTED)) return;
	txs[TXS_STARTED] = false;

	g_pD3D->Close( );

	if( g_pAudio != NULL )
	{
		delete g_pAudio;
		g_pAudio = NULL;
	}

//	gui = auto_ptr<ZTexture>(NULL);
//	pcHUD = NULL;

	for(int i = 0; i < ( int )pvpEffect->size(); i++)
	{
		(*pvpEffect)[i]->Destroy();
	}

	SaveCfgItems();
}
