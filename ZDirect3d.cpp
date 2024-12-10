#include "StdAfx.h"
#include "ZDirect3d.h"
#include <d3dx9.h>
#include "ZTexture.h"
#include "error.h"

/*
// D3DXCreateTextureFromFileInMemory( ):
	extern "C" __declspec(dllimport) HRESULT __cdecl D3DXCreateTextureFromFileInMemory( LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcData, LPDIRECT3DTEXTURE8* ppTexture );

	// D3DXCreateTexture( ):
	extern "C" __declspec(dllimport) HRESULT __cdecl D3DXCreateTexture( LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture );
};
*/

extern IDirect3DDevice9* g_pd3dDevice;

ZDirect3D* g_pD3D = NULL;

/*---------------------------------
* Methods
-----------------------------------*/

ZDirect3D::TextureStage::TextureStage()
{
	pTexture = (ZTexture*)-1;
	pNewTexture = (ZTexture*)-1;
}

ZDirect3D::ZDirect3D()
{
	g_pDevice = NULL;
}

HRESULT ZDirect3D::Close()
{
	for (set< ZTexture* >::iterator it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		DestroyTexture(*it);
	}

	g_pDevice = NULL;

	return D3D_OK;
}
int ZDirect3D::GetWidth()
{
	return nWidth;
}
int ZDirect3D::GetHeight()
{
	return nHeight;
}
HRESULT ZDirect3D::Open()
{
	_ASSERT(g_pDevice == NULL);

	g_pDevice = ::g_pd3dDevice;

	printf("GetBackBuffer\n");
	IDirect3DSurface9* pd3dSurface = NULL;
	HRESULT hRes = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dSurface);
	if (FAILED(hRes)) return TraceError(hRes);
	printf("Done\n");

	printf("Get Surface Desc\n");
	D3DSURFACE_DESC d3dsd;
	hRes = pd3dSurface->GetDesc(&d3dsd);
	pd3dSurface->Release();
	if (FAILED(hRes)) return TraceError(hRes);
	printf("Done\n");

	//printf( "surface desc\n" );

	nWidth = d3dsd.Width;
	nHeight = d3dsd.Height;

	printf("Width: %d  Height: %d\n", nWidth, nHeight);

	printf("Get Device Caps\n");
	hRes = g_pDevice->GetDeviceCaps(&g_Caps);
	if (FAILED(hRes)) return TraceError(hRes);
	printf("Done\n");

	//printf( "caps\n" );

	for (set< ZTexture* >::iterator it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		printf("texture\n");

		HRESULT hRes = CreateTexture(*it);

		printf("returned\n");

		if (FAILED(hRes)) return TraceError(hRes);

		printf("after CreateTexture\n");
	}
	return D3D_OK;
}
ZTexture* ZDirect3D::Find(int nType)
{
	int mt = 0;

	set< ZTexture* >::iterator it;
	for (it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		if ((*it)->m_snType.count(nType) > 0) mt++;
	}

	int ct = rand() * mt / (RAND_MAX + 1);

	for (it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		if ((*it)->m_snType.count(nType) > 0)
		{
			if (ct == 0) return *it;
			ct--;
		}

	}
	return NULL;
}
HRESULT ZDirect3D::DrawIndexedPrimitive(ZArray<ZVertexTL>& pVertex, ZArray<ZFace>& pFace)
{
	HRESULT hRes;

	hRes = FlushRenderState();
	if (FAILED(hRes)) return TraceError(hRes);

	hRes = FlushTextureState();
	if (FAILED(hRes)) return TraceError(hRes);

	hRes = g_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pVertex.GetLength(), pFace.GetLength(), pFace.GetBuffer(), D3DFMT_INDEX16, pVertex.GetBuffer(), sizeof(ZVertexTL));
	if (FAILED(hRes)) return TraceError(hRes);

	return D3D_OK;
}
void ZDirect3D::SetState(int nFlags)
{
	ResetRenderState();
	if (!(nFlags & Shade))
	{
		SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}
	if (nFlags & Multiply)
	{
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	else if (nFlags & InverseMultiply)
	{
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	else if (nFlags & LuminanceOpacity)
	{
		// dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	else if (nFlags & Transparent)
	{
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	//	if(!(nFlags & PerspectiveCorrect))
	//	{
	//		mpNewRender[D3DRS_TEXTUREPERSPECTIVE] = FALSE;
	//	}
	if (!(nFlags & ZBuffer))
	{
		SetRenderState(D3DRS_ZENABLE, FALSE);
		SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
}
void ZDirect3D::ResetRenderState()
{
	//int ichk = 0;
	mpNewRender.clear();
	//printf( "ZDirect3d::ResetRenderState %d to %d\n", mpRender.begin(), mpRender.end() );
	for (StateBuffer::iterator it = mpRender.begin(); it != mpRender.end(); it++)
	{
		//printf( "ZDirect3d::ResetRenderState loop #%d - %d - %d\n", ichk, it->first, it->second.first );
		SetRenderState(it->first, it->second.second);
		//ichk ++;
	}
	//printf( "Done\n" );
}
void ZDirect3D::SetRenderState(DWORD dwKey, DWORD dwValue)
{
	mpNewRender[dwKey] = dwValue;
}
void ZDirect3D::GetStateChanges(NewStateBuffer& mp_new, StateBuffer& mp_current, vector< StateBufferChange >& vc)
{
	// all those which have to be inserted
	StateBuffer::iterator it_current = mp_current.begin();

	for (NewStateBuffer::iterator it_new = mp_new.begin(); it_new != mp_new.end(); it_new++)
	{
		for (;;)
		{
			if (it_current == mp_current.end() || it_current->first > it_new->first)
			{
				// don't know what the current state is, so need to get it all
				pair<DWORD, DWORD> pn = pair< DWORD, DWORD >(it_new->second, 0);
				it_current = mp_current.insert(it_current, StateBuffer::value_type(it_new->first, pn));
				vc.push_back(StateBufferChange(it_current, true));
				it_current++;
				break;
			}
			else if (it_current->first == it_new->first)
			{
				if (it_new->second != it_current->second.first)
				{
					it_current->second.first = it_new->second;
					vc.push_back(StateBufferChange(it_current, false));
				}
				it_current++;
				break;
			}
			else
			{
				if (it_current->second.second != it_current->second.first)
				{
					it_current->second.first = it_current->second.second;
					vc.push_back(StateBufferChange(it_current, false));
				}
				it_current++;
			}
		}
	}
}
HRESULT ZDirect3D::FlushRenderState()
{
	HRESULT hRes;
	vector<StateBufferChange> vc;
	GetStateChanges(mpNewRender, mpRender, vc);

	for (int i = 0; i < (int)vc.size(); i++)
	{
		StateBuffer::iterator it = vc[i].first;

		if (vc[i].second)
		{
			hRes = g_pDevice->GetRenderState((D3DRENDERSTATETYPE)it->first, &it->second.second);
			if (FAILED(hRes)) return TraceError(hRes);
		}

		hRes = g_pDevice->SetRenderState((D3DRENDERSTATETYPE)it->first, it->second.first);
		if (FAILED(hRes)) return TraceError(hRes);
	}

	mpNewRender.clear();

	return D3D_OK;
}

HRESULT ZDirect3D::FlushTextureState()
{
	HRESULT hRes;
	for (DWORD dwStage = 0; dwStage < 8; dwStage++)
	{
		if (pTextureStage[dwStage].pTexture != pTextureStage[dwStage].pNewTexture && pTextureStage[dwStage].pNewTexture != (ZTexture*)-1)
		{
			IDirect3DTexture9* pTex = NULL;
			if (pTextureStage[dwStage].pNewTexture != NULL)
			{
				pTex = pTextureStage[dwStage].pNewTexture->m_pd3dTexture;
			}

			hRes = g_pDevice->SetTexture(dwStage, pTex);
			if (FAILED(hRes)) return TraceError(hRes);

			pTextureStage[dwStage].pTexture = pTextureStage[dwStage].pNewTexture;

			ZTexture* pTexture = pTextureStage[dwStage].pTexture;
			if (pTexture != NULL)
			{
				if (pTexture->m_nFlags & ZTexture::F_MIP_CHAIN)
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
					}
					else if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
					}
				}
				else
				{
					g_pDevice->SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
				}

				if (pTexture->m_nFlags & ZTexture::F_FILTERING)
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					}
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					}
				}
				else
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
					}
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
					{
						g_pDevice->SetSamplerState(dwStage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
					}
				}
			}
		}

		vector< StateBufferChange > vc;
		GetStateChanges(pTextureStage[dwStage].mpNewState, pTextureStage[dwStage].mpState, vc);

		for (int i = 0; i < (int)vc.size(); i++)
		{
			StateBuffer::iterator it = vc[i].first;

			if (vc[i].second)
			{
				hRes = g_pDevice->GetTextureStageState(dwStage, (D3DTEXTURESTAGESTATETYPE)it->first, &it->second.second);
				if (FAILED(hRes)) return TraceError(hRes);
			}

			hRes = g_pDevice->SetTextureStageState(dwStage, (D3DTEXTURESTAGESTATETYPE)it->first, it->second.first);
			if (FAILED(hRes)) return TraceError(hRes);
		}

		pTextureStage[dwStage].mpNewState.clear();
	}
	return D3D_OK;
}
void ZDirect3D::SetTexture(DWORD dwStage, ZTexture* pTexture, DWORD dwOp, DWORD dwArg2)
{
	ResetTextureStageState(dwStage);

	pTextureStage[dwStage].pNewTexture = pTexture;
	if (pTexture != NULL)
	{
		SetTextureStageState(dwStage, D3DTSS_COLOROP, dwOp);
		SetTextureStageState(dwStage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(dwStage, D3DTSS_COLORARG2, dwArg2);
	}
	else
	{
		SetTextureStageState(dwStage, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}
	SetTextureStageState(dwStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

HRESULT ZDirect3D::AddTexture(ZTexture* pTexture)
{
	pair< set< ZTexture* >::iterator, bool > itb = g_spTexture.insert(pTexture);
	if (itb.second && g_pDevice != NULL)
	{
		HRESULT hRes = CreateTexture(pTexture);
		if (FAILED(hRes)) return TraceError(hRes);
	}
	return D3D_OK;
}

HRESULT ZDirect3D::UploadTexture(ZTexture* pTexture)
{
	if (pTexture->m_pSrcData == NULL) return D3D_OK;

	const ZRect< int > rSrc(0, 0, 256, 256);

	SINT32 nMipLevels = pTexture->m_pd3dTexture->GetLevelCount();
	for (SINT32 i = 0; i < nMipLevels; i++)
	{
		IDirect3DSurface9* pSurface;

		HRESULT hRes = pTexture->m_pd3dTexture->GetSurfaceLevel(i, &pSurface);
		if (FAILED(hRes)) return TraceError(hRes);

		D3DSURFACE_DESC d3dsd;
		hRes = pSurface->GetDesc(&d3dsd);
		if (FAILED(hRes)) return TraceError(hRes);

		D3DLOCKED_RECT d3dr;
		if (d3dsd.Width == 256 &&
			d3dsd.Height == 256 &&
			d3dsd.Format == pTexture->m_nSrcFmt &&
			SUCCEEDED(pSurface->LockRect(&d3dr, NULL, 0)))
		{
			const UINT8* pnSrc = (const UINT8*)pTexture->m_pSrcData;
			for (SINT32 i = 0; i < 256; i++)
			{
				//					( ( UINT8* )d3dr.pBits )[ i ] = rand( );
				memcpy(d3dr.pBits, pnSrc, 256);
				d3dr.pBits = (UINT8*)(d3dr.pBits) + d3dr.Pitch;
				pnSrc += pTexture->m_nSrcSpan;
			}

			hRes = pSurface->UnlockRect();
			if (FAILED(hRes)) return TraceError(hRes);
		}
		else
		{
			//				hRes = D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pTexture->m_pSrcData, 
			//					pTexture->m_nSrcFmt, pTexture->m_nSrcSpan, pTexture->m_pSrcPalette, 
			//					&rSrc, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			//				if( FAILED( hRes ) ) return TraceError( hRes );
		}
	}
	return D3D_OK;
}

HRESULT ZDirect3D::CreateTexture(ZTexture* pTexture)
{
	_ASSERT(g_pd3dDevice != NULL);
	_ASSERT(pTexture->m_pd3dTexture == NULL);

	printf("%p\n", pTexture);

	if (pTexture->m_nSrcFmt == D3DFMT_UNKNOWN)//pTexture->m_nFlags & ZTexture::F_SRC_FILE )
	{
		HRESULT hRes = D3DXCreateTextureFromFileInMemory(g_pDevice, pTexture->m_pSrcData, pTexture->m_nSrcSize, &pTexture->m_pd3dTexture);
		if (FAILED(hRes)) return TraceError(hRes);
	}
	else
	{
		UINT32 nMipLevels = (pTexture->m_nFlags & ZTexture::F_MIP_CHAIN) ? 0 : 1;
		//			UINT32 nUsage = 0;//( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DUSAGE_DYNAMIC : 0;
		//			D3DPOOL nPool = ( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		//			D3DPOOL nPool = ( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DPOOL_SYSMEMORY : D3DPOOL_MANAGED;

		UINT32 nUsage = 0;//( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DUSAGE_DYNAMIC : 0;
		D3DPOOL nPool = D3DPOOL_MANAGED;
		/*
			hRes = pd3dDevice->CreateTexture(
				256, 256,
				nLevels,
				pTexture->nFlags.test(ZTexture::TF_DYNAMIC)? D3DUSAGE_DYNAMIC :
			0,
				pTexture->nFormat,
				D3DPOOL_MANAGED, &pTexture->pd3dTexture);
		*/

		HRESULT hRes = D3DXCreateTexture(g_pDevice, 256, 256, nMipLevels, nUsage, pTexture->m_nSrcFmt /*D3DFMT_R5G6B5*/, nPool, &pTexture->m_pd3dTexture);
		if (FAILED(hRes)) return TraceError(hRes);

		PALETTEENTRY pe[256];
		for (int i = 0; i < 256; i++)
		{
			pe[i].peRed = pe[i].peGreen = pe[i].peBlue = i;
			pe[i].peFlags = 0xff;
		}

		g_pDevice->SetPaletteEntries( 0, pe );
		g_pDevice->SetCurrentTexturePalette( 0 );

		UploadTexture(pTexture);
	}








	/*		int nLevels = pTexture->m_pd3dTexture->GetLevelCount( );
		pTexture->m_nFlags.set(ZTexture::F_HAS_MIPMAPS, (nLevels > 1));
		for( int i = 0; i < nLevels; i++ )
		{
			IDirect3DSurface8 *pSurface = NULL;
			pTexture->m_pd3dTexture->GetSurfaceLevel( i, &pSurface );

			ZRect< int > r( 0, 0, 256, 256 );
			if( pcPalette != NULL )
			{
				PALETTEENTRY an[ 256 ];
				for( int i = 0; i < 256; i++ )
				{
					an[ i ].peRed = pcPalette[ i ].m_nR;
					an[ i ].peGreen = pcPalette[ i ].m_nG;
					an[ i ].peBlue = pcPalette[ i ].m_nB;
					an[ i ].peFlags = 0xff;
				}
				D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pnIndex, D3DFMT_P8, 256, an, &r, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			}
			else
			{
				ZColour *pcSrc = (ZColour*)&pTexture->m_vnData[ 0 ];
				UINT32 an[ 256 * 256 ];
				for( int i = 0; i < 256 * 256; i++ ) an[ i ] = pcSrc[ i ].dw;//.Reverse( ).GetCOLORREF( );
				D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, an, D3DFMT_X8R8G8B8, 256 * 4, NULL, &r, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			}

			pSurface->Release( );
		}
	*/

	return D3D_OK;
}
void ZDirect3D::DestroyTexture(ZTexture* pTexture)
{
	//		assert(HasTexture(pTexture));

	if (pTexture->m_pd3dTexture != NULL)
	{
		pTexture->m_pd3dTexture->Release();
		pTexture->m_pd3dTexture = NULL;
	}
}
void ZDirect3D::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwKey, DWORD dwValue)
{
	pTextureStage[dwStage].mpNewState[(DWORD)dwKey] = dwValue;
}
void ZDirect3D::ResetTextureStageState(DWORD dwStage)
{
	if (pTextureStage[dwStage].pNewTexture != (ZTexture*)-1)
	{
		pTextureStage[dwStage].pNewTexture = NULL;
	}

	pTextureStage[dwStage].mpNewState.clear();
	for (StateBuffer::iterator it = pTextureStage[dwStage].mpState.begin(); it != pTextureStage[dwStage].mpState.end(); it++)
	{
		if (it->second.first != it->second.second)
		{
			SetTextureStageState(dwStage, (D3DTEXTURESTAGESTATETYPE)it->first, it->second.second);
		}
	}
}
void ZDirect3D::BuildSprite(ZArray<ZVertexTL>& pVertex, ZArray<ZFace>& pFace, const ZPoint<int>& p, const ZRect<int>& spr, ZColour cDiffuse, ZColour cSpecular)
{
	pVertex.SetLength(4);

	const float fMult = 1.0f / 256.0f;

	ZPoint<float> p2((spr.left * fMult), (spr.top * fMult));//(spr.left + 0.5f) * fMult, (spr.top + 0.5f) * fMult);

	float fW = (float)(spr.right - spr.left);
	float fH = (float)(spr.bottom - spr.top);
	float fTexW = fW * fMult;
	float fTexH = fH * fMult;
	for (int i = 0; i < 4; i++)
	{
		pVertex[i].m_vPos = ZVector(p.x - 0.5f, p.y - 0.5f, 0.1f);
		pVertex[i].m_aTex[0] = p2;
		if (i == 1 || i == 2)
		{
			pVertex[i].m_vPos.m_fX += fW;
			pVertex[i].m_aTex[0].x += fTexW;//fW * fMult) - (;
		}
		if (i == 2 || i == 3)
		{
			pVertex[i].m_vPos.m_fY += fH;
			pVertex[i].m_aTex[0].y += fTexH;//fH * fMult;
		}
		pVertex[i].m_fRHW = 1.0f;
		pVertex[i].m_cSpecular = cSpecular;
		pVertex[i].m_cDiffuse = cDiffuse;
	}

	pFace.SetLength(2);
	pFace[0] = ZFace(0, 1, 3);
	pFace[1] = ZFace(1, 2, 3);
}
HRESULT ZDirect3D::DrawSprite(const ZPoint<int>& p, const ZRect<int>& spr, ZColour cDiffuse, ZColour cSpecular)
{
	ZArray<ZVertexTL> pv;
	ZArray<ZFace> pf;
	BuildSprite(pv, pf, p, spr, cDiffuse, cSpecular);
	return DrawIndexedPrimitive(pv, pf);
}
void ZDirect3D::ResetTextureState()
{
	for (DWORD dwStage = 0; dwStage < 8; dwStage++)
	{
		ResetTextureStageState(dwStage);
	}
}