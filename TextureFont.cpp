#include "Platform.h"
#include "TextureFont.h"
#include "Error.h"

TextureFont::TextureFont()
{
	width = height = 0;
	memset(char_to_glyph, 0xff, sizeof(char_to_glyph));
}

TextureFont::~TextureFont()
{
}

bool TextureFont::LineAboveThreshold(BYTE* data)
{
	for (int i = 0; i < height; i++)
	{
		if (*data > 70) return true;
		data += width;
	}
	return false;
}

void TextureFont::Add(const uint8* source_data)
{
	memset(char_to_glyph, 0xff, sizeof(char_to_glyph));

	width = *((const int*)source_data);
	source_data += sizeof(int);

	height = *((const int*)source_data);
	source_data += sizeof(int);

	int length = *((const int*)source_data);
	source_data += sizeof(int);

	const char* alphabet = (const char*)source_data;
	source_data += length;

	for (int i = 0; i < length; i++)
	{
		glyphs.resize(glyphs.size() + 1);
		Glyph& glyph = glyphs.back();

		glyph.character = alphabet[i];
		glyph.data = std::make_unique<uint8[]>(width * height);
		memcpy(glyph.data.get(), source_data, width * height);
		source_data += width * height;

		for (glyph.end = width - 1; glyph.end > 0; glyph.end--)
		{
			if (LineAboveThreshold(glyph.data.get() + glyph.end))
			{
				break;
			}
		}
		for (glyph.start = 0; glyph.start < glyph.end; glyph.start++)
		{
			if (LineAboveThreshold(glyph.data.get() + glyph.start))
			{
				break;
			}
		}

		glyph.start = 0;
		glyph.end++;

		char_to_glyph[(uint8)glyph.character] = (uint8)(glyphs.size() - 1);
	}
}

TextureFont::Glyph* TextureFont::FindGlyph(char character)
{
	uint8 index = char_to_glyph[(uint8)character];
	if (index == 0xff)
	{
		return nullptr;
	}
	return &glyphs[index];
}

ColorRgb* TextureFont::GetBitmap()
{
	if (pixels == NULL)
	{
		pixels = std::make_unique<ColorRgb[]>(256 * 256);
	}
	memset(pixels.get(), 0, 256 * 256 * sizeof(ColorRgb));

	POINT p = { 0, 0 };
	for (int i = 0; i < glyphs.size(); i++)
	{
		if (p.y + height >= 256)
		{
			p.x += width;
			p.y = 0;
		}
		if (p.x + width >= 256)
		{
			for (; i < glyphs.size(); i++)
			{
				glyphs[i].bitmap_x = 0;
				glyphs[i].bitmap_y = 0;
				glyphs[i].start = 0;
				glyphs[i].end = 0;
			}
			break;
		}
		glyphs[i].bitmap_x = p.x;
		glyphs[i].bitmap_y = p.y;

		int src = 0;
		int dst = (256 * p.y) + p.x;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (glyphs[i].character != ' ')
				{
					unsigned char c = glyphs[i].data[src++];
					pixels[dst++] = ColorRgb(c, c, c);
				}
			}
			dst += 256 - width;
		}

		p.y += height;
	}
	return pixels.get();
}

Texture* TextureFont::GetTexture()
{
	if (texture.get() == NULL)
	{
		texture = std::make_unique<Texture>();
		texture->SetSource(D3DFMT_X8R8G8B8, GetBitmap(), 256 * 256 * 4, 256 * 4);
	}
	return texture.get();
}

void TextureFont::Draw(SpriteBuffer& sb, Glyph* glyph, const Point<int>& pos, ColorRgb c)
{
	if (glyph->character != ' ')
	{
		sb.AddSprite(pos, texture.get(), ZDirect3D::LuminanceOpacity, Rect<int>(glyph->bitmap_x, glyph->bitmap_y, width, height), c);
	}
}

void TextureFont::Draw(SpriteBuffer* sb, const char* text, Point<int> pos, ColorRgb color, int frame_in, int frame_out, int frame, int flags)
{
	int relative_in = frame - frame_in;
	int relative_out = frame - frame_out;
	ColorRgb glyph_color = color;
	bool faded = (frame != -1 && relative_in != 0 && relative_out != 0);
	if (flags & TF_FADE_IN_FINISH)
	{
		relative_in -= ((int)strlen(text) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
	}
	if (flags & TF_FADE_OUT_FINISH)
	{
		relative_out -= ((int)strlen(text) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
	}

	// just draw
	int start_x = pos.x;
	for (int i = 0; text[i] != 0; i++)
	{
		Glyph* glyph = FindGlyph(text[i]);
		if (glyph != NULL)
		{
			if (sb != NULL)
			{
				if (faded)
				{
					float brightness = Bound<float>(float(relative_in) / LETTER_APPEAR_FRAMES, 0, 1);
					brightness *= 1 - Bound<float>(float(relative_out) / LETTER_APPEAR_FRAMES, 0, 1);
					glyph_color = color * brightness;
				}

				Draw(*sb, glyph, Point<int>(pos.x - glyph->start, pos.y), glyph_color);
			}
			pos.x += glyph->end - glyph->start;

			if (faded)
			{
				relative_in -= LETTER_DIFF_FRAMES;
				relative_out -= LETTER_DIFF_FRAMES;
			}
		}
	}
}

void TextureFont::Draw(SpriteBuffer* psb, const char* text, const Point<int>& p, ColorRgb c, int flags)
{
	Draw(psb, text, p, c, -1, -1, -1, flags);
}

int TextureFont::GetWidth(char c)
{
	char sBuf[2] = { c, 0 };
	return GetWidth(sBuf);
}

int TextureFont::GetWidth(const char* text)
{
	int width = 0;
	for (int i = 0; text[i] != 0; i++)
	{
		Glyph* glyph = FindGlyph(text[i]);
		if (glyph != NULL)
		{
			width += glyph->end - glyph->start;
		}
	}
	return width;
}
