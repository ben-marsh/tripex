#include "Platform.h"
#include "Canvas.h"
#include "error.h"
#include "Texture.h"
#include "ZDirect3D.h"

/*---------------------------------
* Constructor:
-----------------------------------*/

Canvas::Canvas( int32 nWidth, int32 nHeight )
{
	int32 nTextures = nWidth * nHeight;
	uint32 nSpan = nWidth * 256;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_anData = new uint8[ nSpan * nHeight * 256 ];
	m_aTexture = new Texture[ nTextures ];
	memset(m_aPalette, 0, sizeof(m_aPalette));

	int32 nTexture = 0;
	for( int32 nY = 0; nY < nHeight; nY++ )
	{
		for( int32 nX = 0; nX < nWidth; nX++ )
		{
			uint8 *pnData = &m_anData[ ( nX * 256 ) + ( nY * 256 * nSpan ) ];

			Texture *pTexture = &m_aTexture[ nTexture++ ];
			pTexture->SetFlags( Texture::F_DYNAMIC );
			pTexture->SetSource( m_aPalette, pnData, 256 * 256, nSpan );
		}
	}
}

/*---------------------------------
* Create( ):
-----------------------------------*/

Error* Canvas::Create( )
{
	for( int32 i = 0; i < m_nWidth * m_nHeight; i++ )
	{
		Error* error = g_pD3D->AddTexture( &m_aTexture[ i ] );
		if( error ) return TraceError( error );
	}
	return nullptr;
}


/*---------------------------------
* GetDataPtr( ):
-----------------------------------*/

uint8 *Canvas::GetDataPtr( )
{
	return m_anData;
}

/*---------------------------------
* UploadTextures( ):
-----------------------------------*/

Error* Canvas::UploadTextures( )
{
	for( int32 i = 0; i < m_nWidth * m_nHeight; i++ )
	{
		Error* error = g_pD3D->UploadTexture( &m_aTexture[ i ] );
		if( error ) return TraceError( error );
	}
	return nullptr;
}

/*---------------------------------
* GetTexture( ):
-----------------------------------*/

Texture *Canvas::GetTexture( int32 nX, int32 nY )
{
	return &m_aTexture[ ( nY * m_nWidth ) + nX ];
}

/*---------------------------------
* Render( ):
-----------------------------------*/

