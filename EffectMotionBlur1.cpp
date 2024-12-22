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
			pObj[i].flags.set( Actor::F_DRAW_TRANSPARENT );
			pObj[i].flags.set( Actor::F_NO_CULL );
//			pObj[i].Set(ZObject::NoClip);
			pObj[i].flags.set( Actor::F_DO_ROTATION_HISTORY );
			pObj[i].textures[0].type = Actor::TextureType::Lightmap;//EnvironmentMap;
			pObj[i].exposure = 9;
			pObj[i].frame_history = 7.0f;//7.0f;//7.0f;////13.0f;
			pObj[i].FindVertexNormals();
			pObj[i].frame_time = 1.0f;//1;//2.0f;//1.0f;

			rs[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
			ps[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
			ys[i] = (0.5 + (0.5 * i / RINGS)) * 0.6 * ((rand() - (RAND_MAX/2)) * 3.14592 / 22.0) / (RAND_MAX / 2);
		}
	}
	bool CanRender(float fElapsed)
	{
		return (fElapsed > 0.5);
	}
	Error* Calculate(const CalculateParams& params) override
	{
		for(int i = 0; i < RINGS; i++)
		{
			pObj[i].roll += rs[i] * params.audio_data->GetIntensity( ) * params.elapsed;
			pObj[i].pitch += ps[i] * params.audio_data->GetIntensity( ) * params.elapsed;
			pObj[i].yaw += ys[i] * std::max(0.1f, params.audio_data->GetIntensity( )) * params.elapsed;
			pObj[i].ambient_light_color = ColorRgb::Grey(params.brightness * 20.0f);// / pObj[i].nExposure);
			pObj[i].Calculate(&camera, params.elapsed);
		}
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render( );
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		Texture *t = g_pD3D->Find(TextureClass::MotionBlurEnvMap);
		for (int i = 0; i < RINGS; i++)
		{
			pObj[i].textures[0].texture = t;
		}
		fFirstCalc = true;
		return nullptr;
	}
};

EXPORT_EFFECT(MotionBlur1, EffectMotionBlur1)
