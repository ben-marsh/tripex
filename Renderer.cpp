#include "Renderer.h"
#include <assert.h>

////// TextureStage //////

TextureStage::TextureStage()
{
	texture = nullptr;
}

////// RenderState //////

RenderState::RenderState()
{
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
