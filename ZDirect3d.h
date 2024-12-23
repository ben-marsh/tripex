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
	ZDirect3D();

	Error* Open();
	Error* Close();

	Error* BeginFrame() override;
	Error* EndFrame() override;

	using Renderer::DrawIndexedPrimitive;

	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces) override;

	Error* AddTexture(Texture* texture);
	Error* UploadTexture(Texture* texture);

	Error* CreateTexture(Texture* texture);
	void DestroyTexture(Texture* texture);

	int GetWidth() const;
	int GetHeight() const;

	Rect<float> GetClipRect() const;

	Error* DrawSprite(const RenderState& render_state, const Point<int>& p, const Rect<int>& spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());

private:
	IDirect3DDevice9* device;
	D3DCAPS9 caps;
	unsigned int width, height;
	std::set<Texture*> created_textures;
};

extern ZDirect3D* g_pD3D;
