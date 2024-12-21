#pragma once

#include <d3d9.h>
#include "ZDirect3D.h"

class Texture
{
public:
	enum
	{
		F_DYNAMIC = (1 << 0),
		F_MIP_CHAIN = (1 << 1),
		F_FILTERING = (1 << 2),
	};

	uint32 flags;
	IDirect3DTexture9* d3d_texture;

	D3DFORMAT format;
	uint32 data_size;
	uint32 data_stride;
	const void* data;
	const PALETTEENTRY* palette;

	std::set<TextureClass> classes;

	Texture();
	~Texture();

	void SetFlags(uint32 flags);

	void SetSource(const void* data, uint32 data_size);
	void SetSource(D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride);
	void SetSource(const PALETTEENTRY* pPalette, const void* data, uint32 data_size, uint32 data_stride);
};
