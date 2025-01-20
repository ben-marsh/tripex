#include "Platform.h"
#include "Effect.h"
#include "error.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "effect.h"

class EffectSun : public Effect
{
public:
	static const int RINGS = 5;//3
	static const int EXPOSURE = 3;//5//7//9//15//4//3//7//9
	const float EXPDIV = 5.0f;//4.5//5//7
	static const int EXPFRAMES = 2;//4//10//6//3

	static const int TRI = 10;
	//#define PI 3.141592
	const float PI2 = (PI * 2);
	static const int LAYERS = 3;
	static const int INNER = 30;
	static const int OUTER = (INNER + 30);
	const float ANGSZ = (20.0f * DEG_TO_RAD);

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
		cCamera.position = Vector3(0, 0, -70);//-70);
//		scene->property(worldCull, false);
	
		for(int o = 0; o < RINGS; o++)
		{
//			obj[o] = new Object();
//			obj[o]->Create((TRI * 2 * ((LAYERS * 2) + 1)) + 1, TRI * 2 * ((LAYERS * 2) + 1));
//			obj[o]->Create(TRI * 2 * ((LAYERS * 2) + 1), TRI * ((LAYERS * 2) + 1));
//			obj[o]->Create((LAYERS*2+1)*TRI*2, TRI * 2 * (LAYERS*2+1));
	//		pObj[o].pVertex.SetFormat(D3DFVF_DIFFUSE);
			pObj[o].vertices.resize((LAYERS * 2 + 1) * TRI * 2);
			pObj[o].faces.resize(TRI * 2 * (LAYERS * 2 + 1));
//			obj[o]->x = 400.0 * cos(o * 2.0 * 3.14159 / RINGS);
//			obj[o]->y = 400.0 * sin(o * 2.0 * 3.14159 / RINGS);
//				(TRI * 2 * ((LAYERS * 2) + 1)) + 1, TRI * 2 * ((LAYERS * 2) + 1));

			int base = 0;
			for(int a = -LAYERS; a <= LAYERS; a++)
			{
				float ang = 10 * PI * a / (180 * LAYERS);
				for(int i = 0; i < TRI; i++)
				{
					pObj[o].vertices[base + i].position.x = sin(i * PI2 / TRI) * INNER * cos(ang);
					pObj[o].vertices[base + i].position.y = cos(i * PI2 / TRI) * INNER * cos(ang);
					pObj[o].vertices[base + i].position.z = INNER * sin(ang);
//					pObj[o].pVertex[base + i].GetDiffuse() = ZColour(128.0 / EXPDIV, 77.0 / EXPDIV, 243.0 / EXPDIV); //0.6 / 25, 0.5 / 25);

					pObj[o].vertices[base + i + TRI].position.x = sin((i * PI2 / TRI) + (PI / TRI)) * OUTER * cos(ang);
					pObj[o].vertices[base + i + TRI].position.y = cos((i * PI2 / TRI) + (PI / TRI)) * OUTER * cos(ang);
					pObj[o].vertices[base + i + TRI].position.z = OUTER * sin(ang);
//					pObj[o].pVertex[base + i + TRI].GetDiffuse() = ZColour(0, 64.0 / EXPDIV, 0);

					pObj[o].faces[base + i][0] = base + i;
					pObj[o].faces[base + i][1] = base + ((i + 1) % TRI);
					pObj[o].faces[base + i][2] = base + i + TRI;

					pObj[o].faces[base + i + TRI][0] = base + i;
					pObj[o].faces[base + i + TRI][2] = base + ((i + 1) % TRI);
					pObj[o].faces[base + i + TRI][1] = base + i + TRI;
				}
				base += TRI * 2;
			}

			pObj[o].flags.set(Actor::F_DRAW_TRANSPARENT);
			pObj[o].flags.set(Actor::F_DRAW_Z_BUFFER, false);
			pObj[o].exposure = EXPOSURE;
			pObj[o].frame_history = EXPFRAMES;

		
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
	Error* Calculate(const CalculateParams& params) override
	{
		fBezPos += params.elapsed * params.audio_data.GetIntensity( ) * 0.01f;
		fBezPos2 += params.elapsed * params.audio_data.GetIntensity( ) * 0.02f;
		cCamera.position = Vector3(0, 0, -70);//bez.Calculate(fBezPos);
		cCamera.SetTarget(bez2.Calculate(fBezPos2));

		static double accum = 0;
		accum += params.elapsed * 1.5;
		static float fMove = 21;
		fMove += params.elapsed;
		if(fMove > 20)
		{
			for(int i = 0; i < RINGS; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					as[i][j][0] = (float)_copysign(1.0f,as[i][j][0]) * (4.0f + (rand() * 4.0f / RAND_MAX)) * DEG_TO_RAD;
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
					pObj[i].roll += 0.1f * as[i][0][0];
					pObj[i].pitch += params.audio_data.GetDampenedBand( sensitivity, 3.0f/16.0f, 4.0f/16.0f) * as[i][1][0];
					pObj[i].yaw += params.audio_data.GetDampenedBand( sensitivity, 4.0f/16.0f, 5.0f/16.0f) * as[i][2][0];
					pObj[i].pitch = Clamp<float>(pObj[i].pitch, -ANGSZ, ANGSZ);
					pObj[i].yaw = Clamp<float>(pObj[i].yaw, -ANGSZ, ANGSZ);
				}
				if((i % 3) == 1)
				{
					pObj[i].roll += params.audio_data.GetDampenedBand(sensitivity, 2.0f/16.0f, 3.0f/16.0f) * as[i][0][0];
					pObj[i].pitch += 0.1f * as[i][1][0];
					pObj[i].yaw += 1.5f * params.audio_data.GetDampenedBand(sensitivity, 2.0f/16.0f, 4.0f/16.0f) * as[i][2][0];
				}
				if((i % 3) == 2) 
				{
					pObj[i].roll += params.audio_data.GetDampenedBand(sensitivity, 1/16.0f, 2/16.0f) * as[i][0][0];
					pObj[i].pitch += params.audio_data.GetIntensity( ) * as[i][1][0];
					pObj[i].yaw += 0.1f * as[i][2][0];
				}
	//for(int o = 0; o < RINGS; o++)
	//{
				int base = 0;
				for(int a = -LAYERS; a <= LAYERS; a++)
				{
					for(int j = 0; j < TRI; j++)
					{
						pObj[i].vertices[base + j].diffuse = ColorRgb((uint8)(params.brightness * 128.0f / EXPDIV), (uint8)(params.brightness * 77.0f / EXPDIV), (uint8)(params.brightness * 243.0f / EXPDIV)); //0.6 / 25, 0.5 / 25);
						pObj[i].vertices[base + j + TRI].diffuse = ColorRgb(0, (uint8)(params.brightness * 64.0f / EXPDIV), 0);
					}
					base += 2 * TRI;
				}
				pObj[i].flags.set(Actor::F_VALID_VERTEX_DIFFUSE);
	//}
//			obj[i]->ExposureCapture();
				pObj[i].Calculate(params.renderer, &cCamera, 1.0);
			}
			accum--;
		}
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;
		for(int i = 0; i < RINGS; i++)
		{
			error = pObj[i].Render(params.renderer);
			if(error) return TraceError(error);
		}
		return nullptr;
	//	return scene->render(d3d);
	}
};

EXPORT_EFFECT(Sun, EffectSun)
