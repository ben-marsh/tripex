#include "StdAfx.h"
#include "effect.h"
#include "ZObject.h"
#include "error.h"

#define RINGS 10//6//10//12 //7

#define NFR 50

class ZEffectMotionBlur1 : public ZEffectBase
{
public:
	bool fSetTexture;
	ZCamera camera;
	ZObject pObj[RINGS];//, *o2, *o3;
//static World *scene;
	float r, p, y;
	double rs[RINGS], ps[RINGS], ys[RINGS];
	bool fFirstCalc;
	double accum;

	ZEffectMotionBlur1()
	{
		accum = 1.5;
		fFirstCalc = false;
		fSetTexture = true;
		// set lastframe to some constant thing

		camera.m_vPosition.m_fZ = -110;

		for(int i = 0; i < RINGS; i++)
		{
//			pObj[i].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[i].CreateTorus(60 + (i * 40.0 / RINGS), 5.0, 20, 5);
			pObj[i].m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
			pObj[i].m_bsFlag.set( ZObject::F_NO_CULL );
//			pObj[i].Set(ZObject::NoClip);
			pObj[i].m_bsFlag.set( ZObject::F_DO_ROTATION_HISTORY );
			pObj[i].pTexture[0].m_nType = ZObject::Texture::T_LIGHTMAP;//EnvironmentMap;
			pObj[i].nExposure = 9;
			pObj[i].fFrameHistory = 7.0f;//7.0f;//7.0f;////13.0f;
			pObj[i].FindVertexNormals();
			pObj[i].fFrameTime = 1.0f;//1;//2.0f;//1.0f;

			rs[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
			ps[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
			ys[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
		}
	}
	bool CanRender(float fElapsed)
	{
		return (fElapsed > 0.5);
	}
	HRESULT Calculate(float brightness, float elapsed)
	{
		for(int i = 0; i < RINGS; i++)
		{
			pObj[i].fRoll += rs[i] * g_pAudio->GetIntensity( )* elapsed;
			pObj[i].fPitch += ps[i] * g_pAudio->GetIntensity( )* elapsed;
			pObj[i].fYaw += ys[i] * max(0.1, g_pAudio->GetIntensity( )) * elapsed;
			pObj[i].wcAmbientLight = ZColour::Grey(brightness * 20.0f);// / pObj[i].nExposure);
			pObj[i].Calculate(&camera, elapsed);
		}
		return S_OK;
	}
	HRESULT Render( )
	{
		HRESULT hRes;
		for(int i = 0; i < RINGS; i++)
		{
			hRes = pObj[i].Render( );
			if(FAILED(hRes)) return TraceError(hRes);
		}
		return S_OK;
	}
	HRESULT Reconfigure( )
	{
		ZTexture *t = g_pD3D->Find(TC_EMMOTIONBLUR);
		for(int i = 0; i < RINGS; i++) pObj[i].pTexture[0].m_pTexture = t;
		fFirstCalc = true;
		return S_OK;
	}
};

EXPORT_EFFECT(MotionBlur1, ZEffectMotionBlur1)