Error* Canvas::Render( )
{
	Error* error;
	ZArray<Face> pFace;
	pFace.Add(Face(0, 1, 3));
	pFace.Add(Face(1, 2, 3));
	//WORD face[6] = { 0, 1, 3, 1, 2, 3 };

	ZArray<ZVertexTL> v(4);
//	ZVertexTL v[4];
	v[0].m_vPos.m_fZ = 1;
	v[0].m_fRHW = 1;
	v[1].m_vPos.m_fZ = 1;
	v[1].m_fRHW = 1;
	v[2].m_vPos.m_fZ = 1;
	v[2].m_fRHW = 1;
	v[3].m_vPos.m_fZ = 1;
	v[3].m_fRHW = 1;

//	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 1;
//	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1;
	v[0].m_cDiffuse = v[1].m_cDiffuse = v[2].m_cDiffuse = v[3].m_cDiffuse = m_cColour;

	int width = m_nWidth * 256;
	int height = m_nHeight * 256;
	int horizontal = m_nWidth;
	int vertical = m_nHeight;

	float s = float(g_pD3D->GetWidth()) / width;
//	double ys = double(d3d->GetHeight()) / height;
//	double s = xs;//min(xs, ys);

	float xc = (g_pD3D->GetWidth() - (horizontal * 254 * s)) / 2;
	float yc = (g_pD3D->GetHeight() - (vertical * 254 * s)) / 2; 

	int i = 0;
	for(int vi = 0; vi < vertical; vi++)
	{
		for(int hi = 0; hi < horizontal; hi++)
		{
			v[0].m_vPos.m_fX = xc + (hi * 254 * s);
			v[0].m_vPos.m_fY = yc + (vi * 254 * s);
			v[0].m_aTex[0].x = (1 / 256.0);
			v[0].m_aTex[0].y = (1 / 256.0);

			v[1].m_vPos.m_fX = xc + ((hi+1) * 254 * s);
			v[1].m_vPos.m_fY = v[0].m_vPos.m_fY;//yc + (vi * 254 * s);
			v[1].m_aTex[0].x = (255 / 256.0);
			v[1].m_aTex[0].y = (1 / 256.0);

			v[2].m_vPos.m_fX = xc + ((hi + 1) * 254 * s);
			v[2].m_vPos.m_fY = yc + ((vi + 1) * 254 * s);
			v[2].m_aTex[0].x = (255 / 256.0);//1 - (0.5 / 256.0);
			v[2].m_aTex[0].y = (255 / 256.0);//1;// - (1 / 256.0);

			v[3].m_vPos.m_fX = xc + (hi * 254 * s);
			v[3].m_vPos.m_fY = v[2].m_vPos.m_fY;//yc + ((vi + 1) * 254 * s);
			v[3].m_aTex[0].x = (1 / 256.0);
			v[3].m_aTex[0].y = (255 / 256.0);//1;// - (1 / 256.0);

			if(v[0].m_vPos.m_fY < 0)
			{
				if(v[2].m_vPos.m_fY < 0) continue;
				float fPos = (0 - v[0].m_vPos.m_fY) / (v[2].m_vPos.m_fY - v[0].m_vPos.m_fY);

				v[0].m_vPos.m_fY = 0;
				v[0].m_aTex[0].x = (v[0].m_aTex[0].x * (1 - fPos)) + (v[3].m_aTex[0].x * fPos);
				v[0].m_aTex[0].y = (v[0].m_aTex[0].y * (1 - fPos)) + (v[3].m_aTex[0].y * fPos);

				v[1].m_vPos.m_fY = 0;
				v[1].m_aTex[0].x = (v[1].m_aTex[0].x * (1 - fPos)) + (v[2].m_aTex[0].x * fPos);
				v[1].m_aTex[0].y = (v[1].m_aTex[0].y * (1 - fPos)) + (v[2].m_aTex[0].y * fPos);
			}
			if(v[3].m_vPos.m_fY > g_pD3D->GetHeight())
			{
				if(v[0].m_vPos.m_fY > g_pD3D->GetHeight()) continue;

				float fPos = (g_pD3D->GetHeight() - v[0].m_vPos.m_fY) / (v[3].m_vPos.m_fY - v[0].m_vPos.m_fY);
				v[2].m_aTex[0].x = (v[1].m_aTex[0].x * (1 - fPos)) + (v[2].m_aTex[0].x * fPos);
				v[2].m_aTex[0].y = (v[1].m_aTex[0].y * (1 - fPos)) + (v[2].m_aTex[0].y * fPos);

				v[3].m_aTex[0].x = (v[0].m_aTex[0].x * (1 - fPos)) + (v[3].m_aTex[0].x * fPos);
				v[3].m_aTex[0].y = (v[0].m_aTex[0].y * (1 - fPos)) + (v[3].m_aTex[0].y * fPos);

				v[2].m_vPos.m_fY = v[3].m_vPos.m_fY = (float)g_pD3D->GetHeight();
			}

			g_pD3D->SetTexture(0, GetTexture(hi,vi));// ppTexture[i++]);

			error = g_pD3D->DrawIndexedPrimitive(v, pFace);//pVertex, pFace);
//				->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, v, 4, face, 6, 0);
			if(error) return TraceError(error);
		}
	}
//	d3d->SetTexture(0, ppTexture[0]);
//	CopySprite(d3d, ZColour::White(), 0, 0, 1, 1, 254, 254);
//	d3d->SetTexture(0, ppTexture[1]);
//	CopySprite(d3d, ZColour::White(), 258, 0, 1, 1, 254, 254);
	return nullptr;
}

