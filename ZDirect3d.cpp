#include "Platform.h"
#include "ZDirect3d.h"
#include <d3dx9.h>
#include "Texture.h"
#include "SpriteBuffer.h"
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
	pTexture = (Texture*)-1;
	pNewTexture = (Texture*)-1;
}

ZDirect3D::ZDirect3D()
{
	g_pDevice = NULL;
}

Error* ZDirect3D::Close()
{
	for (std::set< Texture* >::iterator it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		DestroyTexture(*it);
	}

	g_pDevice = NULL;

	return nullptr;
}
int ZDirect3D::GetWidth()
{
	return nWidth;
}
int ZDirect3D::GetHeight()
{
	return nHeight;
}
Error* ZDirect3D::Open()
{
	_ASSERT(g_pDevice == NULL);

	g_pDevice = ::g_pd3dDevice;

	IDirect3DSurface9* pd3dSurface = NULL;
	HRESULT hRes = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dSurface);
	if (FAILED(hRes)) return TraceError(hRes);

	D3DSURFACE_DESC d3dsd;
	hRes = pd3dSurface->GetDesc(&d3dsd);
	pd3dSurface->Release();
	if (FAILED(hRes)) return TraceError(hRes);

	nWidth = d3dsd.Width;
	nHeight = d3dsd.Height;

	hRes = g_pDevice->GetDeviceCaps(&g_Caps);
	if (FAILED(hRes)) return TraceError(hRes);

	for (std::set< Texture* >::iterator it = g_spTexture.begin(); it != g_spTexture.end(); it++)
	{
		Error* error = CreateTexture(*it);
		if (error) return TraceError(error);
	}
	return nullptr;
}
Texture* ZDirect3D::Find(int nType)
{
	int mt = 0;

	std::set< Texture* >::iterator it;
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

Error* ZDirect3D::DrawIndexedPrimitive(ZArray<VertexTL>& vertices, ZArray<Face>& faces)
{
	Error* error = DrawIndexedPrimitive(vertices.GetLength(), vertices.GetBuffer(), faces.GetLength(), faces.GetBuffer());
	return TraceError(error);
}

Error* ZDirect3D::DrawIndexedPrimitive(const std::vector<VertexTL>& vertices, const std::vector<Face>& faces)
{
	Error* error = DrawIndexedPrimitive((uint32_t)vertices.size(), vertices.data(), (uint32_t)faces.size(), faces.data());
	return TraceError(error);
}

Error* ZDirect3D::DrawIndexedPrimitive(uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces)
{
	Error* error;

	error = FlushRenderState();
	if (error) return TraceError(error);

	error = FlushTextureState();
	if (error) return TraceError(error);

	HRESULT hRes = g_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, num_vertices, num_faces, faces, D3DFMT_INDEX16, vertices, sizeof(VertexTL));
	if (FAILED(hRes)) return TraceError(hRes);

	return nullptr;
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
	mpNewRender.clear();
	for (StateBuffer::iterator it = mpRender.begin(); it != mpRender.end(); it++)
	{
		SetRenderState(it->first, it->second.second);
	}
}
void ZDirect3D::SetRenderState(DWORD dwKey, DWORD dwValue)
{
	mpNewRender[dwKey] = dwValue;
}
void ZDirect3D::GetStateChanges(NewStateBuffer& mp_new, StateBuffer& mp_current, std::vector< StateBufferChange >& vc)
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
				std::pair<DWORD, DWORD> pn = std::pair< DWORD, DWORD >(it_new->second, 0);
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
Error* ZDirect3D::FlushRenderState()
{
	HRESULT hRes;
	std::vector<StateBufferChange> vc;
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

	return nullptr;
}

