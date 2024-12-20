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

		Point<int> p;
		Rect<int> r;
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

	void Darken(const Rect<int> &r, ColorRgb cDiffuse);
	void AddSprite(Point<int> p, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
	void AddBlendedSprites(Point<int> p, float fBlend, const Rect<int> &spr1, const Rect<int> &spr2, ColorRgb cDiffuse = ColorRgb::White());
	void AddSprite(Point<int> p, Texture *pTexture, int nState, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White());
	void AddBlendedSprites(Point<int> p, Texture *pTexture, int nState, float fBlend, const Rect<int> &spr1, const Rect<int> &spr2, ColorRgb cDiffuse = ColorRgb::White());
};
