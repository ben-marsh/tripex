#pragma once

#include "ZDirect3D.h"
#include "Texture.h"
#include "ColorRgb.h"
#include "SpriteBuffer.h"
#include <memory>

#define TF_FADE_IN_START 8
#define TF_FADE_IN_FINISH 0 // default
#define TF_FADE_OUT_START 0 // default
#define TF_FADE_OUT_FINISH 16

class TextureFont
{
public:
	static const int LETTER_APPEAR_FRAMES = 200;
	static const int LETTER_DIFF_FRAMES = 10;

	int width, height;

	struct Glyph
	{
		char character;
		int start, end;
		int bitmap_x, bitmap_y;
		std::unique_ptr<uint8[]> data;
	};

	std::vector<Glyph> glyphs;
	std::unique_ptr<Texture> texture;
	std::unique_ptr<ColorRgb[]> pixels;
	uint8 char_to_glyph[256];

	bool LineAboveThreshold(BYTE *data);

	TextureFont();
	~TextureFont();
	void Add(const uint8 *source_data);
	ColorRgb *GetBitmap();
	Texture* GetTexture();
	Glyph *FindGlyph(char character);
	int GetWidth(char c);
	int GetWidth(const char *text);
	void Draw(SpriteBuffer &sb, Glyph *glyph, const Point<int> &p, ColorRgb c);
	void Draw(SpriteBuffer* psb, const char* text, Point<int> p, ColorRgb c, int frame_in, int frame_out, int frame, int flags = 0);
	void Draw(SpriteBuffer *psb, const char *text, const Point<int> &p, ColorRgb c = ColorRgb::White(), int flags = 0);
};

