#include "Platform.h"
//#include "tripex2.h"
#include "effect.h"
#include "error.h"
#include "StarModel.h"
#include "Actor.h"
#include "BezierCurve.h"
#include <algorithm>


#define NUMBEZ 10
#define MAX_TILT (20.0 * 3.14159 / 180.0)

#define SAMPLESIZE 8
#define SAMPLESHIFT 2
#define TRISIZE 30.0

extern Texture *pBlankTexture;

class EffectDotStar : public EffectBase
{
public:
	// 256x256
	class ZDotVertex
	{
	public:
		Vector3 vPosition;
		float fRHW;
	};

	Texture *pTexture, *pTint;
	ZArray<VertexTL> pTargetVertex;
	ZArray<Face> pTargetFace;

	Camera camera;
	Actor obj;
	Actor objTent;
	ZArray<unsigned char> pBuffer, pTarget;
	BezierCurve pb[NUMBEZ];
	float pfPos[NUMBEZ];
	double dPos;
	double rs, ps, ys;

	ContainedBezierCurve<1> b;
	ContainedBezierCurve<1> b2;

	float fBezPos;
	float fBezPos2;
	float fSpeed;
	float fTarget;
	float fPos;
	float fChange;
	float fTotal;
	float fTotalElapsed;
	double brt;

