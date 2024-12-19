#include "StdAfx.h"
#include "ZTextureFont.h"
#include "general.h"
#include "error.h"
	
#define LETTER_APPEAR_FRAMES 200 //120
#define LETTER_DIFF_FRAMES 10

ZTextureFont::ZTextureFont()
{
	nWidth = nHeight = 0;
	nSpacing = 1;
	m_pcData = NULL;
}
ZTextureFont::~ZTextureFont()
{
	if( m_pcData != NULL ) delete m_pcData;
}
bool ZTextureFont::LineAboveThreshold(BYTE *pbData)
{
	for(int i = 0; i < nHeight; i++)
	{
		if(*pbData > 70) return true;//128) return true;
		pbData += nWidth;
	}
	return false;
}
void ZTextureFont::Add(BYTE *pbData)
{
	nWidth = ((SIZE*)pbData)->cx;
	nHeight = ((SIZE*)pbData)->cy;
	pbData += sizeof(SIZE);
	int nLength = *((int*)pbData);
	pbData += sizeof(int);
	char *sAlphabet = (char*)pbData;
	pbData += nLength;
	for(int i = 0; i < nLength; i++)
	{
		Letter *pLet = pLetter.AddEmptyPtr();
		pLet->cLetter = sAlphabet[i];
		pLet->pbData = new BYTE[nWidth * nHeight];
		CopyMemory(pLet->pbData, pbData, nWidth * nHeight);
			pbData += nWidth * nHeight;

			for(pLet->nEnd = nWidth - 1; pLet->nEnd > 0; pLet->nEnd--)
			{
				if(LineAboveThreshold(pLet->pbData + pLet->nEnd)) break;
			}
			for(pLet->nStart = 0; pLet->nStart < pLet->nEnd; pLet->nStart++)
			{
				if(LineAboveThreshold(pLet->pbData + pLet->nStart)) break;
			}
			pLet->nStart = 0;
			pLet->nEnd++;
//		}
	}
}
ZTextureFont::Letter *ZTextureFont::GetLetter(char cLetter)
{
	for(int i = 0; i < pLetter.GetLength(); i++)
	{
		if(pLetter[i].cLetter == cLetter) return &pLetter[i];
	}
	return NULL;
}
ZColour *ZTextureFont::GetBitmap()
{
	if( m_pcData == NULL ) m_pcData = new ZColour[ 256 * 256 ];
	memset( m_pcData, 0, 256 * 256 * sizeof( ZColour ) );

	POINT p = { 0, 0 };
	for(int i = 0; i < pLetter.GetLength(); i++)
	{
		if(p.y + nHeight >= 256)
		{
			p.x += nWidth;
			p.y = 0;
		}
		if(p.x + nWidth >= 256)
		{
			for(; i < pLetter.GetLength(); i++)
			{
				pLetter[i].nBitmapX = 0;
				pLetter[i].nBitmapY = 0;
				pLetter[i].nStart = 0;
				pLetter[i].nEnd = 0;
			}
			break;
		}
		pLetter[i].nBitmapX = p.x;
		pLetter[i].nBitmapY = p.y;

		int nSrc = 0;
		int nDst = (256 * p.y) + p.x;
		for(int y = 0; y < nHeight; y++)
		{
			for(int x = 0; x < nWidth; x++)
			{
				if(pLetter[i].cLetter != ' ')
				{
					unsigned char c = pLetter[i].pbData[nSrc++];
					m_pcData[ nDst++ ] = ZColour( c, c, c );
				}
			}
			nDst += 256 - nWidth;
		}

		p.y += nHeight;
	}
	return m_pcData;
}
ZTexture *ZTextureFont::GetTexture()
{
	if(pTexture.get() == NULL)
	{
		pTexture = std::make_unique<ZTexture>();
		pTexture->SetSource( D3DFMT_X8R8G8B8, GetBitmap( ), 256 * 256 * 4, 256 * 4 );
	}
	return pTexture.get();
}
void ZTextureFont::Draw(ZSpriteBuffer &sb, Letter *pLet, const ZPoint<int> &p, ZColour c)
{
	if(pLet->cLetter != ' ')
	{
		sb.AddSprite(p, pTexture.get(), g_pD3D->LuminanceOpacity, ZRect<int>(pLet->nBitmapX, pLet->nBitmapY, nWidth, nHeight), c);
	}
//	else return DD_OK;
//		return CopySprite(d3d, c, nX, nY, pLet->nBitmapX, pLet->nBitmapY, nWidth, nHeight);
}
void ZTextureFont::Draw(ZSpriteBuffer *psb, const char *sText, ZPoint<int> p, ZColour c, int nInFrame, int nOutFrame, int nFrame, int nFlags, int *pnWidth)
{
/*	if(d3d != NULL)
	{
		HRESULT hRes = d3d->SetTexture(0, GetTexture());
		if(FAILED(hRes)) return TraceError(hRes);
	}
*/	if((nFlags & TF_LIMITWIDTH) && pnWidth != NULL && GetWidth(sText) > *pnWidth)
	{
		// try with ellipsis
		std::unique_ptr<char[]> sBufOwner(new char[strlen(sText) + 5]);
		char* sBuf = sBufOwner.get();

		strcpy(sBuf, sText);
		for(int i = ( int )strlen(sText) - 1;;i--)
		{
			if(GetWidth(sBuf) < *pnWidth)
			{
				Draw(psb, sBuf, p, c, nInFrame, nOutFrame, nFrame, nFlags & ~TF_LIMITWIDTH, pnWidth);
				return;
//				return Draw(d3d, sBuf, nX, nY, c, nInFrame, nOutFrame, nFrame, nFlags & ~TF_LIMITWIDTH, pnWidth);
			}
			if(i < 0) break;
			strcpy(sBuf + i, "...");
		}
//		return DD_OK;
	}
	else
	{
		if(nFlags & TF_ALIGNRIGHT) p.x -= GetWidth(sText);
		else if(nFlags & TF_ALIGNCENTER) p.x -= GetWidth(sText) / 2;

		int nRelIn = nFrame - nInFrame;
		int nRelOut = nFrame - nOutFrame;
		ZColour cLetter = c;
		bool bFaded = (nFrame != -1 && nRelIn != 0 && nRelOut != 0);
		if(nFlags & TF_FADE_IN_FINISH)
		{
			nRelIn -= ((int)strlen(sText) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
		}
		if(nFlags & TF_FADE_OUT_FINISH)
		{
			nRelOut -= ((int)strlen(sText) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
		}

		// just draw
		int nStartX = p.x;
		for(int i = 0; sText[i] != 0; i++)
		{
			Letter *pLet = GetLetter(sText[i]);
			if(pLet != NULL)
			{
				if(psb != NULL)
				{
					if(bFaded)
					{
						float fBr = Bound<float>(float(nRelIn) / LETTER_APPEAR_FRAMES, 0, 1);
						fBr *= 1 - Bound<float>(float(nRelOut) / LETTER_APPEAR_FRAMES, 0, 1);
						cLetter = c * fBr;
					}

					Draw(*psb, pLet, ZPoint<int>(p.x - pLet->nStart, p.y), cLetter);
//					HRESULT hRes = Draw(d3d, pLet, nX - pLet->nStart, nY, cLetter);
//					if(FAILED(hRes)) return hRes;
				}
				p.x += pLet->nEnd - pLet->nStart;
//				nX += pLet->nEnd - pLet->nStart;
//				nX++;

				if(bFaded)
				{
					nRelIn -= LETTER_DIFF_FRAMES;
					nRelOut -= LETTER_DIFF_FRAMES;
				}
			}
		}
		if(pnWidth != NULL) *pnWidth = p.x - nStartX;
//		return DD_OK;
	}
}
void ZTextureFont::Draw(ZSpriteBuffer *psb, const char *sText, const ZPoint<int> &p, ZColour c, int nFlags, int *pnWidth)
{
	Draw(psb, sText, p, c, -1, -1, -1, nFlags, pnWidth);
}
int ZTextureFont::GetWidth(char c)
{
	char sBuf[2] = { c, 0 };
	return GetWidth(sBuf);
}
int ZTextureFont::GetWidth(const char *sText)
{
	int nTextWidth;
	Draw(NULL, sText, ZPoint<int>(0, 0), ZColour::Black(), 0, &nTextWidth);
	return nTextWidth;
}
void ZTextureFont::Draw(BYTE *pbData, int nSpan, const char *sText, int nX, int nY, ZColour c, int nInFrame, int nOutFrame, int nFrame, int nFlags, int *pnWidth)
{
	if((nFlags & TF_LIMITWIDTH) && pnWidth != NULL && GetWidth(sText) > *pnWidth)
	{
		// try with ellipsis
		char *sBuf = new char[strlen(sText) + 5];
		strcpy(sBuf, sText);
		for(int i = (int)strlen(sText) - 1;;i--)
		{
			if(GetWidth(sBuf) <= *pnWidth)
			{
				Draw(pbData, nSpan, sBuf, nX, nY, c, nInFrame, nOutFrame, nFrame, nFlags & ~TF_LIMITWIDTH, pnWidth);
				delete sBuf;
				return;
			}
			if(i < 0) break;
			strcpy(sBuf + i, "...");
		}
		delete sBuf;
	}
	else
	{
		if(nFlags & TF_ALIGNRIGHT) nX -= GetWidth(sText);
		else if(nFlags & TF_ALIGNCENTER) nX -= GetWidth(sText) / 2;

		int nRelIn = nFrame - nInFrame;
		int nRelOut = nFrame - nOutFrame;
		ZColour cLetter = c;
		bool bFaded = (nFrame != -1 && nRelIn != 0 && nRelOut != 0);
		if(nFlags & TF_FADE_IN_FINISH)
		{
			nRelIn -= ((int)strlen(sText) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
		}
		if(nFlags & TF_FADE_OUT_FINISH)
		{
			nRelOut -= ((int)strlen(sText) * LETTER_DIFF_FRAMES) + (LETTER_APPEAR_FRAMES - LETTER_DIFF_FRAMES);
		}

		// just draw
		int nStartX = nX;
		for(int i = 0; sText[i] != 0; i++)
		{
			Letter *pLet = GetLetter(sText[i]);
			if(pLet != NULL)
			{
				if(bFaded)
				{
					float fBr = Bound<float>(float(nRelIn) / LETTER_APPEAR_FRAMES, 0, 1);
					fBr *= 1 - Bound<float>(float(nRelOut) / LETTER_APPEAR_FRAMES, 0, 1);
					cLetter = c * fBr;
				}

				if(pbData != NULL)
				{
					BYTE *pbSrc = pLet->pbData, *pbDst = pbData + nX + (nY * nSpan); 
					for(int j = 0; j < nHeight; j++)
					{
						for(int k = 0; k < nWidth; k++)
						{
							*pbDst = std::min(255, *pbDst + *pbSrc);
							pbSrc++;
							pbDst++;
						}
						pbDst += nSpan - nWidth; 
					}
				}
				nX += pLet->nEnd - pLet->nStart;
//				nX++;

				if(bFaded)
				{
					nRelIn -= LETTER_DIFF_FRAMES;
					nRelOut -= LETTER_DIFF_FRAMES;
				}
			}	
		}
		if(pnWidth != NULL) *pnWidth = nX - nStartX;
	}
}