Error* ZDirect3D::FlushTextureState()
{
	HRESULT hRes;
	for (DWORD dwStage = 0; dwStage < 8; dwStage++)
	{
		if (pTextureStage[dwStage].pTexture != pTextureStage[dwStage].pNewTexture && pTextureStage[dwStage].pNewTexture != (Texture*)-1)
		{
			IDirect3DTexture9* pTex = NULL;
			if (pTextureStage[dwStage].pNewTexture != NULL)
			{
				pTex = pTextureStage[dwStage].pNewTexture->m_pd3dTexture;
			}

			hRes = g_pDevice->SetTexture(dwStage, pTex);
			if (FAILED(hRes)) return TraceError(hRes);

			pTextureStage[dwStage].pTexture = pTextureStage[dwStage].pNewTexture;

			Texture* pTexture = pTextureStage[dwStage].pTexture;
			if (pTexture != NULL)
			{
				if (pTexture->m_nFlags & Texture::F_MIP_CHAIN)
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
					{
						SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
					}
					else if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
					{
						SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
					}
				}
				else
				{
					SetSamplerState(dwStage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
				}

				if (pTexture->m_nFlags & Texture::F_FILTERING)
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
					{
						SetSamplerState(dwStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					}
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
					{
						SetSamplerState(dwStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					}
				}
				else
				{
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)
					{
						SetSamplerState(dwStage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
					}
					if (g_Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
					{
						SetSamplerState(dwStage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
					}
				}
			}
		}

		{
			std::vector< StateBufferChange > vc;
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

		{
			std::vector< StateBufferChange > vc;
			GetStateChanges(pTextureStage[dwStage].mpNewSamplerState, pTextureStage[dwStage].mpSamplerState, vc);

			for (int i = 0; i < (int)vc.size(); i++)
			{
				StateBuffer::iterator it = vc[i].first;

				if (vc[i].second)
				{
					hRes = g_pDevice->GetSamplerState(dwStage, (D3DSAMPLERSTATETYPE)it->first, &it->second.second);
					if (FAILED(hRes)) return TraceError(hRes);
				}

				hRes = g_pDevice->SetSamplerState(dwStage, (D3DSAMPLERSTATETYPE)it->first, it->second.first);
				if (FAILED(hRes)) return TraceError(hRes);
			}

			pTextureStage[dwStage].mpNewSamplerState.clear();
		}
	}
	return nullptr;
}
void ZDirect3D::SetTexture(DWORD dwStage, Texture* pTexture, DWORD dwOp, DWORD dwArg2)
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

Error* ZDirect3D::AddTexture(Texture* pTexture)
{
	std::pair< std::set< Texture* >::iterator, bool > itb = g_spTexture.insert(pTexture);
	if (itb.second && g_pDevice != NULL)
	{
		Error* error = CreateTexture(pTexture);
		if (error) return TraceError(error);
	}
	return nullptr;
}

Error* ZDirect3D::UploadTexture(Texture* pTexture)
{
	if (pTexture->m_pSrcData == NULL) return nullptr;

	const Rect< int > rSrc(0, 0, 256, 256);

	int32 nMipLevels = pTexture->m_pd3dTexture->GetLevelCount();
	for (int32 i = 0; i < nMipLevels; i++)
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
			const uint8* pnSrc = (const uint8*)pTexture->m_pSrcData;
			for (int32 i = 0; i < 256; i++)
			{
				//					( ( uint8* )d3dr.pBits )[ i ] = rand( );
				memcpy(d3dr.pBits, pnSrc, 256);
				d3dr.pBits = (uint8*)(d3dr.pBits) + d3dr.Pitch;
				pnSrc += pTexture->m_nSrcSpan;
			}

			hRes = pSurface->UnlockRect();
			if (FAILED(hRes)) return TraceError(hRes);
		}
		else
		{
			RECT src_rect = { rSrc.left, rSrc.top, rSrc.right, rSrc.bottom };
			hRes = D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pTexture->m_pSrcData, 
				pTexture->m_nSrcFmt, pTexture->m_nSrcSpan, pTexture->m_pSrcPalette, 
				&src_rect, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			if( FAILED( hRes ) ) return TraceError( hRes );
		}
	}
	return nullptr;
}

Error* ZDirect3D::CreateTexture(Texture* pTexture)
{
	_ASSERT(g_pd3dDevice != NULL);
	_ASSERT(pTexture->m_pd3dTexture == NULL);

	if (pTexture->m_nSrcFmt == D3DFMT_UNKNOWN)//pTexture->m_nFlags & ZTexture::F_SRC_FILE )
	{
		HRESULT hRes = D3DXCreateTextureFromFileInMemory(g_pDevice, pTexture->m_pSrcData, pTexture->m_nSrcSize, &pTexture->m_pd3dTexture);
		if (FAILED(hRes)) return TraceError(hRes);
	}
	else
	{
		uint32 nMipLevels = (pTexture->m_nFlags & Texture::F_MIP_CHAIN) ? 0 : 1;
		//			uint32 nUsage = 0;//( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DUSAGE_DYNAMIC : 0;
		//			D3DPOOL nPool = ( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		//			D3DPOOL nPool = ( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DPOOL_SYSMEMORY : D3DPOOL_MANAGED;

		uint32 nUsage = 0;//( pTexture->m_nFlags & ZTexture::F_DYNAMIC )? D3DUSAGE_DYNAMIC : 0;
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
				uint32 an[ 256 * 256 ];
				for( int i = 0; i < 256 * 256; i++ ) an[ i ] = pcSrc[ i ].dw;//.Reverse( ).GetCOLORREF( );
				D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, an, D3DFMT_X8R8G8B8, 256 * 4, NULL, &r, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			}

			pSurface->Release( );
		}
	*/

	return nullptr;
}
void ZDirect3D::DestroyTexture(Texture* pTexture)
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
void ZDirect3D::SetSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE dwKey, DWORD dwValue)
{
	pTextureStage[dwStage].mpNewSamplerState[(DWORD)dwKey] = dwValue;
}
void ZDirect3D::ResetTextureStageState(DWORD dwStage)
{
	if (pTextureStage[dwStage].pNewTexture != (Texture*)-1)
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

	pTextureStage[dwStage].mpNewSamplerState.clear();
	for (StateBuffer::iterator it = pTextureStage[dwStage].mpSamplerState.begin(); it != pTextureStage[dwStage].mpSamplerState.end(); it++)
	{
		if (it->second.first != it->second.second)
		{
			SetSamplerState(dwStage, (D3DSAMPLERSTATETYPE)it->first, it->second.second);
		}
	}
}
Error* ZDirect3D::DrawSprite(const Point<int>& p, const Rect<int>& spr, ColorRgb cDiffuse, ColorRgb cSpecular)
{
	std::vector<VertexTL> pv;
	std::vector<Face> pf;
	SpriteBuffer::AddSprite(p, spr, cDiffuse, cSpecular, pv, pf);
	return DrawIndexedPrimitive(pv, pf);
}
void ZDirect3D::ResetTextureState()
{
	for (DWORD dwStage = 0; dwStage < 8; dwStage++)
	{
		ResetTextureStageState(dwStage);
	}
}
