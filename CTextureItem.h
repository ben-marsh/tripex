#pragma once

#include "ZTexture.h"

class CTextureItem
{
public:
	bool bInternal;
	int nInternalID;
	string sFilename;
	set<int> snClass;
	ZTexture *pTexture;
//	auto_ptr<ZTexture> pTexture;

	static CTextureItem *Internal(int nID, const char *sUsage = "");
	static CTextureItem *External(const char *sPath, const char *sUsage = "");

	string GetPath();
	string GetFile();

	bool ParseUsageString(const char *sText);
	string MakeUsageString();
};

extern const UINT32 *g_apnIntTexture[ ];
//const char *psIntTexture[];
extern const int nIntTextures;
//extern CTextureItem pInternalTexture[];
//extern vector< auto_ptr < CTextureItem > > ppTexture;

