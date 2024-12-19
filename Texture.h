#pragma once

#include <d3d9.h>
#include "ZDirect3D.h"

class Texture
{
//protected:
//	friend HRESULT g_pD3D->FlushTextureState( );
//	friend void g_pD3D->SetTexture(DWORD dwStage, ZTexture *pTexture, DWORD dwOp, DWORD dwArg2);
public:
	UINT32 m_nFlags;
	IDirect3DTexture9 *m_pd3dTexture;

	enum
	{
		F_DYNAMIC = ( 1 << 0 ),
		F_MIP_CHAIN = ( 1 << 1 ),
		F_FILTERING = ( 1 << 2 ),
//		F_SRC_FILE = ( 1 << 3 ),
	};

	// if palette is set before texture created? must be constant ptr
	// if palette is set in dynamic texture?

	D3DFORMAT m_nSrcFmt;
	UINT32 m_nSrcSize;
	UINT32 m_nSrcSpan;
	const void *m_pSrcData;
	const PALETTEENTRY *m_pSrcPalette;

	std::set< int > m_snType;

	// Constructor:
	Texture( );

	// Destructor:
	~Texture( );

	// SetFlags( ):
	void SetFlags( UINT32 nFlag );

	// SetSource( ):
	void SetSource( const void *pSrcData, UINT32 nSrcSize );
	void SetSource( D3DFORMAT nSrcFmt, const void *pSrcData, UINT32 nSrcSize, UINT32 nSrcSpan );
	void SetSource( const PALETTEENTRY *pPalette, const void *pSrcData, UINT32 nSrcSize, UINT32 nSrcSpan );

	// SetSpan( ):

};

//	class ZTexture
//	{
//	public:
//	/*	enum Type
//		{
//			T_RAW,
//	//		T_RAW_INDEXED,
//	//		T_RAW_XRGB32,
//			T_COMPRESSED,
//		};
//	*/
//		// for compressed, use
//		//   D3DXCreateTextureFromFileInMemoryEx
//		// otherwise, use 
//		//   D3DXCreateTexture
//		//   and call Upload( )
//	
//	protected:
//	//	vector< ZColour > m_vcPalette;
//	//	vector< unsigned char > m_vnData;
//	
//	//		ST_NONE,
//	//		ST_MEM_RAW,
//		ST_MEM_FILE,
//	};
//	
//	//	SourceType m_nSourceType;
//	
//	public:
//		D3DFORMAT m_nPrefFormat;
//	//	vector< D3DFORMAT > m_vnFormat;
//	
//		const BYTE *m_pnData;
//		const PALETTEENTRY *m_pnPalette;
//		UINT32 m_nDataSize;
//	
//		// texture types
//	
//		// enum
//		//	{
//		//		F_MIP_CHAIN,
//		//		F_FILTERING,
//		//		F_DYNAMIC,				( cannot have mip chain )
//		//		F_SOURCE_FILE,			( cannot be dynamic )
//		//	}
//	
//		enum
//		{
//			F_DYNAMIC,
//			F_MIP_CHAIN,
//			F_FILTERING,
//			F_SOURCE_FILE,
//	
//	//		F_LOADED,
//	//		F_DYNAMIC,
//	//		F_CREATE_MIPMAPS,
//	//		F_HAS_MIPMAPS,
//	//		F_POINT_FILTER,
//			F_LAST
//		};
//	
//		bitset<F_LAST> m_nFlags;
//	//	D3DFORMAT m_nFormat;
//		IDirect3DTexture8 *m_pd3dTexture;
//		set<int> m_snType;
//	
//		// preferred texture formats, if created as
//	
//	 	ZTexture();
//	ZTexture(ZColour *pcData);
//	ZTexture(unsigned char *pnIndex, ZColour *pcPalette);
//		~ZTexture();
//	
//	//	void SetDepths(int n1, int n2 = 0, int n3 = 0);
//	
//		HRESULT Create( );
//		HRESULT Upload( );
//		HRESULT Destroy( );
//	
//	//	SourceType GetSourceType( ) const;
//	};
//	
