#include "StdAfx.h"
#include "Actor.h"
#include "effect.h"
#include "BezierCurve.h"
#include "TexturedGrid.h"
#include "general.h"
#include "effect.h"

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

		camera.m_vPosition.m_fZ = -50;
		b.Create(4);
		for(int i = 0; i < SOURCES; i++)
		{
			position[i] = rand() * 2.0 * 3.14159 / RAND_MAX;
			speed[i] = rand() * 20.0 * (3.14159 / 180.0) / RAND_MAX;

			er[i] = rand() * 30.0 / RAND_MAX;
			ehp[i] = rand() * 2.0 * 3.14159 / RAND_MAX;
			ehps[i] = rand() * 3.0 * (3.14159 / 180.0) / RAND_MAX;
		}

		obj.m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITE_HISTORY );

		obj.fSpriteHistoryLength = 4.0f;
		obj.fFrameHistory = 12.0f;
		obj.fSpriteSize = 2.0f;//2.0f;
		obj.wcExposureLightChange = WideColorRgb(-2, -2, -2);//10, -10, -10);
	
		obj.pVertex.SetLength(SOURCES);
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		brt = brightness;
		accum += elapsed * 3.0;
		obj.wcAmbientLight = ColorRgb::Grey(brightness * 255.0);
		for(; accum > 1.0f; accum--)
		{
			float elapsed = 1.0f;
			for(int i = 0; i < SOURCES; i++)
			{
				position[i] += 0.25 * speed[i] * pAudio->GetIntensity( ) * elapsed;
				ehp[i] += 0.25 * ehps[i] * (0.4 + 2 * (pAudio->GetIntensity( )+ pAudio->GetBeat( ))) * elapsed;

				double r = 2.0 * er[i] * cos(ehp[i]);

				obj.pVertex[i].m_vPos.m_fX = (46.0 + (r * sin(ehp[i]))) * sin(position[i]);
				obj.pVertex[i].m_vPos.m_fZ = (46.0 + (r * cos(ehp[i]))) * cos(position[i]);
				obj.pVertex[i].m_vPos.m_fY = er[i] * sin(ehp[i]);	
			}
	
			static double a2 = 0;
			a2 += elapsed * (pAudio->GetIntensity( ) + pAudio->GetBeat( )) * 3.14159 / 180.0;

			obj.fRoll += elapsed * 0.25 * pAudio->GetIntensity( ) * 4.0 * 3.14159 / 180.0;
			obj.fPitch += elapsed * 0.25 * pAudio->GetIntensity( ) * 3.0 * 3.14159 / 180.0;
			obj.fYaw += elapsed * 0.25 * 2.0 * 3.14159 / 180.0;

			obj.pTexture[0].Set(Actor::TextureEntry::T_SPRITE, tx);
			obj.Calculate(&camera, elapsed);
		}
		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		tx = g_pD3D->Find(TC_LBLIGHTRING);
		ptTint = g_pD3D->Find(TC_WTLIGHTRING);
		return nullptr;
	}
	Error* Render( ) override
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
			g_pD3D->DrawSprite(ZPoint<int>(0, 0), ZRect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
		}
		return nullptr;
	}
	bool CanRender(double dElapsed)
	{
		return (dElapsed > 0.1);//1.0);
	}
};
EXPORT_EFFECT( LightRing, EffectLightRing )
