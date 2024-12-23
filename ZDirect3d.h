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
#include "TextureLibrary.h"
#include "Renderer.h"

class Texture;
class ZPalette;

class ZDirect3D : public Renderer
{
public:
	IDirect3DDevice9 *g_pDevice;
	D3DCAPS9 g_Caps;
	unsigned int nWidth, nHeight;
	std::set<Texture*> created_textures;

	ZDirect3D( );

	Error* Open( );
	Error* Close( );

	using Renderer::DrawIndexedPrimitive;

	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces) override;

	Error* AddTexture( Texture *pTexture );
	Error* UploadTexture( Texture *pTexture );

	Error* CreateTexture(Texture *pTexture);
	void DestroyTexture(Texture *pTexture);

	int GetWidth( );
	int GetHeight( );

	Error* DrawSprite(const RenderState& render_state, const Point<int> &p, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
};

extern ZDirect3D *g_pD3D;
