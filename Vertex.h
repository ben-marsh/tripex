#pragma once

#include "Vector3.h"
#include "ColorRgb.h"
#include "Point.h"

#define MAX_TEXTURES 1

struct Vertex
{
	Vector3 position;
	Vector3 normal;
	ColorRgb diffuse;
	ColorRgb specular;
	Point<float> tex_coord[MAX_TEXTURES];
};

struct VertexTL // Transformed and lit vertex
{
	Vector3 position;
	float rhw;
	ColorRgb diffuse;
	ColorRgb specular;
	Point<float> tex_coord[MAX_TEXTURES];
};
