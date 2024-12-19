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

		obj.pVertex.SetLength(DEPTH*DEPTH*DEPTH);
		obj.fFrameHistory = 5.0f;
		obj.m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
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
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		elapsed *= 1.5;

		float fMult = elapsed;//10.0 / FRAMEFAC;

		a += fMult * (pAudio->GetIntensity( ) + 0.1) * 3.14159 / 180.0;
		obj.fRoll += fMult * (pAudio->GetIntensity( ) + (pAudio->GetBeat( ) * 2.0)) * 2/*5*//*7*/ * 3.14159 / 180.0;
		obj.fPitch += fMult * (pAudio->GetIntensity( ) + 0.1) * 4/*4*/ /*8*/ * 3.14159 / 180;
		obj.fYaw += fMult * pAudio->GetBeat( ) * 3/*3*//*5*/ * 3.14159 / 180.0;

		obj.wcAmbientLight = ColorRgb::Grey(2 * 0.15 * brightness * 255.0);//color = D3DRGB(0.15 * brightness, 0.15 * brightness, 0.15 * brightness);

//		FILE *file = fopen("c:\\elapsed.txt", "at");
//		fprintf(file, "elapsed: %f\n", elapsed);
//		fclose(file);
		obj.wcExposureLightChange = WideColorRgb(-2, -2, 0);

		obj.Calculate(&camera, elapsed);

		camera.m_vPosition = Vector3(40 * sin(a) * cos(a * 1.2), 40 * cos(a * 0.7) * sin(a * 0.9), 40 * cos(a * 1.4) * sin(0.7));
		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		Texture *tx = g_pD3D->Find(TC_LBPHASED);
		obj.pTexture[0].Set(Actor::TextureEntry::T_SPRITE, tx);
		return nullptr;
	}
	Error* Render() override
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
