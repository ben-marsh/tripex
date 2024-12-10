#include "StdAfx.h"
#include "ZObject.h"
#include "effect.h"

#define PTDIST 15
#define PTCIRCUM 40
#define PTLENGTH 20

#define HEIGHTST 30
#define HEIGHTM 0.08f

#define CIRADIUS 40.0f
#define CORADIUS 50.0f
#define CANGDIFF (5.0f * g_fDegToRad)
#define CLENGTH 100
#define CDIFF (PTLENGTH * PTDIST / CLENGTH)

class ZEffectTube : public ZEffectBase
{
public:
	float height[PTLENGTH][PTCIRCUM];
	bool fRefresh;
	float accwf[PTCIRCUM];

	ZObject obj, coil;
	ZCamera cCamera;

	ZEffectTube()
	{
		int i, j;
		coil.pVertex.SetLength(CLENGTH * 4);
		coil.pFace.SetLength((CLENGTH - 1) * 8);
		coil.m_bsFlag.set(ZObject::F_DRAW_TRANSPARENT);
		coil.m_bsFlag.set(ZObject::F_DRAW_Z_BUFFER);
		coil.m_bsFlag.set(ZObject::F_NO_CULL);

		int v = 0, f = 0;
		for(i = 0; i < CLENGTH; i++)
		{
			float angle = i * 4.0f * PI2 / CLENGTH;

			coil.pVertex[v].m_vPos.m_fX = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.pVertex[v].m_vPos.m_fY = CORADIUS * sinf(angle - CANGDIFF);
			coil.pVertex[v].m_vPos.m_fZ = CORADIUS * cosf(angle - CANGDIFF);
			v++;

			coil.pVertex[v].m_vPos.m_fX = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.pVertex[v].m_vPos.m_fY = CORADIUS * sinf(angle + CANGDIFF);
			coil.pVertex[v].m_vPos.m_fZ = CORADIUS * cosf(angle + CANGDIFF);
			v++;

			coil.pVertex[v].m_vPos.m_fX = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.pVertex[v].m_vPos.m_fY = CIRADIUS * sinf(angle + CANGDIFF);
			coil.pVertex[v].m_vPos.m_fZ = CIRADIUS * cosf(angle + CANGDIFF);
			v++;

			coil.pVertex[v].m_vPos.m_fX = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.pVertex[v].m_vPos.m_fY = CIRADIUS * sinf(angle - CANGDIFF);
			coil.pVertex[v].m_vPos.m_fZ = CIRADIUS * cosf(angle - CANGDIFF);
			v++;
		}
		for(i = 0; i < CLENGTH - 1; i++)
		{
			for(j = 0; j < 4; j++)
			{
				coil.pFace[f][0] = (i * 4) + j;
				coil.pFace[f][2] = ((i + 1) * 4) + j;
				coil.pFace[f][1] = (i * 4) + ((j + 1) % 4);
				f++;

				coil.pFace[f][0] = ((i + 1) * 4) + j;
				coil.pFace[f][2] = ((i + 1) * 4) + ((j + 1) % 4);
				coil.pFace[f][1] = (i * 4) + ((j + 1) % 4);
				f++;
			}
		}
		_ASSERT(f == coil.pFace.GetLength());
//	coil.FindVertexNormals();

		obj.pVertex.SetLength(PTCIRCUM * PTLENGTH);
		obj.pFace.SetLength((PTCIRCUM * 2) * (PTLENGTH - 1));
		obj.m_bsFlag.set( ZObject::F_DRAW_Z_BUFFER );
		obj.m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );

		f = 0;
		for(i = 0; i < PTLENGTH - 1; i++)
		{
			for(j = 0; j < PTCIRCUM; j++)
			{
/*
				obj->face[f].v[0] = (i * PTCIRCUM) + j;
				obj->face[f].v[1] = ((i + 1) * PTCIRCUM) + j;//(i + 1) * PTCIRCUM) + j;
				obj->face[f].v[2] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;

				obj->face[f].v[0] = ((i + 1) * PTCIRCUM) + j;
				obj->face[f].v[1] = ((i + 1) * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				obj->face[f].v[2] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;
*/
				obj.pFace[f][0] = (i * PTCIRCUM) + j;
				obj.pFace[f][2] = ((i + 1) * PTCIRCUM) + j;//(i + 1) * PTCIRCUM) + j;
				obj.pFace[f][1] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;

				obj.pFace[f][0] = ((i + 1) * PTCIRCUM) + j;
				obj.pFace[f][2] = ((i + 1) * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				obj.pFace[f][1] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;
			}
		}
		for(i = 0; i < PTLENGTH; i++)
		{
			float br = min(1.0f, sinf(i * PI / PTLENGTH) * 1);

			for(j = 0; j < PTCIRCUM; j++)
			{
				obj.pVertex[(i * PTCIRCUM) + j].m_cDiffuse = ZColour::Grey((int)(br * 255.0f));
			}
		}
		obj.m_bsFlag.set(ZObject::F_VALID_VERTEX_DIFFUSE);
		coil.m_bsFlag.set(ZObject::F_VALID_VERTEX_DIFFUSE);
		_ASSERT(f == obj.pFace.GetLength());

//	scene = new World();
//	scene->vpObject.Add(obj);
//	scene->vpObject.Add(coil);
		cCamera.m_vPosition.m_fZ = -120;//120;
	}
	HRESULT Calculate(float brightness, float elapsed)
	{
		int i, j;
		for(i = 0; i < 128; i++)
		{
			accwf[i * PTCIRCUM / 128] += g_pAudio->GetSample(i*4) * 128.0f * 4.0f * elapsed * (PTCIRCUM / 128.0f);
		}

		static double accum = 1;
		accum += elapsed;
		double dChange = accum;

		bool fChanged = false;
		while(accum > 1.0)
		{
			for(i = 0; i < CLENGTH; i++)
			{
				double br = min(1, sin(i * 3.14159 / CLENGTH) * 5) * brightness;
				for(j = 0; j < 4; j++)
				{
					coil.pVertex[(i * 4) + j].m_cDiffuse = ZColour::Grey((int)(255.0f * br));
				}
			}
			for(i = 0; i < PTLENGTH; i++)
			{
				double br = min(1, sin(i * 3.14159 / PTLENGTH) * 1) * brightness;
	
				for(j = 0; j < PTCIRCUM; j++)
				{
					obj.pVertex[(i * PTCIRCUM) + j].m_cDiffuse = ZColour::Grey((int)(255.0f * br));
				}
			}

			for(i = PTLENGTH - 1; i > 0; i--)
			{
				for(j = 0; j < PTCIRCUM; j++) height[i][j] = height[i-1][j];
			}

			for(j = 0; j < PTCIRCUM; j++) height[0][j] = HEIGHTST + (accwf[j] * HEIGHTM);
//waveform[j * 127 / PTCIRCUM]
			int k = 0;
			for(i = 0; i < PTLENGTH; i++)
			{
				float fPos = 1.0f - fabs(((PTLENGTH / 2) - i) / (PTLENGTH / 2.0f));
				float fLenMult = 0.8f + 0.4f * sin(fPos * PI / 2.0f);
				for(j = 0; j < PTCIRCUM; j++)
				{
					float angle = j * PI2 / PTCIRCUM;

					obj.pVertex[k].m_vPos.m_fX = (i - (PTLENGTH*0.5f)) * PTDIST;
					obj.pVertex[k].m_vPos.m_fY = height[i][j] * fLenMult * sinf(angle);	
					obj.pVertex[k].m_vPos.m_fZ = height[i][j] * fLenMult * cosf(angle);

					k++;
				}
			}	

			obj.fPitch += 2.0f * g_fDegToRad;//2 * 3.14159 / 180.0;
			obj.fYaw += 1.5f * g_fDegToRad;//1.5 * 3.14159 / 180.0;
			coil.fPitch += 2.0f * g_fDegToRad;//2 * 3.14159 / 180.0;
			coil.fYaw += 1.5f * g_fDegToRad;//1.5 * 3.14159 / 180.0;

			accum--;
			obj.m_bsFlag.set(ZObject::F_VALID_VERTEX_NORMALS, false);
//			fChanged = true;
		}
//	if(fChanged)
//	{
//		obj.FindVertexNormals();
//		coil.FindVertexNormals();
		//		obj->NormalizeFaces();
//		obj->Normalize();
//	}
//	obj->x = accum * PTDIST;
		for(i = 0; i < PTCIRCUM; i++) accwf[i] = 0;

		obj.Calculate(&cCamera, elapsed);
		coil.Calculate(&cCamera, elapsed);
		return S_OK;
	}
	HRESULT Render()
	{
		HRESULT hRes;

//	obj->property(objTransparent, true);//fDrawn);
//	obj->property(objZBuffer, false);//fDrawn);
//	coil->property(objTransparent, true);
//	coil->property(objZBuffer, false);

		hRes = obj.Render();
		if(FAILED(hRes)) return hRes;

		hRes = coil.Render();
		if(FAILED(hRes)) return hRes;

		return S_OK;
	}
	HRESULT Reconfigure()
	{
		ZTexture *tx = g_pD3D->Find(TC_EMTUBE);
		coil.pTexture[0].Set(ZObject::Texture::T_ENVMAP, tx);
		obj.pTexture[0].Set(ZObject::Texture::T_ENVMAP, tx);

//	coil->SetTexture(tx);
//	obj->SetTexture(tx);

		for(int i = 0; i < PTLENGTH; i++)
		{
			for(int j = 0; j < PTCIRCUM; j++) height[i][j] = 0;
		}
		return S_OK;
	}
};

EXPORT_EFFECT(Tube, ZEffectTube)
