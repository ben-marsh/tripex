#include "Platform.h"
#include "Canvas.h"
#include "error.h"
#include "Texture.h"
#include "ZDirect3D.h"

/*---------------------------------
* Constructor:
-----------------------------------*/

Canvas::Canvas(int width, int height)
	: num_textures_x((width + (texture_w - 1)) / texture_w)
	, num_textures_y((height + (texture_h - 1)) / texture_h)
	, stride(num_textures_x * texture_w)
{
	int num_textures = num_textures_x * num_textures_y;
	data = std::make_unique<uint8[]>(stride * num_textures_y * texture_h);
	textures.resize(num_textures);
	memset(palette, 0, sizeof(palette));

	int32 texture_idx = 0;
	for (int32 y = 0; y < num_textures_y; y++)
	{
		for (int32 x = 0; x < num_textures_x; x++)
		{
			uint8* source_data = &data[(x * 256) + (y * 256 * stride)];

			Texture& texture = textures[texture_idx++];
			texture.SetFlags(Texture::F_DYNAMIC);
			texture.SetSource(palette, source_data, 256 * 256, stride);
		}
	}
}

Error* Canvas::Create()
{
	for (Texture& texture : textures)
	{
		Error* error = g_pD3D->AddTexture(&texture);
		if (error) return TraceError(error);
	}
	return nullptr;
}

uint8* Canvas::GetDataPtr()
{
	return data.get();
}

Error* Canvas::UploadTextures()
{
	for (Texture& texture : textures)
	{
		Error* error = g_pD3D->UploadTexture(&texture);
		if (error) return TraceError(error);
	}
	return nullptr;
}

Texture* Canvas::GetTexture(int x, int y)
{
	return &textures[(y * num_textures_x) + x];
}

Error* Canvas::Render()
{
	Error* error;
	ZArray<Face> faces;
	faces.Add(Face(0, 1, 3));
	faces.Add(Face(1, 2, 3));
	//WORD face[6] = { 0, 1, 3, 1, 2, 3 };

	ZArray<VertexTL> v(4);
	//	ZVertexTL v[4];
	v[0].position.z = 1;
	v[0].rhw = 1;
	v[1].position.z = 1;
	v[1].rhw = 1;
	v[2].position.z = 1;
	v[2].rhw = 1;
	v[3].position.z = 1;
	v[3].rhw = 1;

	//	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 1;
	//	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1;
	v[0].diffuse = v[1].diffuse = v[2].diffuse = v[3].diffuse = color;

	int width = num_textures_x * 256;
	int height = num_textures_y * 256;

	float s = float(g_pD3D->GetWidth()) / width;
	//	double ys = double(d3d->GetHeight()) / height;
	//	double s = xs;//min(xs, ys);

	float xc = (g_pD3D->GetWidth() - (num_textures_x * 254 * s)) / 2;
	float yc = (g_pD3D->GetHeight() - (num_textures_y * 254 * s)) / 2;

	int i = 0;
	for (int vi = 0; vi < num_textures_y; vi++)
	{
		for (int hi = 0; hi < num_textures_x; hi++)
		{
			v[0].position.x = xc + (hi * 254 * s);
			v[0].position.y = yc + (vi * 254 * s);
			v[0].tex_coords[0].x = (1 / 256.0);
			v[0].tex_coords[0].y = (1 / 256.0);

			v[1].position.x = xc + ((hi + 1) * 254 * s);
			v[1].position.y = v[0].position.y;//yc + (vi * 254 * s);
			v[1].tex_coords[0].x = (255 / 256.0);
			v[1].tex_coords[0].y = (1 / 256.0);

			v[2].position.x = xc + ((hi + 1) * 254 * s);
			v[2].position.y = yc + ((vi + 1) * 254 * s);
			v[2].tex_coords[0].x = (255 / 256.0);//1 - (0.5 / 256.0);
			v[2].tex_coords[0].y = (255 / 256.0);//1;// - (1 / 256.0);

			v[3].position.x = xc + (hi * 254 * s);
			v[3].position.y = v[2].position.y;//yc + ((vi + 1) * 254 * s);
			v[3].tex_coords[0].x = (1 / 256.0);
			v[3].tex_coords[0].y = (255 / 256.0);//1;// - (1 / 256.0);

			if (v[0].position.y < 0)
			{
				if (v[2].position.y < 0) continue;
				float fPos = (0 - v[0].position.y) / (v[2].position.y - v[0].position.y);

				v[0].position.y = 0;
				v[0].tex_coords[0].x = (v[0].tex_coords[0].x * (1 - fPos)) + (v[3].tex_coords[0].x * fPos);
				v[0].tex_coords[0].y = (v[0].tex_coords[0].y * (1 - fPos)) + (v[3].tex_coords[0].y * fPos);

				v[1].position.y = 0;
				v[1].tex_coords[0].x = (v[1].tex_coords[0].x * (1 - fPos)) + (v[2].tex_coords[0].x * fPos);
				v[1].tex_coords[0].y = (v[1].tex_coords[0].y * (1 - fPos)) + (v[2].tex_coords[0].y * fPos);
			}
			if (v[3].position.y > g_pD3D->GetHeight())
			{
				if (v[0].position.y > g_pD3D->GetHeight()) continue;

				float fPos = (g_pD3D->GetHeight() - v[0].position.y) / (v[3].position.y - v[0].position.y);
				v[2].tex_coords[0].x = (v[1].tex_coords[0].x * (1 - fPos)) + (v[2].tex_coords[0].x * fPos);
				v[2].tex_coords[0].y = (v[1].tex_coords[0].y * (1 - fPos)) + (v[2].tex_coords[0].y * fPos);

				v[3].tex_coords[0].x = (v[0].tex_coords[0].x * (1 - fPos)) + (v[3].tex_coords[0].x * fPos);
				v[3].tex_coords[0].y = (v[0].tex_coords[0].y * (1 - fPos)) + (v[3].tex_coords[0].y * fPos);

				v[2].position.y = v[3].position.y = (float)g_pD3D->GetHeight();
			}

			g_pD3D->SetTexture(0, GetTexture(hi, vi));

			error = g_pD3D->DrawIndexedPrimitive(v, faces);
			if (error) return TraceError(error);
		}
	}
	return nullptr;
}

