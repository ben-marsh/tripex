#include "StdAfx.h"
#include "effect.h"
#include "ZObject.h"
#include "ZBezier.h"
#include "error.h"

#define PIPE_RADIUS 10.0
#define PIPE_CIRCUMPOINTS 6
#define PIPE_TWISTS 3
#define PIPE_LENGTH 50
#define PIPE_TWISTRADIUS 30.0
#define PIPES 3
#define CIRCLE_RADIUS 60.0

static const int nVertices = PIPE_LENGTH * PIPE_CIRCUMPOINTS;

#define UPDATETIME 60
#define ANG (5.0 * 3.14159 / 180.0)

class ZEffectMotionBlur2 : public ZEffectBase
{
public:
	ZObject pObj[PIPES];
	ZCamera camera;//World *pScene;
	ZContainedBezier<1> b;
	float fBezPos;
	float fTime;
	float pf[2][3], pfa[3];
	float fChange;

	ZEffectMotionBlur2() : b(ZVector(-100, -100, -20), ZVector(100, 100, 100))
	{
		for(int i = 0; i < 3; i++) pf[1][i] = 0.0f;

		fChange = 0;
		fTime = UPDATETIME + 1;
		fBezPos = 0;
		for(int p = 0; p < PIPES; p++)
		{
			pObj[p].CreateTetrahedron(100);
//		pObj[p].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[p].pVertex.SetLength(nVertices);
			pObj[p].pFace.SetLength(PIPE_LENGTH * PIPE_CIRCUMPOINTS * 2);
			pObj[p].m_bsFlag.set( ZObject::F_NO_CULL );
			pObj[p].m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
			pObj[p].fFrameHistory = 4.0f;
			pObj[p].nExposure = 6;
			pObj[p].wcExposureLightChange = ZWideColour(-10, -10, -1);

			int v = 0, f = 0;
			for(int i = 0; i < PIPE_LENGTH; i++)
			{
				double dAng = (i * 2.0 * 3.14159 / PIPE_LENGTH);
				double dTwistAng = (i * PIPE_TWISTS * 2.0 * 3.14159 / PIPE_LENGTH) + (p * 2.0 * 3.14159 / PIPES);
				for(int cp = 0; cp < PIPE_CIRCUMPOINTS; cp++)
				{
					double dCircumAng = cp * 2.0 * 3.14159 / PIPE_CIRCUMPOINTS;

					ZVector vec;
//				Vector3 vec;
					vec.m_fX = 0;//(cp - (cp / 2)) * 20;
					vec.m_fY = PIPE_TWISTRADIUS + (PIPE_RADIUS * cos(dCircumAng));
					vec.m_fZ = PIPE_TWISTRADIUS + (PIPE_RADIUS * sin(dCircumAng));
					vec *= ZMatrix::RotationX(dTwistAng);
					vec.m_fY += CIRCLE_RADIUS;
					vec *= ZMatrix::RotationZ(dAng);// = RotateVector3(vec, dAng, 0, 0);
					pObj[p].pVertex[v + cp].m_vPos = vec;

					pObj[p].pFace[f][0] = v + cp;
					pObj[p].pFace[f][1] = v + ((cp + 1) % PIPE_CIRCUMPOINTS);
					pObj[p].pFace[f][2] = (v + cp + PIPE_CIRCUMPOINTS) % nVertices;
					f++;

					pObj[p].pFace[f][0] = v + ((cp + 1) % PIPE_CIRCUMPOINTS);
					pObj[p].pFace[f][1] = (v + ((cp + 1) % PIPE_CIRCUMPOINTS) + PIPE_CIRCUMPOINTS) % nVertices;
					pObj[p].pFace[f][2] = (v + cp + PIPE_CIRCUMPOINTS) % nVertices;
					f++;
				}
				v += PIPE_CIRCUMPOINTS;
			}
			pObj[p].FindVertexNormals();
		}
		camera.m_vPosition.m_fZ = -130;
	}
	ZError* Calculate(float brightness, float elapsed, ZAudio* pAudio)
	{
		camera.m_vPosition = b.Calculate(fBezPos);
		camera.SetTarget(ZVector::Origin());
		fBezPos += 0.02 * pAudio->GetIntensity( ) * elapsed;

		fTime += elapsed;
		fChange = min(1.0f, fChange + (elapsed / 3.0));

		if(fTime > UPDATETIME)
		{
			for(int i = 0; i < 3; i++) pf[0][i] = pf[1][i];
			pf[1][0] = (rand() * ANG / RAND_MAX) - (ANG / 2.0);
			pf[1][1] = (rand() * ANG / RAND_MAX) - (ANG / 2.0);
			pf[1][2] = (ANG * 1.5) - fabs(pf[1][0]) - fabs(pf[1][1]);
			fTime -= UPDATETIME;
			fChange = 0;
		}

		for(int i = 0; i < 3; i++)
		{
			pfa[i] = (pf[0][i] * (1 - fChange)) + (pf[1][i] * fChange);
		}

		for(int i = 0; i < 3; i++)
		{
			pObj[i].wcAmbientLight = ZColour::Grey(60.0 * brightness);
			pObj[i].fRoll += pfa[0] * elapsed * (pAudio->GetIntensity( ) + 0.1);
			pObj[i].fPitch += pfa[1] * elapsed * pAudio->GetIntensity( ) ;
			pObj[i].fYaw += pfa[2] * elapsed * (pAudio->GetIntensity( ) + 0.2);
			pObj[i].Calculate(&camera, elapsed);
		}
		return nullptr;
	}
	ZError* Reconfigure(ZAudio* pAudio) override
	{
		ZTexture *pTexture = g_pD3D->Find(TC_EMMOTIONBLUR2);//2);
		for(int i = 0; i < PIPES; i++)
		{
			pObj[i].pTexture[0].m_nType = ZObject::Texture::T_LIGHTMAP;
			pObj[i].pTexture[0].m_pTexture = pTexture;
		}
		return nullptr;
	}
	ZError* Render() override
	{
		for(int i = 0; i < PIPES; i++)
		{
			ZError* error = pObj[i].Render();
			if(error) return TraceError(error);
		}
		return nullptr;
	}
};

EXPORT_EFFECT(MotionBlur2, ZEffectMotionBlur2)
