#include "Platform.h"
#include "Actor.h"
#include "effect.h"
#include "error.h"

#define DEPTH 8
#define MAG 20

#define FRAMEFAC 50

#define AVERAGES 10

class EffectPhased : public EffectBase
{
public:
	Actor obj;
	Camera camera;

	double accum;
	bool fFirstCalc;
	int lastbeat;
	double a;

	EffectPhased()
	{
		a = 0;
		lastbeat = 0;
		fFirstCalc = false;
		accum = 0;

		obj.vertices.SetLength(DEPTH*DEPTH*DEPTH);
		obj.frame_history = 5.0f;
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
//		obj.Set(ZObject::DoRotationHistory);
//		obj.Set(ZObject::DoFrameHistory);// = (1 << 24L),

		obj.exposure = 6;//3;//4;//6;
//		obj.wcAmbientLight = ZColour::Grey(64);

		int index = 0;
		for(int x = 0; x < DEPTH; x++)
		{
			for(int y = 0; y < DEPTH; y++)
			{
				for(int z = 0; z < DEPTH; z++)
				{
					obj.vertices[index].position.x = (x - (DEPTH/2)) * MAG;
					obj.vertices[index].position.y = (y - (DEPTH/2)) * MAG;
					obj.vertices[index].position.z = (z - (DEPTH/2)) * MAG;
					index++;
				}
			}
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		float elapsed = params.elapsed * 1.5;

		float fMult = elapsed;//10.0 / FRAMEFAC;

		a += fMult * (params.audio_data->GetIntensity( ) + 0.1) * 3.14159 / 180.0;
		obj.roll += fMult * (params.audio_data->GetIntensity( ) + (params.audio_data->GetBeat( ) * 2.0)) * 2/*5*//*7*/ * 3.14159 / 180.0;
		obj.pitch += fMult * (params.audio_data->GetIntensity( ) + 0.1) * 4/*4*/ /*8*/ * 3.14159 / 180;
		obj.yaw += fMult * params.audio_data->GetBeat( ) * 3/*3*//*5*/ * 3.14159 / 180.0;

		obj.ambient_light_color = ColorRgb::Grey(2 * 0.15 * params.brightness * 255.0);//color = D3DRGB(0.15 * brightness, 0.15 * brightness, 0.15 * brightness);

//		FILE *file = fopen("c:\\elapsed.txt", "at");
//		fprintf(file, "elapsed: %f\n", elapsed);
//		fclose(file);
		obj.exposure_light_delta = WideColorRgb(-2, -2, 0);

		obj.Calculate(&camera, elapsed);

		camera.position = Vector3(40 * sin(a) * cos(a * 1.2), 40 * cos(a * 0.7) * sin(a * 0.9), 40 * cos(a * 1.4) * sin(0.7));
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		Texture *tx = g_pD3D->Find(TextureClass::PhasedSprite);
		obj.textures[0].Set(Actor::TextureType::Sprite, tx);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error = obj.Render();
		if(error) return TraceError(error);

		return nullptr;
	}
	bool CanRender(float fElapsed)
	{
		return fElapsed > 0.5f;//(fElapsed > 0.5f);
	}
};

EXPORT_EFFECT(Phased, EffectPhased)
