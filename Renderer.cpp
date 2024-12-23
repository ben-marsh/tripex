#include "Renderer.h"
#include <assert.h>

////// TextureStage //////

TextureStage::TextureStage()
{
	texture = nullptr;
	address_u = D3DTADDRESS_WRAP;
	address_v = D3DTADDRESS_WRAP;
};

////// RenderState //////

RenderState::RenderState()
{
	enable_culling = true;
	enable_shading = true;
	enable_specular = false;
	enable_zbuffer = true;
	enable_zbuffer_write = true;
	zfunc = D3DCMP_LESS;
	src_blend = D3DBLEND_ONE;
	dst_blend = D3DBLEND_ZERO;
}

////// Renderer //////

Error* Renderer::DrawIndexedPrimitive(const RenderState& render_state, ZArray<VertexTL>& vertices, ZArray<Face>& faces)
{
	Error* error = DrawIndexedPrimitive(render_state, vertices.GetLength(), vertices.GetBuffer(), faces.GetLength(), faces.GetBuffer());
	return TraceError(error);
}

Error* Renderer::DrawIndexedPrimitive(const RenderState& render_state, const std::vector<VertexTL>& vertices, const std::vector<Face>& faces)
{
	Error* error = DrawIndexedPrimitive(render_state, (uint32_t)vertices.size(), vertices.data(), (uint32_t)faces.size(), faces.data());
	return TraceError(error);
}

Error* Renderer::DrawIndexedPrimitive(const RenderState& render_state, const GeometryBuffer& geometry_buffer)
{
	Error* error = DrawIndexedPrimitive(render_state, (uint32_t)geometry_buffer.vertices.size(), geometry_buffer.vertices.data(), (uint32_t)geometry_buffer.faces.size(), geometry_buffer.faces.data());
	return TraceError(error);
}
