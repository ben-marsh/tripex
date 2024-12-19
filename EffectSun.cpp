#include "Platform.h"
#include "Effect.h"
#include "error.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "effect.h"

#define RINGS 5//3
#define EXPOSURE 3//5//7//9//15//4//3//7//9
#define EXPDIV 5//4.5//5//7
#define EXPFRAMES 2//4//10//6//3

#define TRI 10
//#define PI 3.141592
#define PI2 (PI * 2)
#define LAYERS 3
#define INNER 30
#define OUTER (INNER + 30)
#define ANGSZ (20.0f * g_fDegToRad)

class EffectSun : public EffectBase
{
public:
	Actor pObj[RINGS];
	Camera cCamera;
	float as[RINGS][3][2], ps[RINGS][2], ys[RINGS][2];
	float sa;
	ContainedBezierCurve<2> bez, bez2;
	float fBezPos;
	float fBezPos2;

	EffectSun() : 
		bez(Vector3(4 * 20, 4 * 20, 70), Vector3(4 * -20,4 * -20,-60)), 
		bez2(Vector3(10, 10, 10), Vector3(-10,-10,-20))
	{
		sa = 0;
		fBezPos = 4;
		fBezPos2 = 4;
		// set lastframe to some constant thing
/*		obj = new Object();
		obj->create(TRI * 2 * LAYERS * 2, TRI * LAYERS * 2);
		o2 = new Object();
		o2->create(TRI * 2 * LAYERS * 2, TRI * LAYERS * 2);
*/
		cCamera.m_vPosition = Vector3(0, 0, -70);//-70);
//		scene->property(worldCull, false);
	
		for(int o = 0; o < RINGS; o++)
		{
//			obj[o] = new Object();
//			obj[o]->Create((TRI * 2 * ((LAYERS * 2) + 1)) + 1, TRI * 2 * ((LAYERS * 2) + 1));
//			obj[o]->Create(TRI * 2 * ((LAYERS * 2) + 1), TRI * ((LAYERS * 2) + 1));
//			obj[o]->Create((LAYERS*2+1)*TRI*2, TRI * 2 * (LAYERS*2+1));
	//		pObj[o].pVertex.SetFormat(D3DFVF_DIFFUSE);
			pObj[o].pVertex.SetLength((LAYERS * 2 + 1) * TRI * 2);
			pObj[o].pFace.SetLength(TRI * 2 * (LAYERS * 2 + 1));
//			obj[o]->x = 400.0 * cos(o * 2.0 * 3.14159 / RINGS);
//			obj[o]->y = 400.0 * sin(o * 2.0 * 3.14159 / RINGS);
//				(TRI * 2 * ((LAYERS * 2) + 1)) + 1, TRI * 2 * ((LAYERS * 2) + 1));

			int base = 0;
			for(int a = -LAYERS; a <= LAYERS; a++)
			{
				float ang = 10 * PI * a / (180 * LAYERS);
				for(int i = 0; i < TRI; i++)
				{
					pObj[o].pVertex[base + i].m_vPos.m_fX = sin(i * PI2 / TRI) * INNER * cos(ang);
					pObj[o].pVertex[base + i].m_vPos.m_fY = cos(i * PI2 / TRI) * INNER * cos(ang);
					pObj[o].pVertex[base + i].m_vPos.m_fZ = INNER * sin(ang);
//					pObj[o].pVertex[base + i].GetDiffuse() = ZColour(128.0 / EXPDIV, 77.0 / EXPDIV, 243.0 / EXPDIV); //0.6 / 25, 0.5 / 25);

					pObj[o].pVertex[base + i + TRI].m_vPos.m_fX = sin((i * PI2 / TRI) + (PI / TRI)) * OUTER * cos(ang);
					pObj[o].pVertex[base + i + TRI].m_vPos.m_fY = cos((i * PI2 / TRI) + (PI / TRI)) * OUTER * cos(ang);
					pObj[o].pVertex[base + i + TRI].m_vPos.m_fZ = OUTER * sin(ang);
//					pObj[o].pVertex[base + i + TRI].GetDiffuse() = ZColour(0, 64.0 / EXPDIV, 0);

					pObj[o].pFace[base + i][0] = base + i;
					pObj[o].pFace[base + i][1] = base + ((i + 1) % TRI);
					pObj[o].pFace[base + i][2] = base + i + TRI;

					pObj[o].pFace[base + i + TRI][0] = base + i;
					pObj[o].pFace[base + i + TRI][2] = base + ((i + 1) % TRI);
					pObj[o].pFace[base + i + TRI][1] = base + i + TRI;
				}
				base += TRI * 2;
			}

			pObj[o].m_bsFlag.set(Actor::F_DRAW_TRANSPARENT);
			pObj[o].m_bsFlag.set(Actor::F_DRAW_Z_BUFFER, false);
			pObj[o].nExposure = EXPOSURE;
			pObj[o].fFrameHistory = EXPFRAMES;

		
		//		obj[o]->property(objTransparent | /*objDecal | */objVertexColoured, true);
//			obj[o]->fExposure(EXPOSURE, EXPFRAMES);//EXPOSURE, 1);
//			obj[o]->property(/*objClip2D | */objZBuffer, false);
		
//			scene->vpObject.Add(obj[o]);
		}
		for(int i = 0; i < RINGS; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				as[i][j][0] = 1.0;
				as[i][j][1] = 1000.0;
			}
		}
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		fBezPos += elapsed * pAudio->GetIntensity( ) * 0.01f;
		fBezPos2 += elapsed * pAudio->GetIntensity( ) * 0.02f;
		cCamera.m_vPosition = Vector3(0, 0, -70);//bez.Calculate(fBezPos);
		cCamera.SetTarget(bez2.Calculate(fBezPos2));

