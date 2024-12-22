#include "Platform.h"
#include "Actor.h"
#include "effect.h"

#define PTDIST 15
#define PTCIRCUM 40
#define PTLENGTH 20

#define HEIGHTST 30
#define HEIGHTM 0.08f

#define CIRADIUS 40.0f
#define CORADIUS 50.0f
#define CANGDIFF (5.0f * g_fDegToRad)
#define CLENGTH 100
#define CDIFF (PTLENGTH * PTDIST / CLENGTH)

class EffectTube : public EffectBase
{
public:
	float height[PTLENGTH][PTCIRCUM];
	bool fRefresh;
	float accwf[PTCIRCUM];

	Actor obj, coil;
	Camera cCamera;

	EffectTube()
	{
		int i, j;
		coil.vertices.SetLength(CLENGTH * 4);
		coil.faces.SetLength((CLENGTH - 1) * 8);
		coil.flags.set(Actor::F_DRAW_TRANSPARENT);
		coil.flags.set(Actor::F_DRAW_Z_BUFFER);
		coil.flags.set(Actor::F_NO_CULL);

		int v = 0, f = 0;
		for(i = 0; i < CLENGTH; i++)
		{
			float angle = i * 4.0f * PI2 / CLENGTH;

			coil.vertices[v].position.x = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.vertices[v].position.y = CORADIUS * sinf(angle - CANGDIFF);
			coil.vertices[v].position.z = CORADIUS * cosf(angle - CANGDIFF);
			v++;

			coil.vertices[v].position.x = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.vertices[v].position.y = CORADIUS * sinf(angle + CANGDIFF);
			coil.vertices[v].position.z = CORADIUS * cosf(angle + CANGDIFF);
			v++;

			coil.vertices[v].position.x = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.vertices[v].position.y = CIRADIUS * sinf(angle + CANGDIFF);
			coil.vertices[v].position.z = CIRADIUS * cosf(angle + CANGDIFF);
			v++;

			coil.vertices[v].position.x = (i - (CLENGTH * 0.5f)) * CDIFF;
			coil.vertices[v].position.y = CIRADIUS * sinf(angle - CANGDIFF);
			coil.vertices[v].position.z = CIRADIUS * cosf(angle - CANGDIFF);
			v++;
		}
		for(i = 0; i < CLENGTH - 1; i++)
		{
			for(j = 0; j < 4; j++)
			{
				coil.faces[f][0] = (i * 4) + j;
				coil.faces[f][2] = ((i + 1) * 4) + j;
				coil.faces[f][1] = (i * 4) + ((j + 1) % 4);
				f++;

				coil.faces[f][0] = ((i + 1) * 4) + j;
				coil.faces[f][2] = ((i + 1) * 4) + ((j + 1) % 4);
				coil.faces[f][1] = (i * 4) + ((j + 1) % 4);
				f++;
			}
		}
		_ASSERT(f == coil.faces.GetLength());
//	coil.FindVertexNormals();

		obj.vertices.SetLength(PTCIRCUM * PTLENGTH);
		obj.faces.SetLength((PTCIRCUM * 2) * (PTLENGTH - 1));
		obj.flags.set( Actor::F_DRAW_Z_BUFFER );
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );

