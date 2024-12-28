// average = (sensitivity * average)
#include "Platform.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "AudioData.h"
#include "effect.h"
#include <conio.h>
#include "error.h"
#include "TextureData.h"

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

class EffectBezierCube : public Effect
{
public:
	const TextureClass sprite_texture_class =
	{
		"Sprite",
		{ g_anTexLight }
	};

	const TextureClass tint_texture_class =
	{
		"Tint",
		{ g_anTexEyes, g_anTexFlesh, g_anTexForest }
	};

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

	EffectBezierCube()
		: Effect({ &sprite_texture_class, &tint_texture_class })
		, bcEdge(TWISTPLANES)
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
		obj.textures[0].type = Actor::TextureType::Sprite;
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.flags.set( Actor::F_DRAW_TRANSPARENT);
		obj.frame_history = 8.0f;
		obj.sprite_history_length = 0.3f;
		//pObjPlane[i].pVertex.SetFormat(D3DFVF_TEX1);

		for(int i = 0; i < TWISTPLANES; i++)
		{
			pfPos[i] = 2;
			pfSpeed[i] = 0;

			pObjPlane[i].flags.set( Actor::F_DRAW_TRANSPARENT );
			pObjPlane[i].flags.set( Actor::F_DRAW_VERTEX_SPRITES );
//		pObjPlane[i].Clear(ZObject::DoScreenTransform);
			pObjPlane[i].sprite_size = 5.0f;//fRenderAsLights(10.0);
			pObjPlane[i].flags.set( Actor::F_DO_FRAME_HISTORY );

			pObjPlane[i].vertices.resize(TWISTPLANECORNERS);
			Vector3 vCorner[BEZIERS];
			for(int j = 0; j < BEZIERS; j++)
			{
				float fRadius = TWISTPLANERADIUS;
				float dAng = j * 2.0 * PI / BEZIERS;
				if(i != 0 && i != TWISTPLANES - 1) fRadius *= 1.2f;
				vCorner[j].x = 0;
				vCorner[j].y = fRadius * cos(dAng);
				vCorner[j].z = fRadius * sin(dAng);
			}
			for(int j = 0; j < TWISTPLANECORNERS; j++)
			{
				float fPos = float(j % TWISTPLANEEDGE) / TWISTPLANEEDGE;
				Vector3 &v1 = vCorner[j / TWISTPLANEEDGE];
				Vector3 &v2 = vCorner[((j / TWISTPLANEEDGE) + 1) % BEZIERS];

				pObjPlane[i].vertices[j].position = (v1 * (1 - fPos)) + (v2 * fPos);
			}

			pObjPlane[i].textures[0].type = Actor::TextureType::Sprite;

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
			pObj[i].vertices.resize(BEZIERPOINTS);
			pObj[i].flags.set( Actor::F_DRAW_TRANSPARENT );
			pObj[i].flags.set( Actor::F_DRAW_VERTEX_SPRITES );
			for(int j = 0; j < BEZIERPOINTS; j++)
			{
//				pObj[i].pVertex[j].GetDiffuse() = ZColour::Grey(255.0 * (0.4 + (0.6 * fabs(cos(j * 3.14159 / BEZIERPOINTS)))));
			}
//			pObj[i]->Clear(ZObject::DoScreenTransform);
			pObj[i].flags.set( Actor::F_NO_TRANSFORM );
			pObj[i].sprite_size = 5.0;
			pObj[i].frame_history = 1.0f;
			pObj[i].sprite_history_length = 60.0f;
//			pObj[i].Set(ZObject::DrawVertexSpriteHistory);
			pObj[i].textures[0].type = Actor::TextureType::Sprite;
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		double dMultDest = 1 - params.audio_data.GetDampenedBand(sensitivity, 0.0f, 1.0f);//average;
		camera.position.z = -110;//pScene->camera.z = -110;//60;
		double sm = 1.3 * params.elapsed;

		brt = params.brightness;
	
		if(dMultDest < dMult) dMult = std::max(dMultDest, dMult - 0.01);
		if(dMultDest > dMult) dMult = std::min(dMultDest, dMult + 0.01);

		double dTwistAng = PI * sin(dAng) / 2.0;
		dAng += sm * params.audio_data.GetDampenedBand(sensitivity, 0, 0.5f) * 0.25 * 4 * g_fDegToRad;

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

			pObjPlane[i].roll += pfRS[i] * (params.audio_data.GetIntensity( ) + 0.1);
			pObjPlane[i].yaw += pfYS[i] * (params.audio_data.GetIntensity( ) + 0.1);
			pObjPlane[i].pitch += pfPS[i] * params.audio_data.GetIntensity( );

			pObjPlane[i].position.x = -(BEZIERHEIGHT / 2) + (i * BEZIERHEIGHT / (TWISTPLANES - 1.0));
			pObjPlane[i].position.z = -60;
			double dBr = params.brightness * 0.2 * (0.1 + (0.9 * fabs((i / dCentre) - 1)));
			pObjPlane[i].ambient_light_color = ColorRgb::Grey(255.0 * dBr);
			pObjPlane[i].Calculate(params.renderer, &camera, params.elapsed);
		}