		static double accum = 0;
		accum += elapsed * 1.5;
		static float fMove = 21;
		fMove += elapsed;
		if(fMove > 20)
		{
			for(int i = 0; i < RINGS; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					as[i][j][0] = (float)_copysign(1.0f,as[i][j][0]) * (4.0f + (rand() * 4.0f / RAND_MAX)) * g_fDegToRad;
					if(rand() > RAND_MAX * 0.75f) as[i][j][0] = -as[i][j][0];
//					as[i][j][1] = 0.0;
				}
			}
			fMove -= 20;
		}
	
		while(accum > 1.0)
		{
			for(int i = 0; i < RINGS; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					as[i][j][1] += as[i][j][0];
				}
 
				if((i % 3) == 0)
				{
					pObj[i].fRoll += 0.1f * as[i][0][0];
					pObj[i].fPitch += pAudio->GetDampenedBand( pEffectPtr->fSensitivity, 3.0f/16.0f, 4.0f/16.0f) * as[i][1][0];
					pObj[i].fYaw += pAudio->GetDampenedBand( pEffectPtr->fSensitivity, 4.0f/16.0f, 5.0f/16.0f) * as[i][2][0];
					pObj[i].fPitch = Bound<float>(pObj[i].fPitch, -ANGSZ, ANGSZ);
					pObj[i].fYaw = Bound<float>(pObj[i].fYaw, -ANGSZ, ANGSZ);
				}
				if((i % 3) == 1)
				{
					pObj[i].fRoll += pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 2.0f/16.0f, 3.0f/16.0f) * as[i][0][0];
					pObj[i].fPitch += 0.1f * as[i][1][0];
					pObj[i].fYaw += 1.5f * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 2.0f/16.0f, 4.0f/16.0f) * as[i][2][0];
				}
				if((i % 3) == 2) 
				{
					pObj[i].fRoll += pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 1/16.0f, 2/16.0f) * as[i][0][0];
					pObj[i].fPitch += pAudio->GetIntensity( ) * as[i][1][0];
					pObj[i].fYaw += 0.1f * as[i][2][0];
				}
	//for(int o = 0; o < RINGS; o++)
	//{
				int base = 0;
				for(int a = -LAYERS; a <= LAYERS; a++)
				{
					for(int j = 0; j < TRI; j++)
					{
						pObj[i].pVertex[base + j].m_cDiffuse = ColorRgb((UINT8)(brightness * 128.0f / EXPDIV), (UINT8)(brightness * 77.0f / EXPDIV), (UINT8)(brightness * 243.0f / EXPDIV)); //0.6 / 25, 0.5 / 25);
						pObj[i].pVertex[base + j + TRI].m_cDiffuse = ColorRgb(0, (UINT8)(brightness * 64.0f / EXPDIV), 0);
					}
					base += 2 * TRI;
				}
				pObj[i].m_bsFlag.set(Actor::F_VALID_VERTEX_DIFFUSE);
	//}
//			obj[i]->ExposureCapture();
				pObj[i].Calculate(&cCamera, 1.0);
			}
			accum--;
		}
		return nullptr;
	}
	Error* Render()
	{
		Error* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render();
			if(error) return TraceError(error);
		}
		return nullptr;
	//	return scene->render(d3d);
	}
};
EXPORT_EFFECT(Sun, EffectSun)
