// average = (sensitivity * average)
#include "StdAfx.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "AudioData.h"
#include "effect.h"
#include <conio.h>
#include "error.h"
#include "general.h"

#define BEZIERS 5
#define BEZIERPOINTS 50
#define BEZIERHEIGHT 80

#define TWISTPLANES 4
#define TWISTPLANEEDGE 20
#define TWISTPLANECORNERS (BEZIERS * TWISTPLANEEDGE)
#define TWISTPLANERADIUS 30.0

//#define bcTwistPlanes 4			// >= 2
//#define bcTwistPlaneCorners 4
#define bcBezierPoints 50	//100
#define bcCornerLinePoints 10 //20
#define bcCubeRadius 30	// radius of the container cylinder of the cube
#define bcCubeHeight 80	// height of the cube

extern Texture *pBlankTexture;

class EffectBezierCube : public EffectBase
{
public:
	int nCornerIndex[BEZIERS];

	Texture *pTexture, *pTint;
	Actor obj;
	Actor pObj[BEZIERS];
	Actor pObjPlane[TWISTPLANES];
	Camera camera;
//static Object **bcBezierPoint, **bcTemplate, **bcCornerLinePoint;
	BezierCurve bcEdge;
	float fAng;

	float pfRS[TWISTPLANES], pfPS[TWISTPLANES], pfYS[TWISTPLANES], pfPos[TWISTPLANES], pfSpeed[TWISTPLANES];
	double brt;

	Actor testobj;
	double dAng, dAngX, dAngY, dAngZ;
	double dMult;

	EffectBezierCube() : bcEdge(TWISTPLANES)
	{
		brt = 0.0;
		pTexture = nullptr;
		pTint = nullptr;

		memset(pfRS, 0, sizeof(pfRS));
		memset(pfPS, 0, sizeof(pfPS));
		memset(pfYS, 0, sizeof(pfYS));
		memset(pfPos, 0, sizeof(pfPos));
		memset(pfSpeed, 0, sizeof(pfSpeed));

		dAng = 0;
		dAngX = 0;
		dAngY = 0;
		dAngZ = 0;
		dMult = 0;

		fAng = 0.0f;
		obj.pTexture[0].m_nType = Actor::TextureEntry::T_SPRITE;
		obj.m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.m_bsFlag.set( Actor::F_DRAW_TRANSPARENT);
		obj.fFrameHistory = 8.0f;
		obj.fSpriteHistoryLength = 0.3f;
		//pObjPlane[i].pVertex.SetFormat(D3DFVF_TEX1);

		for(int i = 0; i < TWISTPLANES; i++)
		{
			pfPos[i] = 2;
			pfSpeed[i] = 0;

			pObjPlane[i].m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
			pObjPlane[i].m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
//		pObjPlane[i].Clear(ZObject::DoScreenTransform);
			pObjPlane[i].fSpriteSize = 5.0f;//fRenderAsLights(10.0);
			pObjPlane[i].m_bsFlag.set( Actor::F_DO_FRAME_HISTORY );

			pObjPlane[i].pVertex.SetLength(TWISTPLANECORNERS);
			Vector3 vCorner[BEZIERS];
			for(int j = 0; j < BEZIERS; j++)
			{
				float fRadius = TWISTPLANERADIUS;
				float dAng = j * 2.0 * PI / BEZIERS;
				if(i != 0 && i != TWISTPLANES - 1) fRadius *= 1.2f;
				vCorner[j].m_fX = 0;
				vCorner[j].m_fY = fRadius * cos(dAng);
				vCorner[j].m_fZ = fRadius * sin(dAng);
			}
			for(int j = 0; j < TWISTPLANECORNERS; j++)
			{
				float fPos = float(j % TWISTPLANEEDGE) / TWISTPLANEEDGE;
				Vector3 &v1 = vCorner[j / TWISTPLANEEDGE];
				Vector3 &v2 = vCorner[((j / TWISTPLANEEDGE) + 1) % BEZIERS];

				pObjPlane[i].pVertex[j].m_vPos = (v1 * (1 - fPos)) + (v2 * fPos);
			}

			pObjPlane[i].pTexture[0].m_nType = Actor::TextureEntry::T_SPRITE;

//		if(i == 0 || i == TWISTPLANES - 1)
//		{
//			pScene->vpObject.Add(pObjPlane[i]);
//		}
		}
		for(int i = 0; i < BEZIERS; i++)
		{
			nCornerIndex[i] = i * TWISTPLANECORNERS / BEZIERS;
		}
		for(int i = 0; i < BEZIERS; i++)
		{
			pObj[i].pVertex.SetLength(BEZIERPOINTS);
			pObj[i].m_bsFlag.set( Actor::F_DRAW_TRANSPARENT );
			pObj[i].m_bsFlag.set( Actor::F_DRAW_VERTEX_SPRITES );
			for(int j = 0; j < BEZIERPOINTS; j++)
			{
//				pObj[i].pVertex[j].GetDiffuse() = ZColour::Grey(255.0 * (0.4 + (0.6 * fabs(cos(j * 3.14159 / BEZIERPOINTS)))));
			}
//			pObj[i]->Clear(ZObject::DoScreenTransform);
			pObj[i].m_bsFlag.set( Actor::F_NO_TRANSFORM );
			pObj[i].fSpriteSize = 5.0;
			pObj[i].fFrameHistory = 1.0f;
			pObj[i].fSpriteHistoryLength = 60.0f;
//			pObj[i].Set(ZObject::DrawVertexSpriteHistory);
			pObj[i].pTexture[0].m_nType = Actor::TextureEntry::T_SPRITE;
		}
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		double dMultDest = 1 - pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0.0f, 1.0f);//average;
		camera.m_vPosition.m_fZ = -110;//pScene->camera.z = -110;//60;
		double sm = 1.3 * elapsed;

