#include "Platform.h"
#include "ZDirect3D.h"
#include "ColorRgb.h"
#include "SpriteBuffer.h"
#include "error.h"
#include "Texture.h"

SpriteBuffer::SpriteBuffer()
{
}

void SpriteBuffer::Clear()
{
	sprites.clear();
}

Error* SpriteBuffer::Flush()
{
	Error* error;

	for (int i = 0; i < (int)sprites.size(); )
	{
		Texture* texture = sprites[i].texture;
		g_pD3D->SetTexture(0, texture);

		int state = sprites[i].state;
		g_pD3D->SetState(state);

		std::vector<VertexTL> vertices;
		std::vector<Face> faces;

		for (; i < sprites.size() && sprites[i].texture == texture && sprites[i].state == state; i++)
		{
			const Sprite& sprite = sprites[i];
			SpriteBuffer::AddSprite(sprite.pos, sprite.src, sprite.diffuse, (ColorRgb)ColorRgb::Black(), vertices, faces);
		}

		error = g_pD3D->DrawIndexedPrimitive(vertices, faces);
		if (error) return TraceError(error);
	}
	Clear();
	return nullptr;
}

void SpriteBuffer::AddItem(const Sprite& item)
{
	_ASSERT(item.texture == NULL || !IsBadReadPtr(item.texture, sizeof(Texture)));
	if (!(item.src.IsValid())) return;

	if (item.diffuse == ColorRgb::Black())
	{
		if (item.state == ZDirect3D::InverseMultiply) return;
		if (item.state == ZDirect3D::LuminanceOpacity) return;
		if (item.state == ZDirect3D::Transparent) return;
	}
	else if (item.diffuse == ColorRgb::White() && item.texture == NULL)
	{
		if (item.state == ZDirect3D::Multiply) return;
	}

	sprites.push_back(item);
}

void SpriteBuffer::AddSprite(const Point<int>& pos, Texture* texture, int state, const Rect<int>& src, ColorRgb diffuse)
{
	Sprite sprite;
	sprite.pos = pos;
	sprite.src = src;
	sprite.texture = texture;
	sprite.state = state;
	sprite.diffuse = diffuse;
	AddItem(sprite);
}

void SpriteBuffer::AddSprite(const Point<int>& pos, const Rect<int>& src, ColorRgb diffuse, ColorRgb specular, std::vector<VertexTL>& vertices, std::vector<Face>& faces)
{
	uint16_t first_vertex = (uint16_t)vertices.size();
	vertices.resize(vertices.size() + 4);

	const float mult = 1.0f / 256.0f;

	Point<float> p2((src.left * mult), (src.top * mult));

	float w = (float)(src.right - src.left);
	float h = (float)(src.bottom - src.top);
	float tex_w = w * mult;
	float tex_h = h * mult;
	for (int i = 0; i < 4; i++)
	{
		VertexTL& vertex = vertices[first_vertex + i];
		vertex.position = Vector3(pos.x - 0.5f, pos.y - 0.5f, 0.1f);
		vertex.tex_coords[0] = p2;
		if (i == 1 || i == 2)
		{
			vertex.position.x += w;
			vertex.tex_coords[0].x += tex_w;
		}
		if (i == 2 || i == 3)
		{
			vertex.position.y += h;
			vertex.tex_coords[0].y += tex_h;
		}
		vertex.rhw = 1.0f;
		vertex.specular = specular;
		vertex.diffuse = diffuse;
	}

	faces.push_back(Face(first_vertex + 0, first_vertex + 1, first_vertex + 3));
	faces.push_back(Face(first_vertex + 1, first_vertex + 2, first_vertex + 3));
}
