#include "StdAfx.h"
#include "ZObject.h"
#include "effect.h"
#include "error.h"

#define DEPTH 8
#define MAG 20

#define FRAMEFAC 50

#define AVERAGES 10

class ZEffectPhased : public ZEffectBase
{
public:
	ZObject obj;
	ZCamera camera;

	double accum;
	bool fFirstCalc;
	int lastbeat;
	double a;

	ZEffectPhased()
	{
		a = 0;
		lastbeat = 0;
		fFirstCalc = false;
		accum = 0;

		obj.pVertex.SetLength(DEPTH*DEPTH*DEPTH);
		obj.fFrameHistory = 5.0f;
		obj.m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( ZObject::F_DRAW_VERTEX_SPRITES );
//		obj.Set(ZObject::DoRotationHistory);
//		obj.Set(ZObject::DoFrameHistory);// = (1 << 24L),

		obj.nExposure = 6;//3;//4;//6;
//		obj.wcAmbientLight = ZColour::Grey(64);

		int index = 0;
		for(int x = 0; x < DEPTH; x++)
		{
			for(int y = 0; y < DEPTH; y++)
			{
				for(int z = 0; z < DEPTH; z++)
				{
					obj.pVertex[index].m_vPos.m_fX = (x - (DEPTH/2)) * MAG;
					obj.pVertex[index].m_vPos.m_fY = (y - (DEPTH/2)) * MAG;
					obj.pVertex[index].m_vPos.m_fZ = (z - (DEPTH/2)) * MAG;
					index++;
				}
			}
		}
	}
	HRESULT Calculate(float brightness, float elapsed)
	{
		elapsed *= 1.5;

		float fMult = elapsed;//10.0 / FRAMEFAC;

		a += fMult * (g_pAudio->GetIntensity( ) + 0.1) * 3.14159 / 180.0;
		obj.fRoll += fMult * (g_pAudio->GetIntensity( ) + (g_pAudio->GetBeat( ) * 2.0)) * 2/*5*//*7*/ * 3.14159 / 180.0;
		obj.fPitch += fMult * (g_pAudio->GetIntensity( ) + 0.1) * 4/*4*/ /*8*/ * 3.14159 / 180;
		obj.fYaw += fMult * g_pAudio->GetBeat( ) * 3/*3*//*5*/ * 3.14159 / 180.0;

		obj.wcAmbientLight = ZColour::Grey(2 * 0.15 * brightness * 255.0);//color = D3DRGB(0.15 * brightness, 0.15 * brightness, 0.15 * brightness);

//		FILE *file = fopen("c:\\elapsed.txt", "at");
//		fprintf(file, "elapsed: %f\n", elapsed);
//		fclose(file);
		obj.wcExposureLightChange = ZWideColour(-2, -2, 0);

		obj.Calculate(&camera, elapsed);

		camera.m_vPosition = ZVector(40 * sin(a) * cos(a * 1.2), 40 * cos(a * 0.7) * sin(a * 0.9), 40 * cos(a * 1.4) * sin(0.7));
		return S_OK;
	}
	HRESULT Reconfigure()
	{
		ZTexture *tx = g_pD3D->Find(TC_LBPHASED);
		obj.pTexture[0].Set(ZObject::Texture::T_SPRITE, tx);
		return S_OK;
	}
	HRESULT Render()
	{
		HRESULT hRes;
		hRes = obj.Render();
		if(FAILED(hRes)) return TraceError(hRes);
		return S_OK;
	}
	bool CanRender(float fElapsed)
	{
		return fElapsed > 0.5f;//(fElapsed > 0.5f);
	}
};

EXPORT_EFFECT(Phased, ZEffectPhased)
