#include "StdAfx.h"
#include "effect.h"
#include "ZObject.h"
#include "ZBezier.h"
#include "error.h"

#define HILLSZ 0.1
#define LANDSCAPEITER 2000

#define NOBJ 4

class ZEffectMorphingSphere : public ZEffectBase
{
public:
	ZObject pObj[NOBJ + 1];
	ZCamera camera;
	ZArray<float> pfAng;
	ZContainedBezier<1> b;
	float fBezPos;
	
	ZEffectMorphingSphere() : b(ZVector(-100, -100, -600), ZVector(100, 100, -200))
	{
		fBezPos = 0;
		pObj[NOBJ].CreateTetrahedronGeosphere(1.0, 4);
		pObj[NOBJ].FindFaceOrder(ZVector::Origin());
		pfAng.SetLength(pObj[NOBJ].pVertex.GetLength());
		for(int i = 0; i < pObj[NOBJ].pVertex.GetLength(); i++)
		{
			pfAng[i] = pObj[NOBJ].pVertex[i].m_vPos.m_fZ * PI * 2;
		}
		for(i = 0; i < LANDSCAPEITER; i++)
		{
			ZVector v;
			v.m_fX = 0.5 - (float(rand()) / RAND_MAX);
			v.m_fY = 0.5 - (float(rand()) / RAND_MAX);
			v.m_fZ = 0.5 - (float(rand()) / RAND_MAX);
			v.Normalize();

			for(int j = 0; j < pObj[NOBJ].pVertex.GetLength(); j++)
			{
				if(pObj[NOBJ].pVertex[j].m_vPos.Dot(v) < 0)
				{
					pfAng[j] += 2.0f * g_fDegToRad;
				}
				else 
				{
					pfAng[j] -= 2.0f * g_fDegToRad;
				}
			}
		}

		for(i = 0; i < NOBJ; i++)
		{
			pObj[i].pFace = pObj[NOBJ].pFace;
			pObj[i].pVertex.SetLength(pObj[NOBJ].pVertex.GetLength());
			pObj[i].m_bsFlag.set( ZObject::F_DRAW_TRANSPARENT );
			pObj[i].m_bsFlag.set( ZObject::F_NO_CULL );
		}

		camera.m_vPosition.m_fZ = -320;
	}
	HRESULT Calculate(float br, float elapsed)
	{
		fBezPos += elapsed * g_pAudio->GetIntensity( ) * 0.04;
		camera.m_vPosition = b.Calculate(fBezPos);
//	float fPos = 0;
//	fPos += elapsed * 3.14159 / 180.0;
//	camera.vPosition = ZVector(0, sin(fPos), cos(fPos)) * 200.0f;
//	camera.PointAt(ZVector::Origin());

		for(int i = 0; i < pObj[NOBJ].pVertex.GetLength(); i++)
		{
			pfAng[i] += g_pAudio->GetIntensity( ) * elapsed * 8.0 * 3.14159 / 180.0;
		}
		for(i = 0; i < NOBJ; i++)
		{
			float fOfs = i * 60.0 * 3.14159 / 180.0;
			float fMult = 1.0 + (i * 1.0 / NOBJ);
			for(int j = 0; j < pObj[i].pVertex.GetLength(); j++)
			{
				pObj[i].pVertex[j].m_vPos = pObj[NOBJ].pVertex[j].m_vPos * 200.0f * fMult * (1.0f + ((float)HILLSZ * (float)sin(pfAng[j] + fOfs)));
			}
			pObj[i].fPitch += elapsed * 2.0 * PI / 180.0;
			pObj[i].fYaw += elapsed * 2.0 * PI / 180.0;
			pObj[i].m_bsFlag.set( ZObject::F_VALID_VERTEX_NORMALS, false );
			float fBlend = float(i) / NOBJ;
			float fMult2 = (i == 0)? 1.0f : min(1.0, g_pAudio->GetIntensity( ) * 1.5);//(averagefloat(i) / NOBJ);
			pObj[i].wcAmbientLight = br * (ZColour::Grey(96 * fMult2 * (1 - (0.4 * float(i) / NOBJ))) - ZColour(fBlend * 40.0, fBlend * 40.0, fBlend * 5.0));
			pObj[i].Calculate(&camera, elapsed);
		}
		return S_OK;
	}
	HRESULT Reconfigure( )
	{
		ZTexture *tx = g_pD3D->Find(TC_EMMORPHINGSPHERE);
		for(int i = 0; i < NOBJ; i++)
		{
			pObj[i].pTexture[0].Set(ZObject::Texture::T_ENVMAP, tx);
		}
		return S_OK;
	}
	HRESULT Render( )
	{
		HRESULT hRes;

		for(int i = 0; i < NOBJ; i++)
		{
			hRes = pObj[i].Render( ); 
			if(FAILED(hRes)) return TraceError(hRes);
		}
		return S_OK;
	}
};
EXPORT_EFFECT(MorphingSphere, ZEffectMorphingSphere)
