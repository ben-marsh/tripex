#include "Platform.h"
#include "VertexGrid.h"
#include "effect.h"
#include "error.h"
#include "BezierCurve.h"
#include "TextureData.h"
//#include "tripex2.h"

template < bool bLight > class EffectDistortion2T : public Effect
{
public:
	const TextureClass background_texture_class =
	{
		"Background",
		bLight ? std::vector<const uint32*>{ tex_flesh } : std::vector<const uint32*>{ tex_eyes, tex_flesh, tex_forest }
	};

	static const int GRW = 80;
	static const int GRH = 50;
	static const int SPIKES = 15;

	static const int NSPIKES = 8;//15
	static const int NCENTRES = 5;

	BezierCurve pb[NCENTRES];
	double pdPos[NCENTRES];

	bool fSetTexture;

	VertexGrid grid;
	float xp, yp, t, fac;
	int angle;
	double pdHeight[GRW][GRH];
	float precalc_atan[(GRW + 1) * (GRH + 1)];

	ColorRgb pc[NCENTRES];
	float pdA[NCENTRES], pdAS[NCENTRES];
	Vector3 pvPos[NCENTRES];
	double br;
	Texture *tx;

	EffectDistortion2T()
		: Effect({ &background_texture_class })
		, grid(GRW, GRH)
	{
		xp = yp = 0;
		t = 0;
		fac = 1;
		angle = 0;

		for(int i = 0; i < NCENTRES; i++)
		{
			pb[i].Create(4);// = new ZBezier(4);
			pdPos[i] = 2 + rand() * 1.0 / RAND_MAX;
			pdA[i] = i * 20.0 * 3.14159 / 180.0;//rand() * (3.14159 * 2.0) / RAND_MAX;
			pdAS[i] = 0.25 * ((rand() * 40.0 / RAND_MAX) - 20) * 3.14159 / 180.0;
			if((i % 3) == 0) pc[i] = ColorRgb(255, 255, 0);
			else if((i % 3) == 1) pc[i] = ColorRgb(255, 0, 255);
			else pc[i] = ColorRgb(0, 255, 255);
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		float elapsed = params.elapsed * 1.5;

		br = params.brightness;
		xp += 0.008 * std::max(0.1f, params.audio_data.GetDampenedBand( sensitivity, 0, 0.2f ) ) * elapsed;
		yp += 0.008 * std::max(0.1f, params.audio_data.GetDampenedBand(sensitivity, 0.15f, 0.5f ) + params.audio_data.GetBeat( ) ) * elapsed;
		t += elapsed * std::max(0.5f, params.audio_data.GetDampenedBand(sensitivity, 0, 1.0f)) * 20 * 3.14159 / 180.0;

		angle += 1 * elapsed;
		fac = 0.5 + (0.15 * cos(angle * 3.14159 / 256.0));

		int x, y, i = 0;
		for(i = 0; i < NCENTRES; i++)
		{
			Vector3 v1, v2;
			for(pdPos[i] += 0.02 * elapsed * (0.3 + (0.7 * params.audio_data.GetIntensity( ))); pdPos[i] >= 1.0; pdPos[i] -= 1.0)
			{
				pb[i][0] = pb[i][3];
				pb[i][1] = pb[i][3] + (pb[i][3] - pb[i][2]);
				pb[i][2] = Vector3(rand() * GRW / RAND_MAX, rand() * GRH / RAND_MAX, 0);
				pb[i][3] = Vector3(rand() * GRW / RAND_MAX, rand() * GRH / RAND_MAX, 0);
			}
			pdA[i] += (params.audio_data.GetIntensity( ) /*+ 0.001*/) * pdAS[i] * elapsed;
			pvPos[i] = pb[i].Calculate(pdPos[i]);
		}

		float pdMax[] = { 0.3f, 0.4f, 0.5f };//min(average, 0.4) / 0.4, max(0, min(1.0, (average - 0.4) / 0.4)) }; 

		i = 0;
		float fChange = 1.0f / std::max(grid.width, grid.height);
		float fPosX = xp;
		const float fMulX = 1.0f / GRW;
		const float fMulY = 1.0f / GRH;
		for(x = 0; x <= grid.width; x++)
		{
			fPosX += fChange;

			float fPosY = yp;
			for(y = 0; y <= grid.height; y++)
			{
				fPosY += fChange;

				float fBr = 0;
				float fX = fPosX;
				float fY = fPosY;

				WideColorRgb cCol(0, 0, 0);
				for(int j = 0; j < NCENTRES; j++)
				{
					float fPosX = x - pvPos[j].x;
					float fPosY = y - pvPos[j].y;
					float fNormX = fPosX * fMulX;//(1.0f / GRW);
					float fNormY = fPosY * fMulY;//(1.0f / GRH);

					float fRad = std::min(1.0, 100.0 / fabs(fPosX * fPosX + fPosY * fPosY));
					fBr += fRad;
					float fAng = atan2(fNormY, fNormX);
					float fLen = fRad * (0.75 + (0.5 * cos((fAng * NSPIKES) + t + pdA[j])));
					fX += fLen * fNormX;
					fY += fLen * fNormY;

					float fColMult = params.brightness * 0.5 * std::max(0.0f, 1.0f - (sqrtf(fPosX * fPosX + fPosY * fPosY) / 30.0f));
					cCol.r += pc[j].r * fColMult;
					cCol.g += pc[j].g * fColMult;
					cCol.b += pc[j].b * fColMult;
				}

				grid.vertices[i].tex_coords[0].x = fX;
				grid.vertices[i].tex_coords[0].y = fY;
				ColorRgb cGrey = ColorRgb::Grey(std::min(1.0f, params.brightness * fBr) * 255.0);
				if(bLight) 
				{
					grid.vertices[i].specular = cCol * params.brightness * fBr * 0.5;
					grid.vertices[i].diffuse =  ColorRgb::Blend(cGrey, (ColorRgb)cCol, 0.2f);//ZColour::Grey(255);
				}
				else 
				{
					grid.vertices[i].diffuse = cGrey;
					grid.vertices[i].specular = ColorRgb::Black();
				}
				i++;
			}
		}
		
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		RenderState render_state;
		render_state.blend_mode = BlendMode::Add;
		render_state.depth_mode = DepthMode::Disable;
		render_state.enable_specular = true;
		render_state.texture_stages[0].texture = tx;

		Error* error = grid.Render(params.renderer, render_state);
		if(error) return TraceError(error);

		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		tx = params.texture_library.Find(background_texture_class);
		return nullptr;
	}
};

EXPORT_EFFECT(Distortion2, EffectDistortion2T< false >)
EXPORT_EFFECT( Distortion2Col, EffectDistortion2T< true > )

//	typedef ZEffectDistortion2T<true> ZEffectDistortion2Col;
//	DECLARE_EFFECT_PTR(ZEffectDistortion2Col, pEffectDistortion2Col)
//
//	typedef ZEffectDistortion2T<false> ZEffectDistortion2;
//	DECLARE_EFFECT_PTR(ZEffectDistortion2, pEffectDistortion2)

