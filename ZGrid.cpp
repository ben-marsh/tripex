#include "StdAfx.h"
#include "zgrid.h"
#include "error.h"
#include "ZPtr.h"

ZGrid::ZGrid(int nWidth, int nHeight)//, DWORD dwVertexFormat)//, GridCoordCallbackFunction callback)
{
	this->nWidth = nWidth;
	this->nHeight = nHeight;
//	this->nWidth = 3;//nWidth;
//	this->nHeight = 3;//nHeight;
	nScrWidth = nScrHeight = -1;
//	this->width = width;
//	this->height = height;
//	this->sWidth = this->sHeight = -1;

//	pVertex.SetFormat(dwVertexFormat);
	pVertex.SetLength((this->nWidth + 1) * (this->nHeight + 1));
//	vertices = (width + 1) * (height + 1);
//	vertex = new D3DTLVERTEX[vertices];
//	pFace.SetLength(this->nWidth * this->nHeight * 2);
//	faces = width * height * 2;
//	face = new WORD[faces*3];

//	pbOut.SetLength((nWidth + 1) * (nHeight + 1));

/*	int f = 0, v = 0;
	for(int x = 0; x < nWidth; x++)
	{
		for(int y = 0; y < nHeight; y++)
		{
			pFace[f++] = ZFace(v, v + nHeight + 1, v + 1);
			pFace[f++] = ZFace(v + 1, v + nHeight + 1, v + 1 + nHeight + 1);
/*			pFace[f + 1] = v + (height+1);
			face[f + 2] = v + 1;//(width+1);

			face[f + 3] = v + 1;
			face[f + 4] = v + (height+1);
			face[f + 5] = v + (height+1) + 1;//(width+1);
*/
//			f += 6;
//			v++;
//		}
//		v++;
//	}
}
/*ZGrid::ZGrid(int width, int height, GridCallbackFunction callback)
{
	this->callback = callback;
	this->coordcallback = NULL;

	Initialise(width, height);
}
*/
//ZGrid::~ZGrid(void)
//{/
//	delete vertex;
//	delete face;
//}
/*
HRESULT ZGrid::Calculate()
{
	double fx, fy;
	int i = 0;
	if(coordcallback != NULL)
	{
		for(int x = 0; x <= width; x++)
		{
			for(int y = 0; y <= height; y++)
			{
				coordcallback(x, y, &vertex[i].tu, &vertex[i].tv, &vertex[i].color);
				i++;
			}
		}
	}
	else
	{
		for(int x = 0; x <= width; x++)
		{
			for(int y = 0; y <= height; y++)
			{
				fx = (x - (width/2.0)) / double(width);
				fy = (y - (height/2.0)) / double(height);
	
				callback(fx, fy, &vertex[i].tu, &vertex[i].tv, &vertex[i].color);
				i++;
			}
		}
	}
	return DD_OK;
}
*/
//void ZGrid::SetCallback(GridCallbackFunction fn)
//{
//	callback = fn;
//}
ZError* ZGrid::Render( )
{
	if(nScrWidth != g_pD3D->GetWidth() || nScrHeight != g_pD3D->GetHeight() )
	{
		nScrWidth = g_pD3D->GetWidth();
		nScrHeight = g_pD3D->GetHeight();
//		float fScaleX = nScrWidth / 640.0;
//		float fScaleY = nScrHeight / 480.0;

//		scaleY = scaleX = max(scaleX, scaleY);
		float fScale = max(nScrWidth / 640.0f, nScrHeight / 480.0f);

		int i = 0;
//		ZFlexibleVertex fv(pVertex);
		nStartX = -1;
		nStartY = -1;

		ZPtr<float> pfY = new float[nHeight + 1];
		for(int y = 0; y <= nHeight; y++)
		{
			pfY[y] = (y * 479 * fScale / nHeight) + ((nScrHeight - (480 * fScale)) / 2);
			if(nStartY == -1 && y > 0 && pfY[y] > 0)
			{
				nStartY = y;
				fPosY = pfY[y] / (pfY[y] - pfY[y - 1]);
			}
		}
		ZPtr<float> pfX = new float[nWidth + 1];
		for(int x = 0; x <= nWidth; x++)
		{
			pfX[x] = (x * 639 * fScale / nWidth) + ((nScrWidth - (640 * fScale)) / 2);
			if(nStartX == -1 && x > 0 && pfX[x] > 0)
			{
				nStartX = x;
				fPosX = pfX[x] / (pfX[x] - pfX[x - 1]);
			}
		}
		for(int x = 0; x <= nWidth; x++)
		{
			for(int y = 0; y <= nHeight; y++)
			{
				pVertex[i].m_vPos.m_fX = pfX[x];
				pVertex[i].m_vPos.m_fY = pfY[y];
				pVertex[i].m_vPos.m_fZ = pVertex[i].m_fRHW = 1;
				i++;
			}
		}
		pFace.SetLength(0);
//		nStartX = nStartY = 3;
//		nStartX+=1;
//		nStartY+=1;
// no limit texas holdem
//		pFace.SetLength(4);//(nWidth - ((nStartX - 1) * 2)) * (nHeight - ((nStartY - 1) * 2)));
//		int nSpan = nHeight - ((nStartY - 1) * 2) + 1;
		int nVertex = 0;
		i = 0;
		for(int x = 0; x < nWidth; x++)
		{
			for(int y = 0; y <= nHeight; y++)
			{
				if(x >= nStartX - 1 && x <= nWidth - nStartX && y >= nStartY - 1 && y <= nHeight - nStartY)
				{
					pFace.Add(ZFace(nVertex, nVertex + nHeight + 1, nVertex + nHeight + 2));
					pFace.Add(ZFace(nVertex, nVertex + nHeight + 2, nVertex + 1));
				}
				nVertex++;
			}
		}
		bUpdateEdges = true;
//		assert(i == pFace.GetLength());
	}
	if(bUpdateEdges)
	{
		int nIndex1 = (nStartX - 1) * (nHeight + 1);
		int nIndex2 = (nWidth - nStartX + 1) * (nHeight + 1);
		for(int y = 0; y <= nHeight; y++)
		{
			pVertex[nIndex1].m_vPos.m_fX = -0.25f;
			pVertex[nIndex1].m_aTex[0].x = (fPosX * pVertex[nIndex1].m_aTex[0].x) + ((1 - fPosX) * pVertex[nIndex1 + (nHeight + 1)].m_aTex[0].x);
			pVertex[nIndex1].m_aTex[0].y = (fPosX * pVertex[nIndex1].m_aTex[0].y) + ((1 - fPosX) * pVertex[nIndex1 + (nHeight + 1)].m_aTex[0].y);
			nIndex1++;

			pVertex[nIndex2].m_vPos.m_fX = g_pD3D->GetWidth() - 1.25f;
			pVertex[nIndex2].m_aTex[0].x = (fPosX * pVertex[nIndex2].m_aTex[0].x) + ((1 - fPosX) * pVertex[nIndex2 - (nHeight + 1)].m_aTex[0].x);
			pVertex[nIndex2].m_aTex[0].y = (fPosX * pVertex[nIndex2].m_aTex[0].y) + ((1 - fPosX) * pVertex[nIndex2 - (nHeight + 1)].m_aTex[0].y);
			nIndex2++;
		}

		nIndex1 = (nStartY - 1);
		nIndex2 = (nHeight - nStartY + 1);
		for(int x = 0; x <= nWidth; x++)
		{
			pVertex[nIndex1].m_vPos.m_fY = -0.25f;
			pVertex[nIndex1].m_aTex[0].x = (fPosX * pVertex[nIndex1].m_aTex[0].x) + ((1 - fPosX) * pVertex[nIndex1 + 1].m_aTex[0].x);
			pVertex[nIndex1].m_aTex[0].y = (fPosX * pVertex[nIndex1].m_aTex[0].y) + ((1 - fPosX) * pVertex[nIndex1 + 1].m_aTex[0].y);
			nIndex1 += nHeight + 1;

			pVertex[nIndex2].m_vPos.m_fY = g_pD3D->GetHeight() - 1.25f;
			pVertex[nIndex2].m_aTex[0].x = (fPosX * pVertex[nIndex2].m_aTex[0].x) + ((1 - fPosX) * pVertex[nIndex2 - 1].m_aTex[0].x);
			pVertex[nIndex2].m_aTex[0].y = (fPosX * pVertex[nIndex2].m_aTex[0].y) + ((1 - fPosX) * pVertex[nIndex2 - 1].m_aTex[0].y);
			nIndex2 += nHeight + 1;
		}
		bUpdateEdges = false;
	}

	ZError* error = g_pD3D->DrawIndexedPrimitive(pVertex, pFace);
	if(error) return TraceError(error);

	return nullptr;
}
 