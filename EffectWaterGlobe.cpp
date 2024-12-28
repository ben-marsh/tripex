#include "Platform.h"
#include "Actor.h"
#include "error.h"
#include "effect.h"
#include "TextureData.h"
#include <algorithm>

#define accelFac 0.9

#define iPoints 10
#define MINHEIGHT 0.0f
#define BEHEIGHT  50.0f //30, 50, 80 are nice
#define MAXHEIGHT 80.0f

#define MINSZ -0.8f
#define MAXSZ 0.4f
#define AVSZ 1.0f

#define DAMPCHANGE 0.001

class EffectWaterGlobe : public EffectBase
{
private:
	const TextureClass envmap_texture_class =
	{
		"EnvMap",
		{ g_anTexAlienEgg, g_anTexShinySand }
	};

	class ZGeoEdge : public Edge
	{
	public:
		int nSplit;
	};

	class WaterObj : public Actor
	{
	public:
		int nPos;
		float fDamping;
		std::vector<Vector3> pvDir;
		std::vector<float> pfPos, pfVel;
		std::vector<std::unique_ptr<uint16[]>> ppwAdjacent;
		float fSize;
		float fAngle;
		float fAverage;

		WaterObj() : Actor()
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

			ppwAdjacent.resize(vertices.size());

			std::vector<uint16> indices;
			for(int i = 0; i < vertices.size(); i++)
			{
				indices.clear();
				for(int j = 0; vertex_face_list[i][j] != WORD_INVALID_INDEX; j++)
				{
					Face &f = faces[vertex_face_list[i][j]];
					for(int k = 0; k < 3; k++)
					{
						if(f[k] != i && std::find(indices.begin(), indices.end(), f[k]) == indices.end())
						{
							indices.push_back(f[k]);
						}
					}
				}
				indices.push_back(WORD_INVALID_INDEX);

				std::unique_ptr<uint16[]> index_array = std::make_unique<uint16[]>(indices.size());
				memcpy(index_array.get(), indices.data(), indices.size() * sizeof(indices[0]));

				ppwAdjacent[i] = std::move(index_array);
			}

			fDamping = 1.0f;
			pvDir.resize(vertices.size());
			for(int i = 0; i < vertices.size(); i++)
			{
				pvDir[i] = vertices[i].position;
			}
			pfPos.resize(vertices.size());
//			memset(pfPos.data(), 0, pfPos.size() * sizeof(float));
			pfVel.resize(vertices.size());
//			memset(pfVel.data(), 0, pfVel.size() * sizeof(float));
		}
		void Update()
		{
			if(fAngle > 360.0f)
			{
				fAngle = 0.0f;
				nPos = rand() * (int)vertices.size() / RAND_MAX;
				fSize = fAverage * 0.5f;
			}
			fAngle += 20.0f;//= 3.14159 / 180.0;
			for(int i = 0; i < vertices.size(); i++)
			{
				float fAccel = 0;
				int j;
				for(j = 0; ppwAdjacent[i][j] != WORD_INVALID_INDEX; j++)
				{
					fAccel += pfPos[ppwAdjacent[i][j]];
				}
				fAccel /= j;
				pfVel[i] += fAccel - pfPos[i];
			}
			for(int i = 0; i < vertices.size(); i++)
			{
				pfPos[i] += pfVel[i];
				pfPos[i] *= fDamping;
			}
			pfPos[nPos] = sinf(fAngle * g_fDegToRad) * fSize;
		}
		void Create()
		{
			for(int i = 0; i < vertices.size(); i++)
			{
				vertices[i].position = pvDir[i] * (1 + Bound<float>(pfPos[i], MINSZ, MAXSZ));
			}
		}
		void ResetPoint(int nPoint)
		{
			float fPos = 0;
			int i;
			for(i = 0; vertex_face_list[nPoint][i] != WORD_INVALID_INDEX; i++)
			{
				fPos += pfPos[vertex_face_list[nPoint][i]];
			}	
			pfPos[nPoint] = fPos / i;
		}
		float AverageHeight()
		{
			float fHeight = 0;
			for(int i = 0; i < vertices.size(); i++) 
			{
				fHeight += pfPos[i];
			}
			return fHeight / vertices.size();
		}
	};
	WaterObj obj;

public:
	int nPos;
	bool bSetTexture;
	Camera camera;

	EffectWaterGlobe()
		: EffectBase({ &envmap_texture_class })
	{
		nPos = 0;

		obj.Start();
		obj.flags.set(Actor::F_DRAW_Z_BUFFER);
		obj.fAngle = 10000;

		camera.position = Vector3(0, 0, -320);
	}
	Error* Calculate(const CalculateParams& params) override
	{
		static int done = 0;

		static double accum = 2;
		accum += params.elapsed;

		while(accum >= 1)
		{
//			angle += 20;
			obj.fDamping = 0.98f;//95

			obj.fAverage = params.audio_data.GetIntensity( );
/*			float fDamping = 0.9 - (average * average);
			float fAverage = pObj->AverageHeight();
			float fHeight = 0.1 * average * average;
			float fDamping = (fAverage == 0)? 0 : (fHeight / fAverage);
			if(pObj->fDamping < fDamping) pObj->fDamping = min(fDamping, pObj->fDamping + DAMPCHANGE);
			else pObj->fDamping = max(fDamping, pObj->fDamping - DAMPCHANGE);
*/
// 			float fHeight = average * pObj->AverageHeight();

			double bh = params.audio_data.GetIntensity( );// * 50;

//			float fAverage = pObj->AverageHeight();
//			pObj->pfPos[0] = bh * sin(angle * 3.14159 / 128.0);
			float fMult = std::min(1.0f, params.audio_data.GetIntensity( ) * 2) - std::min(1.f, params.audio_data.GetIntensity( ) * 3) - (obj.AverageHeight() / 0.5f); //0.95 - average; //9; //0.95;
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
	
			obj.pitch += 2.0f * g_fDegToRad;
			obj.yaw += 1.5f * g_fDegToRad;
			obj.ambient_light_color = ColorRgb::Grey((int)(255.0f * params.brightness));

//			angle += average;
			accum--;
		}
//	obj.fPitch = pObj->fPitch;
//	obj.fYaw = pObj->fYaw;
		obj.flags.reset(Actor::F_VALID_VERTEX_NORMALS);

		obj.Create();
		obj.Calculate(params.renderer, &camera, params.elapsed);
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		obj.textures[0].Set(Actor::TextureType::Envmap, params.texture_library.Find(envmap_texture_class));
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;
		obj.flags.set(Actor::F_DRAW_TRANSPARENT);

		error = obj.Render(params.renderer);//scene->render(d3d);
		if(error) return TraceError(error);

		return nullptr;
	}
};

EXPORT_EFFECT(WaterGlobe, EffectWaterGlobe)
