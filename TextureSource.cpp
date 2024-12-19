#include "StdAfx.h"
#include "TextureSource.h"
#include "ConfigItem.h"
#include "TextureData.h"

extern const UINT32 *g_apnIntTexture[ ] =
{
	g_anTexRawGUI,
	g_anTexBlank,
	g_anTexAlienEgg,
	g_anTexLight,
	g_anTexEyes,
	g_anTexFlesh,
	g_anTexForest,
	g_anTexShinySand,
	g_anTexBrightLight,
};

/*
const char *psIntTexture[] = 
{
	"GUI", 
	"BlankTexture", 
	"AlienEgg(EnvMap)", 
	"Light(Lightbob)", 
	"Eyes(Wrapped)", 
	"Flesh(Wrapped)", 
	"Forest(Wrapped)", 
	"ShinySand(EnvMap)", 
	"Brightlight(Lightbob)"
};
*/
const int nIntTextures = sizeof( g_apnIntTexture ) / sizeof( g_apnIntTexture[ 0 ] );
//sizeof(psIntTexture) / sizeof(psIntTexture[0]);

TextureSource *TextureSource::Internal(int nID, const char *sUsage)
{
	if(nID >= nIntTextures) return NULL;
	else
	{
		TextureSource *pItem = new TextureSource;
		pItem->bInternal = true;
		pItem->nInternalID = nID;
		pItem->sFilename = "";//psIntTexture[nID];
		pItem->ParseUsageString(sUsage);
		return pItem;
	}
}
TextureSource *TextureSource::External(const char *sPath, const char *sUsage)
{
	TextureSource *pItem = new TextureSource;
	pItem->bInternal = false;
	pItem->sFilename = sPath;
	pItem->ParseUsageString(sUsage);
	return pItem;
}
bool TextureSource::ParseUsageString(const char *sText)
{
	snClass.clear();

	std::vector<int> vn;
	if(!ConfigItem::ParseArrayString(sText, vn)) return false;
	snClass.insert(vn.begin(), vn.end());
	return true;
}
std::string TextureSource::MakeUsageString()
{
	std::vector<int> vn;
	for (int value : snClass)
	{
		vn.push_back(value);
	}
	return ConfigItem::MakeArrayString(',', vn);
}
/*
void CTextureItem::SetUsage(const char *sText)
{
	snClass.clear();
	for(;;)
	{
		while(!isdigit(*sText))
		{
			if(*sText == 0) return;
			sText++;
		}

		snClass.insert(atoi(sText));

		while(isdigit(*sText)) sText++; 
	}
}
string CTextureItem::GetUsage()
{
	string str;
	for(set<int>::iterator it = snClass.begin(); it != snClass.end(); it++)
	{
		if(it != snClass.begin()) str += ",";
		str += MakeString("%d", *it);
	}
	return str;
}
*/
std::string TextureSource::GetPath()
{
	if(bInternal) return "[ Internal ]";
	else
	{
		size_t nPos = sFilename.find_last_of("\\");
		if(nPos != std::string::npos) nPos++;
		return sFilename.substr(0, nPos);
	}
}
std::string TextureSource::GetFile()
{
	if(bInternal) return sFilename;
	else
	{
		size_t nPos = sFilename.find_last_of("\\");
		if(nPos != std::string::npos) nPos++;
		return sFilename.substr(nPos, std::string::npos);
	}
}
