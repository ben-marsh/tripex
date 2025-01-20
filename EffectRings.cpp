#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "TextureData.h"
#include "error.h"

class EffectRings : public Effect
{
public:
	const TextureClass envmap_texture_class =
	{
		"EnvMap",
		{ tex_alien_egg, tex_shiny_sand }
	};

	static const int RINGS = 25;
	const float RINGSTART = 50.0f;
	const float RINGSEP = 10.0f;
	static const int RINGDELAY = 2;

	bool fSetTexture;
	Actor pObj[RINGS];
	Camera camera;
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

	EffectRings()
		: Effect({ &envmap_texture_class })
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
			pObj[i].flags.set(Actor::F_DRAW_TRANSPARENT);//property(objTransparent, true);
			pObj[i].exposure = std::max(1, 5 - (i / 3));
			pObj[i].frame_history = 2.0f;
			pObj[i].exposure_light_delta = WideColorRgb(-8, -8, 0);
		}

		camera.position.z = -120;
	}
	Error* Calculate(const CalculateParams& params) override
	{
		accum += params.elapsed * 1.3;
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
			roll[0] += (0.8f + params.audio_data.GetIntensity( )) * fSpeedR * DEG_TO_RAD;
			pitch[0] += (0.8f + params.audio_data.GetIntensity( )) * fSpeedP * DEG_TO_RAD;
			yaw[0] += (0.8f + params.audio_data.GetIntensity( )) * fSpeedY * DEG_TO_RAD;

			brightness[0] = 0.6f + (0.3f * ((params.audio_data.GetIntensity( ) * 1.4f) + params.audio_data.GetBeat( )));

			pos += params.audio_data.GetIntensity( );
			if(params.audio_data.IsBeat( ) || pos > 4)
			{
				if(pos > 4) pos -= 4;
				fSpeedR = (rand() * 10.0f / RAND_MAX) - 5.0f;
				fSpeedP = (rand() * 10.0f / RAND_MAX) - 5.0f;
				fSpeedY = 0;//(rand() * 6.0 / RAND_MAX) - 3.0;
			}

			for(int i = 0; i < RINGS; i++)
			{
				pObj[i].roll += 1.0f * DEG_TO_RAD;//3.14159 / 180.0;//= roll[i * ringDelay];
				pObj[i].pitch = pitch[i * RINGDELAY];
				pObj[i].yaw = yaw[i * RINGDELAY];
	
				double b = std::max(std::min(1.0f, params.brightness * brightness[i]), 0.0f);

				pObj[i].ambient_light_color = ColorRgb::Grey((int)(b * 255.0f));//->color = D3DRGB(b,b,b);
				pObj[i].position.z = 50;
			}
			bChanged = true;
			accum--;
		}

		if(bChanged)
		{
			for(int i = 0; i < RINGS; i++)
			{
				pObj[i].Calculate(params.renderer, &camera, params.elapsed);
			}
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		Texture *tx = params.texture_library.Find(envmap_texture_class);
		for (int i = 0; i < RINGS; i++)
		{
			pObj[i].textures[0].Set(Actor::TextureType::Envmap, tx);
		}
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render(params.renderer);
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	bool CanRender(double dElapsed)
	{
		return ((dElapsed * 1.3) + accum) >= 1;
	}
};

EXPORT_EFFECT(Rings, EffectRings)
