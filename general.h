#pragma once

#include "ZDirect3D.h"
#include "SpriteBuffer.h"
#include "ColorRgb.h"

void DrawMessage(SpriteBuffer &sb, class TextureFont *pFont, int y, const char *sText, float fBr, float fBackBr);
