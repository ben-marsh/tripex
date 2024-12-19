#include "StdAfx.h"
#include "ZDirect3D.h"
#include "ColorRgb.h"
#include "SpriteBuffer.h"
#include "error.h"
#include "Texture.h"

const int pnStateOrder[] = { g_pD3D->Multiply, g_pD3D->InverseMultiply, g_pD3D->Transparent, g_pD3D->LuminanceOpacity };

int SpriteBuffer::GetOrder(const SpriteBuffer::Item &item)
{
	int i;
	for(i = 0; i < sizeof(pnStateOrder) / sizeof(pnStateOrder[0]); i++)
	{
		if(pnStateOrder[i] == item.nState) return i;
	}
	return i;
}
SpriteBuffer::SpriteBuffer()
{
//	pCurTexture = (ZTexture*)-1;
}
void SpriteBuffer::Clear()
{
	vi.clear();
}
int SpriteBuffer::GetPosition(const Item &item)
{
	return (int)vi.size();
/*	int nBest = 0;
	int nBestIndex = vi.size();
	for(int i = vi.size() - 1; i >= 0 && !vi[i].r.Intersects(item.r); i--)
	{
		int n = 0;
		if(vi[i].pTexture == item.pTexture) n |= 2;
		if(vi[i].nState == item.nState) n |= 1;

		if(n > nBest)
		{
			nBest = n;
			nBestIndex = i + 1;
		}
	}
	return nBestIndex;*/
}
Error* SpriteBuffer::Flush( )
{
	Error* error;

	for(int i = 0; i < (int)vi.size(); i++)
	{
		if(i == 0 || vi[i].pTexture != vi[i - 1].pTexture)
		{
			g_pD3D->SetTexture(0, vi[i].pTexture);
		}
		if(i == 0 || vi[i].nState != vi[i - 1].nState)
		{
			g_pD3D->SetState(vi[i].nState);
		}

		error = g_pD3D->DrawSprite(vi[i].p, vi[i].r, vi[i].cDiffuse);
		if(error) return TraceError(error);
	}
	Clear();
	return nullptr;
}
void SpriteBuffer::AddItem(const Item &item)
{
	_ASSERT(item.pTexture == NULL || !IsBadReadPtr(item.pTexture, sizeof(Texture)));
	if(!(item.r.IsValid())) return;

	if(item.cDiffuse == ColorRgb::Black())
	{
		if(item.nState == g_pD3D->InverseMultiply) return;
		if(item.nState == g_pD3D->LuminanceOpacity) return;
		if(item.nState == g_pD3D->Transparent) return;
	}
	else if(item.cDiffuse == ColorRgb::White() && item.pTexture == NULL)
	{
		if(item.nState == g_pD3D->Multiply) return;
	}

	vi.insert(vi.begin() + GetPosition(item), item);
}
void SpriteBuffer::SetState(int nState)
{
	nCurState = nState;
}
void SpriteBuffer::SetTexture(Texture *pTexture)
{
	pCurTexture = pTexture;
}
// using current state
void SpriteBuffer::Darken(const ZRect<int> &r, ColorRgb cDiffuse)
{
	AddSprite(r.GetTopLeft(), NULL, g_pD3D->InverseMultiply, ZRect<int>(ZPoint<int>(0, 0), r.GetBotRight() - r.GetTopLeft()), cDiffuse);
}
void SpriteBuffer::AddSprite(ZPoint<int> p, const ZRect<int> &spr, ColorRgb cDiffuse, ColorRgb cSpecular)
{
	AddSprite(p, pCurTexture, nCurState, spr, cDiffuse);
}
void SpriteBuffer::AddBlendedSprites(ZPoint<int> p, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ColorRgb cDiffuse)
{
	AddBlendedSprites(p, pCurTexture, nCurState, fBlend, spr1, spr2, cDiffuse);
}
void SpriteBuffer::AddSprite(ZPoint<int> p, Texture *pTexture, int nState, const ZRect<int> &spr, ColorRgb cDiffuse)
{
	Item item;
	item.p = p;
	item.r = spr;
	item.pTexture = pTexture;
	item.nState = nState;
	item.cDiffuse = cDiffuse;
	AddItem(item);
}
void SpriteBuffer::AddBlendedSprites(ZPoint<int> p, Texture *pTexture, int nState, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ColorRgb cDiffuse)
{
	AddSprite(p, pTexture, nState, spr1, ColorRgb(cDiffuse * (1.0f - fBlend)));
	AddSprite(p, pTexture, nState, spr2, ColorRgb(cDiffuse * fBlend));
}
