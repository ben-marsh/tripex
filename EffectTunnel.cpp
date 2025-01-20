#include "Platform.h"
#include "VertexGrid.h"
#include "Actor.h"
#include "BezierCurve.h"
#include "effect.h"
#include "TextureData.h"
#include "error.h"

//static double dPos = 0.9; // start position in the first bezier

class EffectTunnel : public Effect
{
public:
	const TextureClass background_texture_class =
	{
		"Background",
		{ tex_flesh, tex_forest }
	};

	static const int nTunnelR1 = 50;
	static const int nTunnelR2 = 45;
	static const int nBeziers = 2;
	static const int nTunnelL = 30;
	static const int nCrossSection = 10;
	const float fSepZ = 200.0f;
	//#define TRADIUS 50//50//50
	//#define TRADIUS2 45//45//45
	//#define nBeziers 2
	//#define nTunnelL 30
	//#define nCrossSection 10
	//#define ZSEP 200.0
	//50.0//100.0

	const float RANDCH_A1 = (2.0f * 3.14159f / 2.0f);//5.0)
	const float RANDCH_R1 = 900.0f;//400.0

	const float RANDCH_A2 = (3.14159f / 10.0f);
	const float RANDCH_R2 = 100.0f;

	float fPos;
	float fLenPos;
	BezierCurve b[2][nBeziers];
	Actor pObj[2];
	Camera camera;
	int pZ[4];
	Texture *tx;
	bool bFirstRender;
	float fPrevAng;
	float fTBr;

