#include "Platform.h"
#include "effect.h"
#include <conio.h>
#include "Actor.h"
#include "error.h"
#include <algorithm>

#define SOURCES 512

//static ZObject *pObj;

//double dRadAng = 2 * 3.14159 * 2.0;
//double dViewAng = 0;
//int nSpikes = 8;
//Texture *pTint;

//#define alNumLS 512
#define SPREAD 50
#define RADIUS 160
#define SPEEDSPREAD 20
#define FOREGROUNDBR 0.5

extern Texture *pBlankTexture;

class EffectCollapsingLightSphere : public EffectBase
{
public:
	double pdAng[SOURCES];
	double pdYPos[SOURCES];
	double pdTilt[SOURCES];
	double pdRadius[SOURCES];
	double pdSpeed[SOURCES];
	Texture *pTint;
	double dWaitTime;
	double dTilt;
	Actor obj;
	Camera camera;

	// 0 - contracted
	// 1 - moving out
	// 2 - out
	// 3 - moving in
	int nStage;
	double brt;

	EffectCollapsingLightSphere()
	{
		dWaitTime = 0;
		dTilt = 0;

		nStage = 0;
		obj.vertices.SetLength(SOURCES);
	//	pObj->Create(SOURCES, 1);
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.exposure_light_delta = WideColorRgb(-60, -60, -60);
		obj.exposure = 3;
		obj.sprite_size = 7.0;
		obj.frame_history = 2.0;
		camera.position.z = -240;

		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] = rand() * PI2 / RAND_MAX;
			pdSpeed[i] = SPEEDSPREAD * (rand() * (SPEEDSPREAD / 2.0) * g_fDegToRad / RAND_MAX);
			pdTilt[i] = rand() * PI2 / RAND_MAX;
			pdRadius[i] = rand() * RADIUS / RAND_MAX;
			pdYPos[i] = (SPREAD / 2.0) - (double(rand()) * SPREAD / RAND_MAX);
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		brt = params.brightness;
		if(nStage == 0) dWaitTime += params.elapsed * params.audio_data.GetDampenedBand( pEffectPtr->fSensitivity, 0.0f, 1.0f);
		else if(nStage == 1) dTilt += params.elapsed * 1.5 * g_fDegToRad;
		else if(nStage == 2) dWaitTime += params.elapsed * params.audio_data.GetDampenedBand(pEffectPtr->fSensitivity, 0.0f, 1.0f);
		else if(nStage == 3) dTilt -= params.elapsed * 1.5 * g_fDegToRad;

		if((dTilt < 0) || (dTilt > 3.141592 / 2.0) || (dWaitTime > 40.0 && params.audio_data.GetIntensity( ) > 0.6))
		{
			nStage = (nStage + 1) & 3;
			dWaitTime = 0.0;
			dTilt = std::min(std::max(dTilt, 0.0), 3.14159 / 2.0);
		}

		float fTwistMult = sin(dTilt);// * 3.14159 / 128.0);
		float multp = (/*fac*/dTilt * params.audio_data.GetIntensity( )) + (1 - dTilt/*fac*/) + 0.1;

		obj.vertices.SetLength(SOURCES);
		for(int i = 0; i < SOURCES; i++)
		{
			float x = cos(pdAng[i]) * pdRadius[i];
			float y = pdYPos[i];
			float z = -100 + (sin(pdAng[i]) * pdRadius[i]);

			float sin_t = sin(pdTilt[i] * fTwistMult);
			float cos_t = cos(pdTilt[i] * fTwistMult);

			obj.vertices[i].position.x = (y * cos_t) + (x * sin_t);
			obj.vertices[i].position.y = (y * sin_t) + (x * cos_t);
			obj.vertices[i].position.z = z;

			pdAng[i] += params.elapsed * multp * (params.audio_data.GetIntensity( ) + 0.1) * pdSpeed[i] * 3.14159 / 180.0;
			pdTilt[i] += (params.audio_data.GetIntensity( ) + 0.1) * params.elapsed * multp * 1 * 3.14159 / 180.0;

			float fTransMult = FOREGROUNDBR + ((z / -100.0) * (1 - FOREGROUNDBR));
			float fBr = params.brightness * fTransMult / 2.0;

			obj.vertices[i].diffuse = ColorRgb::Grey(0.6 * fBr * 255.0);
	//		alObject[i]->alpha = 128*brightness*transMult;
	//		fvVertex++;
		}
		obj.flags.set( Actor::F_VALID_VERTEX_DIFFUSE );
		obj.Calculate(&camera, params.elapsed);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;

		error = obj.Render( );
		if (error) return TraceError(error);

		if(pTint != pBlankTexture)
		{
			RenderState render_state;
			render_state.src_blend = D3DBLEND_DESTCOLOR;
			render_state.dst_blend = D3DBLEND_ONE;
			render_state.enable_zbuffer = false;
			render_state.texture_stages[0].texture = pTint;

			g_pD3D->DrawSprite(render_state, Point<int>(0, 0), Rect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		obj.textures[0].Set(Actor::TextureType::Sprite, params.texture_library.Find(TextureClass::CollapsingSphereSprite));
		pTint = params.texture_library.Find(TextureClass::CollapsingSphereBackground);
		return nullptr;
	}
};
EXPORT_EFFECT( CollapsingLightSphere, EffectCollapsingLightSphere )