//	
//	
//	
//	
//	
//	
//	
//		// Create( ):
//		void Create( int32 nWidth, uint32 nHeight );
//	
//		// GetDataPtr( ):
//		uint8 *GetDataPtr( );
//	
//		// UploadTextures( ):
//		HRESULT UploadTextures( );
//	
//	
//	
//	unsigned long ConvertBitsToMask(unsigned char v, unsigned long mask)
//	{
//		unsigned char vbit = 1 << 7;
//		unsigned long mbit = 1 << 31;
//	
//		unsigned long result = 0;
//		while(mbit != 0)
//		{
//			if(mask & mbit)
//			{
//				if((v & vbit) != 0) result |= mbit;
//				vbit >>= 1;
//			}
//			mbit >>= 1;
//		}
//		return result;
//	}
//	void ZPaletteCanvas::Create(bool fc, unsigned int horizontal, unsigned int vertical)
//	{
//		this->fc = fc;
//		if(fc)
//		{
//			this->horizontal = 1;
//			this->vertical = 1;
//			this->width = horizontal;
//			this->height = vertical;
//			
//			pBuffer.SetLength(256 * 256);
//	//		= (unsigned char*)malloc(256 * 256);
//		}
//		else
//		{
//			this->horizontal = horizontal;
//			this->vertical = vertical;
//			this->width = horizontal * 256;
//			this->height = vertical * 256;
//		}
//		this->usewidth = this->width;
//		this->useheight = this->height;
//		this->cColour = ZColour::White();//fBrightness = 1.0f;// = D3DRGB(1,1,1);
//	}
//	
//	extern int nFiltering;
//	
//	ZPaletteCanvas::~ZPaletteCanvas()
//	{
//	//	if(fc) free(buffer);
//	}
//	ZTexture *ZPaletteCanvas::GetTexture(unsigned int i)
//	{
//		_ASSERT(i < horizontal * vertical);
//		return ppTexture[i];
//	}
//	HRESULT ZPaletteCanvas::Initialise( )
//	{
//		ppTexture.Empty();
//	
//		for(int nY = 0; nY < vertical; nY++)
//		{
//			for(int nX = 0; nX < horizontal; nX++)
//			{
//				int nSpan = horizontal * 256;
//				int nIndex = (nY * nSpan * 256) + (nX * 256);
//	
//				ZTexture *pTexture = new ZTexture( );
//				pTexture->SetFlags( ZTexture::F_DYNAMIC );
//				pTexture->SetSource( m_aPalette, &pSrc[ nIndex ], 256 * 256, nSpan );
//				g_pD3D->AddTexture( pTexture );
//	
//	//	texture = new ZTexture*[horizontal * vertical];
//	//	for(int i = 0; i < (horizontal * vertical); i++)
//	//	{
//	//		ZTexture *pTexture = new ZTexture();
//	//		pTexture->SetFlags( ZTexture::F_DYNAMIC );//| ZTexture::F_FILTERING );
//	//		pTexture->SetSource( D3DFMT_P8, 256 * 256, 
//	
//	
//	
//		pTexture->SetDepths(16, 32, 24);

