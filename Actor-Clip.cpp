#include "Platform.h"
#include "Actor.h"

#define CLIP_PLANES 6

typedef struct
{
	uint16 wClip;
	uint16 wIndex;
} ClipInfo;

ZArray<ClipInfo> *ppClip;

ZArray<VertexTL> *ppvOut;
ZArray<Face> *ppfOut;
ZArray<int> *ppnEmptyVertex;
ZArray<int> *ppnEmptyFace;

void InitObjectClipper( )
{
	ppClip = new ZArray<ClipInfo>;
	ppvOut = new ZArray<VertexTL>;
	ppfOut = new ZArray<Face>;
	ppnEmptyVertex = new ZArray<int>;
	ppnEmptyFace = new ZArray<int>;
}



bool IsClipRequired(Face &f, uint16 wPlaneMask)
{
	for(int j = 0;;j++)
	{
		if(j >= 3) return false;
		else if((*ppClip)[f[j]].wClip & wPlaneMask) return true;
	}
}
uint16 Actor::GetClipFlag(uint16 wClipMask, Vector3 &v)
{
	uint16 wClip = 0;
	if(wClipMask & CLIP_FLAG_MIN_X && v.x < m_fClipMinX) wClip |= CLIP_FLAG_MIN_X;
	if(wClipMask & CLIP_FLAG_MAX_X && v.x > m_fClipMaxX) wClip |= CLIP_FLAG_MAX_X;

	if(wClipMask & CLIP_FLAG_MIN_Y && v.y < m_fClipMinY) wClip |= CLIP_FLAG_MIN_Y;
	if(wClipMask & CLIP_FLAG_MAX_Y && v.y > m_fClipMaxY) wClip |= CLIP_FLAG_MAX_Y;
		
	if(wClipMask & CLIP_FLAG_MIN_Z && v.z < m_fClipMinZ) wClip |= CLIP_FLAG_MIN_Z;
	if(wClipMask & CLIP_FLAG_MAX_Z && v.z > m_fClipMaxZ) wClip |= CLIP_FLAG_MAX_Z;
	return wClip;
}
//WORD wClipRequired;
int Actor::GetClippedIndex(Face *pf, int nIn, int nOut, uint16 wPlaneFlag, uint16 wClipRequired)
{
	ClipInfo *pci = &(*ppClip)[(*pf)[nIn]];
	VertexTL &vIn = (pci->wClip != 0)? (*ppvOut)[pci->wIndex] : pTransVertex[pci->wIndex];
	VertexTL &vOut = (*ppvOut)[(*ppClip)[(*pf)[nOut]].wIndex];
//	ZVertexTL &vIn = pTransVertex[(*ppClip)[pf->v[nIn]].wIndex];
//	ZVertexTL &vOut = (*ppvOut)[(*ppClip)[pf->v[nOut]].wIndex];

	float fPos;
	switch(wPlaneFlag)
	{
	case CLIP_FLAG_MIN_X:
		fPos = (vIn.position.x - m_fClipMinX) / (vIn.position.x - vOut.position.x);
		break;
	case CLIP_FLAG_MAX_X:
		fPos = (m_fClipMaxX - vIn.position.x) / (vOut.position.x - vIn.position.x);
		break;
	case CLIP_FLAG_MIN_Y:
		fPos = (vIn.position.y - m_fClipMinY) / (vIn.position.y - vOut.position.y);
		break;
	case CLIP_FLAG_MAX_Y:
		fPos = (m_fClipMaxY - vIn.position.y) / (vOut.position.y - vIn.position.y);
		break;
	case CLIP_FLAG_MIN_Z:
		fPos = (vIn.position.z - m_fClipMinZ) / (vIn.position.z - vOut.position.z);
		break;
	case CLIP_FLAG_MAX_Z:
		fPos = (m_fClipMaxZ - vIn.position.z) / (vOut.position.z - vIn.position.z);
		break;
	default:
		__assume(0);
		break;
	}

	Vector3 vPos = (vIn.position * (1 - fPos)) + (vOut.position * fPos);

	int nVertex;
	VertexTL *pNewVertex;

	uint16 wClip = GetClipFlag(wClipRequired & (wPlaneFlag - 1), vPos);
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
	
	pNewVertex->position = vPos;
	pNewVertex->diffuse = (vIn.diffuse * (1 - fPos)) + (vOut.diffuse * fPos);
	pNewVertex->specular = (vIn.specular * (1 - fPos)) + (vOut.specular * fPos);

	if(wPlaneFlag & ~(CLIP_FLAG_MAX_Z | CLIP_FLAG_MIN_Z))
	{
		// work out texture position with texture perspective
		pNewVertex->rhw = (vIn.rhw * (1 - fPos)) + (vOut.rhw * fPos);
		pNewVertex->tex_coord[0].x = ((vIn.tex_coord[0].x * vIn.rhw * (1 - fPos)) + (vOut.tex_coord[0].x * vOut.rhw * fPos)) / pNewVertex->rhw;
		pNewVertex->tex_coord[0].y = ((vIn.tex_coord[0].y * vIn.rhw * (1 - fPos)) + (vOut.tex_coord[0].y * vOut.rhw * fPos)) / pNewVertex->rhw;
	}
	else
	{
		pNewVertex->tex_coord[0].x = (vIn.tex_coord[0].x * (1 - fPos)) + (vOut.tex_coord[0].x * fPos);
		pNewVertex->tex_coord[0].y = (vIn.tex_coord[0].y * (1 - fPos)) + (vOut.tex_coord[0].y * fPos);
	}
	return nIndex;
}
void Actor::AddClippedFace(Face &fNew, uint16 wPlaneMask, ZArray<Face> &pfDst)
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
void Actor::Clip(ZArray<Face> &pfSrc, ZArray<Face> &pfDst, uint16 wClipMask)
{
	pfDst.Empty();
	(*ppvOut).Empty();

	(*ppfOut).Empty();
	(*ppnEmptyFace).Empty();

	(*ppClip).SetLength(pTransVertex.GetLength());
	uint16 wClipRequired = 0;

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
		(*ppClip)[i].wClip = GetClipFlag(wClipMask, pTransVertex[i].position);
		if((*ppClip)[i].wClip != 0)
		{
			(*ppClip)[i].wIndex = (uint16)(*ppvOut).Add(pTransVertex[i]);
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
	uint16 pwIndex[3];
	bool pbOut[3];
	for(int nPlane = CLIP_PLANES - 1; nPlane >= 0; nPlane--)
	{
		uint16 wPlaneFlag = (uint16)CLIP_FLAG(nPlane);
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
