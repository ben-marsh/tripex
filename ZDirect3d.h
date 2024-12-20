#pragma once

#include <d3d9.h>
#include "ZArray.h"
#include "ColorRgb.h"
#include "Vector3.h"
#include "Face.h"
#include "Edge.h"
#include "Point.h"
#include "Rect.h"
#include <map>
#include <set>
#include "error.h"
#include "Vertex.h"

class Texture;
class ZPalette;

class ZDirect3D
{
public:
	typedef std::map< DWORD, std::pair< DWORD, DWORD > > StateBuffer;
	typedef std::map< DWORD, DWORD > NewStateBuffer;
	typedef std::pair< StateBuffer::iterator, bool > StateBufferChange;

	class TextureStage
	{
		friend ZDirect3D;

	protected:
		StateBuffer mpState;
		NewStateBuffer mpNewState;
		Texture *pTexture;
		Texture *pNewTexture;

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
	std::set< std::pair< uint16, uint16 > > g_spnFreePalette;
	std::set< Texture* > g_spTexture;

	ZDirect3D( );

	Error* Open( );
	Error* Close( );
	Error* DrawIndexedPrimitive(ZArray<VertexTL> &pVertex, ZArray<Face> &pfFace);

	Error* AddTexture( Texture *pTexture );
	Error* UploadTexture( Texture *pTexture );
	void GetStateChanges(NewStateBuffer &mp_new, StateBuffer &mp_current, std::vector< StateBufferChange > &vc);

	Error* CreateTexture(Texture *pTexture);
	void DestroyTexture(Texture *pTexture);

	Texture *Find(int nType);

	int GetWidth( );
	int GetHeight( );

	// render state
	void SetState(int nFlags);
	void ResetRenderState();
	void SetRenderState(DWORD dwKey, DWORD dwValue);
	Error* FlushRenderState();

	// texture state
	void ResetTextureState();
	void ResetTextureStageState(DWORD dwStage);
	void SetTexture(DWORD dwStage, Texture *pTexture, DWORD dwOp = D3DTOP_MODULATE, DWORD dwArg2 = D3DTA_CURRENT);
	void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwKey, DWORD dwValue);
	Error* FlushTextureState();

	static void BuildSprite(ZArray<VertexTL> &pVertex, ZArray<Face> &pFace, const Point<int> &p, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
	Error* DrawSprite(const Point<int> &p, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
};

extern ZDirect3D *g_pD3D;
