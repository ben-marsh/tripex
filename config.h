#pragma once

#include "ZArray.h"
#include "ZPtr.h"
#include "ZDirect3D.h"
#include <vector>

#define HKEY_ROOT (HKEY_LOCAL_MACHINE)
#define KEY_MAIN "Software\\-\\Tripex"
#define KEY_TEXTURES "Software\\-\\Tripex\\Textures"

class CTextureItem;

bool LoadTextureSettings(std::vector< CTextureItem* > &ppItem);
bool SaveTextureSettings(std::vector< CTextureItem* > &ppItem);

// config-directx.cpp
//extern string sDevice;
//extern string sResolution, sFiltering;
//extern bool bFullscreen;
//extern bool bAntiAlias;
//extern int nPrefFiltering, nFiltering;
//extern DWORD dwFullscreenW, dwFullscreenH, dwFullscreenBPP;
extern bool bMeshHQ;
//extern bool bUseSoftware;


// config-effects.cpp
extern float fFairness;
extern int nFlowmapSize;

// config-display.cpp
extern bool bShowProgress;
extern bool bShowName;
extern bool bShowNameStart;
extern bool bShowNameEnd;
extern bool bShowTime;
extern bool bShowHUD;
extern bool bShowHUDTitle;
extern bool bShowMessages;
extern float fHUDTransparency;

// config-general.cpp
extern bool bAvoidBigReactions;
extern float fCrossfading;
extern float fFadeIn;
extern float fFadeOut;

extern int nFlowmapW;
extern int nFlowmapH;

