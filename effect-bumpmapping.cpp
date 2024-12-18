#include "StdAfx.h"
#include "ZGrid.h"
#include "effect.h"
#include "error.h"
#include "ZPaletteCanvas.h"
#include "ZTexture.h"
#include "ZObject.h"

//#define USE_ASM_BUMPMAPPING

#define MIN_FRAME_TIME 0.3f //1.0//0.4//25
#define GRIDW 30
#define GRIDH 40

#define ARROWHEADS 25
#define WIDTH (10.0 * g_fDegToRad)
#define DENT (3.0 * g_fDegToRad)
#define HEIGHT 6.0
#define RADIUS 100.0
#define COLOUR D3DRGB(0.2,0.2,0.2)

#define VPDISTANCE 0.15f//2
#define XM 0.55f //0.2
#define YM 0.55f //0.2//5 //0.2

extern bool bMeshHQ;
extern std::vector< std::unique_ptr< ZTexture > > vpTexture;
extern ZTexture *pBlankTexture;
extern bool fBigBeat;

void MakeTentacles(ZObject &obj, int segs, float l, float r)
{
#define TARMS 6
#define TTEMP 4
	ZVector dv[TARMS] = { ZVector(1, 0, 0), ZVector(-1, 0, 0), ZVector(0, 1, 0), ZVector(0,-1, 0), ZVector(0, 0, 1), ZVector(0, 0, -1) };
	ZVector mu[TARMS] = { ZVector(0, 1, 0), ZVector(0, 1, 0), ZVector(1, 0, 0), ZVector(1, 0, 0), ZVector(1, 0, 0), ZVector(1, 0, 0) };
	ZVector mv[TARMS] = { ZVector(0, 0, 1), ZVector(0, 0, 1), ZVector(0, 0, 1), ZVector(0, 0, 1), ZVector(0, 1, 0), ZVector(0, 1, 0) };

	float u[TTEMP] = { r,-r,-r, r }, v[TTEMP] = { r, r,-r,-r };

	obj.pVertex.SetLength(TARMS * (((segs - 1) * TTEMP) + 1));

	int vn = 0, fn = 0;
	for(int i = 0; i < 6; i++) // each arm
	{
		ZVector center, direction;
		for(int j = 0; j < segs; j++) // dont put one at the base
		{
			float f = j * l / segs;

			center = dv[i] * f;

			if(j < segs - 1)
			{
				for(int k = 0; k < TTEMP; k++)
				{
					if(j == segs - 2)
					{
						obj.pFace.Add(ZFace(vn + k, vn + ((k + 1) % TTEMP), vn + TTEMP));
					}
					else
					{
						obj.pFace.Add(ZFace(vn + k, vn + k + TTEMP, vn + ((k + 1) % TTEMP)));
						obj.pFace.Add(ZFace(vn + ((k + 1) % TTEMP), vn + k + TTEMP, vn + TTEMP + ((k + 1) % TTEMP)));
					}
				}
				for(int k = 0; k < TTEMP; k++)
				{
					ZVector vDir = ZVector((mu[i].m_fX * u[k]) + (mv[i].m_fX * v[k]), (mu[i].m_fY * u[k]) + (mv[i].m_fY * v[k]), (mu[i].m_fZ * u[k]) + (mv[i].m_fZ * v[k]));
					obj.pVertex[vn].m_vPos = center + (vDir * ((segs - 1.0f - j) / (segs - 1.0f)));
					vn++;
				}
			}
			else 
			{
				obj.pVertex[vn].m_vPos = center;
				vn++;
			}
		}
	}
	obj.FindFaceOrder(ZVector::Origin());
	obj.FindVertexNormals();
}

class ZEffectBumpmapping : public ZEffectBase
{
public:
	class BumpmapData
	{
	public:
		ZTexture *pTexture;
		ZArray<unsigned char> pBumpmap;
	};

	ZTexture *light, *texture;

	ZPaletteCanvas pc;

	ZGrid grid;
	ZGrid gridbm;
	float tx, ty;
	unsigned char pnLightMap[256 * 256];//envmap
	unsigned char pnBuf[256 * 256];
//	ZArray<unsigned char> envmap;
	unsigned short *pnCurrentBump;
//	ZArray<unsigned char> *pCurrentBumpmap;
//	ZArray<short int> pBumpX;
//	ZArray<short int> pBumpY;
//	ZArray<unsigned short int> pBumpIndex;
//	ZPtrArray<BumpmapData*> pBump;
//	ZArray<unsigned char> pBuf;

