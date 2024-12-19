#pragma once

#include "ColorRgb.h"
#include "Point.h"
#include "Rect.h"
#include "error.h"
#include <vector>

class SpriteBuffer
{
protected:
	class Item
	{
	public:
		int nState;
		class Texture *pTexture;

		ZPoint<int> p;
		ZRect<int> r;
		ColorRgb cDiffuse;
	};

	std::vector<Item> vi;

	int nCurState;
	Texture *pCurTexture;
	void AddItem(const Item &item);
	int GetPosition(const Item &item);
	int GetOrder(const Item &item);
public:

	SpriteBuffer();
	void Clear();
	Error* Flush( );

	void SetState(int nState);
	void SetTexture(Texture *pTexture);

	void Darken(const ZRect<int> &r, ColorRgb cDiffuse);
	void AddSprite(ZPoint<int> p, const ZRect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
	void AddBlendedSprites(ZPoint<int> p, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ColorRgb cDiffuse = ColorRgb::White());
	void AddSprite(ZPoint<int> p, Texture *pTexture, int nState, const ZRect<int> &spr, ColorRgb cDiffuse = ColorRgb::White());
	void AddBlendedSprites(ZPoint<int> p, Texture *pTexture, int nState, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ColorRgb cDiffuse = ColorRgb::White());
};
