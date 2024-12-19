#include "StdAfx.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "effect.h"
#include "error.h"
#include "general.h"

#define NMPOINTS 30//50//30
#define LENGTH 50//40
#define STEPSIZE 4//1
// 60 3

extern Texture *pBlankTexture;

class EffectLightTentacles : public EffectBase
{
public:
	Actor obj;
	Camera camera;
//static double p[3], v[3], a[3];
	BezierCurve b;
	Vector3 pvPoint[2][4];//, py[2][4], pz[2][4];
	double brt;
	Texture *ptTint;

	EffectLightTentacles() : b(4)
	{
		Actor sphere;
		sphere.CreateGeosphere(1.0, NMPOINTS);

		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.m_bsFlag.set( Actor::F_DO_POSITION_DELAY );
		obj.m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
//		obj.Set(ZObject::ValidVertexColours);
		obj.m_bsFlag.set( Actor::F_VALID_VERTEX_DIFFUSE );
//		obj.pVertex.SetFormat(D3DFVF_DIFFUSE);
		obj.pVertex.SetLength(LENGTH * NMPOINTS);
		obj.fSpriteSize = 5;//6;
		obj.nExposure = 1;
		obj.fFrameHistory = 3.0f;

		ColorRgb cEdge = ColorRgb(255, 225, 200);//222, 211);

		for(int i = 0; i < NMPOINTS; i++)
		{
			for(int j = 0; j < LENGTH; j++)
			{
				obj.pVertex[(i * LENGTH) + j].m_vPos = sphere.pVertex[i].m_vPos * (50 + (j * STEPSIZE));
				double dBr = (double(j) / LENGTH);
				dBr = 1.0 - (dBr * dBr);
				obj.pVertex[(i * LENGTH) + j].m_cDiffuse = ColorRgb::Blend(ColorRgb::White(), cEdge, (float)j / LENGTH) * dBr;//Grey(255.0 * dBr);
			}
		}
		obj.fFrameHistory = 11;
		obj.fDelayHistory = 10.0f;
		obj.FindDelayValues();
		camera.m_vPosition.m_fZ = -120;
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		brt = brightness;

		static double pos = 2;
		pos += 0.02 * pAudio->GetIntensity( );

		if(pAudio->GetIntensity( ) > 0.5)
		{
			obj.fRoll += ((pAudio->GetIntensity( ) - 0.5) / 0.5) * elapsed * 2.0 * 3 * 3.14159 / 180.0;
		}
		if(pAudio->GetIntensity( ) > 0.3)
		{
			obj.fPitch += ((pAudio->GetIntensity( ) - 0.3) / 0.7) * elapsed * 1.7 * 1.5 * 6.0 * 3.14159 / 180.0;
		}
		obj.fYaw += pAudio->GetIntensity( ) * elapsed * 4.0 * 3.14159 / 180.0;
		obj.wcAmbientLight = ColorRgb::Grey(brightness * 255.0);
//		obj.cAmbientLight = ZWideColour(255, 255, 255);
		obj.Calculate(&camera, elapsed);

		while(pos > 1)
		{
			for(int i = 0; i < 2; i++)
			{
				pvPoint[i][0] = pvPoint[i][3];
				pvPoint[i][1] = pvPoint[i][3] + (pvPoint[i][3] - pvPoint[i][2]);

				double r = (i == 0)? 300 : 50;

				pvPoint[i][2].m_fX = ((double(rand()) / RAND_MAX) - 0.5) * r;
				pvPoint[i][2].m_fY = ((double(rand()) / RAND_MAX) - 0.5) * r;
				pvPoint[i][2].m_fZ = ((double(rand()) / RAND_MAX) - 0.5) * r;
	
				pvPoint[i][3].m_fX = ((double(rand()) / RAND_MAX) - 0.5) * r;
				pvPoint[i][3].m_fY = ((double(rand()) / RAND_MAX) - 0.5) * r;
				pvPoint[i][3].m_fZ = ((double(rand()) / RAND_MAX) - 0.5) * r;
			}
			pos--;
		}
		for(int i = 0; i < 4; i++) b[i] = pvPoint[0][i];

		camera.m_vPosition = b.Calculate(pos);//.moveTo(x, y, z);
		for(int i = 0; i < 4; i++) b[i] = pvPoint[1][i];//->setPoint(i, px[1][i], py[1][i], pz[1][i]);
		Vector3 v = b.Calculate(pos);//, &x, &y, &z);
		camera.m_fPitch = (v - camera.m_vPosition).GetPitch();
		camera.m_fYaw = (v - camera.m_vPosition).GetYaw();
		return nullptr;
	}
	Error* Render( ) override
	{
		Error* error = obj.Render( );
		if(error) return TraceError(error);

		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		obj.pTexture[0].m_nType = Actor::TextureEntry::T_SPRITE;
		obj.pTexture[0].m_pTexture = g_pD3D->Find(TC_LBLIGHTTENTACLES);
		ptTint = g_pD3D->Find(TC_WTLIGHTTENTACLES);
		return nullptr;
	}
};

EXPORT_EFFECT(LightTentacles, EffectLightTentacles)
