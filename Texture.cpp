#include "Platform.h"
#include "Texture.h"
#include "error.h"

Texture::Texture()
{
	d3d_texture = NULL;
	flags = 0;

	format = D3DFMT_UNKNOWN;
	data_size = 0;
	data_stride = 0;
	data = NULL;
	palette = NULL;
}

Texture::~Texture()
{
	_ASSERT(d3d_texture == NULL);
}

void Texture::SetFlags(uint32 flags)
{
	_ASSERT(d3d_texture == NULL);
	this->flags = flags;
}

void Texture::SetSource(const void* data, uint32 data_size)
{
	format = D3DFMT_UNKNOWN;
	this->data = data;
	this->data_size = data_size;
	palette = NULL;
}

void Texture::SetSource(const PALETTEENTRY* palette, const void* data, uint32 data_size, uint32 data_stride)
{
	format = D3DFMT_P8;
	this->data = data;
	this->data_size = data_size;
	this->data_stride = data_stride;
	this->palette = palette;
}

void Texture::SetSource(D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride)
{
	_ASSERT(format != D3DFMT_P8);

	this->format = format;
	this->data = data;
	this->data_size = data_size;
	this->data_stride = data_stride;

	palette = NULL;
}