		brt = brightness;
	
		if(dMultDest < dMult) dMult = std::max(dMultDest, dMult - 0.01);
		if(dMultDest > dMult) dMult = std::min(dMultDest, dMult + 0.01);

		double dTwistAng = PI * sin(dAng) / 2.0;
		dAng += sm * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0, 0.5f) * 0.25 * 4 * g_fDegToRad;

		double dCentre = (TWISTPLANES - 1.0) / 2.0;
		for(int i = 0; i < TWISTPLANES; i++)
		{
			pfPos[i] += pfSpeed[i];
			if(pfPos[i] > 1.0)
			{
				pfRS[i] = (3 + (rand() * 10.0 / RAND_MAX)) * g_fDegToRad;
				pfPS[i] = (3 + (rand() / RAND_MAX)) * g_fDegToRad;
				pfYS[i] = (3 + (rand() / RAND_MAX)) * g_fDegToRad;
				pfPos[i] = pfPos[i] - (int)pfPos[i];
			}

			pObjPlane[i].fRoll += pfRS[i] * (pAudio->GetIntensity( ) + 0.1);
			pObjPlane[i].fYaw += pfYS[i] * (pAudio->GetIntensity( ) + 0.1);
			pObjPlane[i].fPitch += pfPS[i] * pAudio->GetIntensity( );

			pObjPlane[i].vPosition.m_fX = -(BEZIERHEIGHT / 2) + (i * BEZIERHEIGHT / (TWISTPLANES - 1.0));
			pObjPlane[i].vPosition.m_fZ = -60;
			double dBr = brightness * 0.2 * (0.1 + (0.9 * fabs((i / dCentre) - 1)));
			pObjPlane[i].wcAmbientLight = ColorRgb::Grey(255.0 * dBr);
			pObjPlane[i].Calculate(&camera, elapsed);
		}

		for(int i = 0; i < BEZIERS; i++)
		{
			for(int j = 0; j < TWISTPLANES; j++)
			{
				bcEdge[j] = pObjPlane[j].ppFrame[0]->m_pvPosition[nCornerIndex[i]];
				// - ZVector(d3d->GetWidth() / 2.0, d3d->GetHeight() / 2.0, 0);
					//rvertex[nCornerIndex[i]].sx, pObjPlane[j]->rvertex[nCornerIndex[i]].sy, pObjPlane[j]->rvertex[nCornerIndex[i]].sz);
			}
			for(int j = 0; j < BEZIERPOINTS; j++)	
			{
				pObj[i].pVertex[j].m_vPos = bcEdge.Calculate(double(j) / BEZIERPOINTS);
			//	, &pObj[i]->vertex[j].x, &pObj[i]->vertex[j].y, &pObj[i]->vertex[j].z);
			}
			double dBr = brightness * 0.2;
			pObj[i].wcAmbientLight = ColorRgb::Grey(255.0 * dBr);//(255D3DRGB(dBr, dBr, dBr);
			pObj[i].Calculate(&camera, elapsed);
		}
		
		static float fel = 0.0f;
		fel += elapsed;
		for(; fel > 1.0; fel--)
		{
			fAng += 8.0f * g_fDegToRad;
			obj.pVertex.SetLength(1);
			obj.pVertex[0].m_vPos.m_fX = 5 * cos(fAng);//pObj[0].pVertex[0].GetPosition();
			obj.pVertex[0].m_vPos.m_fY = 5 * sin(fAng);//pObj[0].pVertex[0].GetPosition();
			obj.pVertex[0].m_vPos.m_fZ = -100;//pObj[0].pVertex[0].GetPosition();
		}
		obj.Calculate(&camera, elapsed);

		dMult = pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0, 1.0f);//average;
		dAngX += sm * dMult * 9 * g_fDegToRad;

		dAngY += sm * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0/16.0f, 3/16.0f) * 3.4 * g_fDegToRad;
		dAngZ += sm * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 3/16.0f, 9/16.0f) * 4.2 * g_fDegToRad;
		while(dAngX > PI2) dAngX -= PI2;
		while(dAngY > PI2) dAngY -= PI2;
		while(dAngZ > PI2) dAngZ -= PI2;

		camera.m_fRoll += sm * pAudio->GetIntensity( ) * 4 * g_fDegToRad;
	//	pScene->camera.turn(sm * average * 4 * 3.14159 / 180.0, 0, 0);
		return nullptr;
	}
	Error* Render( )
	{
		Error* error;
	//	hRes = obj.Render(d3d);
	//	if(FAILED(hRes)) return TraceError(hRes);

		error = pObjPlane[0].Render();
		if(error) return TraceError(error);
		error = pObjPlane[TWISTPLANES-1].Render();
		if(error) return TraceError(error);
	//	for(int i = 0; i < TWISTPLANES; i++)
	//	{
	//		hRes = pObjPlane[i].Render(d3d);
	//		if(FAILED(hRes)) return TraceError(hRes);
	//	}
		for(int i = 0; i < BEZIERS; i++)
		{
			error = pObj[i].Render();
			if(error) return TraceError(error);
		}

	//	hRes = pScene->render(d3d);
	//	if(FAILED(hRes)) return hRes;

		if(pTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, pTint);

			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			error = g_pD3D->DrawSprite(ZPoint<int>(0, 0), ZRect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		pTexture = g_pD3D->Find(TC_LBBEZIERCUBE);
		testobj.pTexture[0].m_pTexture = pTexture;
		obj.pTexture[0].m_pTexture = pTexture;
		for(int i = 0; i < TWISTPLANES; i++)
		{
			pObjPlane[i].pTexture[0].m_pTexture = pTexture;
		}
		for(int i = 0; i < BEZIERS; i++)
		{
			pObj[i].pTexture[0].m_pTexture = pTexture;
		}
		pTint = g_pD3D->Find(TC_WTBEZIERCUBE);
		return nullptr;
	}
};

EXPORT_EFFECT( BezierCube, EffectBezierCube )
