#pragma once

#include "Error.h"
#include "Vertex.h"
#include "Face.h"
#include "ZArray.h"
#include "Texture.h"

struct TextureStage
{
	Texture* texture;

	TextureStage();
};

struct RenderState
{
	static const int NumStages = 1;

	TextureStage stages[NumStages];

	RenderState();
};

class Renderer
{
public:
	Error* DrawIndexedPrimitive(const RenderState& render_state, ZArray<VertexTL>& vertices, ZArray<Face>& faces);
	Error* DrawIndexedPrimitive(const RenderState& render_state, const std::vector<VertexTL>& vertices, const std::vector<Face>& faces);
	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces) = 0;
};