		for(int i = 0; i < BEZIERS; i++)
		{
			for(int j = 0; j < TWISTPLANES; j++)
			{
				bcEdge[j] = pObjPlane[j].frames[0]->positions[nCornerIndex[i]];
				// - ZVector(d3d->GetWidth() / 2.0, d3d->GetHeight() / 2.0, 0);
					//rvertex[nCornerIndex[i]].sx, pObjPlane[j]->rvertex[nCornerIndex[i]].sy, pObjPlane[j]->rvertex[nCornerIndex[i]].sz);
			}
			for(int j = 0; j < BEZIERPOINTS; j++)	
			{
				pObj[i].vertices[j].position = bcEdge.Calculate(double(j) / BEZIERPOINTS);
			//	, &pObj[i]->vertex[j].x, &pObj[i]->vertex[j].y, &pObj[i]->vertex[j].z);
			}
			double dBr = params.brightness * 0.2;
			pObj[i].ambient_light_color = ColorRgb::Grey(255.0 * dBr);//(255D3DRGB(dBr, dBr, dBr);
			pObj[i].Calculate(params.renderer, &camera, params.elapsed);
		}
		
		static float fel = 0.0f;
		fel += params.elapsed;
		for(; fel > 1.0; fel--)
		{
			fAng += 8.0f * g_fDegToRad;
			obj.vertices.resize(1);
			obj.vertices[0].position.x = 5 * cos(fAng);//pObj[0].pVertex[0].GetPosition();
			obj.vertices[0].position.y = 5 * sin(fAng);//pObj[0].pVertex[0].GetPosition();
			obj.vertices[0].position.z = -100;//pObj[0].pVertex[0].GetPosition();
		}
		obj.Calculate(params.renderer, &camera, params.elapsed);

		dMult = params.audio_data.GetDampenedBand(sensitivity, 0, 1.0f);//average;
		dAngX += sm * dMult * 9 * g_fDegToRad;

		dAngY += sm * params.audio_data.GetDampenedBand(sensitivity, 0/16.0f, 3/16.0f) * 3.4 * g_fDegToRad;
		dAngZ += sm * params.audio_data.GetDampenedBand(sensitivity, 3/16.0f, 9/16.0f) * 4.2 * g_fDegToRad;
		while(dAngX > PI2) dAngX -= PI2;
		while(dAngY > PI2) dAngY -= PI2;
		while(dAngZ > PI2) dAngZ -= PI2;

		camera.roll += sm * params.audio_data.GetIntensity( ) * 4 * g_fDegToRad;
	//	pScene->camera.turn(sm * average * 4 * 3.14159 / 180.0, 0, 0);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;
	//	hRes = obj.Render(d3d);
	//	if(FAILED(hRes)) return TraceError(hRes);

		error = pObjPlane[0].Render(params.renderer);
		if(error) return TraceError(error);

		error = pObjPlane[TWISTPLANES-1].Render(params.renderer);
		if(error) return TraceError(error);
	//	for(int i = 0; i < TWISTPLANES; i++)
	//	{
	//		hRes = pObjPlane[i].Render(d3d);
	//		if(FAILED(hRes)) return TraceError(hRes);
	//	}
		for(int i = 0; i < BEZIERS; i++)
		{
			error = pObj[i].Render(params.renderer);
			if(error) return TraceError(error);
		}

	//	hRes = pScene->render(d3d);
	//	if(FAILED(hRes)) return hRes;

		if(pTint != nullptr)
		{
			RenderState render_state;
			render_state.blend_mode = BlendMode::Tint;
			render_state.depth_mode = DepthMode::Disable;
			render_state.texture_stages[0].texture = pTint;

			error = params.renderer.DrawSprite(render_state, Point<int>(0, 0), Rect<int>(0, 0, params.renderer.GetWidth(), params.renderer.GetHeight()), ColorRgb::Grey(brt * 255.0));
			if(error) return TraceError(error);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		pTexture = params.texture_library.Find(sprite_texture_class);
		testobj.textures[0].texture = pTexture;
		obj.textures[0].texture = pTexture;
		for(int i = 0; i < TWISTPLANES; i++)
		{
			pObjPlane[i].textures[0].texture = pTexture;
		}
		for(int i = 0; i < BEZIERS; i++)
		{
			pObj[i].textures[0].texture = pTexture;
		}

		pTint = params.texture_library.Find(tint_texture_class);
		return nullptr;
	}
};

EXPORT_EFFECT( BezierCube, EffectBezierCube )