	Vector3 GetBezPos(int j, float fPos)
	{
		for(int i = 0;;i++)
		{
			assert(i < nBeziers);
			if(fPos < 1.0f) return b[j][i].Calculate(fPos);
			fPos--;
		}
	}
	float GetRand(float fMax)
	{
		return ((rand() * fMax) / RAND_MAX) - (fMax / 2.0f);
	}

	
	EffectTunnel()
		: Effect({ &background_texture_class })
	{
		fPos = 0.9f;
		fPrevAng = 0;
		fTBr = 0;
		bFirstRender = true;

		pZ[0] = (int)(fSepZ * 0);
		pZ[1] = (int)(fSepZ * 1);
		pZ[2] = (int)(fSepZ * 3);
		pZ[3] = (int)(fSepZ * 4);

		for(int no = 0; no < 2; no++)
		{
			for(int i = 0; i < nBeziers; i++)
			{
				b[no][i].Create(4);
				for(int k = 0; k < 4; k++)
				{
					b[no][i][k] = Vector3(0.0f, 0.0f, (float)pZ[k]);
				}
			}

			pObj[no].flags.set(Actor::F_DRAW_TRANSPARENT);

			for(int j = 0; j < nTunnelL - 1; j++)
			{
				int nIndex = j * nCrossSection;
				for(int i = 0; i < nCrossSection - 1; i++)
				{
					pObj[no].faces.push_back(Face(nIndex + i, nIndex + (i + 1), nIndex + nCrossSection + i));
					pObj[no].faces.push_back(Face(nIndex + (i + 1), nIndex + nCrossSection + (i + 1), nIndex + nCrossSection + i));
				}
			}
			pObj[no].vertices.resize(nTunnelL * nCrossSection);

			int k = 0;
			for(int j = 0; j < nTunnelL; j++)
			{
				for(int i = 0; i < nCrossSection; i++)
				{
					pObj[no].vertices[k].tex_coord[0].x = /*(no / 2.0) +*/ float(i) / float(nCrossSection - 1);
					pObj[no].vertices[k].tex_coord[0].y = /*(no / 2.0) +*/ float(j) / float(nTunnelL);
					k++;
				}
			}
		}
	}
	Error* Calculate(const CalculateParams& params) override
	{
		Vector3 pvPos[4];
		float fPosChange = 0.015f * params.audio_data.GetIntensity() * params.elapsed;
		static float fTexVOfs = 3;//0.99;1.5;//0.99;
		fTexVOfs += 0.003f * fTBr * params.elapsed;
		while(fTexVOfs > 0) fTexVOfs--;
		if (params.audio_data.GetIntensity() > fTBr)
		{
			fTBr += std::min(params.audio_data.GetIntensity() - fTBr, 0.05f) * params.elapsed;
		}
		if (params.audio_data.GetIntensity() < fTBr)
		{
			fTBr += std::max(params.audio_data.GetIntensity() - fTBr, -0.05f) * params.elapsed;
		}

		bool fReset = false;
		if(bFirstRender)
		{
			fPos += 2;
			bFirstRender = false;
		}
		else fPos += fPosChange;

		for(; fPos >= 1.0; fPos -= 1)
		{
			float fAng1 = fPrevAng + GetRand(RANDCH_A1 * params.audio_data.GetIntensity( ));
			float fAng2 = fAng1 + GetRand(RANDCH_A2 * params.audio_data.GetIntensity( ));
			fPrevAng = fAng2;// + 3.14159;
			for(int j = 0; j < 2; j++)
			{
				for(int i = 0; i < nBeziers - 1; i++)
				{
					Vector3 v = b[j][i+1][0];
					for(int k = 0; k < 4; k++)
					{
						pvPos[k] = b[j][i+1][k];
						pvPos[k].z -= v.z;
						b[j][i][k] = pvPos[k];
					}
				}

				// 0
				pvPos[0] = pvPos[3];

				// 1
				pvPos[1] = pvPos[3] + (pvPos[3] - pvPos[2]);

				if(j == 1)
				{
					pvPos[2] = b[0][nBeziers-1][2];
					pvPos[3] = b[0][nBeziers-1][3];
				}
				else
				{
					pvPos[2].x = pvPos[1].x + (cosf(fAng1) * RANDCH_R1);
					pvPos[2].y = pvPos[1].y + (sinf(fAng1) * RANDCH_R1);
					pvPos[2].z = pvPos[1].z + (fSepZ * 2);//cos(dAng1) * RANDCH_R1);

					pvPos[3].x = pvPos[2].x + (cosf(fAng2) * RANDCH_R2);
					pvPos[3].y = pvPos[2].y + (sinf(fAng2) * RANDCH_R2);
					pvPos[3].z = pvPos[2].z + (fSepZ * 3);//cos(dAng1) * RANDCH_R1);
				}

				for(int k = 0; k < 4; k++)
				{
					b[j][nBeziers - 1][k] = pvPos[k];
				}
			}
			fReset = true;
		}

		int nStart = (int)(fPos * nTunnelL);
		int nCalc = 0;//fReset? 0 : max(0, (nTunnelL - max(0, nStart - nOldStart)));
		if(nCalc > 0 && nCalc < nTunnelL)
		{
			for(int no = 0; no < 2; no++)
			{
				memmove(&pObj[no].vertices[0].position, &pObj[no].vertices[nCrossSection * (nTunnelL - nCalc)].position, nCalc * nCrossSection * sizeof(Vertex));//pObj[no].pVertex.GetItemSize());
			}
		}

		for(int i = 0; i < nTunnelL; i++)
		{
			float fThisBr = ((nStart + i + 1) - (fPos * nTunnelL)) / float(nTunnelL + 1);
			fThisBr = std::min(1.0f, std::max(0.0f, (0.5f / fThisBr ) - 0.5f));
			fThisBr = 1.0f - powf(1.0f - fThisBr, 5.0f);//1.6);

			fThisBr = 1.0f;

			ColorRgb c = ColorRgb::Grey((int)(255.0f * fThisBr * params.brightness));
			ColorRgb c2 = ColorRgb::Grey((int)(255.0f * fThisBr * params.brightness));

			float fTexV = float(nStart + i) / float(nTunnelL);
			int nIndex = i * nCrossSection;
			for(int j = 0; j < nCrossSection; j++)
			{
				pObj[0].vertices[nIndex].diffuse = c;
				pObj[0].vertices[nIndex].tex_coord[0].y = fTexV;//fvVertex.GetTexture().fV = fTexV;
				nIndex++;
			}
			pObj[0].flags.set(Actor::F_VALID_VERTEX_DIFFUSE);
	//		fvVertex = pObj[1].pVertex[i * nCrossSection];
			nIndex = i * nCrossSection;
			for(int j = 0; j < nCrossSection; j++)
			{
				pObj[1].vertices[nIndex].diffuse = c2;//.GetDiffuse() = c2;
				pObj[1].vertices[nIndex].tex_coord[0].y = fTexV + fTexVOfs;
				nIndex++;
	//			fvVertex++;
			}
			pObj[1].flags.set(Actor::F_VALID_VERTEX_DIFFUSE);
		}
		for(; nCalc < nTunnelL; nCalc++)
		{
			int nPos = nStart + nCalc;

			for(int no = 0; no < 2; no++)
			{
				float fRadius = (float)((no == 0)? nTunnelR1 : nTunnelR2);
				Vector3 v = GetBezPos(0/*no*/, float(nPos) / nTunnelL);

				//Vector3 n, v[3];
				Vector3 v2 = GetBezPos(0/*no*/, float(nPos + 0.1f) / float(nTunnelL));//, &xp2, &yp2, &zp2);
				Vector3 n = (v2 - v).Normal(1.0);
				pvPos[0] = Vector3(n.z, n.y, -n.x);
				pvPos[1] = Vector3(-n.y, n.z, n.x);
				pvPos[2] = Vector3(n.x, n.y, n.z);

				int nIndex = nCalc * nCrossSection;
				for(int j = 0; j < nCrossSection; j++)
				{
					float fAng = j * PI2 / (nCrossSection - 1);
					float fX = cosf(fAng);
					float fY = sinf(fAng);

					Vector3 vn;
					vn.x = -((pvPos[0].x * fX) + (pvPos[1].x * fY));
					vn.y = -((pvPos[0].y * fX) + (pvPos[1].y * fY));
					vn.z = -((pvPos[0].z * fX) + (pvPos[1].z * fY));

					pObj[no].vertices[nIndex].position = v - (vn * fRadius);
					nIndex++;
				}
			}
		}

		camera.position = GetBezPos(0, fPos + 0.2f);

		Vector3 vDir = GetBezPos(0, fPos + 0.6f) - camera.position;
		camera.pitch = vDir.GetPitch();
		camera.yaw = vDir.GetYaw();

		for(int i = 0; i < 2; i++)
		{
			pObj[i].Calculate(params.renderer, &camera, params.elapsed);
		}
		return nullptr;
	}
	Error* Reconfigure(const ReconfigureParams& params) override
	{
		tx = params.texture_library.Find(background_texture_class);
		pObj[0].textures[0].Set(Actor::TextureType::Normal, tx);//SetTexture(tx);
		pObj[1].textures[0].Set(Actor::TextureType::Normal, tx);///.pTexture = tx;//SetTexture(tx);
		return nullptr;
	}
	Error* Render(const RenderParams& params) override
	{
		for(int i = 0; i < 2; i++)
		{
			Error* error = pObj[i].Render(params.renderer);
			if(error) return TraceError(error);
		}
		return nullptr;
	}
};

EXPORT_EFFECT( Tunnel, EffectTunnel )
