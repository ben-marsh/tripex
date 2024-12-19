#pragma once

#include "ZArray.h"
#include "SpriteBuffer.h"
#include "EFFECT.h"
#include "AudioData.h"

enum
{
	TXS_STARTED,
	TXS_VISIBLE_BEATS,
	TXS_RESET_TIMING,
	TXS_CHANGE_EFFECT,
	TXS_IN_FADE,
	TXS_EFFECT_LEFT,
	TXS_EFFECT_RIGHT,
	TXS_RECONFIGURE,
	TXS_HOLD,
	TXS_RESET_TARGET,

	TXS_LAST,
};

//extern bitset<TXS_LAST> txs;

//extern int active;
//extern ZArray<EFFECT*> ppEffect;
extern class TextureFont tef;

// main-misc.cpp
//void ShowStatusMsg(const char *sFormat, ...);

// main-beat.cpp
//void InitBeats();
//void AddFrameTime(bool bReal, DWORD dwTime);
//float GetFPS(bool bReal);
//void UpdateBeat(double elapsed);
//void DrawLineBar(ZSpriteBuffer &sb, int x, int y, int h, double p);
//void DrawBeat(ZSpriteBuffer &sb);
//double MusicRandom();
//double GetStrength(double sensitivity, int nStart, int nEnd);
