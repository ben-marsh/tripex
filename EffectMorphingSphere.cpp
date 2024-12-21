#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "error.h"

#define HILLSZ 0.1
#define LANDSCAPEITER 2000

#define NOBJ 4

class EffectMorphingSphere : public EffectBase
{
public:
	Actor pObj[NOBJ + 1];
	Camera camera;
	ZArray<float> pfAng;
	ContainedBezierCurve<1> b;
	float fBezPos;
	
	EffectMorphingSphere() : b(Vector3(-100, -100, -600), Vector3(100, 100, -200))
	{
		fBezPos = 0;
		pObj[NOBJ].CreateTetrahedronGeosphere(1.0, 4);
		pObj[NOBJ].FindFaceOrder(Vector3::Origin());
		pfAng.SetLength(pObj[NOBJ].vertices.GetLength());
		for(int i = 0; i < pObj[NOBJ].vertices.GetLength(); i++)
		{
			pfAng[i] = pObj[NOBJ].vertices[i].position.z * PI * 2;
		}
		for(int i = 0; i < LANDSCAPEITER; i++)
		{
			Vector3 v;
			v.x = 0.5 - (float(rand()) / RAND_MAX);
			v.y = 0.5 - (float(rand()) / RAND_MAX);
			v.z = 0.5 - (float(rand()) / RAND_MAX);
			v.Normalize();

			for(int j = 0; j < pObj[NOBJ].vertices.GetLength(); j++)
			{
				if(pObj[NOBJ].vertices[j].position.Dot(v) < 0)
				{
					pfAng[j] += 2.0f * g_fDegToRad;
				}
				else 
				{
					pfAng[j] -= 2.0f * g_fDegToRad;
				}
			}
		}

		for(int i = 0; i < NOBJ; i++)
		{
			pObj[i].faces = pObj[NOBJ].faces;
			pObj[i].vertices.SetLength(pObj[NOBJ].vertices.GetLength());
			pObj[i].flags.set( Actor::F_DRAW_TRANSPARENT );
			pObj[i].flags.set( Actor::F_NO_CULL );
		}

		camera.position.z = -320;
	}
	Error* Calculate(float br, float elapsed, AudioData* pAudio) override
	{
		fBezPos += elapsed * pAudio->GetIntensity( ) * 0.04;
		camera.position = b.Calculate(fBezPos);
//	float fPos = 0;
//	fPos += elapsed * 3.14159 / 180.0;
//	camera.vPosition = ZVector(0, sin(fPos), cos(fPos)) * 200.0f;
//	camera.PointAt(ZVector::Origin());

		for(int i = 0; i < pObj[NOBJ].vertices.GetLength(); i++)
		{
			pfAng[i] += pAudio->GetIntensity( ) * elapsed * 8.0 * 3.14159 / 180.0;
		}
		for(int i = 0; i < NOBJ; i++)
		{
			float fOfs = i * 60.0 * 3.14159 / 180.0;
			float fMult = 1.0 + (i * 1.0 / NOBJ);
			for(int j = 0; j < pObj[i].vertices.GetLength(); j++)
			{
				pObj[i].vertices[j].position = pObj[NOBJ].vertices[j].position * 200.0f * fMult * (1.0f + ((float)HILLSZ * (float)sin(pfAng[j] + fOfs)));
			}
			pObj[i].pitch += elapsed * 2.0 * PI / 180.0;
			pObj[i].yaw += elapsed * 2.0 * PI / 180.0;
			pObj[i].flags.set( Actor::F_VALID_VERTEX_NORMALS, false );
			float fBlend = float(i) / NOBJ;
			float fMult2 = (i == 0)? 1.0f : std::min(1.0, pAudio->GetIntensity( ) * 1.5);//(averagefloat(i) / NOBJ);
			pObj[i].ambient_light_color = br * (ColorRgb::Grey(96 * fMult2 * (1 - (0.4 * float(i) / NOBJ))) - ColorRgb(fBlend * 40.0, fBlend * 40.0, fBlend * 5.0));
			pObj[i].Calculate(&camera, elapsed);
		}
		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		Texture *tx = g_pD3D->Find(TextureClass::MorphingSphereEnvMap);
		for(int i = 0; i < NOBJ; i++)
		{
			pObj[i].textures[0].Set(Actor::TextureType::Envmap, tx);
		}
		return nullptr;
	}
	Error* Render( ) override
	{
		for(int i = 0; i < NOBJ; i++)
		{
			Error* error = pObj[i].Render( ); 
			if(error) return TraceError(error);
		}

		return nullptr;
	}
};
EXPORT_EFFECT(MorphingSphere, EffectMorphingSphere)
