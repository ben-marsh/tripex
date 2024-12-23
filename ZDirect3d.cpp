#include "Platform.h"
#include "ZDirect3d.h"
#include <d3dx9.h>
#include "Texture.h"
#include "error.h"

extern IDirect3DDevice9* g_pd3dDevice;

ZDirect3D* g_pD3D = NULL;

ZDirect3D::ZDirect3D()
{
	device = NULL;
	width = 0;
	height = 0;
	memset(&caps, 0, sizeof(caps));
}

Error* ZDirect3D::Close()
{
	for (std::set< Texture* >::iterator it = created_textures.begin(); it != created_textures.end(); it++)
	{
		DestroyTexture(*it);
	}

	device = NULL;

	return nullptr;
}

Error* ZDirect3D::BeginFrame()
{
	HRESULT hRes = g_pD3D->device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	if (FAILED(hRes)) return TraceError(hRes);

	hRes = g_pD3D->device->BeginScene();
	if (FAILED(hRes)) return TraceError(hRes);

	hRes = g_pD3D->device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
	if (FAILED(hRes)) return TraceError(hRes);

	return nullptr;
}

Error* ZDirect3D::EndFrame()
{
	HRESULT hRes = g_pD3D->device->EndScene();
	if (FAILED(hRes)) return TraceError(hRes);

	hRes = g_pD3D->device->Present(NULL, NULL, NULL, NULL);
	if (FAILED(hRes)) return TraceError(hRes);

	return nullptr;
}

int ZDirect3D::GetWidth() const
{
	return width;
}
int ZDirect3D::GetHeight() const
{
	return height;
}
Rect<float> ZDirect3D::GetClipRect() const
{
	Rect<float> rect;
	rect.left = std::min(-0.25f, g_pD3D->caps.GuardBandLeft);
	rect.right = std::max(width - 0.25f, g_pD3D->caps.GuardBandRight);
	rect.top = std::min(-0.25f, g_pD3D->caps.GuardBandTop);
	rect.bottom = std::max(height - 0.25f, g_pD3D->caps.GuardBandBottom);
	return rect;
}

Error* ZDirect3D::Open()
{
	_ASSERT(device == NULL);

	device = ::g_pd3dDevice;

	IDirect3DSurface9* pd3dSurface = NULL;
	HRESULT hRes = device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dSurface);
	if (FAILED(hRes)) return TraceError(hRes);

	D3DSURFACE_DESC d3dsd;
	hRes = pd3dSurface->GetDesc(&d3dsd);
	pd3dSurface->Release();
	if (FAILED(hRes)) return TraceError(hRes);

	width = d3dsd.Width;
	height = d3dsd.Height;

	hRes = device->GetDeviceCaps(&caps);
	if (FAILED(hRes)) return TraceError(hRes);

	for (std::set< Texture* >::iterator it = created_textures.begin(); it != created_textures.end(); it++)
	{
		Error* error = CreateTexture(*it);
		if (error) return TraceError(error);
	}
	return nullptr;
}

Error* ZDirect3D::DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces)
{
	device->SetRenderState(D3DRS_CULLMODE, render_state.enable_culling? D3DCULL_CCW : D3DCULL_NONE);
	device->SetRenderState(D3DRS_SHADEMODE, render_state.enable_shading ? D3DSHADE_GOURAUD : D3DSHADE_FLAT);
	device->SetRenderState(D3DRS_SPECULARENABLE, render_state.enable_specular ? TRUE : FALSE);
	device->SetRenderState(D3DRS_ZENABLE, render_state.enable_zbuffer? TRUE : FALSE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, render_state.enable_zbuffer_write? TRUE : FALSE);

	if (render_state.dst_blend == D3DBLEND_ZERO && render_state.src_blend == D3DBLEND_ONE)
	{
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		device->SetRenderState(D3DRS_SRCBLEND, render_state.src_blend);
		device->SetRenderState(D3DRS_DESTBLEND, render_state.dst_blend);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}

	for (int stage = 0; stage < RenderState::NUM_TEXTURE_STAGES; stage++)
	{
		Texture* texture = render_state.texture_stages[stage].texture;
		if (texture == nullptr)
		{
			device->SetTexture(stage, nullptr);
		}
		else
		{
			device->SetTexture(stage, texture->d3d_texture);

			if ((texture->flags & Texture::F_MIP_CHAIN) == 0)
			{
				if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
				{
					device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
				}
				else if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
				{
					device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
				}
			}
			else
			{
				device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			}

			if (texture->flags & Texture::F_FILTERING)
			{
				if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
				{
					device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
				}
				if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
				{
					device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				}
			}
			else
			{
				if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)
				{
					device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
				}
				if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
				{
					device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				}
			}
		}

		device->SetSamplerState(stage, D3DSAMP_ADDRESSU, render_state.texture_stages[stage].address_u);
		device->SetSamplerState(stage, D3DSAMP_ADDRESSV, render_state.texture_stages[stage].address_v);
	}

	HRESULT hRes = device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, num_vertices, num_faces, faces, D3DFMT_INDEX16, vertices, sizeof(VertexTL));
	if (FAILED(hRes)) return TraceError(hRes);

	return nullptr;
}
/*
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
*/
Error* ZDirect3D::AddTexture(Texture* pTexture)
{
	std::pair< std::set< Texture* >::iterator, bool > itb = created_textures.insert(pTexture);
	if (itb.second && device != NULL)
	{
		Error* error = CreateTexture(pTexture);
		if (error) return TraceError(error);
	}
	return nullptr;
}

