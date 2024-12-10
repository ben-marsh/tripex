#include "StdAfx.h"
#include "ZGrid.h"
#include "ZObject.h"
#include "ZBezier.h"
#include "effect.h"
#include "error.h"

static const int nTunnelR1 = 50;
static const int nTunnelR2 = 45;
static const int nBeziers = 2;
static const int nTunnelL = 30;
static const int nCrossSection = 10;
static const float fSepZ = 200.0f;
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

//static double dPos = 0.9; // start position in the first bezier

class ZEffectTunnel : public ZEffectBase
{
public:
	float fPos;
	float fLenPos;
	ZBezier b[2][nBeziers];
	ZObject pObj[2];
	ZCamera camera;
	int pZ[4];
	ZTexture *tx;
	bool bFirstRender;
	float fPrevAng;
	float fTBr;

	ZVector GetBezPos(int j, float fPos)
	{
		for(int i = 0;;i++)
		{
			_ASSERT(i < nBeziers);
			if(fPos < 1.0f) return b[j][i].Calculate(fPos);
			fPos--;
		}
	}
	float GetRand(float fMax)
	{
		return ((rand() * fMax) / RAND_MAX) - (fMax / 2.0f);
	}

	
	ZEffectTunnel()
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
					b[no][i][k] = ZVector(0.0f, 0.0f, (float)pZ[k]);
				}
			}

			pObj[no].m_bsFlag.set(ZObject::F_DRAW_TRANSPARENT);

			int f = 0;
			for(int j = 0; j < nTunnelL - 1; j++)
			{
				int nIndex = j * nCrossSection;
				for(int i = 0; i < nCrossSection - 1; i++)
				{
					pObj[no].pFace.Add(ZFace(nIndex + i, nIndex + (i + 1), nIndex + nCrossSection + i)); 
					pObj[no].pFace.Add(ZFace(nIndex + (i + 1), nIndex + nCrossSection + (i + 1), nIndex + nCrossSection + i)); 
				}
			}
			pObj[no].pVertex.SetLength(nTunnelL * nCrossSection);

			int k = 0;
			for(j = 0; j < nTunnelL; j++)
			{
				for(int i = 0; i < nCrossSection; i++)
				{
					pObj[no].pVertex[k].m_aTex[0].x = /*(no / 2.0) +*/ float(i) / float(nCrossSection - 1);
					pObj[no].pVertex[k].m_aTex[0].y = /*(no / 2.0) +*/ float(j) / float(nTunnelL);
					k++;
				}
			}
		}
	}
	HRESULT Calculate(float fBr, float fElapsed)
	{
		ZVector pvPos[4];
		float fPosChange = 0.015f * g_pAudio->GetIntensity( ) * fElapsed;
		static float fTexVOfs = 3;//0.99;1.5;//0.99;
		fTexVOfs += 0.003f * fTBr * fElapsed;
		while(fTexVOfs > 0) fTexVOfs--;
		if(g_pAudio->GetIntensity( ) > fTBr) fTBr += min(g_pAudio->GetIntensity( ) - fTBr, 0.05f) * fElapsed;
		if(g_pAudio->GetIntensity( ) < fTBr) fTBr += max(g_pAudio->GetIntensity( ) - fTBr, -0.05f) * fElapsed;

		int nOldStart = (int)(fPos * nTunnelL);
		bool fReset = false;
		if(bFirstRender)
		{
			fPos += 2;
			bFirstRender = false;
		}
		else fPos += fPosChange;

		for(; fPos >= 1.0; fPos -= 1)
		{
			float fAng1 = fPrevAng + GetRand(RANDCH_A1 * g_pAudio->GetIntensity( ));
			float fAng2 = fAng1 + GetRand(RANDCH_A2 * g_pAudio->GetIntensity( ));
			fPrevAng = fAng2;// + 3.14159;
			for(int j = 0; j < 2; j++)
			{
				for(int i = 0; i < nBeziers - 1; i++)
				{
					ZVector v = b[j][i+1][0];
					for(int k = 0; k < 4; k++)
					{
						pvPos[k] = b[j][i+1][k];
						pvPos[k].m_fZ -= v.m_fZ;
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
					pvPos[2].m_fX = pvPos[1].m_fX + (cosf(fAng1) * RANDCH_R1);
					pvPos[2].m_fY = pvPos[1].m_fY + (sinf(fAng1) * RANDCH_R1);
					pvPos[2].m_fZ = pvPos[1].m_fZ + (fSepZ * 2);//cos(dAng1) * RANDCH_R1);

					pvPos[3].m_fX = pvPos[2].m_fX + (cosf(fAng2) * RANDCH_R2);
					pvPos[3].m_fY = pvPos[2].m_fY + (sinf(fAng2) * RANDCH_R2);
					pvPos[3].m_fZ = pvPos[2].m_fZ + (fSepZ * 3);//cos(dAng1) * RANDCH_R1);
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
				memmove(&pObj[no].pVertex[0].m_vPos, &pObj[no].pVertex[nCrossSection * (nTunnelL - nCalc)].m_vPos, nCalc * nCrossSection * sizeof(ZVertex));//pObj[no].pVertex.GetItemSize());
			}
		}

		for(int i = 0; i < nTunnelL; i++)
		{
			float fThisBr = ((nStart + i + 1) - (fPos * nTunnelL)) / float(nTunnelL + 1);
			fThisBr = min(1.0f, max(0.0f, (0.5f / fThisBr ) - 0.5f));
			fThisBr = 1.0f - powf(1.0f - fThisBr, 5.0f);//1.6);

			fThisBr = 1.0f;

			ZColour c = ZColour::Grey((int)(255.0f * fThisBr * fBr));
			ZColour c2 = ZColour::Grey((int)(255.0f * fThisBr * fBr));

			float fTexV = float(nStart + i) / float(nTunnelL);
			int nIndex = i * nCrossSection;
			for(int j = 0; j < nCrossSection; j++)
			{
				pObj[0].pVertex[nIndex].m_cDiffuse = c;
				pObj[0].pVertex[nIndex].m_aTex[0].y = fTexV;//fvVertex.GetTexture().fV = fTexV;
				nIndex++;
			}
			pObj[0].m_bsFlag.set(ZObject::F_VALID_VERTEX_DIFFUSE);
	//		fvVertex = pObj[1].pVertex[i * nCrossSection];
			nIndex = i * nCrossSection;
			for(j = 0; j < nCrossSection; j++)
			{
				pObj[1].pVertex[nIndex].m_cDiffuse = c2;//.GetDiffuse() = c2;
				pObj[1].pVertex[nIndex].m_aTex[0].y = fTexV + fTexVOfs;
				nIndex++;
	//			fvVertex++;
			}
			pObj[1].m_bsFlag.set(ZObject::F_VALID_VERTEX_DIFFUSE);
		}
		for(; nCalc < nTunnelL; nCalc++)
		{
			int nPos = nStart + nCalc;

			for(int no = 0; no < 2; no++)
			{
				float fRadius = (float)((no == 0)? nTunnelR1 : nTunnelR2);
				ZVector v = GetBezPos(0/*no*/, float(nPos) / nTunnelL);

				//Vector3 n, v[3];
				ZVector v2 = GetBezPos(0/*no*/, float(nPos + 0.1f) / float(nTunnelL));//, &xp2, &yp2, &zp2);
				ZVector n = (v2 - v).Normal(1.0);
				pvPos[0] = ZVector(n.m_fZ, n.m_fY, -n.m_fX);
				pvPos[1] = ZVector(-n.m_fY, n.m_fZ, n.m_fX);
				pvPos[2] = ZVector(n.m_fX, n.m_fY, n.m_fZ);

				int nIndex = nCalc * nCrossSection;
				for(int j = 0; j < nCrossSection; j++)
				{
					float fAng = j * PI2 / (nCrossSection - 1);
					float fX = cosf(fAng);
					float fY = sinf(fAng);

					ZVector vn;
					vn.m_fX = -((pvPos[0].m_fX * fX) + (pvPos[1].m_fX * fY));
					vn.m_fY = -((pvPos[0].m_fY * fX) + (pvPos[1].m_fY * fY));
					vn.m_fZ = -((pvPos[0].m_fZ * fX) + (pvPos[1].m_fZ * fY));

					pObj[no].pVertex[nIndex].m_vPos = v - (vn * fRadius);
					nIndex++;
				}
			}
		}

		camera.m_vPosition = GetBezPos(0, fPos + 0.2f);

		ZVector vDir = GetBezPos(0, fPos + 0.6f) - camera.m_vPosition;
		camera.m_fPitch = vDir.GetPitch();
		camera.m_fYaw = vDir.GetYaw();

		for(i = 0; i < 2; i++)
		{
			pObj[i].Calculate(&camera, fElapsed);
		}
		return S_OK;
	}
	HRESULT Reconfigure()
	{
		tx = g_pD3D->Find(TC_WTTUNNEL);
		pObj[0].pTexture[0].Set(ZObject::Texture::T_USER, tx);//SetTexture(tx);
		pObj[1].pTexture[0].Set(ZObject::Texture::T_USER, tx);///.pTexture = tx;//SetTexture(tx);
		return S_OK;
	}
	HRESULT Render()
	{
		for(int i = 0; i < 2; i++)
		{
			HRESULT hRes = pObj[i].Render();
			if(FAILED(hRes)) return TraceError(hRes);
		}
		return S_OK;
	}
};

EXPORT_EFFECT( Tunnel, ZEffectTunnel )
