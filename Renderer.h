#pragma once

#include "Error.h"
#include "Vertex.h"
#include "Face.h"
#include "Texture.h"
#include "GeometryBuffer.h"
#include <d3d9.h>
#include <memory>

struct TextureStage
{
	Texture* texture;
	D3DTEXTUREADDRESS address_u;
	D3DTEXTUREADDRESS address_v;

	TextureStage();
};

struct RenderState
{
	static const int NUM_TEXTURE_STAGES = 1;

	bool enable_culling;
	bool enable_shading;
	bool enable_specular;
	bool enable_zbuffer;
	bool enable_zbuffer_write;
	D3DCMPFUNC zfunc;
	D3DBLEND src_blend; // new pixel
	D3DBLEND dst_blend; // existing pixel

	TextureStage texture_stages[NUM_TEXTURE_STAGES];

	RenderState();
};

class Renderer
{
public:
	virtual Error* BeginFrame() = 0;
	virtual Error* EndFrame() = 0;

	int GetWidth() const;
	int GetHeight() const;

	virtual Rect<int> GetViewportRect() const = 0;
	virtual Rect<float> GetClipRect() const = 0;

	virtual Error* CreateTexture(int width, int height, D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride, const PALETTEENTRY* palette, TextureFlags flags, std::shared_ptr<Texture> &out_texture) = 0;
	virtual Error* CreateTextureFromImage(const void* data, uint32 data_size, std::shared_ptr<Texture>& out_texture) = 0;

	Error* DrawIndexedPrimitive(const RenderState& render_state, const std::vector<VertexTL>& vertices, const std::vector<Face>& faces);
	Error* DrawIndexedPrimitive(const RenderState& render_state, const GeometryBuffer& geometry_buffer);

	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, size_t num_vertices, const VertexTL* vertices, size_t num_faces, const Face* faces) = 0;

	Error* DrawSprite(const RenderState& render_state, const Point<int>& p, const Rect<int>& spr, ColorRgb diffuse = ColorRgb::White(), ColorRgb specular = ColorRgb::Black());
};
