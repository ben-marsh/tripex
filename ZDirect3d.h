#pragma once

#include "ZArray.h"
#include "ZColour.h"
#include "ZVector.h"
#include "ZFace.h"
#include "ZEdge.h"
#include "ZPoint.h"
#include "ZRect.h"

#define MAX_TEXTURES 1

class ZVertex
{
public:
	ZVector m_vPos;
	ZVector m_vNormal;
	ZColour m_cDiffuse;
	ZColour m_cSpecular;
	ZPoint< FLOAT32 > m_aTex[MAX_TEXTURES];
};
class ZVertexTL
{
public:
	ZVector m_vPos;
	FLOAT32 m_fRHW;
	ZColour m_cDiffuse;
	ZColour m_cSpecular;
	ZPoint< FLOAT32 > m_aTex[MAX_TEXTURES];
};

class ZTexture;
class ZPalette;

class ZDirect3D
{
public:
	typedef map< DWORD, pair< DWORD, DWORD > > StateBuffer;
	typedef map< DWORD, DWORD > NewStateBuffer;
	typedef pair< StateBuffer::iterator, bool > StateBufferChange;

	class TextureStage
	{
		friend ZDirect3D;

	protected:
		StateBuffer mpState;
		NewStateBuffer mpNewState;
		ZTexture *pTexture;
		ZTexture *pNewTexture;

	public:
		TextureStage();
	};

	enum
	{
		Transparent = (1 << 0L),
		Multiply = (1 << 1L),
		Perspective = (1 << 2L),
		Shade = (1 << 3L),
		ZBuffer = (1 << 4L),
		PerspectiveCorrect = (1 << 5L),
		LuminanceOpacity = (1 << 6L),
		InverseMultiply = (1 << 7L),
	};

	IDirect3DDevice9 *g_pDevice;
	D3DCAPS9 g_Caps;
	unsigned int nWidth, nHeight;
	StateBuffer mpRender;
	NewStateBuffer mpNewRender;
	TextureStage pTextureStage[8];
	set< pair< UINT16, UINT16 > > g_spnFreePalette;
	set< ZTexture* > g_spTexture;

	ZDirect3D( );

	HRESULT Open( );
	HRESULT Close( );
	HRESULT DrawIndexedPrimitive(ZArray<ZVertexTL> &pVertex, ZArray<ZFace> &pfFace);

	HRESULT AddTexture( ZTexture *pTexture );
	HRESULT UploadTexture( ZTexture *pTexture );
	void GetStateChanges(NewStateBuffer &mp_new, StateBuffer &mp_current, vector< StateBufferChange > &vc);

	HRESULT CreateTexture(ZTexture *pTexture);
	void DestroyTexture(ZTexture *pTexture);

	ZTexture *Find(int nType);

	int GetWidth( );
	int GetHeight( );

	// render state
	void SetState(int nFlags);
	void ResetRenderState();
	void SetRenderState(DWORD dwKey, DWORD dwValue);
	HRESULT FlushRenderState();

	// texture state
	void ResetTextureState();
	void ResetTextureStageState(DWORD dwStage);
	void SetTexture(DWORD dwStage, ZTexture *pTexture, DWORD dwOp = D3DTOP_MODULATE, DWORD dwArg2 = D3DTA_CURRENT);
	void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwKey, DWORD dwValue);
	HRESULT FlushTextureState();

	static void BuildSprite(ZArray<ZVertexTL> &pVertex, ZArray<ZFace> &pFace, const ZPoint<int> &p, const ZRect<int> &spr, ZColour cDiffuse = ZColour::White(), ZColour cSpecular = ZColour::Black());
	HRESULT DrawSprite(const ZPoint<int> &p, const ZRect<int> &spr, ZColour cDiffuse = ZColour::White(), ZColour cSpecular = ZColour::Black());
};

extern ZDirect3D *g_pD3D;
