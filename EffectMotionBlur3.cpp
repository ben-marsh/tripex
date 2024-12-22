#include "Platform.h"
#include "effect.h"
#include "error.h"
#include "Actor.h"
#include "BezierCurve.h"

//static ZObject pObj[RINGS];//, *o2, *o3;
//static World *scene;

//static float fChange, fShown = 0;
//static int nEffect, nNewEffect = -1;


template < bool bAltBlur > class EffectMotionBlur3T : public EffectBase
{
public:
	Camera camera;
	Actor pObj[9];
	Actor obj;

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

	ContainedBezierCurve<2> bz;
	Camera cam;

	EffectMotionBlur3T() : bz(Vector3(20, 20, 20), -Vector3(20, 20, 20))//, cam(0)
	{
		cam.flags.set( Camera::F_SCREEN_TRANSFORM, false );

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

		camera.position.z = -80;//110;

		for(int i = 0; i < 9; i++)
		{
//		pObj[i].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[i].CreateTorus(30, 5.0, 20, 5);
			pObj[i].flags.set( Actor::F_DRAW_TRANSPARENT );
			if(bAltBlur) pObj[i].flags.set( Actor::F_DO_MIX_EXPOSURE_FACES );
			pObj[i].textures[0].type = Actor::TextureType::Lightmap;
			pObj[i].exposure = 9;
			pObj[i].frame_history = 4.0f;
			if(!bAltBlur) pObj[i].exposure_light_delta = WideColorRgb(-3,-3,-0);
			pObj[i].FindVertexNormals();
		}
	}
	Vector3 GetPos(int nEffect, int i)
	{
		if(nEffect == 0)
		{
			if(i < 8)
			{
				float fRadius = (1 + (i / 4)) * 40.0;
				float fX = cos(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fY = sin(i * 3.14159 / 2.0) * fRadius;//80.0;
				return Vector3(fX, fY, 0);
			}
			else return Vector3::Origin();
		}
		else if(nEffect == 1)
		{
			if(i == 0) return Vector3(0, 40, 40);
			else if(i == 1) return Vector3(0, -40, 40);
			else if(i == 2) return Vector3(0, 0, 0);
			else if(i == 3) return Vector3(0, 0, 80);
			else if(i == 4) return Vector3(40, 0, 40);
			else if(i == 5) return Vector3(-40, 0, 40);
			else if(i == 6) return Vector3(80, 0, 40);
			else if(i == 7) return Vector3(-80, 0, 40);
			else return Vector3(0, 0, 40);
		}
		else if(nEffect == 2)
		{
			if(i < 8)
			{
				float fRadius = (1 + (i / 4)) * 40.0;
				float fX = cos(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fY = sin(i * 3.14159 / 2.0) * fRadius;//80.0;
				float fZ = sin((1 + (i / 4)) * 3.14159 / 6.0) * 40.0;
				return Vector3(fX, fY, fZ);
			}
			else return Vector3::Origin();
		}
		else if(nEffect == 3)
		{
			float fAng = (i % 3) * 2.0 * 3.14159 / 3.0f;
			float fLength = 120.0f * ((i / 3) + 0.5f) / 4.0f;
			return Vector3(cos(fAng) * fLength, sin(fAng) * fLength, 0);
		}
		else //if(nEffect == 2)
		{
			float fX = cos(i * 3.14159 * 2.0 / 9) * 80.0;
			float fY = sin(i * 3.14159 * 2.0 / 9) * 80.0;
			return Vector3(fX, fY, 0);
		}
		return Vector3(0, 0, 0);
	}
	Error* Calculate(const CalculateParams& params) override
	{
		fAvTotal += params.elapsed * params.audio_data->GetIntensity( );
		fAvTime += params.elapsed;

		cam.position.z = -80;

		obj.roll += params.elapsed * 3.14159 / 180.0f;
		obj.pitch += params.elapsed * 2.0f * 3.14159 / 180.0f;
		obj.yaw += params.elapsed * 3.0f * 3.14159 / 180.0f;
		obj.Calculate(&cam, params.elapsed);

		if(fAvTotal > 6 || bReset)
		{
			fCamTarget = -250 + (std::min(1.0f, fAvTotal / fAvTime) * 190);
			if(bReset)
			{
				camera.position.z = fCamTarget;
				bReset = false;
			}
			fAvTotal = 0;
			fAvTime = 0;
			fSpeed = fabs(camera.position.z - fCamTarget) / 32.0;
		}

		camera.position.z = StepTo<float>(camera.position.z, fCamTarget, fSpeed * params.elapsed);// * 2.0 * (-camera.vPosition.k / 250.0));//2);

		fCamPos += params.audio_data->GetIntensity( ) * params.elapsed * 0.02;
		camera.SetTarget(bz.Calculate(fCamPos));

		if(nNewEffect != -1)
		{
			fChange += params.elapsed / 20.0;
			if(fChange > 1)
			{
				nEffect = nNewEffect;
				nNewEffect = -1;
			}
		}
		else
		{
			fShown += params.elapsed;
			if(fShown > 50)
			{
				nNewEffect = rand() % 5;
				fShown = 0;
				fChange = 0;
			}
		}

		camera.roll += params.audio_data->GetIntensity( ) * 3.14159 * 2.0 * params.elapsed / 180.0;

		for(int i = 0; i < 9; i++)
		{
			if(nNewEffect != -1)
			{
				pObj[i].position = (GetPos(nEffect, i) * (1 - fChange)) + (GetPos(nNewEffect, i) * fChange);
			}
			else
			{
				pObj[i].position = GetPos(nEffect, i);
			}	

			float fSpeed = params.audio_data->GetDampenedBand( pEffectPtr->fSensitivity, i / 10.0, (i + 1) / 10.0);
			pObj[i].roll += params.elapsed * 4.0 * 3.14159 / 180.0;
			pObj[i].pitch += params.audio_data->GetIntensity( ) * params.elapsed * 10.0 * 3.14159 / 180.0;
			pObj[i].yaw += (params.audio_data->GetIntensity( ) + params.audio_data->GetBeat( ) ) * params.elapsed * 7 * 3.14159 / 180.0;
			pObj[i].ambient_light_color = ColorRgb::Grey(params.brightness * 48.0f);// / pObj[i].nExposure);
			pObj[i].Calculate(&camera, params.elapsed);
		}

		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		for(int i = 0; i < 9; i++)
		{
			Error* error = pObj[i].Render();
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		fAvTime = 16;
		fAvTotal = params.audio_data->GetIntensity( ) * fAvTime;
		bReset = true;
		Texture *t = g_pD3D->Find(bAltBlur? TextureClass::MotionBlur3AltEnvMap : TextureClass::MotionBlur3EnvMap);
		for(int i = 0; i < 9; i++) pObj[i].textures[0].texture = t;
		bFirstCalc = true;
		return nullptr;
	}
};

EXPORT_EFFECT(MotionBlur3, EffectMotionBlur3T<false>)
EXPORT_EFFECT(MotionBlur3Alt, EffectMotionBlur3T<true>)

//typedef ZEffectMotionBlur3T<false> ZEffectMotionBlur3;
//DECLARE_EFFECT_PTR(ZEffectMotionBlur3, pEffectMotionBlur3)

//typedef ZEffectMotionBlur3T<true> ZEffectMotionBlur3Alt;
//DECLARE_EFFECT_PTR(ZEffectMotionBlur3Alt, pEffectMotionBlur3Alt)
