#include "StdAfx.h"
#include "ZDirect3D.h"
#include "ZColour.h"
#include "ZSpriteBuffer.h"
#include "error.h"
#include "ZTexture.h"

const int pnStateOrder[] = { g_pD3D->Multiply, g_pD3D->InverseMultiply, g_pD3D->Transparent, g_pD3D->LuminanceOpacity };

int ZSpriteBuffer::GetOrder(const ZSpriteBuffer::Item &item)
{
	int i;
	for(i = 0; i < sizeof(pnStateOrder) / sizeof(pnStateOrder[0]); i++)
	{
		if(pnStateOrder[i] == item.nState) return i;
	}
	return i;
}
ZSpriteBuffer::ZSpriteBuffer()
{
//	pCurTexture = (ZTexture*)-1;
}
void ZSpriteBuffer::Clear()
{
	vi.clear();
}
int ZSpriteBuffer::GetPosition(const Item &item)
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
HRESULT ZSpriteBuffer::Flush( )
{
	HRESULT hRes;

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
		hRes = g_pD3D->DrawSprite(vi[i].p, vi[i].r, vi[i].cDiffuse);
		if(FAILED(hRes)) return TraceError(hRes);
	}
	Clear();
	return D3D_OK;
}
void ZSpriteBuffer::AddItem(const Item &item)
{
	_ASSERT(item.pTexture == NULL || !IsBadReadPtr(item.pTexture, sizeof(ZTexture)));
	if(!(item.r.IsValid())) return;

	if(item.cDiffuse == ZColour::Black())
	{
		if(item.nState == g_pD3D->InverseMultiply) return;
		if(item.nState == g_pD3D->LuminanceOpacity) return;
		if(item.nState == g_pD3D->Transparent) return;
	}
	else if(item.cDiffuse == ZColour::White() && item.pTexture == NULL)
	{
		if(item.nState == g_pD3D->Multiply) return;
	}

	vi.insert(vi.begin() + GetPosition(item), item);
}
void ZSpriteBuffer::SetState(int nState)
{
	nCurState = nState;
}
void ZSpriteBuffer::SetTexture(ZTexture *pTexture)
{
	pCurTexture = pTexture;
}
// using current state
void ZSpriteBuffer::Darken(const ZRect<int> &r, ZColour cDiffuse)
{
	AddSprite(r.GetTopLeft(), NULL, g_pD3D->InverseMultiply, ZRect<int>(ZPoint<int>(0, 0), r.GetBotRight() - r.GetTopLeft()), cDiffuse);
}
void ZSpriteBuffer::AddSprite(ZPoint<int> p, const ZRect<int> &spr, ZColour cDiffuse, ZColour cSpecular)
{
	AddSprite(p, pCurTexture, nCurState, spr, cDiffuse);
}
void ZSpriteBuffer::AddBlendedSprites(ZPoint<int> p, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ZColour cDiffuse)
{
	AddBlendedSprites(p, pCurTexture, nCurState, fBlend, spr1, spr2, cDiffuse);
}
void ZSpriteBuffer::AddSprite(ZPoint<int> p, ZTexture *pTexture, int nState, const ZRect<int> &spr, ZColour cDiffuse)
{
	Item item;
	item.p = p;
	item.r = spr;
	item.pTexture = pTexture;
	item.nState = nState;
	item.cDiffuse = cDiffuse;
	AddItem(item);
}
void ZSpriteBuffer::AddBlendedSprites(ZPoint<int> p, ZTexture *pTexture, int nState, float fBlend, const ZRect<int> &spr1, const ZRect<int> &spr2, ZColour cDiffuse)
{
	AddSprite(p, pTexture, nState, spr1, ZColour(cDiffuse * (1.0f - fBlend)));
	AddSprite(p, pTexture, nState, spr2, ZColour(cDiffuse * fBlend));
}
