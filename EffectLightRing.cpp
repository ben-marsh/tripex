#include "Platform.h"
#include "Actor.h"
#include "Effect.h"
#include "BezierCurve.h"
#include "TexturedGrid.h"

#define SOURCES 40//40//35//50
#define FRAMES 12
#define LENGTH 1
#define BUFFERSTEP 100

//static ZObject::Transformed *pFirst;

//static double linearity = 0;

extern Texture *pBlankTexture;

//static unsigned int buffersize = 0;
//static D3DTLVERTEX *vb = NULL;

class EffectLightRing : public EffectBase
{
public:
	BezierCurve b;
	Vector3 ls[SOURCES][4];
	double position[SOURCES];
	double speed[SOURCES];
	double length[FRAMES][SOURCES];
	double er[SOURCES];
	double ehp[SOURCES];
	double ehps[SOURCES];
	bool fNotRendered;
	double accum, r, p, y;
	double brt;
	Texture *ptTint;
	Actor obj;
	Camera camera;

	Texture *tex;
	bool fSetTexture;
	Texture *tx;
	double br;

	EffectLightRing()
	{
		fSetTexture = true;
		fNotRendered = true;
		accum = 0;
		r = p = y = 0;

		camera.position.z = -50;
		b.Create(4);
		for(int i = 0; i < SOURCES; i++)
		{
			position[i] = rand() * 2.0 * 3.14159 / RAND_MAX;
			speed[i] = rand() * 20.0 * (3.14159 / 180.0) / RAND_MAX;

			er[i] = rand() * 30.0 / RAND_MAX;
			ehp[i] = rand() * 2.0 * 3.14159 / RAND_MAX;
			ehps[i] = rand() * 3.0 * (3.14159 / 180.0) / RAND_MAX;
		}

		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITE_HISTORY );

		obj.sprite_history_length = 4.0f;
		obj.frame_history = 12.0f;
		obj.sprite_size = 2.0f;//2.0f;
		obj.exposure_light_delta = WideColorRgb(-2, -2, -2);//10, -10, -10);
	
		obj.vertices.SetLength(SOURCES);
	}
	Error* Calculate(const CalculateParams& params) override
	{
		brt = params.brightness;
		accum += params.elapsed * 3.0;
		obj.ambient_light_color = ColorRgb::Grey(params.brightness * 255.0);
		for(; accum > 1.0f; accum--)
		{
			float elapsed = 1.0f;
			for(int i = 0; i < SOURCES; i++)
			{
				position[i] += 0.25 * speed[i] * params.audio_data->GetIntensity( ) * elapsed;
				ehp[i] += 0.25 * ehps[i] * (0.4 + 2 * (params.audio_data->GetIntensity( ) + params.audio_data->GetBeat( ))) * elapsed;

				double r = 2.0 * er[i] * cos(ehp[i]);

				obj.vertices[i].position.x = (46.0 + (r * sin(ehp[i]))) * sin(position[i]);
				obj.vertices[i].position.z = (46.0 + (r * cos(ehp[i]))) * cos(position[i]);
				obj.vertices[i].position.y = er[i] * sin(ehp[i]);	
			}
	
			static double a2 = 0;
			a2 += elapsed * (params.audio_data->GetIntensity( ) + params.audio_data->GetBeat( )) * 3.14159 / 180.0;

			obj.roll += elapsed * 0.25 * params.audio_data->GetIntensity( ) * 4.0 * 3.14159 / 180.0;
			obj.pitch += elapsed * 0.25 * params.audio_data->GetIntensity( ) * 3.0 * 3.14159 / 180.0;
			obj.yaw += elapsed * 0.25 * 2.0 * 3.14159 / 180.0;

			obj.textures[0].Set(Actor::TextureType::Sprite, tx);
			obj.Calculate(&camera, elapsed);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		tx = params.texture_library.Find(TextureClass::LightRingSprite);
		ptTint = params.texture_library.Find(TextureClass::LightRingBackground);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;

		error = obj.Render( );
		if(error) return TraceError(error);	

		if(ptTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, ptTint);

			g_pD3D->ResetRenderState();
			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_pD3D->DrawSprite(Point<int>(0, 0), Rect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
		}
		return nullptr;
	}
	bool CanRender(double dElapsed)
	{
		return (dElapsed > 0.1);//1.0);
	}
};
EXPORT_EFFECT( LightRing, EffectLightRing )
