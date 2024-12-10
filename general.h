#pragma once

#include "ZDirect3D.h"
#include "ZSpriteBuffer.h"
#include "ZColour.h"

void DrawMessage(ZSpriteBuffer &sb, class ZTextureFont *pFont, int y, const char *sText, float fBr, float fBackBr);
