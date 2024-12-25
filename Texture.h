#pragma once

#include <d3d9.h>
#include "Error.h"

enum class TextureFlags
{
	None = 0,
	Dynamic = 1,
	CreateMips = 2,
	Filter = 4,
};

class Texture
{
public:
	const int width;
	const int height;
	const D3DFORMAT format;
	const TextureFlags flags;

	Texture(int width, int height, D3DFORMAT format, TextureFlags flags);
	virtual ~Texture();
	virtual void SetDirty() = 0;
	virtual Error* GetPixelData(std::vector<uint8>& buffer) const = 0;
};

IMPLEMENT_ENUM_FLAGS(TextureFlags)
