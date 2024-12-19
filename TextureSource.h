#pragma once

#include "Texture.h"
#include <string>

class TextureSource
{
public:
	bool bInternal;
	int nInternalID;
	std::string sFilename;
	std::set<int> snClass;
	Texture *pTexture;
//	auto_ptr<ZTexture> pTexture;

	static TextureSource *Internal(int nID, const char *sUsage = "");
	static TextureSource *External(const char *sPath, const char *sUsage = "");

	std::string GetPath();
	std::string GetFile();

	bool ParseUsageString(const char *sText);
	std::string MakeUsageString();
};

extern const uint32 *g_apnIntTexture[ ];
//const char *psIntTexture[];
extern const int nIntTextures;
//extern CTextureItem pInternalTexture[];
//extern vector< auto_ptr < CTextureItem > > ppTexture;

