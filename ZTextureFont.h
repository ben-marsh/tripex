#pragma once

#include "ZDirect3D.h"
#include "ZTexture.h"
#include "ZColour.h"
#include "ZSpriteBuffer.h"
#include <memory>

#define TF_ALIGNLEFT 0
#define TF_ALIGNRIGHT 1
#define TF_ALIGNCENTER 2
#define TF_LIMITWIDTH 4
#define TF_FADE_IN_START 8
#define TF_FADE_IN_FINISH 0 // default
#define TF_FADE_OUT_START 0 // default
#define TF_FADE_OUT_FINISH 16

class ZTextureFont
{
protected:
public:
	int nWidth, nHeight;
	int nSubPixel;
	class Letter
	{
	public:
		char cLetter;
		int nStart, nEnd;
		int nBitmapX, nBitmapY;
		BYTE *pbData;
	};
	ZArray<Letter> pLetter;
	std::unique_ptr< ZTexture > pTexture;
	ZColour *m_pcData;

	bool LineAboveThreshold(BYTE *pbData);

public:
	int nSpacing;
	ZTextureFont();
	~ZTextureFont();
	bool Add(HINSTANCE hInstance, char *sID);
	void Add(BYTE *pbData);
	ZColour *GetBitmap();
	ZTexture* GetTexture();
	Letter *GetLetter(char cLetter);
	int GetWidth(char c);
	int GetWidth(const char *sText);
	void Draw(ZSpriteBuffer &sb, Letter *pLet, const ZPoint<int> &p, ZColour c);
	void Draw(ZSpriteBuffer *psb, const char *sText, ZPoint<int> p, ZColour c, int nInFrame, int nOutFrame, int nFrame, int nFlags = 0, int *pnWidth = NULL);
	void Draw(ZSpriteBuffer *psb, const char *sText, const ZPoint<int> &p, ZColour c = ZColour::White(), int nFlags = 0, int *pnWidth = NULL);
	void Draw(BYTE *pbData, int nSpan, const char *sText, int nX, int nY, ZColour c, int nInFrame, int nOutFrame, int nFrame, int nFlags, int *pnWidth = NULL);
/*	HRESULT Draw(ZDirect3D *d3d, char *sText, int nX, int nY, ZColour c, int nFlags);

#define TF_ALIGNLEFT 0
#define TF_ALIGNRIGHT 1
#define TF_ALIGNCENTER 2
#define TF_ELLIPSIS 4
#define TF_FADED 8
#define TF_NODRAW 16
*/
//	HRESULT Draw(ZDirect3D *d3d, char *sText, int nX, int nY, ZColour c);
};

