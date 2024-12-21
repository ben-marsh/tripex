#pragma once

#include "ZDirect3D.h"
#include "ColorRgb.h"
#include <memory>

class Canvas
{
public:
	static const int texture_w = 256;
	static const int texture_h = 256;

	const int num_textures_x;
	const int num_textures_y;
	const int stride;

	ColorRgb color;
	PALETTEENTRY palette[256];

	Canvas(int width, int height);
	Error* Create();
	uint8* GetDataPtr();
	Error* UploadTextures();
	Texture* GetTexture(int x, int y);
	Error* Render();

private:
	std::unique_ptr<uint8[]> data;
	std::vector<Texture> textures;
};
