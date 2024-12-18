#include "StdAfx.h"
#include "general.h"
#include "error.h"
#include "ZColour.h"
#include "ZTextureFont.h"

int GetClippedLineLength(ZTextureFont *pFont, const char *sText, int nClipWidth)
{
	bool bFirstWord = true;
	int nLastEnd = 0;
	for(int i = 0;;)
	{
		bool bSpace = (isspace(sText[i]) != 0);
		if(bSpace || sText[i] == 0 || bFirstWord)
		{
			if(bSpace) bFirstWord = false;

			std::string sLine = std::string(sText, i);
			if(pFont->GetWidth(sLine.c_str()) > nClipWidth) return nLastEnd;
			else if(sText[i] == 0) return i;

			nLastEnd = i;

			if(bSpace)
			{
				while(isspace(sText[i])) i++;
			}
			else i++;
		}
		else i++;
	}
}

void DrawMessage(ZSpriteBuffer &sb, ZTextureFont *pFont, int y, const char *sText, float fBr, float fBackBr)
{
//	static ZArray<ZVertexTL> pVertex;
//	static ZArray<ZFace> face;

	const int nClipWidth = g_pD3D->GetWidth() - 40;
	const int nLineHeight = 20;

	std::vector<std::string> vsLine;
	int nWidth = 0;
	for(;;)
	{
		while(isspace(*sText)) sText++;
		if(*sText == 0) break;
		int nLength = GetClippedLineLength(pFont, sText, nClipWidth);
		if(nLength > 0)
		{
			std::string sLine(sText, nLength);
			vsLine.push_back(sLine);
			nWidth = max(nWidth, pFont->GetWidth(sLine.c_str()));
			sText += nLength;
		}
	}

	int nCentreX = g_pD3D->GetWidth() / 2;

	if(fBackBr > FLOAT_ZERO)
	{
		float fDarkBr = (fBackBr * fBr);
		const ZPoint<int> p(nCentreX - (nWidth / 2) - 15, y);
		const ZRect<int> r(0, 0, nWidth + 30, 25 + ((int)vsLine.size() - 1) * nLineHeight);
		sb.AddSprite(p, NULL, g_pD3D->InverseMultiply, r, fDarkBr);
	}

	for(int i = 0; i < (int)vsLine.size(); i++)
	{
		int nLineWidth = pFont->GetWidth(vsLine[i].c_str());
		pFont->Draw(&sb, vsLine[i].c_str(), ZPoint<int>(nCentreX - (nLineWidth / 2), y + 5 + (i * nLineHeight)), ZColour::Grey((int)(fBr * 255.0f)));
	}
}
