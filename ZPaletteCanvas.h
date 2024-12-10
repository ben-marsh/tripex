#pragma once
#include "ZDirect3D.h"
#include "ZColour.h"

class ZPaletteCanvas
{
protected:
	SINT32 m_nWidth;
	SINT32 m_nHeight;
	UINT8 *m_anData;
	ZTexture *m_aTexture;

public:
	ZColour m_cColour;
	PALETTEENTRY m_aPalette[ 256 ];

	// Constructor:
	ZPaletteCanvas( SINT32 nWidth, SINT32 nHeight );

	// Create( ):
	HRESULT Create( );

	// GetDataPtr( ):
	UINT8 *GetDataPtr( );

	// UploadTextures( ):
	HRESULT UploadTextures( );

	// GetTexture( ):
	ZTexture *GetTexture( SINT32 nX, SINT32 nY );

	// Render( ):
	HRESULT Render( );
};
