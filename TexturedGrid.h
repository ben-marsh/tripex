#pragma once

#include "ZDirect3D.h"

class TexturedGrid
{
private:
	int scr_width, scr_height;

public:
	ZArray<VertexTL> vertices;
	ZArray<Face> faces;
	int width, height;
	int start_x, start_y;
	float pos_x, pos_y;
	bool update_edges;

	TexturedGrid(int width, int height);
	Error* Render(const RenderState& render_state);
};
