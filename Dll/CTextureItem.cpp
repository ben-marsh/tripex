#include "StdAfx.h"
#include "CTextureItem.h"
#include "CCfgItem.h"
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

CTextureItem *CTextureItem::Internal(int nID, const char *sUsage)
{
	if(nID >= nIntTextures) return NULL;
	else
	{
		CTextureItem *pItem = new CTextureItem;
		pItem->bInternal = true;
		pItem->nInternalID = nID;
		pItem->sFilename = "";//psIntTexture[nID];
		pItem->ParseUsageString(sUsage);
		return pItem;
	}
}
CTextureItem *CTextureItem::External(const char *sPath, const char *sUsage)
{
	CTextureItem *pItem = new CTextureItem;
	pItem->bInternal = false;
	pItem->sFilename = sPath;
	pItem->ParseUsageString(sUsage);
	return pItem;
}
bool CTextureItem::ParseUsageString(const char *sText)
{
	snClass.clear();

	vector<int> vn;
	if(!CCfgItem::ParseArrayString(sText, vn)) return false;
	snClass.insert(vn.begin(), vn.end());
	return true;
}
string CTextureItem::MakeUsageString()
{
	vector<int> vn;
	copy(snClass.begin(), snClass.end(), back_inserter(vn));
	return CCfgItem::MakeArrayString(',', vn);
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
string CTextureItem::GetPath()
{
	if(bInternal) return "[ Internal ]";
	else
	{
		size_t nPos = sFilename.find_last_of("\\");
		if(nPos != string::npos) nPos++;
		return sFilename.substr(0, nPos);
	}
}
string CTextureItem::GetFile()
{
	if(bInternal) return sFilename;
	else
	{
		size_t nPos = sFilename.find_last_of("\\");
		if(nPos != string::npos) nPos++;
		return sFilename.substr(nPos, string::npos);
	}
}
