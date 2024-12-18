#include "StdAfx.h"
#include "effect.h"
#include "ZObject.h"
#include "error.h"
#include "general.h"

#define SOURCES 512

extern ZTexture *pBlankTexture;

class ZEffectLightStar : public ZEffectBase
{
public:
	ZObject obj;
	ZCamera camera;
	float brt;
	double pdAng[SOURCES];
	double pdSpeed[SOURCES];
	double pdHeight[SOURCES];
	double pdRadius[SOURCES];
	ZTexture *pTint;
	double dRadAng;
	double dViewAng;
	int nSpikes;

	ZEffectLightStar()
	{
		dRadAng = 2 * 3.14159 * 2.0;
		dViewAng = 0;
		nSpikes = 8;

		obj.pVertex.SetLength(SOURCES);
		obj.m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( ZObject::F_DRAW_Z_BUFFER, false );
		obj.m_bsFlag.set( ZObject::F_DRAW_VERTEX_SPRITES );
		obj.pTexture[0].m_nType = ZObject::Texture::T_SPRITE;
		obj.fSpriteSize = 7.5;//pObj->fRenderAsLights(15.0);
		camera.m_vPosition.m_fZ = -100;

		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] = rand() * (3.14159 * 2.0) / RAND_MAX;
			pdSpeed[i] = rand() * (2.0 * 3.14159 / 180.0) / RAND_MAX;
			pdHeight[i] = 35 - (rand() * 70.0 / RAND_MAX);
			pdRadius[i] = 100 + (rand() * 60.0 / RAND_MAX);
			pdHeight[i] = -10.0 + (rand() * 20.0 / RAND_MAX);
		}
	}
	HRESULT Calculate(float brightness, float elapsed, ZAudio* pAudio)
	{
		brt = brightness;
		dRadAng += (1.0 + (elapsed * (6.0 * pAudio->GetIntensity( )))) * 3.14159 / 128.0;
		if(dRadAng > 3.14159 * 2.0)
		{
			if(pAudio->GetIntensity( ) < 0.2) nSpikes = 0;
			else if(pAudio->GetIntensity( ) < 0.45) nSpikes = 1;
			else if(pAudio->GetIntensity( ) < 0.6) nSpikes = 2;
			else if(pAudio->GetIntensity( ) < 0.75) nSpikes = 3;
			else if(pAudio->GetIntensity( ) < 0.9) nSpikes = 4;
			else nSpikes = 5;

			while(dRadAng > 3.14159 * 2.0) dRadAng -= 3.14159 * 2.0;
		}
		obj.fYaw += 1.0f * g_fDegToRad;

		dViewAng += 3.14159 / 180.0;
		camera.m_vPosition.m_fZ = -50;
		camera.m_vPosition.m_fY = -120;
		//camera.pointAt(0,0,0);
		ZVector vDir = ZVector::Origin() - camera.m_vPosition;
		camera.m_fPitch = vDir.GetPitch();
		camera.m_fYaw = vDir.GetYaw();

		double dRadMult = 50.0 * sin(dRadAng);
		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] += pdSpeed[i] * elapsed;
			pdRadius[i] = 100 + (dRadMult * sin(pdAng[i] * nSpikes));

			obj.pVertex[i].m_vPos.m_fX = sin(pdAng[i]) * pdRadius[i];
			obj.pVertex[i].m_vPos.m_fY = pdHeight[i];
			obj.pVertex[i].m_vPos.m_fZ = cos(pdAng[i]) * pdRadius[i];
		}
		obj.wcAmbientLight = ZColour::Grey(128.0 * brightness);//->color = D3DRGB(dBr, dBr, dBr);
		obj.Calculate(&camera, elapsed);

		return S_OK;
	}
	HRESULT Render( )
	{
		HRESULT hRes = obj.Render( );
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
		obj.pTexture[0].m_pTexture = g_pD3D->Find(TC_LBLIGHTSTAR);
		pTint = g_pD3D->Find(TC_WTLIGHTSTAR);
		return S_OK;
	}
};
EXPORT_EFFECT( LightStar, ZEffectLightStar )
