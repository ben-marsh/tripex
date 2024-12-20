#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "error.h"
#include <algorithm>

#define RINGS 10//6//10//12 //7

#define NFR 50

class EffectMotionBlur1 : public EffectBase
{
public:
	bool fSetTexture;
	Camera camera;
	Actor pObj[RINGS];//, *o2, *o3;
//static World *scene;
	float r, p, y;
	double rs[RINGS], ps[RINGS], ys[RINGS];
	bool fFirstCalc;
	double accum;

	EffectMotionBlur1()
	{
		accum = 1.5;
		fFirstCalc = false;
		fSetTexture = true;
		// set lastframe to some constant thing

		camera.position.z = -110;

		for(int i = 0; i < RINGS; i++)
		{
//			pObj[i].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[i].CreateTorus(60 + (i * 40.0 / RINGS), 5.0, 20, 5);
			pObj[i].m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
			pObj[i].m_bsFlag.set( Actor::F_NO_CULL );
//			pObj[i].Set(ZObject::NoClip);
			pObj[i].m_bsFlag.set( Actor::F_DO_ROTATION_HISTORY );
			pObj[i].pTexture[0].m_nType = Actor::TextureEntry::T_LIGHTMAP;//EnvironmentMap;
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
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		for(int i = 0; i < RINGS; i++)
		{
			pObj[i].fRoll += rs[i] * pAudio->GetIntensity( )* elapsed;
			pObj[i].fPitch += ps[i] * pAudio->GetIntensity( )* elapsed;
			pObj[i].fYaw += ys[i] * std::max(0.1f, pAudio->GetIntensity( )) * elapsed;
			pObj[i].wcAmbientLight = ColorRgb::Grey(brightness * 20.0f);// / pObj[i].nExposure);
			pObj[i].Calculate(&camera, elapsed);
		}
		return nullptr;
	}
	Error* Render( ) override
	{
		Error* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render( );
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		Texture *t = g_pD3D->Find(TC_EMMOTIONBLUR);
		for (int i = 0; i < RINGS; i++)
		{
			pObj[i].pTexture[0].m_pTexture = t;
		}
		fFirstCalc = true;
		return nullptr;
	}
};

EXPORT_EFFECT(MotionBlur1, EffectMotionBlur1)
