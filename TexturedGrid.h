#pragma once

#include "ZDirect3D.h"

class TexturedGrid
{
protected:
	int nScrWidth, nScrHeight;

public:
	ZArray<VertexTL> pVertex;
	ZArray<Face> pFace;
	int nWidth, nHeight;
	int nStartX, nStartY;
	float fPosX, fPosY;
	bool bUpdateEdges;

	TexturedGrid( int nWidth, int nHeight );
	Error* Render( );
};
