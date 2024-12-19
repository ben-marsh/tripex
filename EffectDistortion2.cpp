#include "Platform.h"
#include "TexturedGrid.h"
#include "effect.h"
#include "error.h"
#include "BezierCurve.h"
//#include "tripex2.h"

#define GRW 80
#define GRH 50
#define SPIKES 15

#define NSPIKES 8//15
#define NCENTRES 5

template < bool bLight > class EffectDistortion2T : public EffectBase
{
public:
	BezierCurve pb[NCENTRES];
	double pdPos[NCENTRES];

	bool fSetTexture;

	TexturedGrid grid;
	float xp, yp, t, fac;
	int angle;
	double pdHeight[GRW][GRH];
	float precalc_atan[(GRW + 1) * (GRH + 1)];

	ColorRgb pc[NCENTRES];
	float pdA[NCENTRES], pdAS[NCENTRES];
	Vector3 pvPos[NCENTRES];
	double br;
	Texture *tx;

	EffectDistortion2T() : grid(GRW, GRH)
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
	Error* Calculate(float brightness, float elapsed, AudioData* g_pAudio) override
	{
		elapsed *= 1.5;

		br = brightness;
		xp += 0.008 * std::max(0.1f, g_pAudio->GetDampenedBand( pEffectPtr->fSensitivity, 0, 0.2f ) ) * elapsed;
		yp += 0.008 * std::max(0.1f, g_pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0.15f, 0.5f ) + g_pAudio->GetBeat( ) ) * elapsed;
		t += elapsed * std::max(0.5f, g_pAudio->GetDampenedBand(pEffectPtr->fSensitivity, 0, 1.0f)) * 20 * 3.14159 / 180.0;

		angle += 1 * elapsed;
		fac = 0.5 + (0.15 * cos(angle * 3.14159 / 256.0));

		int x, y, i = 0;
		for(i = 0; i < NCENTRES; i++)
		{
			Vector3 v1, v2;
			for(pdPos[i] += 0.02 * elapsed * (0.3 + (0.7 * g_pAudio->GetIntensity( ))); pdPos[i] >= 1.0; pdPos[i] -= 1.0)
			{
				pb[i][0] = pb[i][3];
				pb[i][1] = pb[i][3] + (pb[i][3] - pb[i][2]);
				pb[i][2] = Vector3(rand() * GRW / RAND_MAX, rand() * GRH / RAND_MAX, 0);
				pb[i][3] = Vector3(rand() * GRW / RAND_MAX, rand() * GRH / RAND_MAX, 0);
			}
			pdA[i] += (g_pAudio->GetIntensity( ) /*+ 0.001*/) * pdAS[i] * elapsed;
			pvPos[i] = pb[i].Calculate(pdPos[i]);
		}

		double av = g_pAudio->GetIntensity( );//((average * average) + average) / 2;
		double w2 = grid.nWidth / 2.0, h2 = grid.nHeight / 2.0;
		double rw = 1.0 / grid.nWidth, rh = 1.0 / grid.nHeight;

		float pdMax[] = { 0.3f, 0.4f, 0.5f };//min(average, 0.4) / 0.4, max(0, min(1.0, (average - 0.4) / 0.4)) }; 

		i = 0;
		float fChange = 1.0f / std::max(grid.nWidth, grid.nHeight);
		float fPosX = xp;
		const float fMulX = 1.0f / GRW;
		const float fMulY = 1.0f / GRH;
		for(x = 0; x <= grid.nWidth; x++)
		{
			fPosX += fChange;

			float fPosY = yp;
			for(y = 0; y <= grid.nHeight; y++)
			{
				fPosY += fChange;

				float fBr = 0;
				float fX = fPosX;
				float fY = fPosY;

				WideColorRgb cCol(0, 0, 0);
				for(int j = 0; j < NCENTRES; j++)
				{
					float fPosX = x - pvPos[j].m_fX;
					float fPosY = y - pvPos[j].m_fY;
					float fNormX = fPosX * fMulX;//(1.0f / GRW);
					float fNormY = fPosY * fMulY;//(1.0f / GRH);

					float fRad = std::min(1.0, 100.0 / fabs(fPosX * fPosX + fPosY * fPosY));
					fBr += fRad;
					float fAng = atan2(fNormY, fNormX);
					float fLen = fRad * (0.75 + (0.5 * cos((fAng * NSPIKES) + t + pdA[j])));
					fX += fLen * fNormX;
					fY += fLen * fNormY;

					float fColMult = brightness * 0.5 * std::max(0.0f, 1.0f - (sqrtf(fPosX * fPosX + fPosY * fPosY) / 30.0f));
					cCol.m_nR += pc[j].m_nR * fColMult;
					cCol.m_nG += pc[j].m_nG * fColMult;
					cCol.m_nB += pc[j].m_nB * fColMult;
				}

				grid.pVertex[i].m_aTex[0].x = fX;
				grid.pVertex[i].m_aTex[0].y = fY;
				ColorRgb cGrey = ColorRgb::Grey(std::min(1.0f, brightness * fBr) * 255.0);
				if(bLight) 
				{
					grid.pVertex[i].m_cSpecular = cCol * brightness * fBr * 0.5;
					grid.pVertex[i].m_cDiffuse =  ColorRgb::Blend(cGrey, (ColorRgb)cCol, 0.2f);//ZColour::Grey(255);
				}
				else 
				{
					grid.pVertex[i].m_cDiffuse = cGrey;
					grid.pVertex[i].m_cSpecular = ColorRgb::Black();
				}
				i++;
			}
		}
		
		return nullptr;
	}
	Error* Render( )
	{
		Error* error;

		g_pD3D->SetState(ZDirect3D::Transparent | ZDirect3D::Shade);
		g_pD3D->SetRenderState(D3DRS_SPECULARENABLE, true);

		g_pD3D->SetTexture(0, tx);

		error = grid.Render( );
		if(error) return TraceError(error);

		return nullptr;
	}
	Error* Reconfigure(AudioData* pAudio) override
	{
		tx = g_pD3D->Find(bLight? TC_WTDISTORTION2COL : TC_WTDISTORTION2);
		return nullptr;
	}
};

EXPORT_EFFECT( Distortion2, EffectDistortion2T< false > )
EXPORT_EFFECT( Distortion2Col, EffectDistortion2T< true > )

//	typedef ZEffectDistortion2T<true> ZEffectDistortion2Col;
//	DECLARE_EFFECT_PTR(ZEffectDistortion2Col, pEffectDistortion2Col)
//
//	typedef ZEffectDistortion2T<false> ZEffectDistortion2;
//	DECLARE_EFFECT_PTR(ZEffectDistortion2, pEffectDistortion2)

