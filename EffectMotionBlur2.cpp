#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "BezierCurve.h"
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

class EffectMotionBlur2 : public EffectBase
{
public:
	Actor pObj[PIPES];
	Camera camera;//World *pScene;
	ContainedBezierCurve<1> b;
	float fBezPos;
	float fTime;
	float pf[2][3], pfa[3];
	float fChange;

	EffectMotionBlur2() : b(Vector3(-100, -100, -20), Vector3(100, 100, 100))
	{
		for(int i = 0; i < 3; i++) pf[1][i] = 0.0f;

		fChange = 0;
		fTime = UPDATETIME + 1;
		fBezPos = 0;
		for(int p = 0; p < PIPES; p++)
		{
			pObj[p].CreateTetrahedron(100);
//		pObj[p].pVertex.SetFormat(D3DFVF_NORMAL | D3DFVF_TEX1);
			pObj[p].vertices.resize(nVertices);
			pObj[p].faces.resize(PIPE_LENGTH * PIPE_CIRCUMPOINTS * 2);
			pObj[p].flags.set( Actor::F_NO_CULL );
			pObj[p].flags.set( Actor::F_DRAW_TRANSPARENT );
			pObj[p].frame_history = 4.0f;
			pObj[p].exposure = 6;
			pObj[p].exposure_light_delta = WideColorRgb(-10, -10, -1);

			int v = 0, f = 0;
			for(int i = 0; i < PIPE_LENGTH; i++)
			{
				double dAng = (i * 2.0 * 3.14159 / PIPE_LENGTH);
				double dTwistAng = (i * PIPE_TWISTS * 2.0 * 3.14159 / PIPE_LENGTH) + (p * 2.0 * 3.14159 / PIPES);
				for(int cp = 0; cp < PIPE_CIRCUMPOINTS; cp++)
				{
					double dCircumAng = cp * 2.0 * 3.14159 / PIPE_CIRCUMPOINTS;

					Vector3 vec;
//				Vector3 vec;
					vec.x = 0;//(cp - (cp / 2)) * 20;
					vec.y = PIPE_TWISTRADIUS + (PIPE_RADIUS * cos(dCircumAng));
					vec.z = PIPE_TWISTRADIUS + (PIPE_RADIUS * sin(dCircumAng));
					vec *= Matrix44::RotateAroundX(dTwistAng);
					vec.y += CIRCLE_RADIUS;
					vec *= Matrix44::RotateAroundZ(dAng);// = RotateVector3(vec, dAng, 0, 0);
					pObj[p].vertices[v + cp].position = vec;

					pObj[p].faces[f][0] = v + cp;
					pObj[p].faces[f][1] = v + ((cp + 1) % PIPE_CIRCUMPOINTS);
					pObj[p].faces[f][2] = (v + cp + PIPE_CIRCUMPOINTS) % nVertices;
					f++;

					pObj[p].faces[f][0] = v + ((cp + 1) % PIPE_CIRCUMPOINTS);
					pObj[p].faces[f][1] = (v + ((cp + 1) % PIPE_CIRCUMPOINTS) + PIPE_CIRCUMPOINTS) % nVertices;
					pObj[p].faces[f][2] = (v + cp + PIPE_CIRCUMPOINTS) % nVertices;
					f++;
				}
				v += PIPE_CIRCUMPOINTS;
			}
			pObj[p].FindVertexNormals();
		}
		camera.position.z = -130;
	}
	Error* Calculate(const CalculateParams& params) override
	{
		camera.position = b.Calculate(fBezPos);
		camera.SetTarget(Vector3::Origin());
		fBezPos += 0.02 * params.audio_data.GetIntensity( ) * params.elapsed;

		fTime += params.elapsed;
		fChange = std::min(1.0f, fChange + (params.elapsed / 3.0f));

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
			pObj[i].ambient_light_color = ColorRgb::Grey(60.0 * params.brightness);
			pObj[i].roll += pfa[0] * params.elapsed * (params.audio_data.GetIntensity( ) + 0.1);
			pObj[i].pitch += pfa[1] * params.elapsed * params.audio_data.GetIntensity( ) ;
			pObj[i].yaw += pfa[2] * params.elapsed * (params.audio_data.GetIntensity( ) + 0.2);
			pObj[i].Calculate(params.renderer, &camera, params.elapsed);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		Texture *pTexture = params.texture_library.Find(TextureClass::MotionBlur2EnvMap);//2);
		for(int i = 0; i < PIPES; i++)
		{
			pObj[i].textures[0].type = Actor::TextureType::Lightmap;
			pObj[i].textures[0].texture = pTexture;
		}
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		for(int i = 0; i < PIPES; i++)
		{
			Error* error = pObj[i].Render(params.renderer);
			if(error) return TraceError(error);
		}
		return nullptr;
	}
};

EXPORT_EFFECT(MotionBlur2, EffectMotionBlur2)
