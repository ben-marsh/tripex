#include "StdAfx.h"
//#include "tripex2.h"
#include "effect.h"
#include "error.h"
#include "general.h"
#include "star.h"
#include "ZObject.h"
#include "ZBezier.h"
#include <algorithm>


#define NUMBEZ 10
#define MAX_TILT (20.0 * 3.14159 / 180.0)

#define SAMPLESIZE 8
#define SAMPLESHIFT 2
#define TRISIZE 30.0

extern ZTexture *pBlankTexture;

class ZEffectSuperSampling : public ZEffectBase
{
public:
	// 256x256
	class ZDotVertex
	{
	public:
		ZVector vPosition;
		float fRHW;
	};

	ZTexture *pTexture, *pTint;
	ZArray<ZVertexTL> pTargetVertex;
	ZArray<ZFace> pTargetFace;

	ZCamera camera;
	ZObject obj;
	ZObject objTent;
	ZArray<unsigned char> pBuffer, pTarget;
	ZBezier pb[NUMBEZ];
	float pfPos[NUMBEZ];
	double dPos;
	double rs, ps, ys;

	ZContainedBezier<1> b;
	ZContainedBezier<1> b2;

	float fBezPos;
	float fBezPos2;
	float fSpeed;
	float fTarget;
	float fPos;
	float fChange;
	float fTotal;
	float fTotalElapsed;
	double brt;

