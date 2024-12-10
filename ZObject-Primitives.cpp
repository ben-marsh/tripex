#include "StdAfx.h"
#include "Misc.h"
#include <float.h>
#include "ZObject.h"
#include "error.h"

class ZGeoEdge : public ZEdge
{
public:
	float fLength;
	WORD pwFace[2];

	void FindLength(ZObject *pObj){ fLength = (pObj->pVertex[(*this)[0]].m_vPos - pObj->pVertex[(*this)[1]].m_vPos).Length(); }
};
class ZTetraGeoEdge : public ZEdge
{
public:
	int nSplit;
};
void ZObject::CreateCube(float fSize)
{
	pVertex.SetLength(8);
	pVertex[0].m_vPos.Set(-fSize, fSize, -fSize);
	pVertex[1].m_vPos.Set(fSize, fSize, -fSize);
	pVertex[2].m_vPos.Set(fSize, -fSize, -fSize);
	pVertex[3].m_vPos.Set(-fSize, -fSize, -fSize);
	pVertex[4].m_vPos.Set(-fSize, fSize, fSize);
	pVertex[5].m_vPos.Set(fSize, fSize, fSize);
	pVertex[6].m_vPos.Set(fSize, -fSize, fSize);
	pVertex[7].m_vPos.Set(-fSize, -fSize, fSize);

	for(int i = 0; i < 8; i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set( F_VALID_VERTEX_NORMALS );

	pFace.SetLength(12);
	pFace[0].Set(0, 3, 2);
	pFace[1].Set(0, 2, 1);
	pFace[2].Set(1, 2, 6);
	pFace[3].Set(1, 6, 5);
	pFace[4].Set(4, 5, 7);
	pFace[5].Set(5, 6, 7);
	pFace[6].Set(0, 4, 7);
	pFace[7].Set(0, 7, 3);
	pFace[8].Set(4, 0, 1);
	pFace[9].Set(4, 1, 5);
	pFace[10].Set(7, 6, 2);	
	pFace[11].Set(7, 2, 3);
}
void ZObject::CreateTetrahedron(float fRadius)
{
	float fSqrt2 = sqrtf(2.0f);
	float fSqrt3 = sqrtf(3.0f);
	float fX = 2.0f * fSqrt2 * fRadius / 3.0f;
	float fY = fRadius / 3.0f;
	float fP = fSqrt2 * fRadius / 3.0f;
	float fQ = fSqrt2 * fRadius / fSqrt3;

	pVertex.SetLength(4);
	pVertex[0].m_vPos = ZVector(0, fRadius, 0);
	pVertex[1].m_vPos = ZVector(fX, -fY, 0);
	pVertex[2].m_vPos = ZVector(-fP, -fY, fQ);
	pVertex[3].m_vPos = ZVector(-fP, -fY, -fQ);
	pFace.SetLength(4);
	pFace[0] = ZFace(0, 1, 3);
	pFace[1] = ZFace(0, 2, 1);
	pFace[2] = ZFace(0, 3, 2);
	pFace[3] = ZFace(3, 1, 2);
	for(int i = 0; i < 4; i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}

	m_bsFlag.set(F_VALID_VERTEX_NORMALS);
}
void ZObject::CreateGeosphere(float fRadius, int nVertices)
{
	CreateCube(fRadius / sqrtf(3.0f));
	ZArray<ZGeoEdge> pGeoEdge;
	pVertex.SetSize(nVertices);

	for(int i = 0; i < pFace.GetLength(); i++)
	{
		for(int j = 0; j < 3; j++)
		{
			ZEdge e = pFace[i].GetEdge(j).Ordered();
			for(int k = 0;;k++)
			{
				if(k < pGeoEdge.GetLength())
				{
					if(pGeoEdge[k] == e)
					{
						pGeoEdge[k].pwFace[1] = i;
						break;
					}
				}
				else
				{
					k = pGeoEdge.AddEmpty();
					pGeoEdge[k].Set(e);
					pGeoEdge[k].FindLength(this);
					pGeoEdge[k].pwFace[0] = i;
					break;
				}
			}
		}
	}

	while(pVertex.GetLength() < nVertices)
	{
		int nSplitEdge = 0;
		float fLongestEdge = pGeoEdge[nSplitEdge].fLength;
		for(int j = 1; j < pGeoEdge.GetLength(); j++)
		{
			if(pGeoEdge[j].fLength > fLongestEdge)
			{
				nSplitEdge = j;
				fLongestEdge = pGeoEdge[j].fLength;
			}
		}

		ZGeoEdge te = pGeoEdge[nSplitEdge];

		// and split it
		float fScale = (0.5f * fRadius) / sqrtf((fRadius * fRadius) - (fLongestEdge * fLongestEdge / 4.0f));
		int nNewVertex = pVertex.AddEmpty();
		pVertex[nNewVertex].m_vPos = (pVertex[te[0]].m_vPos + pVertex[te[1]].m_vPos) * fScale;
////////////////////////////////////////////
		// add the main edges
		pGeoEdge[nSplitEdge][0] = te[0];
		pGeoEdge[nSplitEdge][1] = nNewVertex;
		pGeoEdge[nSplitEdge].fLength /= 2.0;
		int nNewEdge = pGeoEdge.AddEmpty();
		pGeoEdge[nNewEdge][0] = te[1];
		pGeoEdge[nNewEdge][1] = nNewVertex;
		pGeoEdge[nNewEdge].fLength = pGeoEdge[nSplitEdge].fLength;

		// add two more edges (one may be the same)
		for(int i = 0; i < 2; i++)
		{
			int nFace1 = te.pwFace[i];
			int nFace2 = pFace.AddEmpty();

			pFace[nFace2] = pFace[nFace1];
			int nVertex;
			for(int k = 0; k < 3; k++)
			{
				if(pFace[nFace1][k] != te[0] && pFace[nFace1][k] != te[1])
				{
					nVertex = pFace[nFace1][k];
				}

				if(pFace[nFace1][k] == te[1])
				{
					pFace[nFace1][k] = nNewVertex;
				}
				if(pFace[nFace2][k] == te[0])
				{
					pFace[nFace2][k] = nNewVertex;
				}
			}
			pGeoEdge[nSplitEdge].pwFace[i] = nFace1;
			pGeoEdge[nNewEdge].pwFace[i] = nFace2;

			int nNewSplit = pGeoEdge.AddEmpty();
			pGeoEdge[nNewSplit].Set(ZEdge(nVertex, nNewVertex).Ordered());
			pGeoEdge[nNewSplit].FindLength(this);
			pGeoEdge[nNewSplit].pwFace[0] = nFace1;
			pGeoEdge[nNewSplit].pwFace[1] = nFace2;

			ZEdge ce = ZEdge(te[1], nVertex).Ordered();
			for(int k = 0; k < pGeoEdge.GetLength(); k++)
			{
				if(pGeoEdge[k] == ce)
				{
					if(pGeoEdge[k].pwFace[0] == nFace1) pGeoEdge[k].pwFace[0] = nFace2;
					if(pGeoEdge[k].pwFace[1] == nFace1) pGeoEdge[k].pwFace[1] = nFace2;
				}
			}
		}
	}

	_ASSERT(pVertex.GetLength() == nVertices);

	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set( F_VALID_VERTEX_NORMALS );
}
void ZObject::CreateTorus(float fOuterRad, float fInnerRad, int nOuterPoints, int nInnerPoints)
{
	pVertex.Empty();
	pFace.Empty();

	int nVertices = nOuterPoints * nInnerPoints;
	for(int i = 0; i < nOuterPoints; i++)
	{
		int nBase = pVertex.GetLength();
		for(int j = 0; j < nInnerPoints; j++)
		{
			double dAngOuter = (i + 0.5) * PI2 / nOuterPoints;
			double dAngInner = (j + 0.5) * PI2 / nInnerPoints;

			ZVertex *pv = pVertex.AddEmptyPtr();
			pv->m_vPos.m_fX = (float)(sin(dAngOuter) * (fOuterRad + (cos(dAngInner) * fInnerRad)));
			pv->m_vPos.m_fY = (float)(cos(dAngOuter) * (fOuterRad + (cos(dAngInner) * fInnerRad)));
			pv->m_vPos.m_fZ = (float)(sin(dAngInner) * fInnerRad);

			ZFace f;
			f[0] = nBase + j;
			f[2] = nBase + ((j + 1) % nInnerPoints);
			f[1] = (nBase + ((j + 1) % nInnerPoints) + nInnerPoints) % nVertices;
			pFace.Add(f);

			f[0] = nBase + j;
			f[1] = (nBase + j + nInnerPoints) % nVertices;
			f[2] = (nBase + ((j + 1) % nInnerPoints) + nInnerPoints) % nVertices;
			pFace.Add(f);
		}
	}
	_ASSERT(pVertex.GetLength() == nVertices);
}
void ZObject::CreateTetrahedronGeosphere(float fRadius, int nIterations)
{
	ZArray<ZTetraGeoEdge> pGeoEdge;
	ZArray<ZFace> pGeoFace;

	CreateTetrahedron(fRadius);
	for(int n = 0; n < nIterations; n++)
	{
		pGeoEdge.SetLength(0);
		pGeoFace.SetLength(pFace.GetLength());
		for(int i = 0; i < pFace.GetLength(); i++)
		{
			for(int j = 0; j < 3; j++)
			{
				ZTetraGeoEdge e;
				ZeroMemory(&e, sizeof(e));
				e[0] = pFace[i][j];
				e[1] = pFace[i][(j + 1) % 3];
				for(int k = 0;;k++)
				{
					if(k < pGeoEdge.GetLength())
					{
						if(pGeoEdge[k].Contains(e[0]) && pGeoEdge[k].Contains(e[1]))
						{
							pGeoFace[i][j] = k;
							break;
						}
					}
					else
					{
						int nIndex = pGeoEdge.Add(e);
						pGeoFace[i][j] = k;
						break;
					}
				}
			}
		}

//		ZFlexibleVertex fv(pVertex);
		for(int i = 0; i < pGeoEdge.GetLength(); i++)
		{
			int nVertex = pVertex.AddEmpty();
			pGeoEdge[i].nSplit = nVertex;//pv;//p.SetNewEmpty();
			pVertex[nVertex].m_vPos = (pVertex[pGeoEdge[i][0]].m_vPos + pVertex[pGeoEdge[i][1]].m_vPos).Normal(fRadius);
		}
		pFace.SetLength(0);
		for(int i = 0; i < pGeoFace.GetLength(); i++)
		{
			ZFace &f = pGeoFace[i];
			ZTetraGeoEdge &e1 = pGeoEdge[f[0]];
			ZTetraGeoEdge &e2 = pGeoEdge[f[1]];
			ZTetraGeoEdge &e3 = pGeoEdge[f[2]];
			pFace.Add(ZFace(e1.nSplit, e2.nSplit, e3.nSplit));
			pFace.Add(ZFace(e1.nSplit, e2.nSplit, e1.CommonIndex(e2)));
			pFace.Add(ZFace(e1.nSplit, e3.nSplit, e1.CommonIndex(e3)));
			pFace.Add(ZFace(e2.nSplit, e3.nSplit, e2.CommonIndex(e3)));
		}
	}
	FindFaceOrder(ZVector::Origin());
	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set( F_VALID_VERTEX_NORMALS );
}
