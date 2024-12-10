#ifndef ENG_GRID
#define ENG_GRID

#include "ZDirect3D.h"

class ZGrid
{
protected:
	int nScrWidth, nScrHeight;

public:
	ZArray<ZVertexTL> pVertex;
	ZArray<ZFace> pFace;
	int nWidth, nHeight;
	int nStartX, nStartY;
	float fPosX, fPosY;
	bool bUpdateEdges;

	ZGrid( int nWidth, int nHeight );
	HRESULT Render( );
};

#endif