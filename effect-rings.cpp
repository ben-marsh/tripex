#include "StdAfx.h"
#include "effect.h"
#include "ZObject.h"
#include "error.h"

#define	RINGS 25
#define RINGSTART 50.0f
#define RINGSEP 10
#define RINGDELAY 2

extern bool bMeshHQ;

class ZEffectRings : public ZEffectBase
{
public:
	bool fSetTexture;
	ZObject pObj[RINGS];
	ZCamera camera;
	float roll[RINGDELAY * RINGS];
	float pitch[RINGDELAY * RINGS];
	float yaw[RINGDELAY * RINGS];
	float brightness[RINGS];
	
	int nSpeed;
	float fSpeedR;
	float fSpeedP;
	float fSpeedY;

	bool fFirstCalc;
	double accum;
	double pos;

	ZEffectRings()
	{
		fFirstCalc = true;
		accum = 1.5;
		nSpeed = 0;
		fSpeedR = 1;
		fSpeedP = 0.5;
		fSpeedY = 3;
		pos = 0;

		for(int i = 0; i < RINGDELAY * RINGS; i++)
		{
			roll[i] = pitch[i] = yaw[i] = 0.0f;
		}

		for(int i = 0; i < RINGS; i++)
		{
			pObj[i].CreateTorus(RINGSTART + (i*RINGSEP), 2.0f, 20 + i, 4);//5 + i, 4);
			pObj[i].m_bsFlag.set(ZObject::F_DRAW_TRANSPARENT);//property(objTransparent, true);
			pObj[i].nExposure = std::max(1, 5 - (i / 3));
			pObj[i].fFrameHistory = 2.0f;
			pObj[i].wcExposureLightChange = ZWideColour(-8, -8, 0);
		}

		camera.m_vPosition.m_fZ = -120;
	}
	ZError* Calculate(float br, float elapsed, ZAudio* pAudio) override
	{
		accum += elapsed * 1.3;
		bool bChanged = false;
		while(accum >= 1)
		{
			for(int i = (RINGDELAY * RINGS) - 1; i > 0; i--)
			{
				roll[i] = roll[i-1];
				pitch[i] = pitch[i-1];
				yaw[i] = yaw[i-1];
			}
			for(int i = RINGS - 1; i > 0; i--)
			{
				brightness[i] = brightness[i-1] * 0.9f;//min(brightness[i-1], 1.5 * double(i) / (nRings * 0.7));
			}
			roll[0] += (0.8f + pAudio->GetIntensity( )) * fSpeedR * g_fDegToRad;
			pitch[0] += (0.8f + pAudio->GetIntensity( )) * fSpeedP * g_fDegToRad;
			yaw[0] += (0.8f + pAudio->GetIntensity( )) * fSpeedY * g_fDegToRad;

			brightness[0] = 0.6f + (0.3f * ((pAudio->GetIntensity( ) * 1.4f) + pAudio->GetBeat( )));

			pos += pAudio->GetIntensity( );
			if(pAudio->IsBeat( ) || pos > 4)
			{
				if(pos > 4) pos -= 4;
				fSpeedR = (rand() * 10.0f / RAND_MAX) - 5.0f;
				fSpeedP = (rand() * 10.0f / RAND_MAX) - 5.0f;
				fSpeedY = 0;//(rand() * 6.0 / RAND_MAX) - 3.0;
			}

			for(int i = 0; i < RINGS; i++)
			{
				pObj[i].fRoll += 1.0f * g_fDegToRad;//3.14159 / 180.0;//= roll[i * ringDelay];
				pObj[i].fPitch = pitch[i * RINGDELAY];
				pObj[i].fYaw = yaw[i * RINGDELAY];
	
				double b = std::max(std::min(1.0f, br * brightness[i]), 0.0f);

				pObj[i].wcAmbientLight = ZColour::Grey((int)(b * 255.0f));//->color = D3DRGB(b,b,b);
				pObj[i].vPosition.m_fZ = 50;
			}
			bChanged = true;
			accum--;
		}

		if(bChanged)
		{
			for(int i = 0; i < RINGS; i++)
			{
				pObj[i].Calculate(&camera, elapsed);
			}
		}
		return nullptr;
	}
	ZError* Reconfigure(ZAudio* pAudio) override
	{
		ZTexture *tx = g_pD3D->Find(TC_EMRINGS);
		for (int i = 0; i < RINGS; i++)
		{
			pObj[i].pTexture[0].Set(ZObject::Texture::T_ENVMAP, tx);
		}
		return nullptr;
	}
	ZError* Render() override
	{
		ZError* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render(); 
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	bool CanRender(double dElapsed)
	{
		return ((dElapsed * 1.3) + accum) >= 1;
	}
};

EXPORT_EFFECT(Rings, ZEffectRings)
