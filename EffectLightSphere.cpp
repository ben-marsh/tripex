#include "Platform.h"
#include "BezierCurve.h"
#include "Actor.h"
#include "error.h"
#include "effect.h"
#include "TextureData.h"

//#define SOURCES 65//180//65//80
#define FRAMES 10
#define LENGTH 1
#define BUFFERSTEP 100

extern bool bMeshHQ;

//extern EFFECT lightsphere;

class EffectLightSphere : public EffectBase
{
public:
	const TextureClass sprite_texture_class =
	{
		"Sprite",
		{ g_anTexLight }
	};

	const TextureClass tint_texture_class =
	{
		"Tint",
		{ g_anTexEyes, g_anTexFlesh, g_anTexForest, g_anTexShinySand }
	};

	Actor obj;
	Camera camera;

	Texture *tex;
	Texture *tx;

	std::vector<Vector3> pvPosition[4];
	std::vector<double> position;
	std::vector<double> speed;
	std::vector<double> length[FRAMES];

	bool fNotRendered;
	int nSources;

	double br;
	double brt;

	BezierCurve b;

	double accum;
	double r, p, y;
	double dBrBack;
	Texture *pTint;

	EffectLightSphere()
		: EffectBase({ &sprite_texture_class, &tint_texture_class })
		, b(4)
	{
		accum = 1.1;
		r = p = y = 0;
		fNotRendered = true;
		nSources = 100;
		
		for(int i = 0; i < 4; i++) pvPosition[i].resize(nSources);
		for(int i = 0; i < FRAMES; i++) length[i].resize(nSources);
		position.resize(nSources);
		speed.resize(nSources);

		obj.position.z = 50;
		obj.vertices.resize(nSources);
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITE_HISTORY );
		obj.frame_history = 10.0;
		obj.sprite_size = 2.0f;
		obj.sprite_history_length = 1.0f;
		obj.max_history_length = 25;
		obj.exposure_light_delta = WideColorRgb(-2, -2, -2);
	}
	Error* Calculate(const CalculateParams& params) override
	{
		brt = params.brightness;
		accum += params.elapsed * 2.0f;
		for(; accum > 1.0f; accum--)
		{
			float elapsed = 1.0f;

			br = params.brightness;
			for(int i = 0; i < nSources; i++)
			{
				position[i] += speed[i] * params.audio_data.GetIntensity( ) * 0.3 * elapsed;
				double linearity = std::max(0.0f, 1 - params.audio_data.GetBeat( ) );//);//(bigbeat / 2.0));
			
				while(position[i] > 1.0 || position[i] < 0.0)
				{
					pvPosition[0][i] = pvPosition[3][i];
					pvPosition[1][i] = pvPosition[3][i] + (pvPosition[3][i] - pvPosition[2][i]);

					pvPosition[3][i].x = (rand() * 70.0 / RAND_MAX) - 35.0;
					pvPosition[3][i].y = (rand() * 70.0 / RAND_MAX) - 35.0;
					pvPosition[3][i].z = (rand() * 70.0 / RAND_MAX) - 35.0;

					float c = params.audio_data.GetRandomSample( );
					pvPosition[2][i].x = (pvPosition[3][i].x * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);
					pvPosition[2][i].y = (pvPosition[3][i].y * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);
					pvPosition[2][i].z = (pvPosition[3][i].z * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);

					pvPosition[3][i].y = (pvPosition[0][i].y * (1 - linearity)) + (pvPosition[3][i].y * linearity);
					pvPosition[2][i].y = (pvPosition[1][i].y * (1 - linearity)) + (pvPosition[2][i].y * linearity);

					if(position[i] > 1.0) position[i] -= 1.0;
					if(position[i] < 0.0) position[i] += 1.0;
					speed[i] = (params.audio_data.GetRandomSample( ) * 0.15) + 0.01;
				}

				for(int j = 0; j < 4; j++)
				{
					b[j] = pvPosition[j][i];
				}
				obj.vertices[i].position = b.Calculate(position[i]);
			}

			obj.roll += elapsed * params.audio_data.GetIntensity( ) * 4.0 * 3.14159 / 180.0;
			obj.pitch += elapsed * params.audio_data.GetIntensity( ) * 3.0 * 3.14159 / 180.0;
			obj.yaw += elapsed * 2.0 * 3.14159 / 180.0;
			obj.Calculate(params.renderer, &camera, 1.0);
			obj.ambient_light_color = ColorRgb::Grey(64.0 * params.brightness);
		}
		return nullptr;
	}
	virtual Error* Reconfigure(const ReconfigureParams& params) override
	{
		dBrBack = 1;
		if(fNotRendered)// || (rand() <= (RAND_MAX * 0.3)))
		{
			for(int i = 0; i < nSources; i++)
			{
				position[i] = 1;
				speed[i] = rand() * 0.02 / RAND_MAX;	

				pvPosition[3][i].x = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[3][i].y = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[3][i].z = (rand() * 50.0 / RAND_MAX) - 25.0;

				pvPosition[2][i].x = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[2][i].y = pvPosition[3][i].y;//(rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[2][i].z = (rand() * 50.0 / RAND_MAX) - 25.0;
			}
			fNotRendered = false;
		}

		obj.textures[0].Set(Actor::TextureType::Sprite, params.texture_library.Find(sprite_texture_class));

		pTint = params.texture_library.Find(tint_texture_class);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error = obj.Render(params.renderer);
		if(error) return TraceError(error);

		if(pTint != nullptr)
		{
			RenderState render_state;
			render_state.blend_mode = BlendMode::Tint;
			render_state.depth_mode = DepthMode::Disable;
			render_state.texture_stages[0].texture = pTint;

			params.renderer.DrawSprite(render_state, Point<int>(0, 0), Rect<int>(0, 0, params.renderer.GetWidth(), params.renderer.GetHeight()), ColorRgb::Grey(brt * 255.0));
		}
		return nullptr;
	}
	bool CanRender(double dElapsed)
	{
		return (dElapsed > 0.2);
	}
};

EXPORT_EFFECT( LightSphere, EffectLightSphere )

