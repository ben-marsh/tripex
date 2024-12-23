#pragma once

#include "Error.h"
#include "Vertex.h"
#include "Face.h"
#include "ZArray.h"
#include "Texture.h"
#include "GeometryBuffer.h"
#include <d3d9.h>

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

	Error* DrawIndexedPrimitive(const RenderState& render_state, ZArray<VertexTL>& vertices, ZArray<Face>& faces);
	Error* DrawIndexedPrimitive(const RenderState& render_state, const std::vector<VertexTL>& vertices, const std::vector<Face>& faces);
	Error* DrawIndexedPrimitive(const RenderState& render_state, const GeometryBuffer& geometry_buffer);

	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces) = 0;
};