	std::map< ZTexture*, std::auto_ptr< unsigned short > > mpBumpIndex;

	ZObject obj;
	ZCamera camera;

	float precalc_u[GRIDW+1][GRIDH+1];
	float precalc_v[GRIDW+1][GRIDH+1];
	float precalc_c[GRIDW+1][GRIDH+1];

	float br;
	bool fInBrighten;
	float brangle;
	float brcolour;
	bool fStarted;
	float ya;
	float angle;

	float fTentacleAng;
	float fTentacleDir;
	float fMoveSpeed;
	float accum;

	ZEffectBumpmapping() : grid(GRIDW, GRIDH), gridbm(GRIDW, GRIDH), pc( 1, 1 )
	{
		fTentacleAng = 0.0f;
		fTentacleDir = 1.0f;

		accum = 10;

		fStarted = false;

		ya = 0;
		tx = 0;
		ty = 0;

		if(bMeshHQ) MakeTentacles(obj, /*25*/ 160, 200, 8);
		else MakeTentacles(obj, /*25*/ 80, 200, 8);
		obj.FindFaceOrder(ZVector::Origin());
		obj.m_bsFlag.set( ZObject::F_DO_POSITION_DELAY );
		obj.m_bsFlag.set( ZObject::F_DRAW_Z_BUFFER );
		obj.fFrameHistory = 12.0f;
		obj.fDelayHistory = 12.0f;
		obj.fFrameTime = MIN_FRAME_TIME;//0.2;
		obj.FindDelayValues();
//	obj->fDelay(96, 8);

		camera.m_vPosition.m_fZ = -240;

//	scene->vpObject.Add(obj);
//	scene->camera.z = -240;

		angle = rand() * 2000.0f / RAND_MAX;


//		envmap.SetLength(256 * 256);
//		pBumpX.SetLength(256 * 256);
//		pBumpY.SetLength(256 * 256);
//		pBumpIndex.SetLength(256 * 256);
//		pBuf.SetLength(256 * 256);

//		pc.Create(1, 1);

		int x, y, i;

		ZColour colour[256];
		for(i = 0; i < 256; i++)
		{
			pc.m_aPalette[ i ].peRed = i;
			pc.m_aPalette[ i ].peGreen = i;
			pc.m_aPalette[ i ].peBlue = i;
			pc.m_aPalette[ i ].peFlags = 0xff;
		}
//			colour[i] = ZColour(i, i, i);
//		pc.SetPalette(colour);

		i = 0;
		for(y = 0; y < 256; y++)
		{
			for(x = 0; x < 256; x++)
			{
				float nx = (x - 128.0f) / 128.0f;
				float ny = (y - 128.0f) / 128.0f;
				float nz = 1.0f - sqrtf(nx*nx + ny*ny);
				int br = (int)( (nz + (nz * nz * nz * nz)) * 256.0f );
				pnLightMap[i++] = min(max(br, 0), 255);
			}
		}

		for(y = 0; y < GRIDH+1; y++)
		{
			for(x = 0; x < GRIDW+1; x++)
			{
				float xc = float(x - (GRIDW/2.0f)) / GRIDW;
				float yc = float(y - (GRIDH/2.0f)) / GRIDH;

				// xc, yc = -0.5 ... 0.5

				ZVector d = ZVector(xc, yc, VPDISTANCE).Normal( );//Normalize(1.0f);
//			d = Normalize(d);

				float xp = atan2f(xc, VPDISTANCE);
				float yp = asinf(yc / sqrt((xc*xc)+(yc*yc)+(VPDISTANCE*VPDISTANCE)));

				// xp = -pi/2...pi/2
				// yp = -pi/2...pi/2

				float xpp = xp / PI;
				float ypp = yp / PI;
				float dp = /*1.3 - */1.0f - sqrtf(xpp * xpp + ypp * ypp);//(xp / 3.14159)*(xp / 3.14159) + (yp / (2 * 3.14159))*(yp / (2 * 3.14159)));

				precalc_u[x][y] = (XM * xp);
				precalc_v[x][y] = (YM * yp);
				precalc_c[x][y] = min(max(dp, 0.0f), 1.0f);
			}
		}

//	grid = new Grid(GRIDW, GRIDH);//, doFDS);
//	gridbm = new Grid(GRIDW, GRIDH);//, doFDSbm);

		std::vector< ZTexture* > vpBumpmap;
//		for(unsigned int i = 0; i < vpTexture.size(); i++)
		{
//			if(vpTexture[i]->m_snType.count(TC_WTBUMPMAPBACK) > 0 && vpTexture[i]->m_vnData.size() == (256 * 256 * sizeof(ZColour)) && vpTexture[i].get() != pBlankTexture)
//			{
//				vpBumpmap.push_back(vpTexture[i].get());
//			}
		}

/*		for(i = 0; i < vpBumpmap.size(); i++)
		{
			auto_ptr< unsigned short > pb = auto_ptr< unsigned short >(new unsigned short[256*256]);

			ZColour *pcSrc = (ZColour*)vpBumpmap[i]->m_vnData.begin();
			unsigned char pbBump[256 * 256];
			unsigned char *pbDst = pbBump;
			for(int nY = 0; nY < 256; nY++)
			{
				for(int nX = 0; nX < 256; nX++)
				{
					*(pbDst++) = (unsigned int(pcSrc->m_nR) + unsigned int(pcSrc->m_nG) + unsigned int(pcSrc->m_nB)) / 3;
					pcSrc++;
				}
			}

			int nIndex = 0;
			for(nY = 0; nY < 256; nY++)
			{
				for(int nX = 0; nX < 256; nX++)
				{
					//pBumpX[nIndex] = 
					int nBumpX = (int)pbBump[(nIndex + 1) & 0xffff] - (int)pbBump[(nIndex - 1) & 0xffff] + 128 - nX;
					int nBumpY = (int)pbBump[(nIndex + 256) & 0xffff] - (int)pbBump[(nIndex - 256) & 0xffff] + 128 - nY;
					pb.get()[nIndex] = (((unsigned char)nBumpY) << 8) | ((unsigned char)nBumpX);
					nIndex++;
				}
			}

			mpBumpIndex[vpBumpmap[i]] = pb;
		}
*/	}
	HRESULT Calculate( float brightness, float elapsed, ZAudio* pAudio )
	{
		HRESULT hRes;
		br = brightness;

		tx += /*4*/1 * elapsed;//average * 20.0;
		ty += elapsed * sinf(ya) * cosf(ya * 1.2f) * sinf(ya * 1.5f) * 0.4f;

		ya += elapsed * g_fDegToRad;

		if(texture != pBlankTexture)
		{
			if(!fStarted)
			{
				hRes = pc.Create( );//.Initialise( );
				if(FAILED(hRes)) return TraceError(hRes);
				fStarted = true;
			}

			int x, y, i = 0;
			for(x = 0; x <= grid.nWidth; x++)
			{
				for(y = 0; y <= grid.nHeight; y++)
				{
					grid.pVertex[i].m_aTex[0].x = gridbm.pVertex[i].m_aTex[0].x = precalc_u[x][y] + (tx / 256.0f);
					grid.pVertex[i].m_aTex[0].y = gridbm.pVertex[i].m_aTex[0].y = precalc_v[x][y] + (ty / 256.0f);

					float brav = precalc_c[x][y] * br * pAudio->GetIntensity( );//min(average, 1);
					brav *= 1.6f;
					gridbm.pVertex[i].m_cDiffuse = ZColour::Grey((int)(255.0f * br));//max(0.2, min(brav, 1)) * 255.0);//gridbm->vertex[i].color = D3DRGB(brav, brav, brav);
					grid.pVertex[i].m_cDiffuse = ZColour::Grey((int)(br * precalc_c[(int)(x + tx) % GRIDW][y % GRIDH] * 255.0f));
					i++;
				}
			}

#ifdef USE_ASM_BUMPMAPPING
			unsigned short nOffset = ((unsigned short)((unsigned char)ty) << 8) | (unsigned short)((unsigned char)tx);
			unsigned char *pEnv = pnLightMap;//envmap.GetBuffer();
			unsigned short *pBmp = pnCurrentBump;//pBumpIndex.GetBuffer();
			unsigned char *pInd = pnBuf;//.GetBuffer(256 * 256);
			__asm
			{
				push esi
				push edi
				//push ebp

				mov dx, nOffset
				mov esi, pEnv
				mov edi, pInd
				mov eax, pBmp // do last because other vars are on stack (relative to ebp)

				xor ebx, ebx
				mov ecx, 256*256
			}
	rendloop:
			__asm
			{
				// bx = pBumpIndex[i] + txPos = [ebp] + dx
				mov bx, [eax]
				add bx, dx
				// pind[i] = envmap[(e)bx]
				mov bl, [esi + ebx]
				mov [edi], bl	

				add eax, 2
				inc edi
				dec ecx
				jnz rendloop

//				pop ebp
				pop edi
				pop esi
			}
#else
			UINT8* data = pc.GetDataPtr();
			if (pnCurrentBump == nullptr)
			{
				memset(data, 0, 256 * 256);
			}
			else
			{
				unsigned char lx = (unsigned char)tx;
				for(i = 0; i < 256*256; i++)
				{
					data[i] = pnLightMap[(unsigned short)(pnCurrentBump[i] + lx)];
				}
			}
#endif

			hRes = pc.UploadTextures( );
			if( FAILED( hRes ) ) return TraceError( hRes );
//			hRes = pc.Calculate(pnBuf);
//			if(FAILED(hRes)) return TraceError(hRes);
		}

//	hRes = light->GetSurface()->Unlock(NULL);
//	if(FAILED(hRes)) return hRes;

		/** TENTACLES **********/
		obj.wcAmbientLight = ZColour::Grey((int)(brightness * 205.0f));
		if( pAudio->IsBeat( ) && pAudio->GetBeat( ) > 0.9f) fTentacleDir = -fTentacleDir;
		for(float fPos = 0;;)
		{
			float fNext = fPos + MIN_FRAME_TIME;
			bool bLast = fNext > elapsed;

			float fThis = min(MIN_FRAME_TIME, elapsed - fPos);
			obj.fRoll += fThis * fTentacleDir * g_fDegToRad * 5.0f * (pAudio->GetIntensity( ) + 0.1f);
			obj.fPitch += fThis * fTentacleDir * g_fDegToRad * 20.0f * pAudio->GetIntensity( );
			obj.fYaw += fThis * fTentacleDir * g_fDegToRad * 10.0f * (pAudio->GetIntensity( ) + 0.1f);
			angle += pAudio->GetIntensity( ) * fMoveSpeed * fThis * g_fDegToRad;
			obj.vPosition.m_fX = 200.0f * cosf(angle) * sinf(angle * 1.3f) * cosf(angle * 2.3f) * sinf(angle * 0.6f);
			obj.vPosition.m_fY = 100.0f * cosf(angle * 0.2f) * sinf(angle * 1.1f) * cosf(angle * 1.6f) * sinf(angle * 1.2f);
			obj.vPosition.m_fZ = 150.0f * cosf(angle * 1.6f) * sinf(angle * 0.5f) * cosf(angle * 1.1f) * sinf(angle * 1.2f);
			if(texture == pBlankTexture) obj.vPosition.m_fZ = (obj.vPosition.m_fZ / 2) - 50;
			obj.nExposure = bLast? 1 : 0;
			obj.Calculate(&camera, fThis);

			if(bLast) break;
			fPos = fNext;
		}
		return D3D_OK;
	}
	HRESULT Render( )
	{
		HRESULT hRes;
		static double angle = 0;

		if(texture == pBlankTexture)
		{
		}
		else
		{
			//			hRes = d3d->SetState(g_pD3D->Shade);//D3DRS_SHADE);
//			if(FAILED(hRes)) return TraceError(hRes);

			g_pD3D->SetTexture(0, texture);
			g_pD3D->SetState(g_pD3D->Shade);
			hRes = grid.Render( );
			if(FAILED(hRes)) return TraceError(hRes);

			g_pD3D->SetTexture(0, pc.GetTexture(0, 0));
			g_pD3D->SetState(g_pD3D->Transparent | g_pD3D->Shade);// | D3DRS_SHADE);
			hRes = gridbm.Render( );
			if(FAILED(hRes)) return TraceError(hRes);
		}
//	d3d->lpd3dDevice->SetTexture(0, texture->GetSurface());
//	d3d->lpd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//	d3d->lpd3dDevice->SetTexture(1, pc->GetTexture(0)->GetSurface());
//	d3d->lpd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
//	hRes = grid->Render(d3d);
//	if(FAILED(hRes)) return hRes;
//
//	d3d->lpd3dDevice->SetTexture(0, pc->GetSurface());
//	d3d->lpd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
//	hRes = grid->Render(d3d);
//	if(FAILED(hRes)) return hRes;

/*	grid->SetTexture(texture);
	hRes = grid->Render(d3d);
	if(FAILED(hRes)) return hRes;

	d3d->lpd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	d3d->lpd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	d3d->lpd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
	d3d->lpd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE); //D3DTBLEND_DECAL);
	d3d->lpd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
	gridbm->SetTexture(pc->GetTexture(0));//light);
	hRes = gridbm->Render(d3d);
	if(FAILED(hRes)) return hRes;
*/	
		hRes = obj.Render( );//scene->render(d3d);
		if(FAILED(hRes)) return TraceError(hRes);

		return hRes;
	}
	virtual HRESULT Reconfigure(ZAudio* pAudio) override
	{
		obj.pTexture[0].m_nType = ZObject::Texture::T_ENVMAP;
		obj.pTexture[0].m_pTexture = g_pD3D->Find(TC_EMBUMPMAPTENTACLES);//ENVIRONMENTMAP));
		fMoveSpeed = 1.0f + (rand() * 4.0f / RAND_MAX);

//		if(pBump.GetLength() == 0) texture = pBlankTexture;
//		else
//		{
			texture = g_pD3D->Find(TC_WTBUMPMAPBACK);
			if(texture != pBlankTexture)
			{	
				std::map< ZTexture*, std::auto_ptr< unsigned short > >::iterator it = mpBumpIndex.find(texture);
				if(it != mpBumpIndex.end())
				{
					pnCurrentBump = it->second.get();
				}
				else
				{
					IDirect3DSurface9* pSurface;

					HRESULT hRes = texture->m_pd3dTexture->GetSurfaceLevel(0, &pSurface);
					if (FAILED(hRes)) return TraceError(hRes);

					RECT Rect = { 0, 0, 256, 256 };

					D3DSURFACE_DESC d3dsd;
					hRes = pSurface->GetDesc(&d3dsd);
					if (FAILED(hRes)) return TraceError(hRes);

					D3DLOCKED_RECT d3dr;
					if (d3dsd.Width == 256 &&
						d3dsd.Height == 256 &&
						d3dsd.Format == D3DFMT_X8R8G8B8 &&
						SUCCEEDED(pSurface->LockRect(&d3dr, NULL, 0)))
					{
						std::auto_ptr< unsigned short > pb = std::auto_ptr< unsigned short >(new unsigned short[256 * 256]);

						unsigned char* pcSrc = (unsigned char*)d3dr.pBits;
						unsigned char pbBump[256 * 256];
						unsigned char* pbDst = pbBump;
						for (int nY = 0; nY < 256; nY++)
						{
							for (int nX = 0; nX < 256; nX++)
							{
								*(pbDst++) = ((unsigned)pcSrc[0] + (unsigned)pcSrc[1] + (unsigned)pcSrc[2]) / 3;// unsigned int(pcSrc->m_nR) + unsigned int(pcSrc->m_nG) + unsigned int(pcSrc->m_nB)) / 3;
								pcSrc += 4;
							}
						}

						int nIndex = 0;
						for (int nY = 0; nY < 256; nY++)
						{
							for (int nX = 0; nX < 256; nX++)
							{
								int nBumpX = (int)pbBump[(nIndex + 1) & 0xffff] - (int)pbBump[(nIndex - 1) & 0xffff] + 128 - nX;
								int nBumpY = (int)pbBump[(nIndex + 256) & 0xffff] - (int)pbBump[(nIndex - 256) & 0xffff] + 128 - nY;
								pb.get()[nIndex] = (((unsigned char)nBumpY) << 8) | ((unsigned char)nBumpX);
								nIndex++;
							}
						}
						pnCurrentBump = pb.get(); 
						mpBumpIndex[texture] = std::move(pb);

						pSurface->UnlockRect();
					}
				}

/*				for(int i = 0;;i++)
				{
					if(pBump[i]->pTexture == NULL) return TraceError(E_BUMPMAPNOTFOUND);
					else if(pBump[i]->pTexture == texture)
					{
						pCurrentBumpmap = &pBump[i]->pBumpmap;
						break;
					}
				}	

				unsigned char cX, cY;
				int nIndex = 0;//, sx, sy;
				for(int y = 0; y < 256; y++)
				{
					for(int x = 0; x < 256; x++)
					{
						//pBumpX[nIndex] = 
						cX = int(pCurrentBumpmap->operator[]((unsigned short)(nIndex + 1)) - pCurrentBumpmap->operator[]((unsigned short)(nIndex - 1)) + 128 - x) & 255;
						//pBumpY[nIndex] = 
						cY = int(pCurrentBumpmap->operator[]((unsigned short)(nIndex + 256)) - pCurrentBumpmap->operator[]((unsigned short)(nIndex - 256)) + 128 - y) & 255;
						pBumpIndex[nIndex] = (cY << 8) | cX;
						nIndex++;
					}
				}
*///			}
		}
		return D3D_OK;
	}
};

EXPORT_EFFECT( Bumpmapping, ZEffectBumpmapping )
