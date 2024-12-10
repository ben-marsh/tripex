#include "StdAfx.h"
#include "ZObject.h"
#include "error.h"
#include "effect.h"

#define accelFac 0.9

#define iPoints 10
#define MINHEIGHT 0.0f
#define BEHEIGHT  50.0f //30, 50, 80 are nice
#define MAXHEIGHT 80.0f

#define MINSZ -0.8f
#define MAXSZ 0.4f
#define AVSZ 1.0f

#define DAMPCHANGE 0.001

class ZEffectWaterGlobe : public ZEffectBase
{
	class ZGeoEdge : public ZEdge
	{
	public:
		int nSplit;
	};
	class WaterObj : public ZObject
	{
	public:
		int nPos;
		float fDamping;
		ZArray<ZVector> pvDir;
		ZArray<float> pfPos, pfVel;
		ZArray<WORD*> ppwAdjacent;
		float fSize;
		float fAngle;
		float fAverage;

		WaterObj() : ZObject()
		{
		}
		void Start()
		{
			nPos = 0;
//		pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			CreateTetrahedronGeosphere(200, 5);

//		CreateGeosphere(200, 2400);//1600);//800);
//		FindFaceEdges();
			FindVertexFaceList();

			ppwAdjacent.SetLength(pVertex.GetLength());
			for(int i = 0; i < pVertex.GetLength(); i++)
			{
				ZArray<WORD> pwIndex;
				for(int j = 0; ppwVertexFaceList[i][j] != WORD_INVALID_INDEX; j++)
				{
					ZFace &f = pFace[ppwVertexFaceList[i][j]];
					for(int k = 0; k < 3; k++)
					{
						if(f[k] != i && pwIndex.IndexOf(f[k]) == -1)
						{
							pwIndex.Add(f[k]);
						}
					}
				}
				pwIndex.Add(WORD_INVALID_INDEX);
				ppwAdjacent[i] = pwIndex.Detach();
			}

			fDamping = 1.0f;
			pvDir.SetLength(pVertex.GetLength());
			for(i = 0; i < pVertex.GetLength(); i++)
			{
				pvDir[i] = pVertex[i].m_vPos;
			}
			pfPos.SetLength(pVertex.GetLength());
			pfPos.Fill(0);
			pfVel.SetLength(pVertex.GetLength());
			pfVel.Fill(0);
		}
		void Update()
		{
			if(fAngle > 360.0f)
			{
				fAngle = 0.0f;
				nPos = rand() * pVertex.GetLength() / RAND_MAX;
				fSize = fAverage * 0.5f;
			}
			fAngle += 20.0f;//= 3.14159 / 180.0;
			for(int i = 0; i < pVertex.GetLength(); i++)
			{
				float fAccel = 0;
				for(int j = 0; ppwAdjacent[i][j] != WORD_INVALID_INDEX; j++)
				{
					fAccel += pfPos[ppwAdjacent[i][j]];
				}
				fAccel /= j;
				pfVel[i] += fAccel - pfPos[i];
			}
			for(i = 0; i < pVertex.GetLength(); i++)
			{
				pfPos[i] += pfVel[i];
				pfPos[i] *= fDamping;
			}
			pfPos[nPos] = sinf(fAngle * g_fDegToRad) * fSize;
		}
		void Create()
		{
			for(int i = 0; i < pVertex.GetLength(); i++)
			{
				pVertex[i].m_vPos = pvDir[i] * (1 + Bound<float>(pfPos[i], MINSZ, MAXSZ));
			}
		}
		void ResetPoint(int nPoint)
		{
			float fPos = 0;
			for(int i = 0; ppwVertexFaceList[nPoint][i] != WORD_INVALID_INDEX; i++)
			{
				fPos += pfPos[ppwVertexFaceList[nPoint][i]];
			}	
			pfPos[nPoint] = fPos / i;
		}
		float AverageHeight()
		{
			float fHeight = 0;
			for(int i = 0; i < pVertex.GetLength(); i++) 
			{
				fHeight += pfPos[i];
			}
			return fHeight / pVertex.GetLength();
		}
	};
	WaterObj obj;

public:
	int nPos;
	bool bSetTexture;
	ZCamera camera;

	ZEffectWaterGlobe()
	{
		nPos = 0;

		obj.Start();
		obj.m_bsFlag.set(ZObject::F_DRAW_Z_BUFFER);
		obj.fAngle = 10000;

		camera.m_vPosition = ZVector(0, 0, -320);
	}
	HRESULT Calculate(float brightness, float elapsed)
	{
		static int done = 0;

		static double accum = 2;
		accum += elapsed;

		while(accum >= 1)
		{
//			angle += 20;
			obj.fDamping = 0.98f;//95

			obj.fAverage = g_pAudio->GetIntensity( );
/*			float fDamping = 0.9 - (average * average);
			float fAverage = pObj->AverageHeight();
			float fHeight = 0.1 * average * average;
			float fDamping = (fAverage == 0)? 0 : (fHeight / fAverage);
			if(pObj->fDamping < fDamping) pObj->fDamping = min(fDamping, pObj->fDamping + DAMPCHANGE);
			else pObj->fDamping = max(fDamping, pObj->fDamping - DAMPCHANGE);
*/
// 			float fHeight = average * pObj->AverageHeight();

			double bh = g_pAudio->GetIntensity( );// * 50;

//			float fAverage = pObj->AverageHeight();
//			pObj->pfPos[0] = bh * sin(angle * 3.14159 / 128.0);
			float fMult = min(1, g_pAudio->GetIntensity( ) * 2) - min(1, g_pAudio->GetIntensity( ) * 3) - (obj.AverageHeight() / 0.5f); //0.95 - average; //9; //0.95;
			obj.Update();
//			pObj->fDamping = average / pObj->;//min(0.96, average * 2.0) * Bound<float>(1 - avHeight, 0, 1); /**/ //(AVSZ / avHeight); //0.95 - average; //9; //0.95;

///			for(int i = 0; i < pObj->pVertex.GetLength(); i++)
//			{
//				if(pObj->pfPos[i] > MAXSZ) pObj->pfPos[i] = MAXSZ;
				//position[i] > maxHeight) wmapping-maxHeight = wmapping->position[i];
//			}
			// render
//			pObj->ResetPoint(0);
			//	obj->textureMap(waTexture);
	
			obj.fPitch += 2.0f * g_fDegToRad;
			obj.fYaw += 1.5f * g_fDegToRad;
			obj.wcAmbientLight = ZColour::Grey((int)(255.0f * brightness));

//			angle += average;
			accum--;
		}
//	obj.fPitch = pObj->fPitch;
//	obj.fYaw = pObj->fYaw;
		obj.m_bsFlag.reset(ZObject::F_VALID_VERTEX_NORMALS);

		obj.Create();
		obj.Calculate(&camera, elapsed);
		return S_OK;
	}
	HRESULT Reconfigure()
	{
		obj.pTexture[0].Set(ZObject::Texture::T_ENVMAP, g_pD3D->Find(TC_EMWATERGLOBE));
		return S_OK;
	}
	HRESULT Render()
	{
		HRESULT hRes;
		obj.m_bsFlag.set(ZObject::F_DRAW_TRANSPARENT);

		hRes = obj.Render();//scene->render(d3d);
		if(FAILED(hRes)) return TraceError(hRes);

		return S_OK;
	}
};

EXPORT_EFFECT(WaterGlobe, ZEffectWaterGlobe)
