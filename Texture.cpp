#include "StdAfx.h"
//#include <d3dx8.h>
#include "Texture.h"
#include "error.h"

/*---------------------------------
* ZTexture
-----------------------------------*/

/*---------------------------------
* Constructor
-----------------------------------*/

Texture::Texture( )
{
	m_pd3dTexture = NULL;
	m_nFlags = 0;

	m_nSrcSize = 0;
	m_pSrcData = NULL;
	m_pSrcPalette = NULL;
}

/*---------------------------------
* Destructor
-----------------------------------*/

Texture::~Texture( )
{
	_ASSERT( m_pd3dTexture == NULL );
}

/*---------------------------------
* SetFlags( ):
-----------------------------------*/

void Texture::SetFlags( UINT32 nFlags )
{
	_ASSERT( m_pd3dTexture == NULL );
	m_nFlags = nFlags;
}

/*---------------------------------
* SetSource( )
-----------------------------------*/

void Texture::SetSource( const void *pSrcData, UINT32 nSrcSize )
{
	m_nSrcFmt = D3DFMT_UNKNOWN;
	m_pSrcData = pSrcData;
	m_nSrcSize = nSrcSize;
	m_pSrcPalette = NULL;
}

void Texture::SetSource( const PALETTEENTRY *pSrcPalette, const void *pSrcData, UINT32 nSrcSize, UINT32 nSrcSpan )
{
	m_nSrcFmt = D3DFMT_P8;
	m_pSrcData = pSrcData;
	m_nSrcSize = nSrcSize;
	m_nSrcSpan = nSrcSpan;
	m_pSrcPalette = pSrcPalette;
}