	ZEffectSuperSampling() : //: camera(0),
		b(ZVector(-100, -100, 120), ZVector(100, 100, 140)),
		b2(ZVector(-20, -20, -20), ZVector(20, 20, 20))
	{
		camera.m_bsFlag.set( ZCamera::F_SCREEN_TRANSFORM, false );

		fTotalElapsed = 1;
		dPos = 70;
		fBezPos = 0;
		fBezPos2 = 0;
		fSpeed = 0;
		fTarget = 0;
		fPos = 0;
		fChange = 0;
		fTotal = 0;

		camera.m_vPosition.m_fZ = -80;
		pBuffer.SetLength(512 * 256);
		pBuffer.Fill(0);
		pTarget.SetLength(128 * 64);
		pTarget.Fill(0);	

		for(int i = 0; i < NUMBEZ; i++)
		{
			pb[i].Create(4);// = new ZBezier(4);
			pfPos[i] = 3 + (float(i) / NUMBEZ);
		}

		int nIndex = 0;
		obj.pVertex.SetLength(nStarVertices);
		for(int i = 0; i < nStarVertices; i++)
		{
			obj.pVertex[i].m_vPos = ZVector(pfStarVertex[nIndex], pfStarVertex[nIndex + 1], pfStarVertex[nIndex + 2]);
			nIndex += 3;
		}
		nIndex = 0;
		obj.pFace.SetLength(nStarFaces);
		for(int i = 0; i < nStarFaces; i++)
		{
			obj.pFace[i] = ZFace(pwStarFace[nIndex], pwStarFace[nIndex + 1], pwStarFace[nIndex + 2]);
			nIndex += 3;
		}

		obj.m_bsFlag.set( ZObject::F_DO_POSITION_DELAY );

		obj.fFrameHistory = 10.0f;
		obj.fDelayHistory = 10.0f;
		obj.FindDelayValues();
		obj.fFrameTime = 0.5;
	}
	void DrawTriangle(ZVector *pv1, ZVector *pv2, ZVector *pv3)
	{
		ZVector *pvt;
		if(pv1->m_fY > pv2->m_fY){ pvt = pv1; pv1 = pv2; pv2 = pvt; }
		if(pv2->m_fY > pv3->m_fY){ pvt = pv2; pv2 = pv3; pv3 = pvt; }
		if(pv1->m_fY > pv2->m_fY){ pvt = pv1; pv1 = pv2; pv2 = pvt; }

		int y1 = pv1->m_fY, y2 = pv2->m_fY, y3 = pv3->m_fY;

		float xl = pv1->m_fX;
		float xr = pv1->m_fX;
		float xls = (pv2->m_fX - pv1->m_fX) / (1 + y2 - y1);
		float xrs = (pv3->m_fX - pv1->m_fX) / (1 + y3 - y1);
		int left, right;

		int bPos = y1 * 512;
		for(int y = y1; y <= y3 && y < 256; y++)
		{
			if(y == y2)
			{
				xl = pv2->m_fX;
				xls = (pv3->m_fX - pv2->m_fX) / (1 + y3 - y2);
			}
			if(y >= 0 && y < 256)
			{
				left = std::min(xl, xr);
				right = std::max(xl, xr);
//				for(int i = max(left, 0); i < min(right, 512); i++)
//				{
//					pBuffer[bPos + i] = 1;//++;
//				}
				if(left >= 0 && left < 512) pBuffer[bPos + left] = 1;
				if(right >= 0 && right < 512) pBuffer[bPos + right] = 1;
			}
			xl += xls;
			xr += xrs;
			bPos += 512;
		}
	}
//Object *makeTentacles(int segs, float l, float r);
	ZError* Calculate(float brightness, float elapsed, ZAudio* pAudio) override
	{
		camera.m_vPosition = b.Calculate(fBezPos);
		camera.SetTarget(b2.Calculate(fBezPos2));//ZVector::Origin());
		fBezPos += 0.02 * fSpeed * elapsed;
		fBezPos2 += 0.02 * fSpeed * elapsed;

		fChange += elapsed;
		fTotal += pAudio->GetIntensity( ) * elapsed;
		if(fChange > 20)
		{
			fTarget = fTotal / fChange;
			fTotal = 0;
			fChange = 0;
		}

		fTotalElapsed += elapsed * 2;
		if(fTotalElapsed < 1) return nullptr;

//	fSpeed = 0;
		fSpeed = StepTo<float>(fSpeed, fTarget, 0.02 * elapsed);

		brt = brightness;
		dPos += elapsed;
		while(dPos > 60)
		{
			rs = ((pAudio->GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
			ps = ((pAudio->GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
			ys = ((pAudio->GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
//		if(rand() > (RAND_MAX * 0.7)) rs = -rs;
//		if(rand() > (RAND_MAX * 0.8)) ps = -ps;
//		if(rand() > (RAND_MAX * 0.6)) ys = -ys;

//		if(obj.fPitch > MAX_TILT && ps > 0) ps = -ps;
//		if(obj.fYaw > MAX_TILT && ys > 0) ys = -ys;
//		if(obj.fPitch < -MAX_TILT && ps < 0) ps = -ps;
//		if(obj.fYaw < -MAX_TILT && ys < 0) ys = -ys;

			dPos -= 15;
		}

		fPos += elapsed * 0.1;
		while(fPos >= 1.0f) fPos -= 1.0f;

		pBuffer.Fill(0);
		int nSub = 1.0f * fTotalElapsed;
		for(int k = 0; k < pTarget.GetLength(); k++)
		{
			pTarget[k] = std::max(0, std::min<int>(pTarget[k], 8) - nSub);//min(pTarget[k], 4) - 1);	
		}
		fTotalElapsed -= nSub;

	//	pTarget.Fill(0);
//	ZeroMemory(pBuffer, 256 * 256 * sizeof(unsigned char));
//	ZeroMemory(pTarget, 64 * 64 * sizeof(unsigned char));
		obj.fRoll += pAudio->GetIntensity( ) * elapsed * 4.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
		obj.fPitch += pAudio->GetIntensity( ) * elapsed * 5.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
		obj.fYaw += pAudio->GetIntensity( ) * elapsed * 6.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
//	obj.fPitch += average * elapsed * ps;
//	obj.fYaw += 0.5 * elapsed * ys * 3.14159 / 180.0;

		obj.wClipMask = 0;
		obj.Calculate(&camera, elapsed);
//	ZFlexibleVertex fvVertex = obj.pTransVertex[0];
		for(int i = 0; i < obj.pTransVertex.GetLength(); i++)
		{
			obj.pTransVertex[i].m_vPos.m_fX += 256.0;
			obj.pTransVertex[i].m_vPos.m_fY += 128.0;// - (4 * fPos);
//		fvVertex++;
		}

		for(int i = 0; i < obj.pClippedFace.GetLength(); i++)
		{
			ZFace *pf = &obj.pClippedFace[i];
			DrawTriangle(&obj.pTransVertex[(*pf)[0]].m_vPos, &obj.pTransVertex[(*pf)[1]].m_vPos, &obj.pTransVertex[(*pf)[2]].m_vPos);
		}
 
		int nSrc = 0, nBase = 0, nDst= 0;
		for(int y = 0; y < 256; y++)
		{
			nDst = nBase;
			for(int x = 0; x < 512; x++)
			{
				pTarget[nDst] += pBuffer[nSrc];
				nSrc++;
				if(((x + 1) & 3) == 0) nDst++;
			}
			if(((y + 1) & 3) == 0) nBase += 128;
		}

		int nIndex = 0;
		for(int j = 0; j < 64; j++)
		{
			for(int i = 0; i < 128; i++)
			{
				if(pTarget[nIndex] != 0)
				{
	//				pTarget[nIndex] += 4;
				}
				nIndex++;
			}
		}
		return nullptr;
	}
	ZError* Reconfigure(ZAudio* pAudio) override
	{
		fSpeed = pAudio->GetIntensity( );
		pTexture = g_pD3D->Find(TC_LBDOTSTAR);
		pTint = g_pD3D->Find(TC_WTDOTSTAR);
		return nullptr;
	}
	ZError* Render( ) override
	{
		ZError* error;

		double dSize = /*1.2 **/ (std::min(g_pD3D->GetWidth(), g_pD3D->GetHeight()) - 1) / 64.0f;//min(d3d->GetWidth(), d3d->GetHeight()) / 64.0;

		pTargetVertex.Empty();
		pTargetFace.Empty();
		pTargetVertex.nStep = 100;
		pTargetFace.nStep = 100;

		g_pD3D->SetTexture(0, pTexture);
		g_pD3D->SetState(g_pD3D->Transparent);

		ZColour pcColour[5];
		for(int k = 0; k < 5; k++)
		{
			pcColour[k] = ZColour::Grey(brt * 200.0f * k / 4);
		}

		int nIndex = 0;
		double dY = (g_pD3D->GetHeight() - 1 - (dSize * (64.0 /*- (2.0 * fPos)*/))) / 2;
		double dCY = g_pD3D->GetHeight() / 2.0;
		for(int j = 0; j < 64; j++)
		{
			double dX = (g_pD3D->GetWidth() - 1 - (dSize * 128.0)) / 2;
			double dCX = g_pD3D->GetWidth() / 2.0;
			for(int i = 0; i < 128; i++)
			{
				if(pTarget[nIndex] != 0)
				{
					int nH = std::min<int>(pTarget[nIndex], 16);
					float p = dSize * (16 - nH) / 32.0f;// / 32.0f;  
					double dX1 = dX + p, dX2 = dX + dSize - p;
					double dY1 = dY + p, dY2 = dY + dSize - p;

					if(dX1 >= 0 && dY1 >= 0 && dX2 < g_pD3D->GetWidth() - 1 && dY2 < g_pD3D->GetHeight() - 1)
					{
						int nV = pTargetVertex.AddEmpty(4);
						ZVertexTL *pVertex = &pTargetVertex[nV];

						pVertex[0].m_vPos.m_fX = dX1;//dX + p;
						pVertex[0].m_vPos.m_fY = dY1;//dY + p;

						pVertex[1].m_vPos.m_fX = dX2;//dX + dSize - p;
						pVertex[1].m_vPos.m_fY = dY1;//dY + p;

						pVertex[2].m_vPos.m_fX = dX2;//dX + dSize - p;
						pVertex[2].m_vPos.m_fY = dY2;//dY + dSize - p;
		
						pVertex[3].m_vPos.m_fX = dX1;//dX + p;
						pVertex[3].m_vPos.m_fY = dY2;//dY + dSize - p;

						int nColour = std::min(4, pTarget[nIndex] / 2);
						for(int k = 0; k < 4; k++)
						{
							pVertex[k].m_vPos.m_fZ = 1.0f;
							pVertex[k].m_fRHW = 1.0f;//vPosition.z = 1.0f;
							pVertex[k].m_cDiffuse = pcColour[nColour];
						}
						pVertex[0].m_aTex[0].x = 0;
						pVertex[0].m_aTex[0].y = 0;
		
						pVertex[1].m_aTex[0].x = 1;
						pVertex[1].m_aTex[0].y = 0;

						pVertex[2].m_aTex[0].x = 1;
						pVertex[2].m_aTex[0].y = 1;

						pVertex[3].m_aTex[0].x = 0;
						pVertex[3].m_aTex[0].y = 1;

						ZFace *pFace = pTargetFace.AddEmptyPtr(2);
						pFace[0] = ZFace(nV + 0, nV + 1, nV + 3);
						pFace[1] = ZFace(nV + 1, nV + 2, nV + 3);
					}
				}
				dX += dSize;
				nIndex++;
			}
			dY += dSize;
		}
		error = g_pD3D->DrawIndexedPrimitive(pTargetVertex, pTargetFace);
		if (error) return TraceError(error);

		//lpd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, pVertex, 4, pwFace, 6, 0);

		if(pTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, pTint);
			g_pD3D->ResetRenderState();
			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_pD3D->DrawSprite(ZPoint<int>(0, 0), ZRect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ZColour::Grey(brt * 255.0));
		}

		return nullptr;
	}
};

EXPORT_EFFECT( SuperSampling, ZEffectSuperSampling )