//		if(nFiltering > 0) pTexture->nFlags.set(ZTexture::TF_BILINEAR);
//		pTexture->nFlags.set(ZTexture::TF_DYNAMIC)256, 256, 0, ZTexture::Dynamic);
//		pTexture->pnDepth.Add(8);
//		pTexture->pnDepth.Add(16);
//		pTexture->pnDepth.Add(32);
//	/*		if(nFiltering != 0)
//			{
//				pTexture->tsDefault.AddState(D3DTSS_MINFILTER, D3DTFN_LINEAR);
//				pTexture->tsDefault.AddState(D3DTSS_MAGFILTER, D3DTFG_LINEAR);
//			}
//			if(nFiltering == 3)
//			{
//				pTexture->tsDefault.AddState(D3DTSS_MIPFILTER, D3DTFP_LINEAR);
//			}
//	*/
//			g_pD3D->AddTexture( pTexture );
//	//		g_pD3D->vpTexture.push_back(pTexture);
//	
//	//		HRESULT hRes = g_pD3D->CreateTexture(pTexture);
//	//		HRESULT hRes = pTexture->CreateSurface(d3d);
//	//		if(FAILED(hRes)) return TraceError(hRes);
//	
//			ppTexture.Add(pTexture);
//			}
//	//		d3d->vpTexture.push_back(pTexture);
//		}
//		return S_OK;
//	}
//	void ZPaletteCanvas::SetPalette(ZColour *palette)
//	{
//		CopyMemory(this->palette, palette, 256 * sizeof(D3DCOLOR));
//		fResetPalette = true;
//	}
//	HRESULT ZPaletteCanvas::CopyToTexture(unsigned char *pSrc)
//	{
//		int nTex = 0;
//		for(int nY = 0; nY < vertical; nY++)
//		{
//			for(int nX = 0; nX < horizontal; nX++)
//			{
//				int nSpan = horizontal * 256;
//				int nIndex = (nY * nSpan * 256) + (nX * 256);
//				
//				HRESULT hRes = g_pD3D->CopyTexture(ppTexture[nTex], &pSrc[nIndex], palette, nSpan);
//				if(FAILED(hRes)) return TraceError(hRes);
//	
//				nTex++;
//			}
//		}
//	
//		return S_OK;
//	/*
//		int vi, hi;
//		DDSURFACEDESC2 desc;
//		unsigned int x, y, j;
//		unsigned short *ptr16;
//		unsigned long *ptr32;
//		unsigned char *ptr8;
//		int t = 0;
//		HRESULT hRes;
//	
//		for(vi = 0; vi < vertical; vi++)
//		{
//			for(hi = 0; hi < horizontal; hi++)
//			{
//				desc.dwSize = sizeof(desc);
//				hRes = ppTexture[t]->GetSurface()->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
//				if(FAILED(hRes)) return TraceError(hRes);
//	
//				if(fResetPalette)
//				{
//					if(desc.ddpfPixelFormat.dwRGBBitCount == 8)
//					{
//						for(int i = 0; i < 256; i++)
//						{
//							ppTexture[t]->ppePalette[i].peRed = palette[i].r;
//							ppTexture[t]->ppePalette[i].peGreen = palette[i].g;
//							ppTexture[t]->ppePalette[i].peBlue = palette[i].b;
//							ppTexture[t]->ppePalette[i].peFlags = 0;
//						}
//	
//						hRes = ppTexture[t]->CopyPalette();
//						if(FAILED(hRes)) return TraceError(hRes);
//					}
//					else
//					{
//	//					int r, g, b;
//						for(int i = 0; i < 256; i++)
//						{
//	//					palette16[i] = ((r >> 3) << 11) | ((g >> 3) << 6) | (b >> 3);
//	//					palette32[i] = RGB_MAKE(r, g, b);
//							unsigned long pi = 0;
//							pi |= ConvertBitsToMask(palette[i].r, desc.ddpfPixelFormat.dwRBitMask);
//							pi |= ConvertBitsToMask(palette[i].g, desc.ddpfPixelFormat.dwGBitMask);
//							pi |= ConvertBitsToMask(palette[i].b, desc.ddpfPixelFormat.dwBBitMask);
//		
//							if(desc.ddpfPixelFormat.dwRGBBitCount == 16) palette16[i] = pi;
//							else palette32[i] = pi;
//						}
//						fResetPalette = false;
//					}
//				}
//	
//				j = ((vi * horizontal * 256) + hi) * 254;
//	
//				switch(desc.ddpfPixelFormat.dwRGBBitCount)
//				{
//				case 8:
//					ptr8 = (unsigned char*)desc.lpSurface;
//	
//					for(y = 0; y < 256; y++)
//					{
//						for(x = 0; x < 256; x++)
//						{
//							ptr8[x] = pSrc[j++];
//						}
//						ptr8 += desc.lPitch;
//						j += (horizontal - 1) * 256;
//					}
//					break;
//				case 16:
//					ptr16 = (unsigned short*)desc.lpSurface;
//	
//					for(y = 0; y < 256; y++)
//					{
//						for(x = 0; x < 256; x++)
//						{
//							ptr16[x] = (WORD)palette16[pSrc[j++]];
//						}
//						ptr16 = (unsigned short*)(((unsigned char*)ptr16) + desc.lPitch);
//						j += (horizontal - 1) * 256;
//					}
//					break;
//				case 32:
//					ptr32 = (unsigned long*)desc.lpSurface;
//	
//					for(y = 0; y < 256; y++)
//					{
//						for(x = 0; x < 256; x++)
//						{
//							ptr32[x] = palette32[pSrc[j++]];
//						}
//						ptr32 = (unsigned long*)(((unsigned char*)ptr32) + desc.lPitch);
//						j += (horizontal - 1) * 256;
//					}
//					break;
//				default:
//					return TraceError(DDERR_INVALIDPIXELFORMAT);
//				}
//	
//				hRes = ppTexture[t]->GetSurface()->Unlock(NULL);
//				if(FAILED(hRes)) return TraceError(hRes);
//	
//				t++;
//			}
//		}
//		return DD_OK;
//	*/
//	}
//	HRESULT ZPaletteCanvas::Calculate(unsigned char *pSrc)
//	{
//		if(fc)
//		{
//			// copy src to buffer, as w,h
//			int rw = usewidth;
//			int sp = 0, t = 0;
//	
//			while(rw > 0)
//			{
//				int s = sp;
//				for(int y = 0; y < height; y++)
//				{
//					int sz = min(rw, 256);
//					for(int x = 0; x < sz; x++)
//					{
//						pBuffer[t++] = pSrc[s++];
//					}
//					s += width - sz;
//					t += 256 - sz;
//				}
//				sp += 256;
//				rw -= 256;
//			}
//	
//			return CopyToTexture(pBuffer.GetBuffer());
//		}
//		else return CopyToTexture(pSrc);
//	}
//	HRESULT ZPaletteCanvas::Render(int y)
//	{
//	 	HRESULT hRes = Render(((int)g_pD3D->GetWidth() - usewidth) / 2, y);
//		if(FAILED(hRes)) return TraceError(hRes);
//	
//		return S_OK;
//	}
//	HRESULT ZPaletteCanvas::Render(int x, int y)
//	{
//		if(ppTexture.GetLength() == 0) return S_OK;
//	
//		HRESULT hRes;
//		_ASSERT(fc);
//	
//		g_pD3D->SetTexture(0, ppTexture[0]);
//	
//		int rw = usewidth, sy = 0; 
//		while(rw > 0)
//		{
//	//		hRes = CopySprite(d3d, cColour, x, y, 0, sy, min(rw, 256), height);
//			hRes = g_pD3D->DrawSprite(ZPoint<int>(x, y), ZRect<int>(0, sy, min(rw, 256), height), cColour);
//			if(FAILED(hRes)) return TraceError(hRes);
//	
//			sy += height;
//			x += 256;
//			rw -= 256;
//		}
//		return S_OK;
//	}
//	HRESULT ZPaletteCanvas::Render( )
//	{
//		if(ppTexture.GetLength() == 0) return S_OK;
//	
//		_ASSERT(!fc);
//		HRESULT hRes;
//		ZArray<ZFace> pFace;
//		pFace.Add(ZFace(0, 1, 3));
//		pFace.Add(ZFace(1, 2, 3));
//		//WORD face[6] = { 0, 1, 3, 1, 2, 3 };
//	
//		ZArray<ZVertexTL> v(4);
//	//	ZVertexTL v[4];
//		v[0].m_vPos.m_fZ = 1;
//		v[0].fRHW = 1;
//		v[1].m_vPos.m_fZ = 1;
//		v[1].fRHW = 1;
//		v[2].m_vPos.m_fZ = 1;
//		v[2].fRHW = 1;
//		v[3].m_vPos.m_fZ = 1;
//		v[3].fRHW = 1;
//	
//	//	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 1;
//	//	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1;
//		v[0].cDiffuse = v[1].cDiffuse = v[2].cDiffuse = v[3].cDiffuse = cColour;
//	
//		double s = double(g_pD3D->GetWidth()) / width;
//	//	double ys = double(d3d->GetHeight()) / height;
//	double s = xs;//min(xs, ys);
//	
//		double xc = (g_pD3D->GetWidth() - (horizontal * 254 * s)) / 2;
//		double yc = (g_pD3D->GetHeight() - (vertical * 254 * s)) / 2; 
//	
//		int i = 0;
//		for(int vi = 0; vi < vertical; vi++)
//		{
//			for(int hi = 0; hi < horizontal; hi++)
//			{
//				v[0].m_vPos.m_fX = xc + (hi * 254 * s);
//				v[0].m_vPos.m_fY = yc + (vi * 254 * s);
//				v[0].m_aTex[0].x = (1 / 256.0);
//				v[0].m_aTex[0].y = (1 / 256.0);
//	
//				v[1].m_vPos.m_fX = xc + ((hi+1) * 254 * s);
//				v[1].m_vPos.m_fY = v[0].m_vPos.m_fY;//yc + (vi * 254 * s);
//				v[1].m_aTex[0].x = (255 / 256.0);
//				v[1].m_aTex[0].y = (1 / 256.0);
//	
//				v[2].m_vPos.m_fX = xc + ((hi + 1) * 254 * s);
//				v[2].m_vPos.m_fY = yc + ((vi + 1) * 254 * s);
//				v[2].m_aTex[0].x = (255 / 256.0);//1 - (0.5 / 256.0);
//				v[2].m_aTex[0].y = (255 / 256.0);//1;// - (1 / 256.0);
//	
//				v[3].m_vPos.m_fX = xc + (hi * 254 * s);
//				v[3].m_vPos.m_fY = v[2].m_vPos.m_fY;//yc + ((vi + 1) * 254 * s);
//				v[3].m_aTex[0].x = (1 / 256.0);
//				v[3].m_aTex[0].y = (255 / 256.0);//1;// - (1 / 256.0);
//	
//				if(v[0].m_vPos.m_fY < 0)
//				{
//					if(v[2].m_vPos.m_fY < 0) continue;
//					float fPos = (0 - v[0].m_vPos.m_fY) / (v[2].m_vPos.m_fY - v[0].m_vPos.m_fY);
//	
//					v[0].m_vPos.m_fY = 0;
//					v[0].m_aTex[0].x = (v[0].m_aTex[0].x * (1 - fPos)) + (v[3].m_aTex[0].x * fPos);
//					v[0].m_aTex[0].y = (v[0].m_aTex[0].y * (1 - fPos)) + (v[3].m_aTex[0].y * fPos);
//	
//					v[1].m_vPos.m_fY = 0;
//					v[1].m_aTex[0].x = (v[1].m_aTex[0].x * (1 - fPos)) + (v[2].m_aTex[0].x * fPos);
//					v[1].m_aTex[0].y = (v[1].m_aTex[0].y * (1 - fPos)) + (v[2].m_aTex[0].y * fPos);
//				}
//				if(v[3].m_vPos.m_fY > g_pD3D->GetHeight())
//				{
//					if(v[0].m_vPos.m_fY > g_pD3D->GetHeight()) continue;
//	
//					float fPos = (g_pD3D->GetHeight() - v[0].m_vPos.m_fY) / (v[3].m_vPos.m_fY - v[0].m_vPos.m_fY);
//					v[2].m_aTex[0].x = (v[1].m_aTex[0].x * (1 - fPos)) + (v[2].m_aTex[0].x * fPos);
//					v[2].m_aTex[0].y = (v[1].m_aTex[0].y * (1 - fPos)) + (v[2].m_aTex[0].y * fPos);
//	
//					v[3].m_aTex[0].x = (v[0].m_aTex[0].x * (1 - fPos)) + (v[3].m_aTex[0].x * fPos);
//					v[3].m_aTex[0].y = (v[0].m_aTex[0].y * (1 - fPos)) + (v[3].m_aTex[0].y * fPos);
//	
//					v[2].m_vPos.m_fY = v[3].m_vPos.m_fY = g_pD3D->GetHeight();
//				}
//	
//				g_pD3D->SetTexture(0, ppTexture[i++]);
//	
//				hRes = g_pD3D->DrawIndexedPrimitive(v, pFace);//pVertex, pFace);
//	//				->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, v, 4, face, 6, 0);
//				if(FAILED(hRes)) return TraceError(hRes);
//			}
//		}
//	//	d3d->SetTexture(0, ppTexture[0]);
//	//	CopySprite(d3d, ZColour::White(), 0, 0, 1, 1, 254, 254);
//	//	d3d->SetTexture(0, ppTexture[1]);
//	//	CopySprite(d3d, ZColour::White(), 258, 0, 1, 1, 254, 254);
//		return S_OK;
//	}
//	void ZPaletteCanvas::Blur(unsigned char *src, unsigned int intensity)
//	{
//		unsigned char *wt = new unsigned char[usewidth];
//	//	int nTot;
//		for(int j = 0; j < height; j++)
//		{
//			for(int k = intensity; k < usewidth - intensity; k++)
//			{
//	//			nTot = 0;
//	//			for(int i = k - intensity; i <= k + intensity; i++)
//	//			{
//	//				nTot += src[i];
//	//			}
//	//			wt[k] = nTot / (1 + (2 * intensity));
//	
//	//			nTot /= (2 * intensity);
//	//			src[k]
//				wt[k] = min(255, (src[k-intensity] + src[k+intensity]) / 1.99);
//				//(src[k-intensity] + src[k] + src[k+intensity]) / 2.9;
//			}
//			for(k = intensity; k < usewidth - intensity; k++)
//			{
//				src[k] = wt[k];
//			}
//			src += width;
//		}
//		delete wt;
//	/*
//		intensity = 1;
//		for(int j = 0; j < height; j++)
//		{
//			for(int k = intensity; k < usewidth - intensity; k++)
//			{
//				src[k] = (src[k-intensity] + src[k] + src[k+intensity]) / 3;
//			}
//			src += width;
//		}
//	*/
//	}
//	int ZPaletteCanvas::GetWidth()
//	{
//		return (int)usewidth;
//	}
//	int ZPaletteCanvas::GetHeight()
//	{
//		return (int)useheight;
//	}
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
