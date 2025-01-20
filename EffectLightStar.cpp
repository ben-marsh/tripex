#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "error.h"
#include "TextureData.h"

class EffectLightStar : public Effect
{
public:
	const TextureClass sprite_texture_class =
	{
		"Sprite",
		{ tex_light }
	};

	const TextureClass tint_texture_class =
	{
		"Tint",
		{ tex_eyes, tex_flesh, tex_forest, tex_shiny_sand }
	};

	static const int SOURCES = 512;

	Actor obj;
	Camera camera;
	float brt;
	double pdAng[SOURCES];
	double pdSpeed[SOURCES];
	double pdHeight[SOURCES];
	double pdRadius[SOURCES];
	Texture *pTint;
	double dRadAng;
	double dViewAng;
	int nSpikes;

	EffectLightStar()
		: Effect({ &sprite_texture_class, &tint_texture_class })
	{
		dRadAng = 2 * 3.14159 * 2.0;
		dViewAng = 0;
		nSpikes = 8;

		obj.vertices.resize(SOURCES);
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_Z_BUFFER, false );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.textures[0].type = Actor::TextureType::Sprite;
		obj.sprite_size = 7.5;//pObj->fRenderAsLights(15.0);
		camera.position.z = -100;

		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] = rand() * (3.14159 * 2.0) / RAND_MAX;
			pdSpeed[i] = rand() * (2.0 * 3.14159 / 180.0) / RAND_MAX;
			pdHeight[i] = 35 - (rand() * 70.0 / RAND_MAX);
			pdRadius[i] = 100 + (rand() * 60.0 / RAND_MAX);
			pdHeight[i] = -10.0 + (rand() * 20.0 / RAND_MAX);
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		brt = params.brightness;
		dRadAng += (1.0 + (params.elapsed * (6.0 * params.audio_data.GetIntensity( )))) * 3.14159 / 128.0;
		if(dRadAng > 3.14159 * 2.0)
		{
			if(params.audio_data.GetIntensity( ) < 0.2) nSpikes = 0;
			else if(params.audio_data.GetIntensity( ) < 0.45) nSpikes = 1;
			else if(params.audio_data.GetIntensity( ) < 0.6) nSpikes = 2;
			else if(params.audio_data.GetIntensity( ) < 0.75) nSpikes = 3;
			else if(params.audio_data.GetIntensity( ) < 0.9) nSpikes = 4;
			else nSpikes = 5;

			while(dRadAng > 3.14159 * 2.0) dRadAng -= 3.14159 * 2.0;
		}
		obj.yaw += 1.0f * DEG_TO_RAD;

		dViewAng += 3.14159 / 180.0;
		camera.position.z = -50;
		camera.position.y = -120;
		//camera.pointAt(0,0,0);
		Vector3 vDir = Vector3::Origin() - camera.position;
		camera.pitch = vDir.GetPitch();
		camera.yaw = vDir.GetYaw();

		double dRadMult = 50.0 * sin(dRadAng);
		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] += pdSpeed[i] * params.elapsed;
			pdRadius[i] = 100 + (dRadMult * sin(pdAng[i] * nSpikes));

			obj.vertices[i].position.x = sin(pdAng[i]) * pdRadius[i];
			obj.vertices[i].position.y = pdHeight[i];
			obj.vertices[i].position.z = cos(pdAng[i]) * pdRadius[i];
		}
		obj.ambient_light_color = ColorRgb::Grey(128.0 * params.brightness);//->color = D3DRGB(dBr, dBr, dBr);
		obj.Calculate(params.renderer, &camera, params.elapsed);

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
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		obj.textures[0].texture = params.texture_library.Find(sprite_texture_class);
		pTint = params.texture_library.Find(tint_texture_class);
		return nullptr;
	}
};

EXPORT_EFFECT( LightStar, EffectLightStar )
