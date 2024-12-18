#include "StdAfx.h"
#include "effect.h"
#include <conio.h>
#include "ZObject.h"
#include "error.h"
#include "general.h"

#define SOURCES 512

//static ZObject *pObj;

//double dRadAng = 2 * 3.14159 * 2.0;
//double dViewAng = 0;
//int nSpikes = 8;
//Texture *pTint;

//#define alNumLS 512
#define SPREAD 50
#define RADIUS 160
#define SPEEDSPREAD 20
#define FOREGROUNDBR 0.5

extern ZTexture *pBlankTexture;

class ZEffectCollapsingLightSphere : public ZEffectBase
{
public:
	double pdAng[SOURCES];
	double pdYPos[SOURCES];
	double pdTilt[SOURCES];
	double pdRadius[SOURCES];
	double pdSpeed[SOURCES];
	ZTexture *pTint;
	double dWaitTime;
	double dTilt;
	ZObject obj;
	ZCamera camera;

	// 0 - contracted
	// 1 - moving out
	// 2 - out
	// 3 - moving in
	int nStage;
	double brt;

	ZEffectCollapsingLightSphere()
	{
		dWaitTime = 0;
		dTilt = 0;

		nStage = 0;
		obj.pVertex.SetLength(SOURCES);
	//	pObj->Create(SOURCES, 1);
		obj.m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( ZObject::F_DRAW_VERTEX_SPRITES );
		obj.wcExposureLightChange = ZWideColour(-60, -60, -60);
		obj.nExposure = 3;
		obj.fSpriteSize = 7.0;
		obj.fFrameHistory = 2.0;
		camera.m_vPosition.m_fZ = -240;

		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] = rand() * PI2 / RAND_MAX;
			pdSpeed[i] = SPEEDSPREAD * (rand() * (SPEEDSPREAD / 2.0) * g_fDegToRad / RAND_MAX);
			pdTilt[i] = rand() * PI2 / RAND_MAX;
			pdRadius[i] = rand() * RADIUS / RAND_MAX;
			pdYPos[i] = (SPREAD / 2.0) - (double(rand()) * SPREAD / RAND_MAX);
		}
	}
	HRESULT Calculate(float brightness, float elapsed, ZAudio* pAudio)
	{
		brt = brightness;
		if(nStage == 0) dWaitTime += elapsed * pAudio->GetDampenedBand( pEffectPtr->fSensitivity, 0.0f, 1.0f);
		else if(nStage == 1) dTilt += elapsed * 1.5 * g_fDegToRad;
		else if(nStage == 2) dWaitTime += elapsed * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0.0f, 1.0f);
		else if(nStage == 3) dTilt -= elapsed * 1.5 * g_fDegToRad;

		if((dTilt < 0) || (dTilt > 3.141592 / 2.0) || (dWaitTime > 40.0 && pAudio->GetIntensity( ) > 0.6))
		{
			nStage = (nStage + 1) & 3;
			dWaitTime = 0.0;
			dTilt = min(max(dTilt, 0), 3.14159 / 2.0);
		}

		float fTwistMult = sin(dTilt);// * 3.14159 / 128.0);
		float multp = (/*fac*/dTilt * pAudio->GetIntensity( )) + (1 - dTilt/*fac*/) + 0.1;

		obj.pVertex.SetLength(SOURCES);
		for(int i = 0; i < SOURCES; i++)
		{
			float x = cos(pdAng[i]) * pdRadius[i];
			float y = pdYPos[i];
			float z = -100 + (sin(pdAng[i]) * pdRadius[i]);

			float sin_t = sin(pdTilt[i] * fTwistMult);
			float cos_t = cos(pdTilt[i] * fTwistMult);

			obj.pVertex[i].m_vPos.m_fX = (y * cos_t) + (x * sin_t);
			obj.pVertex[i].m_vPos.m_fY = (y * sin_t) + (x * cos_t);
			obj.pVertex[i].m_vPos.m_fZ = z;

			pdAng[i] += elapsed * multp * (pAudio->GetIntensity( ) + 0.1) * pdSpeed[i] * 3.14159 / 180.0;
			pdTilt[i] += (pAudio->GetIntensity( ) + 0.1) * elapsed * multp * 1 * 3.14159 / 180.0;

			float fTransMult = FOREGROUNDBR + ((z / -100.0) * (1 - FOREGROUNDBR));
			float fBr = brightness * fTransMult / 2.0;

			obj.pVertex[i].m_cDiffuse = ZColour::Grey(0.6 * fBr * 255.0);
	//		alObject[i]->alpha = 128*brightness*transMult;
	//		fvVertex++;
		}
		obj.m_bsFlag.set( ZObject::F_VALID_VERTEX_DIFFUSE );
		obj.Calculate(&camera, elapsed);
		return D3D_OK;
	}
	HRESULT Render( ) override
	{
		HRESULT hRes;
		hRes = obj.Render( );
		if(FAILED(hRes)) return hRes;

		if(pTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, pTint);

			g_pD3D->ResetRenderState();
			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_pD3D->DrawSprite(ZPoint<int>(0, 0), ZRect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ZColour::Grey(brt * 255.0));
		}
		return S_OK;
	}
	virtual HRESULT Reconfigure(ZAudio* pAudio) override
	{
		obj.pTexture[0].Set(ZObject::Texture::T_SPRITE, g_pD3D->Find(TC_LBCOLLAPSINGSPHERE));
		pTint = g_pD3D->Find(TC_WTCOLLAPSINGSPHERE);
		return D3D_OK;
	}
};
EXPORT_EFFECT( CollapsingLightSphere, ZEffectCollapsingLightSphere )

