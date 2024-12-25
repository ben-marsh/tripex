#pragma once

#include <d3d9.h>
#include "ZArray.h"
#include "ColorRgb.h"
#include "Vector3.h"
#include "Face.h"
#include "Edge.h"
#include "Point.h"
#include "Rect.h"
#include <map>
#include <set>
#include "error.h"
#include "Vertex.h"
#include "TextureLibrary.h"
#include "Renderer.h"
#include "ComPtr.h"

class Texture;
class ZPalette;

class ZDirect3D : public Renderer
{
public:
	ZDirect3D();

	Error* Open(HWND hwnd);
	void Close();

	Error* BeginFrame() override;
	Error* EndFrame() override;

	virtual Error* CreateTexture(int width, int height, D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride, const PALETTEENTRY* palette, TextureFlags flags, std::shared_ptr<Texture>& out_texture) override;
	virtual Error* CreateTextureFromImage(const void* data, uint32 data_size, std::shared_ptr<Texture>& out_texture) override;

	using Renderer::DrawIndexedPrimitive;

	virtual Error* DrawIndexedPrimitive(const RenderState& render_state, uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces) override;

	virtual Rect<int> GetViewportRect() const override;
	virtual Rect<float> GetClipRect() const override;

private:
	class TextureImpl : public Texture
	{
	public:
		const void* const data;
		const uint32 data_size;
		const uint32 data_stride;
		const PALETTEENTRY* const palette;

		ComPtr<IDirect3DTexture9> d3d_texture;
		bool dirty;

		TextureImpl(int width, int height, D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride, const PALETTEENTRY* palette, TextureFlags flags);
		virtual ~TextureImpl() override;
		virtual void SetDirty() override;
		virtual Error* GetPixelData(std::vector<uint8>& buffer) const override;
	};

	ComPtr<IDirect3D9> d3d;
	ComPtr<IDirect3DDevice9> device;
	D3DCAPS9 caps;
	unsigned int width, height;

	Error* UploadTexture(IDirect3DTexture9* d3d_texture, int width, int height, D3DFORMAT format, const void* data, uint32 data_size, uint32 data_stride, const PALETTEENTRY* palette);
};