Error* ZDirect3D::UploadTexture(Texture* pTexture)
{
	if (pTexture->data == NULL)
	{
		return nullptr;
	}

	const Rect< int > rSrc(0, 0, 256, 256);

	int32 nMipLevels = pTexture->d3d_texture->GetLevelCount();
	for (int32 i = 0; i < nMipLevels; i++)
	{
		IDirect3DSurface9* pSurface;

		HRESULT hRes = pTexture->d3d_texture->GetSurfaceLevel(i, &pSurface);
		if (FAILED(hRes)) return TraceError(hRes);

		D3DSURFACE_DESC d3dsd;
		hRes = pSurface->GetDesc(&d3dsd);
		if (FAILED(hRes)) return TraceError(hRes);

		D3DLOCKED_RECT d3dr;
		if (d3dsd.Width == 256 &&
			d3dsd.Height == 256 &&
			d3dsd.Format == pTexture->format &&
			SUCCEEDED(pSurface->LockRect(&d3dr, NULL, 0)))
		{
			const uint8* pnSrc = (const uint8*)pTexture->data;
			for (int32 i = 0; i < 256; i++)
			{
				//					( ( uint8* )d3dr.pBits )[ i ] = rand( );
				memcpy(d3dr.pBits, pnSrc, 256);
				d3dr.pBits = (uint8*)(d3dr.pBits) + d3dr.Pitch;
				pnSrc += pTexture->data_stride;
			}

			hRes = pSurface->UnlockRect();
			if (FAILED(hRes)) return TraceError(hRes);
		}
		else
		{
			RECT src_rect = { rSrc.left, rSrc.top, rSrc.right, rSrc.bottom };
			hRes = D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pTexture->data, 
				pTexture->format, pTexture->data_stride, pTexture->palette, 
				&src_rect, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0 );
			if( FAILED( hRes ) ) return TraceError( hRes );
		}
	}
	return nullptr;
}

Error* ZDirect3D::CreateTexture(Texture* pTexture)
{
	_ASSERT(g_pd3dDevice != NULL);
	_ASSERT(pTexture->d3d_texture == NULL);

	if (pTexture->format == D3DFMT_UNKNOWN)//pTexture->m_nFlags & ZTexture::F_SRC_FILE )
	{
		HRESULT hRes = D3DXCreateTextureFromFileInMemory(device, pTexture->data, pTexture->data_size, &pTexture->d3d_texture);
		if (FAILED(hRes)) return TraceError(hRes);
	}
	else
	{
		uint32 nMipLevels = (pTexture->flags & Texture::F_MIP_CHAIN) ? 0 : 1;
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

		HRESULT hRes = D3DXCreateTexture(device, 256, 256, nMipLevels, nUsage, pTexture->format /*D3DFMT_R5G6B5*/, nPool, &pTexture->d3d_texture);
		if (FAILED(hRes)) return TraceError(hRes);

		PALETTEENTRY pe[256];
		for (int i = 0; i < 256; i++)
		{
			pe[i].peRed = pe[i].peGreen = pe[i].peBlue = i;
			pe[i].peFlags = 0xff;
		}

		device->SetPaletteEntries( 0, pe );
		device->SetCurrentTexturePalette( 0 );

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

	if (pTexture->d3d_texture != NULL)
	{
		pTexture->d3d_texture->Release();
		pTexture->d3d_texture = NULL;
	}
}

Error* ZDirect3D::DrawSprite(const RenderState& render_state, const Point<int>& p, const Rect<int>& spr, ColorRgb diffuse, ColorRgb specular)
{
	GeometryBuffer geometry_buffer;
	geometry_buffer.AddSprite(p, spr, diffuse, specular);
	return DrawIndexedPrimitive(render_state, geometry_buffer);
}
