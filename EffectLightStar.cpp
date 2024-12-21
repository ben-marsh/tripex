#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "error.h"

#define SOURCES 512

extern Texture *pBlankTexture;

class EffectLightStar : public EffectBase
{
public:
	Actor obj;
	Camera camera;
	float brt;
	double pdAng[SOURCES];
	double pdSpeed[SOURCES];
	double pdHeight[SOURCES];
	double pdRadius[SOURCES];
	Texture *pTint;
	double dRadAng;
	double dViewAng;
	int nSpikes;

	EffectLightStar()
	{
		dRadAng = 2 * 3.14159 * 2.0;
		dViewAng = 0;
		nSpikes = 8;

		obj.vertices.SetLength(SOURCES);
		obj.flags.set( Actor::F_DRAW_TRANSPARENT );
		obj.flags.set( Actor::F_DRAW_Z_BUFFER, false );
		obj.flags.set( Actor::F_DRAW_VERTEX_SPRITES );
		obj.textures[0].type = Actor::TextureType::Sprite;
		obj.sprite_size = 7.5;//pObj->fRenderAsLights(15.0);
		camera.position.z = -100;

		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] = rand() * (3.14159 * 2.0) / RAND_MAX;
			pdSpeed[i] = rand() * (2.0 * 3.14159 / 180.0) / RAND_MAX;
			pdHeight[i] = 35 - (rand() * 70.0 / RAND_MAX);
			pdRadius[i] = 100 + (rand() * 60.0 / RAND_MAX);
			pdHeight[i] = -10.0 + (rand() * 20.0 / RAND_MAX);
		}
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		brt = brightness;
		dRadAng += (1.0 + (elapsed * (6.0 * pAudio->GetIntensity( )))) * 3.14159 / 128.0;
		if(dRadAng > 3.14159 * 2.0)
		{
			if(pAudio->GetIntensity( ) < 0.2) nSpikes = 0;
			else if(pAudio->GetIntensity( ) < 0.45) nSpikes = 1;
			else if(pAudio->GetIntensity( ) < 0.6) nSpikes = 2;
			else if(pAudio->GetIntensity( ) < 0.75) nSpikes = 3;
			else if(pAudio->GetIntensity( ) < 0.9) nSpikes = 4;
			else nSpikes = 5;

			while(dRadAng > 3.14159 * 2.0) dRadAng -= 3.14159 * 2.0;
		}
		obj.yaw += 1.0f * g_fDegToRad;

		dViewAng += 3.14159 / 180.0;
		camera.position.z = -50;
		camera.position.y = -120;
		//camera.pointAt(0,0,0);
		Vector3 vDir = Vector3::Origin() - camera.position;
		camera.pitch = vDir.GetPitch();
		camera.yaw = vDir.GetYaw();

		double dRadMult = 50.0 * sin(dRadAng);
		for(int i = 0; i < SOURCES; i++)
		{
			pdAng[i] += pdSpeed[i] * elapsed;
			pdRadius[i] = 100 + (dRadMult * sin(pdAng[i] * nSpikes));

			obj.vertices[i].position.x = sin(pdAng[i]) * pdRadius[i];
			obj.vertices[i].position.y = pdHeight[i];
			obj.vertices[i].position.z = cos(pdAng[i]) * pdRadius[i];
		}
		obj.ambient_light_color = ColorRgb::Grey(128.0 * brightness);//->color = D3DRGB(dBr, dBr, dBr);
		obj.Calculate(&camera, elapsed);

		return nullptr;
	}
	Error* Render( )
	{
		Error* error = obj.Render( );
		if(error) return TraceError(error);

		if(pTint != pBlankTexture)
		{
			g_pD3D->SetTexture(0, pTint);
			g_pD3D->ResetRenderState();
			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_pD3D->DrawSprite(Point<int>(0, 0), Rect<int>(0, 0, g_pD3D->GetWidth(), g_pD3D->GetHeight()), ColorRgb::Grey(brt * 255.0));
		}

		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		obj.textures[0].texture = g_pD3D->Find(TC_LBLIGHTSTAR);
		pTint = g_pD3D->Find(TC_WTLIGHTSTAR);
		return nullptr;
	}
};
EXPORT_EFFECT( LightStar, EffectLightStar )
