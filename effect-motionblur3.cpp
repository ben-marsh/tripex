#include "StdAfx.h"
#include "effect.h"
#include "error.h"
#include "ZObject.h"
#include "ZBezier.h"

//static ZObject pObj[RINGS];//, *o2, *o3;
//static World *scene;

//static float fChange, fShown = 0;
//static int nEffect, nNewEffect = -1;


template < bool bAltBlur > class ZEffectMotionBlur3T : public ZEffectBase
{
public:
	ZCamera camera;
	ZObject pObj[9];
	ZObject obj;

	float fChange;
	float fShown;
	int nEffect;
	int nNewEffect;

	bool bFirstCalc;
	float fPos;
	float fAvTotal, fAvTime;
	float fCamTarget;
	float fSpeed;
	bool bReset;
	float fCamPos;

	ZContainedBezier<2> bz;
	ZCamera cam;

	ZEffectMotionBlur3T() : bz(ZVector(20, 20, 20), -ZVector(20, 20, 20))//, cam(0)
	{
		cam.m_bsFlag.set( ZCamera::F_SCREEN_TRANSFORM, false );

		fCamPos = 0.0f;
		fAvTotal = 0;
		fAvTime = 16;
		fCamTarget = 110;
		bReset = false;
		fShown = 0;
		nNewEffect = -1;
		bFirstCalc = false;
		fPos = 0;

		obj.CreateTetrahedron(80.0f);
		nEffect = rand() % 5;
		// set lastframe to some constant thing

		camera.m_vPosition.m_fZ = -80;//110;

		for(int i = 0; i < 9; i++)
		{
//		pObj[i].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[i].CreateTorus(30, 5.0, 20, 5);
			pObj[i].m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
			if(bAltBlur) pObj[i].m_bsFlag.set( ZObject::F_DO_MIX_EXPOSURE_FACES );
			pObj[i].pTexture[0].m_nType = ZObject::Texture::T_LIGHTMAP;
			pObj[i].nExposure = 9;
			pObj[i].fFrameHistory = 4.0f;
			if(!bAltBlur) pObj[i].wcExposureLightChange = ZWideColour(-3,-3,-0);
			pObj[i].FindVertexNormals();
		}
	}
	ZVector GetPos(int nEffect, int i)
	{
		if(nEffect == 0)
		{
			if(i < 8)
			{
				float fRadius = (1 + (i / 4)) * 40.0;
				float fX = cos(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fY = sin(i * 3.14159 / 2.0) * fRadius;//80.0;
				return ZVector(fX, fY, 0);
			}
			else return ZVector::Origin();
		}
		else if(nEffect == 1)
		{
			if(i == 0) return ZVector(0, 40, 40);
			else if(i == 1) return ZVector(0, -40, 40);
			else if(i == 2) return ZVector(0, 0, 0);
			else if(i == 3) return ZVector(0, 0, 80);
			else if(i == 4) return ZVector(40, 0, 40);
			else if(i == 5) return ZVector(-40, 0, 40);
			else if(i == 6) return ZVector(80, 0, 40);
			else if(i == 7) return ZVector(-80, 0, 40);
			else return ZVector(0, 0, 40);
		}
		else if(nEffect == 2)
		{
			if(i < 8)
			{
				float fRadius = (1 + (i / 4)) * 40.0;
				float fX = cos(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fY = sin(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fZ = sin((1 + (i / 4)) * 3.14159 / 6.0) * 40.0;
				return ZVector(fX, fY, fZ);
			}
			else return ZVector::Origin();
		}
		else if(nEffect == 3)
		{
			float fAng = (i % 3) * 2.0 * 3.14159 / 3.0f;
			float fLength = 120.0f * ((i / 3) + 0.5f) / 4.0f;
			return ZVector(cos(fAng) * fLength, sin(fAng) * fLength, 0);
		}
		else //if(nEffect == 2)
		{
			float fX = cos(i * 3.14159 * 2.0 / 9) * 80.0;
			float fY = sin(i * 3.14159 * 2.0 / 9) * 80.0;
			return ZVector(fX, fY, 0);
		}
		return ZVector(0, 0, 0);
	}
	HRESULT Calculate(float brightness, float elapsed)
	{
		fAvTotal += elapsed * g_pAudio->GetIntensity( );
		fAvTime += elapsed;

		cam.m_vPosition.m_fZ = -80;

		obj.fRoll += elapsed * 3.14159 / 180.0f;
		obj.fPitch += elapsed * 2.0f * 3.14159 / 180.0f;
		obj.fYaw += elapsed * 3.0f * 3.14159 / 180.0f;
		obj.Calculate(&cam, elapsed);

		if(fAvTotal > 6 || bReset)
		{
			fCamTarget = -250 + (min(1.0f, fAvTotal / fAvTime) * 190);
			if(bReset)
			{
				camera.m_vPosition.m_fZ = fCamTarget;
				bReset = false;
			}
			fAvTotal = 0;
			fAvTime = 0;
			fSpeed = fabs(camera.m_vPosition.m_fZ - fCamTarget) / 32.0;
		}

		camera.m_vPosition.m_fZ = StepTo<float>(camera.m_vPosition.m_fZ, fCamTarget, fSpeed * elapsed);// * 2.0 * (-camera.vPosition.k / 250.0));//2);

		fCamPos += g_pAudio->GetIntensity( ) * elapsed * 0.02;
		camera.SetTarget(bz.Calculate(fCamPos));

		if(nNewEffect != -1)
		{
			fChange += elapsed / 20.0;
			if(fChange > 1)
			{
				nEffect = nNewEffect;
				nNewEffect = -1;
			}
		}
		else
		{
			fShown += elapsed;
			if(fShown > 50)
			{
				nNewEffect = rand() % 5;
				fShown = 0;
				fChange = 0;
			}
		}

		camera.m_fRoll += g_pAudio->GetIntensity( ) * 3.14159 * 2.0 * elapsed / 180.0;

		for(int i = 0; i < 9; i++)
		{
			if(nNewEffect != -1)
			{
				pObj[i].vPosition = (GetPos(nEffect, i) * (1 - fChange)) + (GetPos(nNewEffect, i) * fChange);
			}
			else
			{
				pObj[i].vPosition = GetPos(nEffect, i);
			}	

			float fSpeed = g_pAudio->GetDampenedBand( pEffectPtr->fSensitivity, i / 10.0, (i + 1) / 10.0);
			pObj[i].fRoll += elapsed * 4.0 * 3.14159 / 180.0;
			pObj[i].fPitch += g_pAudio->GetIntensity( ) * elapsed * 10.0 * 3.14159 / 180.0;
			pObj[i].fYaw += (g_pAudio->GetIntensity( ) + g_pAudio->GetBeat( ) ) * elapsed * 7 * 3.14159 / 180.0;
			pObj[i].wcAmbientLight = ZColour::Grey(brightness * 48.0f);// / pObj[i].nExposure);
			pObj[i].Calculate(&camera, elapsed);
		}

		return S_OK;
	}
	HRESULT Render()
	{
		HRESULT hRes;
		for(int i = 0; i < 9; i++)
		{
			hRes = pObj[i].Render();
			if(FAILED(hRes)) return TraceError(hRes);
		}
		return S_OK;
	}
	HRESULT Reconfigure()
	{
		fAvTime = 16;
		fAvTotal = g_pAudio->GetIntensity( ) * fAvTime;
		bReset = true;
		ZTexture *t = g_pD3D->Find(bAltBlur? TC_EMMOTIONBLUR3ALT : TC_EMMOTIONBLUR3);
		for(int i = 0; i < 9; i++) pObj[i].pTexture[0].m_pTexture = t;
		bFirstCalc = true;
		return S_OK;
	}
};

EXPORT_EFFECT(MotionBlur3, ZEffectMotionBlur3T<false>)
EXPORT_EFFECT(MotionBlur3Alt, ZEffectMotionBlur3T<true>)

//typedef ZEffectMotionBlur3T<false> ZEffectMotionBlur3;
//DECLARE_EFFECT_PTR(ZEffectMotionBlur3, pEffectMotionBlur3)

//typedef ZEffectMotionBlur3T<true> ZEffectMotionBlur3Alt;
//DECLARE_EFFECT_PTR(ZEffectMotionBlur3Alt, pEffectMotionBlur3Alt)