void Texture::SetSource( D3DFORMAT nSrcFmt, const void *pSrcData, UINT32 nSrcSize, UINT32 nSrcSpan )
{
	_ASSERT( nSrcFmt != D3DFMT_P8 );

	m_nSrcFmt = nSrcFmt;
	m_pSrcData = pSrcData;
	m_nSrcSize = nSrcSize;
	m_nSrcSpan = nSrcSpan;
	m_pSrcPalette = NULL;
}
/*
void ZTexture::SetSource( const void *pSrcData, UINT32 nSrcSize, const PALETTEENTRY *pPalette )
{
	m_pSrcData = pSrcData;
	m_nSrcSize = nSrcSize;
	m_pSrcPalette = pPalette;
}

/*
class ZTexture
{
protected:
	UINT32 m_nFlags;
	IDirect3DTexture8 *m_pd3dTexture;

public:
	enum
	{
		F_DYNAMIC = ( 1 << 0 ),
		F_MIP_CHAIN = ( 1 << 1 ),
		F_FILTERING = ( 1 << 2 ),
		F_SRC_FILE = ( 1 << 3 ),
	};

	// if palette is set before texture created? must be constant ptr
	// if palette is set in dynamic texture?

	D3DFORMAT m_nSrcFmt;
	UINT32 m_nSrcSize;
	const void *m_pSrc;
	const PALETTEENTRY *m_pSrcPalette;

	// Constructor:
	ZTexture( UINT32 nFlags );

	// Destructor:
	~ZTexture( );

	// SetSource( ):
	void SetSource( const UINT8 *pnIdx, UINT32 nSize, const PALETTEENTRY *pPalette );
	void SetSource( D3DFORMAT nFmt, void *pSrc, UINT32 nSize );
};






//#define TraceError(x) (x)

//	/*** ZTexture **************************************************************/
//	
//	ZTexture::ZTexture()
//	{
//		m_pd3dTexture = NULL;
//	
//		m_nPrefFormat = D3DFMT_X8R8G8B8;
//		m_pnData = NULL;
//		m_pnPalette = NULL;
//		m_nDataSize = 0;
//	}
//	
//	HRESULT ZTexture::Create( )
//	{
//		if( pTexture->m_nFlags.test( ZTexture::F_SOURCE_FILE ) )
//		{
//			hRes = D3DXCreateTextureFromFileInMemory( g_pDevice, pTexture->m_pnData, pTexture->m_nDataSize, &pTexture->m_pd3dTexture );
//			if( FAILED( hRes ) ) return TraceError( hRes );
//		}
//		else
//		{
//			hRes = D3DXCreateTexture( g_pDevice, 256, 256, 1, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &pTexture->m_pd3dTexture );
//			if( FAILED( hRes ) ) return TraceError( hRes );
//		}
//	}
//	
//	HRESULT ZTexture::Update( )
//	{
//	}
//	
//	HRESULT ZTexture::Destroy( )
//	{
//		if( m_pd3dTexture != NULL )
//		{
//			m_pd3dTexture->Release( );
//			m_pd3dTexture = NULL;
//		}
//	}
//	
//	
//	/*
//	ZTexture::ZTexture(ZColour *pcData)
//	{
//		assert(!IsBadReadPtr(pcData, 256 * 256 * sizeof(ZColour)));
//	
//		m_pd3dTexture = NULL;
//		m_vnData.assign((unsigned char*)(pcData), (unsigned char*)(pcData + (256 * 256)));
//	
//		SetDepths(16, 24, 32);
//	}
//	*/
//	/*
//	ZTexture::SourceType ZTexture::GetSourceType( ) const
//	{
//		return m_nSourceType;
//	}
//	
//	
//	/*
//	ZTexture::ZTexture(unsigned char *pnIndex, ZColour *pcPalette)
//	{
//		assert(!IsBadReadPtr(pnIndex, 256 * 256));
//		assert(!IsBadReadPtr(pcPalette, 256 * sizeof(ZColour)));
//	
//		pd3dTexture = NULL;
//		vnData.assign(pnIndex, pnIndex + (256 * 256));
//		vcPalette.assign(pcPalette, pcPalette + 256);
//	
//		SetDepths(16, 24, 32);
//	}
//	*/
//	
//	ZTexture::~ZTexture()
//	{
//		assert(m_pd3dTexture == NULL);
//	//	if(hbm != NULL) DeleteObject(hbm);
//	}
//	/*
//	HRESULT ZTexture::Load(ZDirect3D *pd3d)
//	{
//		assert(vnFormat.size() > 0);
//		assert(vpAdapterTexture.size() == 0);
//	
//		vpAdapterTexture.clear();
//	
//		for(int i = 0; i < pd3d->vpAdapter.size(); i++)
//		{
//			auto_ptr< AdapterTexture > pTex = auto_ptr< AdapterTexture >(new AdapterTexture);
//	
//			HRESULT hRes = pTex->Load(this, pd3d->vpAdapter[i].get());
//			if(FAILED(hRes)) return TraceError(hRes);
//	
//			vpAdapterTexture.push_back(pTex);
//		}
//	
//		Set(Loaded);
//		return D3D_OK;
//	}
//	HRESULT ZTexture::Copy()
//	{
//		HRESULT hRes = Copy(vcData.begin());
//		if(FAILED(hRes)) return TraceError(hRes);
//	
//		return D3D_OK;
//	}
//	HRESULT ZTexture::Copy(ZColour *pc)
//	{
//		for(int i = 0; i < vpAdapterTexture.size(); i++)
//		{
//			HRESULT hRes = vpAdapterTexture[i]->Copy(pc, NULL);
//			if(FAILED(hRes)) return TraceError(hRes);
//		}
//		return D3D_OK;
//	}
//	HRESULT ZTexture::Copy(unsigned char *pnData, ZColour *pc)
//	{
//		for(int i = 0; i < vpAdapterTexture.size(); i++)
//		{
//			HRESULT hRes = vpAdapterTexture[i]->Copy(pnData, pc);
//			if(FAILED(hRes)) return TraceError(hRes);
//		}
//		return D3D_OK;
//	}
//	*/
//	/*
//	void ZTexture::SetDepths(int n1, int n2, int n3)
//	{
//		int pn[] = { n1, n2, n3 };
//	
//		m_vnFormat.clear();
//		for(int i = 0; i < sizeof(pn) / sizeof(pn[0]); i++)
//		{
//			switch(pn[i])
//			{
//			case 0:
//				break;
//			case 16:
//				m_vnFormat.push_back(D3DFMT_R5G6B5);
//				m_vnFormat.push_back(D3DFMT_X1R5G5B5);
//				break;
//			case 24:
//				m_vnFormat.push_back(D3DFMT_R8G8B8);
//				break;
//			case 32:
//				m_vnFormat.push_back(D3DFMT_X8R8G8B8);
//				break;
//			default:
//				assert(false);
//				break;
//			}
//		}
//		for(i = 0; i < sizeof(pn) / sizeof(pn[0]); i++)
//		{
//			switch(pn[i])
//			{
//			case 16:
//				m_vnFormat.push_back(D3DFMT_A1R5G5B5);
//				break;
//			case 32:
//				m_vnFormat.push_back(D3DFMT_A8R8G8B8);
//				break;
//			}
//		}
//	}
//	*/
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
//	
