#pragma once

#include "ColorRgb.h"
#include "Point.h"
#include "Rect.h"
#include "Face.h"
#include "error.h"
#include "Texture.h"
#include <vector>
#include "Vertex.h"

class SpriteBuffer
{
public:
	SpriteBuffer();
	void Clear();
	Error* Flush();

	void AddSprite(const Point<int>& pos, Texture* texture, int state, const Rect<int>& src, ColorRgb diffuse = ColorRgb::White());

	static void AddSprite(const Point<int>& pos, const Rect<int>& src, ColorRgb diffuse, ColorRgb specular, std::vector<VertexTL>& vertices, std::vector<Face>& faces);

private:
	struct Sprite
	{
		int state;
		class Texture* texture;

		Point<int> pos;
		Rect<int> src;
		ColorRgb diffuse;
	};

	std::vector<Sprite> sprites;

	void AddItem(const Sprite& sprite);
};
