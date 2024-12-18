#pragma once

#include "ZColour.h"
#include "ZPoint.h"
#include "ZRect.h"
#include "error.h"
#include <vector>

class ZSpriteBuffer
{
protected:
	class Item
	{
	public:
		int nState;
		class ZTexture *pTexture;

		ZPoint<int> p;
		ZRect<int> r;
		ZColour cDiffuse;
	};

	std::vector<Item> vi;

	int nCurState;
	ZTexture *pCurTexture;
	void AddItem(const Item &item);
	int GetPosition(const Item &item);
	int GetOrder(const Item &item);
public:

	ZSpriteBuffer();
	void Clear();
	ZError* Flush( );

	void SetState(int nState);
	void SetTexture(ZTexture *pTexture);

	void Darken(const ZRect<int> &r, ZColour cDiffuse);
	void AddSprite(ZPoint<int> p, const ZRect<int> &spr, ZColour cDiffuse = ZColour::White(), ZColour cSpecular = ZColour::Black());
	void AddBlendedSprites(ZPoint<int> p, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ZColour cDiffuse = ZColour::White());
	void AddSprite(ZPoint<int> p, ZTexture *pTexture, int nState, const ZRect<int> &spr, ZColour cDiffuse = ZColour::White());
	void AddBlendedSprites(ZPoint<int> p, ZTexture *pTexture, int nState, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ZColour cDiffuse = ZColour::White());
};
