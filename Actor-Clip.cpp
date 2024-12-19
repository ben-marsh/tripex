#include "StdAfx.h"
#include "Actor.h"

#define CLIP_PLANES 6

typedef struct
{
	WORD wClip;
	WORD wIndex;
} ClipInfo;

ZArray<ClipInfo> *ppClip;

ZArray<ZVertexTL> *ppvOut;
ZArray<Face> *ppfOut;
ZArray<int> *ppnEmptyVertex;
ZArray<int> *ppnEmptyFace;

void InitObjectClipper( )
{
	ppClip = new ZArray<ClipInfo>;
	ppvOut = new ZArray<ZVertexTL>;
	ppfOut = new ZArray<Face>;
	ppnEmptyVertex = new ZArray<int>;
	ppnEmptyFace = new ZArray<int>;
}



bool IsClipRequired(Face &f, WORD wPlaneMask)
{
	for(int j = 0;;j++)
	{
		if(j >= 3) return false;
		else if((*ppClip)[f[j]].wClip & wPlaneMask) return true;
	}
}
WORD Actor::GetClipFlag(WORD wClipMask, Vector3 &v)
{
	WORD wClip = 0;
	if(wClipMask & CLIP_FLAG_MIN_X && v.m_fX < m_fClipMinX) wClip |= CLIP_FLAG_MIN_X;
	if(wClipMask & CLIP_FLAG_MAX_X && v.m_fX > m_fClipMaxX) wClip |= CLIP_FLAG_MAX_X;

	if(wClipMask & CLIP_FLAG_MIN_Y && v.m_fY < m_fClipMinY) wClip |= CLIP_FLAG_MIN_Y;
	if(wClipMask & CLIP_FLAG_MAX_Y && v.m_fY > m_fClipMaxY) wClip |= CLIP_FLAG_MAX_Y;
		
	if(wClipMask & CLIP_FLAG_MIN_Z && v.m_fZ < m_fClipMinZ) wClip |= CLIP_FLAG_MIN_Z;
	if(wClipMask & CLIP_FLAG_MAX_Z && v.m_fZ > m_fClipMaxZ) wClip |= CLIP_FLAG_MAX_Z;
	return wClip;
}
//WORD wClipRequired;
int Actor::GetClippedIndex(Face *pf, int nIn, int nOut, WORD wPlaneFlag, WORD wClipRequired)
{
	ClipInfo *pci = &(*ppClip)[(*pf)[nIn]];
	ZVertexTL &vIn = (pci->wClip != 0)? (*ppvOut)[pci->wIndex] : pTransVertex[pci->wIndex];
	ZVertexTL &vOut = (*ppvOut)[(*ppClip)[(*pf)[nOut]].wIndex];
//	ZVertexTL &vIn = pTransVertex[(*ppClip)[pf->v[nIn]].wIndex];
//	ZVertexTL &vOut = (*ppvOut)[(*ppClip)[pf->v[nOut]].wIndex];

	float fPos;
	switch(wPlaneFlag)
	{
	case CLIP_FLAG_MIN_X:
		fPos = (vIn.m_vPos.m_fX - m_fClipMinX) / (vIn.m_vPos.m_fX - vOut.m_vPos.m_fX);
		break;
	case CLIP_FLAG_MAX_X:
		fPos = (m_fClipMaxX - vIn.m_vPos.m_fX) / (vOut.m_vPos.m_fX - vIn.m_vPos.m_fX);
		break;
	case CLIP_FLAG_MIN_Y:
		fPos = (vIn.m_vPos.m_fY - m_fClipMinY) / (vIn.m_vPos.m_fY - vOut.m_vPos.m_fY);
		break;
	case CLIP_FLAG_MAX_Y:
		fPos = (m_fClipMaxY - vIn.m_vPos.m_fY) / (vOut.m_vPos.m_fY - vIn.m_vPos.m_fY);
		break;
	case CLIP_FLAG_MIN_Z:
		fPos = (vIn.m_vPos.m_fZ - m_fClipMinZ) / (vIn.m_vPos.m_fZ - vOut.m_vPos.m_fZ);
		break;
	case CLIP_FLAG_MAX_Z:
		fPos = (m_fClipMaxZ - vIn.m_vPos.m_fZ) / (vOut.m_vPos.m_fZ - vIn.m_vPos.m_fZ);
		break;
	default:
		__assume(0);
		break;
	}

	Vector3 vPos = (vIn.m_vPos * (1 - fPos)) + (vOut.m_vPos * fPos);

	int nVertex;
	ZVertexTL *pNewVertex;

	WORD wClip = GetClipFlag(wClipRequired & (wPlaneFlag - 1), vPos);
	if(wClip == 0)
	{
		int nLength = (*ppnEmptyVertex).GetLength();
		if(nLength > 0)
		{
			nVertex = (*ppnEmptyVertex)[nLength - 1];
			(*ppnEmptyVertex).SetLength(nLength - 1);
		}
		else
		{
			nVertex = pTransVertex.AddEmpty();
		}
		pNewVertex = &pTransVertex[nVertex];
	}
	else
	{
		nVertex = (*ppvOut).AddEmpty();
		pNewVertex = &(*ppvOut)[nVertex];
	}

	int nIndex = (*ppClip).AddEmpty();
	(*ppClip)[nIndex].wIndex = nVertex;
	(*ppClip)[nIndex].wClip = wClip;
	
	pNewVertex->m_vPos = vPos;
	pNewVertex->m_cDiffuse = (vIn.m_cDiffuse * (1 - fPos)) + (vOut.m_cDiffuse * fPos);
	pNewVertex->m_cSpecular = (vIn.m_cSpecular * (1 - fPos)) + (vOut.m_cSpecular * fPos);

	if(wPlaneFlag & ~(CLIP_FLAG_MAX_Z | CLIP_FLAG_MIN_Z))
	{
		// work out texture position with texture perspective
		pNewVertex->m_fRHW = (vIn.m_fRHW * (1 - fPos)) + (vOut.m_fRHW * fPos);
		pNewVertex->m_aTex[0].x = ((vIn.m_aTex[0].x * vIn.m_fRHW * (1 - fPos)) + (vOut.m_aTex[0].x * vOut.m_fRHW * fPos)) / pNewVertex->m_fRHW;
		pNewVertex->m_aTex[0].y = ((vIn.m_aTex[0].y * vIn.m_fRHW * (1 - fPos)) + (vOut.m_aTex[0].y * vOut.m_fRHW * fPos)) / pNewVertex->m_fRHW;
	}
	else
	{
		pNewVertex->m_aTex[0].x = (vIn.m_aTex[0].x * (1 - fPos)) + (vOut.m_aTex[0].x * fPos);
		pNewVertex->m_aTex[0].y = (vIn.m_aTex[0].y * (1 - fPos)) + (vOut.m_aTex[0].y * fPos);
	}
	return nIndex;
}
void Actor::AddClippedFace(Face &fNew, WORD wPlaneMask, ZArray<Face> &pfDst)
{
	if(IsClipRequired(fNew, wPlaneMask))
	{
		int nLength = (*ppnEmptyFace).GetLength();
		if(nLength > 0)
		{
			int nIndex = (*ppnEmptyFace)[nLength - 1];
			(*ppnEmptyFace).SetLength(nLength - 1);
			(*ppfOut)[nIndex] = fNew;
		}
		else
		{
			(*ppfOut).Add(fNew);
		}
	}
	else
	{
		pfDst.Add(fNew);
	}
}
void Actor::Clip(ZArray<Face> &pfSrc, ZArray<Face> &pfDst, WORD wClipMask)
{
	pfDst.Empty();
	(*ppvOut).Empty();

	(*ppfOut).Empty();
	(*ppnEmptyFace).Empty();

	(*ppClip).SetLength(pTransVertex.GetLength());
	WORD wClipRequired = 0;

	// -0.25
	// pD3D->dwWidth - 0.25

	if(!wClipMask)
	{
		pfDst = pfSrc;
		return;
	}

	if( !m_bsFlag.test( F_VALID_CLIP_PLANES ) )
	{
		m_fClipMinX = std::min(-0.25f, g_pD3D->g_Caps.GuardBandLeft);
		m_fClipMaxX = std::max(g_pD3D->GetWidth() - 0.25f, g_pD3D->g_Caps.GuardBandRight);
		m_fClipMinY = std::min(-0.25f, g_pD3D->g_Caps.GuardBandTop);
		m_fClipMaxY = std::max(g_pD3D->GetHeight() - 0.25f, g_pD3D->g_Caps.GuardBandBottom);
		m_fClipMinZ = 0.001f;
		m_fClipMaxZ = 1500.0f;
	}

	for(int i = 0; i < pTransVertex.GetLength(); i++)
	{
		(*ppClip)[i].wClip = GetClipFlag(wClipMask, pTransVertex[i].m_vPos);
		if((*ppClip)[i].wClip != 0)
		{
			(*ppClip)[i].wIndex = (WORD)(*ppvOut).Add(pTransVertex[i]);
			(*ppnEmptyVertex).Add(i);
		}
		else
		{
			(*ppClip)[i].wIndex = i;
		}
		wClipRequired |= (*ppClip)[i].wClip;
	}

	for(int i = 0; i < pfSrc.GetLength(); i++)
	{
		for(int j = 0;;j++)
		{
			if(j >= 3)
			{
				pfDst.Add(pfSrc[i]);
				break;
			}
			else if((*ppClip)[pfSrc[i][j]].wClip != 0)
			{
				(*ppfOut).Add(pfSrc[i]);
				break;
			}
		}
	}

	int nFirstClip = pfDst.GetLength();

	Face fNew;
	WORD pwIndex[3];
	bool pbOut[3];
	for(int nPlane = CLIP_PLANES - 1; nPlane >= 0; nPlane--)
	{
		WORD wPlaneFlag = (WORD)CLIP_FLAG(nPlane);
		if(!(wClipRequired & wPlaneFlag)) continue;

		for(int nFace = (*ppfOut).GetLength() - 1; nFace >= 0; nFace--)
		{
			Face *pf = &(*ppfOut)[nFace];
			if((*pf)[0] == WORD_INVALID_INDEX) continue;

			int nOut = 0;
			for(int k = 0; k < 3; k++)
			{
				pwIndex[k] = k;	
				pbOut[k] = ((*ppClip)[(*pf)[k]].wClip & wPlaneFlag) != 0;
				if(pbOut[k]) nOut++;
			}

			if(nOut == 0) continue;

			if(nOut != 3)
			{
				// make sure that no realloc is required (changes pointers)
				pTransVertex.MakeExtraSpace(100);
				(*ppvOut).MakeExtraSpace(100);
				(*ppfOut).MakeExtraSpace(100);

				pf = &(*ppfOut)[nFace];

				if(nOut == 1)
				{
					// 0 = out, 1, 2 = in
					if(pbOut[1]) std::swap( pwIndex[0], pwIndex[1] );
					else if(pbOut[2]) std::swap( pwIndex[0], pwIndex[2] );
	
					fNew = *pf;
					fNew[pwIndex[0]] = GetClippedIndex(pf, pwIndex[1], pwIndex[0], wPlaneFlag, wClipRequired);
					AddClippedFace(fNew, wPlaneFlag - 1, pfDst);

					fNew[pwIndex[1]] = fNew[pwIndex[0]];
					fNew[pwIndex[0]] = GetClippedIndex(pf, pwIndex[2], pwIndex[0], wPlaneFlag, wClipRequired);
					AddClippedFace(fNew, wPlaneFlag - 1, pfDst);
				}
				else // if(nOut == 2)
				{
					// 0 = in, 1, 2 = out
					if(!pbOut[1]) std::swap( pwIndex[0], pwIndex[1] );
					else if(!pbOut[2]) std::swap( pwIndex[0], pwIndex[2] );

					fNew[pwIndex[0]] = (*pf)[pwIndex[0]];
					fNew[pwIndex[1]] = GetClippedIndex(pf, pwIndex[0], pwIndex[1], wPlaneFlag, wClipRequired);
					fNew[pwIndex[2]] = GetClippedIndex(pf, pwIndex[0], pwIndex[2], wPlaneFlag, wClipRequired);
					AddClippedFace(fNew, wPlaneFlag - 1, pfDst);
				}
			}
			(*pf)[0] = WORD_INVALID_INDEX;
			(*ppnEmptyFace).Add(nFace);
		}
	}

	for(int i = nFirstClip; i < pfDst.GetLength(); i++)
	{
		for(int j = 0; j < 3; j++)
		{
			pfDst[i][j] = (*ppClip)[pfDst[i][j]].wIndex;
		}
	}
}
