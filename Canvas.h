#pragma once

#include "ZDirect3D.h"
#include "ColorRgb.h"

class Canvas
{
protected:
	int32 m_nWidth;
	int32 m_nHeight;
	uint8 *m_anData;
	Texture *m_aTexture;

public:
	ColorRgb m_cColour;
	PALETTEENTRY m_aPalette[ 256 ];

	// Constructor:
	Canvas( int32 nWidth, int32 nHeight );

	// Create( ):
	Error* Create( );

	// GetDataPtr( ):
	uint8 *GetDataPtr( );

	// UploadTextures( ):
	Error* UploadTextures( );

	// GetTexture( ):
	Texture *GetTexture( int32 nX, int32 nY );

	// Render( ):
	Error* Render( );
};