		f = 0;
		for(i = 0; i < PTLENGTH - 1; i++)
		{
			for(j = 0; j < PTCIRCUM; j++)
			{
/*
				obj->face[f].v[0] = (i * PTCIRCUM) + j;
				obj->face[f].v[1] = ((i + 1) * PTCIRCUM) + j;//(i + 1) * PTCIRCUM) + j;
				obj->face[f].v[2] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;

				obj->face[f].v[0] = ((i + 1) * PTCIRCUM) + j;
				obj->face[f].v[1] = ((i + 1) * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				obj->face[f].v[2] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;
*/
				obj.faces[f][0] = (i * PTCIRCUM) + j;
				obj.faces[f][2] = ((i + 1) * PTCIRCUM) + j;//(i + 1) * PTCIRCUM) + j;
				obj.faces[f][1] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;

				obj.faces[f][0] = ((i + 1) * PTCIRCUM) + j;
				obj.faces[f][2] = ((i + 1) * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				obj.faces[f][1] = (i * PTCIRCUM) + ((j + 1) % PTCIRCUM);
				f++;
			}
		}
		for(i = 0; i < PTLENGTH; i++)
		{
			float br = std::min(1.0f, sinf(i * PI / PTLENGTH) * 1);

			for(j = 0; j < PTCIRCUM; j++)
			{
				obj.vertices[(i * PTCIRCUM) + j].diffuse = ColorRgb::Grey((int)(br * 255.0f));
			}
		}
		obj.flags.set(Actor::F_VALID_VERTEX_DIFFUSE);
		coil.flags.set(Actor::F_VALID_VERTEX_DIFFUSE);
		_ASSERT(f == obj.faces.GetLength());

//	scene = new World();
//	scene->vpObject.Add(obj);
//	scene->vpObject.Add(coil);
		cCamera.position.z = -120;//120;
	}
	Error* Calculate(const CalculateParams& params) override
	{
		int i, j;
		for(i = 0; i < 128; i++)
		{
			accwf[i * PTCIRCUM / 128] += params.audio_data.GetSample(i*4) * 128.0f * 4.0f * params.elapsed * (PTCIRCUM / 128.0f);
		}

		static double accum = 1;
		accum += params.elapsed;
		double dChange = accum;

		bool fChanged = false;
		while(accum > 1.0)
		{
			for(i = 0; i < CLENGTH; i++)
			{
				double br = std::min(1.0, sin(i * 3.14159 / CLENGTH) * 5) * params.brightness;
				for(j = 0; j < 4; j++)
				{
					coil.vertices[(i * 4) + j].diffuse = ColorRgb::Grey((int)(255.0f * br));
				}
			}
			for(i = 0; i < PTLENGTH; i++)
			{
				double br = std::min(1.0, sin(i * 3.14159 / PTLENGTH) * 1) * params.brightness;
	
				for(j = 0; j < PTCIRCUM; j++)
				{
					obj.vertices[(i * PTCIRCUM) + j].diffuse = ColorRgb::Grey((int)(255.0f * br));
				}
			}

			for(i = PTLENGTH - 1; i > 0; i--)
			{
				for(j = 0; j < PTCIRCUM; j++) height[i][j] = height[i-1][j];
			}

			for(j = 0; j < PTCIRCUM; j++) height[0][j] = HEIGHTST + (accwf[j] * HEIGHTM);
//waveform[j * 127 / PTCIRCUM]
			int k = 0;
			for(i = 0; i < PTLENGTH; i++)
			{
				float fPos = 1.0f - fabs(((PTLENGTH / 2) - i) / (PTLENGTH / 2.0f));
				float fLenMult = 0.8f + 0.4f * sin(fPos * PI / 2.0f);
				for(j = 0; j < PTCIRCUM; j++)
				{
					float angle = j * PI2 / PTCIRCUM;

					obj.vertices[k].position.x = (i - (PTLENGTH*0.5f)) * PTDIST;
					obj.vertices[k].position.y = height[i][j] * fLenMult * sinf(angle);	
					obj.vertices[k].position.z = height[i][j] * fLenMult * cosf(angle);

					k++;
				}
			}	

			obj.pitch += 2.0f * g_fDegToRad;//2 * 3.14159 / 180.0;
			obj.yaw += 1.5f * g_fDegToRad;//1.5 * 3.14159 / 180.0;
			coil.pitch += 2.0f * g_fDegToRad;//2 * 3.14159 / 180.0;
			coil.yaw += 1.5f * g_fDegToRad;//1.5 * 3.14159 / 180.0;

			accum--;
			obj.flags.set(Actor::F_VALID_VERTEX_NORMALS, false);
//			fChanged = true;
		}
//	if(fChanged)
//	{
//		obj.FindVertexNormals();
//		coil.FindVertexNormals();
		//		obj->NormalizeFaces();
//		obj->Normalize();
//	}
//	obj->x = accum * PTDIST;
		for(i = 0; i < PTCIRCUM; i++) accwf[i] = 0;

		obj.Calculate(&cCamera, params.elapsed);
		coil.Calculate(&cCamera, params.elapsed);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		Error* error;

//	obj->property(objTransparent, true);//fDrawn);
//	obj->property(objZBuffer, false);//fDrawn);
//	coil->property(objTransparent, true);
//	coil->property(objZBuffer, false);

		error = obj.Render();
		if(error) return TraceError(error);

		error = coil.Render();
		if(error) return TraceError(error);

		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		Texture *tx = params.texture_library.Find(TextureClass::TubeEnvMap);
		coil.textures[0].Set(Actor::TextureType::Envmap, tx);
		obj.textures[0].Set(Actor::TextureType::Envmap, tx);

//	coil->SetTexture(tx);
//	obj->SetTexture(tx);

		for(int i = 0; i < PTLENGTH; i++)
		{
			for (int j = 0; j < PTCIRCUM; j++)
			{
				height[i][j] = 0;
			}
		}
		return nullptr;
	}
};

EXPORT_EFFECT(Tube, EffectTube)
