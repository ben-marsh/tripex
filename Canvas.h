#pragma once

#include "ZDirect3D.h"
#include "ColorRgb.h"

class Canvas
{
protected:
	SINT32 m_nWidth;
	SINT32 m_nHeight;
	UINT8 *m_anData;
	Texture *m_aTexture;

public:
	ColorRgb m_cColour;
	PALETTEENTRY m_aPalette[ 256 ];

	// Constructor:
	Canvas( SINT32 nWidth, SINT32 nHeight );

	// Create( ):
	Error* Create( );

	// GetDataPtr( ):
	UINT8 *GetDataPtr( );

	// UploadTextures( ):
	Error* UploadTextures( );

	// GetTexture( ):
	Texture *GetTexture( SINT32 nX, SINT32 nY );

	// Render( ):
	Error* Render( );
};
