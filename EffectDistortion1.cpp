#include "Platform.h"
#include "TexturedGrid.h"
#include "AudioData.h"
#include "effect.h"
#include "error.h"
//#include "tripex2.h"

#define GRW 80
#define GRH 50
#define SPIKES 15

class EffectDistortion1 : public EffectBase
{
public:
	bool fSetTexture;
	TexturedGrid grid;//(GRW, GRH);//, D3DFVF_DIFFUSE | D3DFVF_TEX1);
	float xp, yp, t, fac;
	int angle;
	float precalc_atan[(GRW + 1) * (GRH + 1)];
	double br;
	bool fBigBeat;
	double bigbeat;
	double average;
	Texture *tx;

	EffectDistortion1() : grid(GRW, GRH)
	{
		xp = yp = 0;
		t = 0;
		fac = 1;
		angle = 0;

		int i = 0;
		for(int x = 0; x <= GRW; x++)
		{
			for(int y = 0; y <= GRH; y++)
			{
				precalc_atan[i] = SPIKES * atan2((y - (GRH / 2.0)) / GRH, (x - (GRW / 2.0)) / GRW);
				i++;
			}
		}
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		br = brightness;
		xp += 0.015/*08*/ * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0, 0.25f) * elapsed;
		yp += 0.02/*08*/ * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0.25f, 0.5f) * elapsed;
		t += elapsed * std::max(0.5, pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 2/16.0f, 5/16.0f) + (3.0 * pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 7/16.0f, 12/16.0f))) * 20 * 3.14159 / 180.0;

		angle += 1 * elapsed;
		fac = 0.5 + (0.15 * cos(angle * 3.14159 / 256.0));

		int x, y, i = 0;
		double av = std::max(0.4f, pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0, 1.0f));
		double fx, fy;
		double w2 = grid.nWidth / 2.0, h2 = grid.nHeight / 2.0;
		double rw = 1.0 / grid.nWidth, rh = 1.0 / grid.nHeight;

		i = 0;
		ColorRgb cColour = ColorRgb::Grey(255.0 * brightness);
		for(x = 0; x <= grid.nWidth; x++)
		{
			for(y = 0; y <= grid.nHeight; y++)
			{
				fx = (x - w2) * rw;
				fy = (y - h2) * rh;

				double len = 0.75 + (av * 0.25 * cos(precalc_atan[i] + t));//);

				grid.pVertex[i].tex_coords[0].x = (len * fx) + xp;
				grid.pVertex[i].tex_coords[0].y = (len * fy) + yp;
				grid.pVertex[i].diffuse = cColour;
				i++;
			}
		}

		grid.bUpdateEdges = true;
		return nullptr;
	}
	Error* Render() override
	{
		Error* error;
		g_pD3D->SetTexture(0, tx);
		g_pD3D->SetState(ZDirect3D::Transparent);//D3DRS_TRANSPARENT);

		error = grid.Render( );
		if(error) return TraceError(error);

		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		tx = g_pD3D->Find(TC_WTDISTORTION);
//	grid->SetTexture(d3d->Select(TC_WRAPTEXTURE));//TC_ENVIRONMENTMAP));
		return nullptr;
	}
};

EXPORT_EFFECT( Distortion1, EffectDistortion1 )
