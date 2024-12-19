#include "Platform.h"
#include "BezierCurve.h"
#include "Actor.h"
#include "error.h"
#include "effect.h"

//#define SOURCES 65//180//65//80
#define FRAMES 10
#define LENGTH 1
#define BUFFERSTEP 100

extern bool bMeshHQ;

extern Texture *pBlankTexture;
//extern EFFECT lightsphere;

class EffectLightSphere : public EffectBase
{
public:
	Actor obj;
	Camera camera;

	Texture *tex;
	Texture *tx;

	ZArray<Vector3> pvPosition[4];
	ZArray<double> position;
	ZArray<double> speed;
	ZArray<double> length[FRAMES];

	bool fNotRendered;
	int nSources;

	double br;
	double brt;

	BezierCurve b;

	double accum;
	double r, p, y;
	double dBrBack;
	Texture *pTint;

	EffectLightSphere() : b(4)
	{
		accum = 1.1;
		r = p = y = 0;
		fNotRendered = true;
		if(bMeshHQ) nSources = 100;
		else nSources = 60;

		for(int i = 0; i < 4; i++) pvPosition[i].SetLength(nSources);
		for(int i = 0; i < FRAMES; i++) length[i].SetLength(nSources);
		position.SetLength(nSources);
		speed.SetLength(nSources);

		obj.vPosition.m_fZ = 50;
		obj.pVertex.SetLength(nSources);
		obj.m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITE_HISTORY );
		obj.fFrameHistory = 10.0;
		obj.fSpriteSize = 2.0f;
		obj.fSpriteHistoryLength = 1.0f;
		obj.nMaxHistoryLength = 25;
		obj.wcExposureLightChange = WideColorRgb(-2, -2, -2);
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		brt = brightness;
		accum += elapsed * 2.0f;
		for(; accum > 1.0f; accum--)
		{
			float elapsed = 1.0f;

			br = brightness;
			for(int i = 0; i < nSources; i++)
			{
				position[i] += speed[i] * pAudio->GetIntensity( ) * 0.3 * elapsed;
				double linearity = std::max(0.0f, 1 - pAudio->GetBeat( ) );//);//(bigbeat / 2.0));
			
				while(position[i] > 1.0 || position[i] < 0.0)
				{
					pvPosition[0][i] = pvPosition[3][i];
					pvPosition[1][i] = pvPosition[3][i] + (pvPosition[3][i] - pvPosition[2][i]);

					pvPosition[3][i].m_fX = (rand() * 70.0 / RAND_MAX) - 35.0;
					pvPosition[3][i].m_fY = (rand() * 70.0 / RAND_MAX) - 35.0;
					pvPosition[3][i].m_fZ = (rand() * 70.0 / RAND_MAX) - 35.0;

					float c = pAudio->GetRandomSample( );
					pvPosition[2][i].m_fX = (pvPosition[3][i].m_fX * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);
					pvPosition[2][i].m_fY = (pvPosition[3][i].m_fY * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);
					pvPosition[2][i].m_fZ = (pvPosition[3][i].m_fZ * (1 - c)) + ((c * rand() * 70.0 / RAND_MAX) - 35.0);

					pvPosition[3][i].m_fY = (pvPosition[0][i].m_fY * (1 - linearity)) + (pvPosition[3][i].m_fY * linearity);
					pvPosition[2][i].m_fY = (pvPosition[1][i].m_fY * (1 - linearity)) + (pvPosition[2][i].m_fY * linearity);

					if(position[i] > 1.0) position[i] -= 1.0;
					if(position[i] < 0.0) position[i] += 1.0;
					speed[i] = (pAudio->GetRandomSample( ) * 0.15) + 0.01;
				}

				for(int j = 0; j < 4; j++)
				{
					b[j] = pvPosition[j][i];
				}
				obj.pVertex[i].m_vPos = b.Calculate(position[i]);
			}

			obj.fRoll += elapsed * pAudio->GetIntensity( ) * 4.0 * 3.14159 / 180.0;
			obj.fPitch += elapsed * pAudio->GetIntensity( ) * 3.0 * 3.14159 / 180.0;
			obj.fYaw += elapsed * 2.0 * 3.14159 / 180.0;
			obj.Calculate(&camera, 1.0);
			obj.wcAmbientLight = ColorRgb::Grey(64.0 * brightness);
		}
		return nullptr;
	}
	virtual Error* Reconfigure(AudioData* pAudio) override
	{
		dBrBack = 1;
		if(fNotRendered)// || (rand() <= (RAND_MAX * 0.3)))
		{
			for(int i = 0; i < nSources; i++)
			{
				position[i] = 1;
				speed[i] = rand() * 0.02 / RAND_MAX;	

				pvPosition[3][i].m_fX = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[3][i].m_fY = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[3][i].m_fZ = (rand() * 50.0 / RAND_MAX) - 25.0;

				pvPosition[2][i].m_fX = (rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[2][i].m_fY = pvPosition[3][i].m_fY;//(rand() * 50.0 / RAND_MAX) - 25.0;
				pvPosition[2][i].m_fZ = (rand() * 50.0 / RAND_MAX) - 25.0;
			}
			fNotRendered = false;
		}

		obj.pTexture[0].Set(Actor::TextureEntry::T_SPRITE, g_pD3D->Find(TC_LBLIGHTSPHERE));

		pTint = g_pD3D->Find(TC_WTLIGHTSPHERE);
		return nullptr;
	}
	Error* Render( ) override
	{
		Error* error = obj.Render( );
		if(error) return TraceError(error);

		if(pTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, pTint);
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
		return (dElapsed > 0.2);
	}
};
EXPORT_EFFECT( LightSphere, EffectLightSphere )

