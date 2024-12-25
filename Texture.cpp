#include "Platform.h"
#include "Texture.h"
#include "error.h"

Texture::Texture(int width, int height, D3DFORMAT format, TextureFlags flags)
	: width(width)
	, height(height)
	, format(format)
	, flags(flags)
{
}

Texture::~Texture()
{
}