	EffectDotStar() : //: camera(0),
		b(Vector3(-100, -100, 120), Vector3(100, 100, 140)),
		b2(Vector3(-20, -20, -20), Vector3(20, 20, 20))
	{
		camera.flags.set( Camera::F_SCREEN_TRANSFORM, false );

		fTotalElapsed = 1;
		dPos = 70;
		fBezPos = 0;
		fBezPos2 = 0;
		fSpeed = 0;
		fTarget = 0;
		fPos = 0;
		fChange = 0;
		fTotal = 0;

		camera.position.z = -80;
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
		obj.vertices.SetLength(nStarVertices);
		for(int i = 0; i < nStarVertices; i++)
		{
			obj.vertices[i].position = Vector3(pfStarVertex[nIndex], pfStarVertex[nIndex + 1], pfStarVertex[nIndex + 2]);
			nIndex += 3;
		}
		nIndex = 0;
		obj.faces.SetLength(nStarFaces);
		for(int i = 0; i < nStarFaces; i++)
		{
			obj.faces[i] = Face(pwStarFace[nIndex], pwStarFace[nIndex + 1], pwStarFace[nIndex + 2]);
			nIndex += 3;
		}

		obj.flags.set( Actor::F_DO_POSITION_DELAY );

		obj.frame_history = 10.0f;
		obj.delay_history = 10.0f;
		obj.FindDelayValues();
		obj.frame_time = 0.5;
	}
	void DrawTriangle(Vector3 *pv1, Vector3 *pv2, Vector3 *pv3)
	{
		Vector3 *pvt;
		if(pv1->y > pv2->y){ pvt = pv1; pv1 = pv2; pv2 = pvt; }
		if(pv2->y > pv3->y){ pvt = pv2; pv2 = pv3; pv3 = pvt; }
		if(pv1->y > pv2->y){ pvt = pv1; pv1 = pv2; pv2 = pvt; }

		int y1 = pv1->y, y2 = pv2->y, y3 = pv3->y;

		float xl = pv1->x;
		float xr = pv1->x;
		float xls = (pv2->x - pv1->x) / (1 + y2 - y1);
		float xrs = (pv3->x - pv1->x) / (1 + y3 - y1);
		int left, right;

		int bPos = y1 * 512;
		for(int y = y1; y <= y3 && y < 256; y++)
		{
			if(y == y2)
			{
				xl = pv2->x;
				xls = (pv3->x - pv2->x) / (1 + y3 - y2);
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
	Error* Calculate(const CalculateParams& params) override
	{
		camera.position = b.Calculate(fBezPos);
		camera.SetTarget(b2.Calculate(fBezPos2));//ZVector::Origin());
		fBezPos += 0.02 * fSpeed * params.elapsed;
		fBezPos2 += 0.02 * fSpeed * params.elapsed;

		fChange += params.elapsed;
		fTotal += params.audio_data.GetIntensity( ) * params.elapsed;
		if(fChange > 20)
		{
			fTarget = fTotal / fChange;
			fTotal = 0;
			fChange = 0;
		}

		fTotalElapsed += params.elapsed * 2;
		if(fTotalElapsed < 1) return nullptr;

//	fSpeed = 0;
		fSpeed = StepTo<float>(fSpeed, fTarget, 0.02 * params.elapsed);

		brt = params.brightness;
		dPos += params.elapsed;
		while(dPos > 60)
		{
			rs = ((params.audio_data.GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
			ps = ((params.audio_data.GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
			ys = ((params.audio_data.GetIntensity( ) * 2.0) + 1.0 + (rand() * 5.0 / RAND_MAX)) * 3.14159 / 180.0;
//		if(rand() > (RAND_MAX * 0.7)) rs = -rs;
//		if(rand() > (RAND_MAX * 0.8)) ps = -ps;
//		if(rand() > (RAND_MAX * 0.6)) ys = -ys;

//		if(obj.fPitch > MAX_TILT && ps > 0) ps = -ps;
//		if(obj.fYaw > MAX_TILT && ys > 0) ys = -ys;
//		if(obj.fPitch < -MAX_TILT && ps < 0) ps = -ps;
//		if(obj.fYaw < -MAX_TILT && ys < 0) ys = -ys;

			dPos -= 15;
		}

		fPos += params.elapsed * 0.1;
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
		obj.roll += params.audio_data.GetIntensity( ) * params.elapsed * 4.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
		obj.pitch += params.audio_data.GetIntensity( ) * params.elapsed * 5.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
		obj.yaw += params.audio_data.GetIntensity( ) * params.elapsed * 6.0 * 3.14159 / 180.0;//(average + 0.05) * elapsed;// * rs;
//	obj.fPitch += average * elapsed * ps;
//	obj.fYaw += 0.5 * elapsed * ys * 3.14159 / 180.0;

		obj.clip_mask = 0;
		obj.Calculate(&camera, params.elapsed);
//	ZFlexibleVertex fvVertex = obj.pTransVertex[0];
		for(int i = 0; i < obj.transformed_vertices.GetLength(); i++)
		{
			obj.transformed_vertices[i].position.x += 256.0;
			obj.transformed_vertices[i].position.y += 128.0;// - (4 * fPos);
//		fvVertex++;
		}

		for(int i = 0; i < obj.clipped_faces.GetLength(); i++)
		{
			Face *pf = &obj.clipped_faces[i];
			DrawTriangle(&obj.transformed_vertices[(*pf)[0]].position, &obj.transformed_vertices[(*pf)[1]].position, &obj.transformed_vertices[(*pf)[2]].position);
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
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		fSpeed = params.audio_data.GetIntensity( );
		pTexture = params.texture_library.Find(TextureClass::DotStarSprite);
		pTint = params.texture_library.Find(TextureClass::DotStarBackground);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;

		double dSize = /*1.2 **/ (std::min(g_pD3D->GetWidth(), g_pD3D->GetHeight()) - 1) / 64.0f;//min(d3d->GetWidth(), d3d->GetHeight()) / 64.0;

		pTargetVertex.Empty();
		pTargetFace.Empty();
		pTargetVertex.nStep = 100;
		pTargetFace.nStep = 100;

		RenderState render_state;
		render_state.dst_blend = D3DBLEND_ONE;
		render_state.enable_zbuffer = false;
		render_state.texture_stages[0].texture = pTexture;

		ColorRgb pcColour[5];
		for(int k = 0; k < 5; k++)
		{
			pcColour[k] = ColorRgb::Grey(brt * 200.0f * k / 4);
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
						VertexTL *pVertex = &pTargetVertex[nV];

						pVertex[0].position.x = dX1;//dX + p;
						pVertex[0].position.y = dY1;//dY + p;

						pVertex[1].position.x = dX2;//dX + dSize - p;
						pVertex[1].position.y = dY1;//dY + p;

						pVertex[2].position.x = dX2;//dX + dSize - p;
						pVertex[2].position.y = dY2;//dY + dSize - p;
		
						pVertex[3].position.x = dX1;//dX + p;
						pVertex[3].position.y = dY2;//dY + dSize - p;

						int nColour = std::min(4, pTarget[nIndex] / 2);
						for(int k = 0; k < 4; k++)
						{
							pVertex[k].position.z = 1.0f;
							pVertex[k].rhw = 1.0f;//vPosition.z = 1.0f;
							pVertex[k].diffuse = pcColour[nColour];
						}
						pVertex[0].tex_coords[0].x = 0;
						pVertex[0].tex_coords[0].y = 0;
		
						pVertex[1].tex_coords[0].x = 1;
						pVertex[1].tex_coords[0].y = 0;

						pVertex[2].tex_coords[0].x = 1;
						pVertex[2].tex_coords[0].y = 1;

						pVertex[3].tex_coords[0].x = 0;
						pVertex[3].tex_coords[0].y = 1;

						Face *pFace = pTargetFace.AddEmptyPtr(2);
						pFace[0] = Face(nV + 0, nV + 1, nV + 3);
						pFace[1] = Face(nV + 1, nV + 2, nV + 3);
					}
				}
				dX += dSize;
				nIndex++;
			}
			dY += dSize;
		}
		error = g_pD3D->DrawIndexedPrimitive(render_state, pTargetVertex, pTargetFace);
		if (error) return TraceError(error);

		//lpd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, pVertex, 4, pwFace, 6, 0);

		if(pTint != pBlankTexture)
		{
			RenderState render_state;
			render_state.src_blend = D3DBLEND_DESTCOLOR;
			render_state.dst_blend = D3DBLEND_ONE;
			render_state.enable_zbuffer = false;
			g_pD3D->DrawSprite(render_state, Point<int>(0, 0), Rect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
		}

		return nullptr;
	}
};

EXPORT_EFFECT( DotStar, EffectDotStar )
