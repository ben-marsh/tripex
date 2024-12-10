#include "StdAfx.h"
#include <memory.h>
#include "config.h"
#include "config-defaults.h"
#include "config-variables.h"
#include "effect.h"

vector< CCfgItem* > *pppCfgItem = NULL;

string *psEffect;

class CI_STR_CMP : public less<string>
{
public:
	bool operator()( const string &s1, const string &s2 ) const
	{
		return _stricmp(s1.c_str(), s2.c_str()) < 0;
	}
};

// TODO: make sure these CfgItems are free'd
map< string, vector< CCfgItem* >, CI_STR_CMP > *pmpCfgItem = NULL;

CCfgItem *AddCfgItem( CCfgItem *pItem )
{
	pppCfgItem->push_back( pItem );
	(*pmpCfgItem)[ pItem->GetKeyName( ) ].push_back( pItem );
	return pItem;
}

void CreateCfgItems()
{
	if(pppCfgItem == NULL )
	{
		pppCfgItem = new vector<CCfgItem*>;
		pmpCfgItem = new map< string, vector< CCfgItem* >, CI_STR_CMP >( );
		psEffect = new string[ pvpEffectList->size( ) ];

		AddCfgItem(CCfgItem::Bool("MeshHQ", &bMeshHQ, true));

		// Display
		AddCfgItem(CCfgItem::Bool("ShowProgress", &bShowProgress));
		AddCfgItem(CCfgItem::Bool("ShowName", &bShowName));
		AddCfgItem(CCfgItem::Bool("ShowNameStart", &bShowNameStart));
		AddCfgItem(CCfgItem::Bool("ShowNameEnd", &bShowNameEnd));
		AddCfgItem(CCfgItem::Bool("ShowTime", &bShowTime));
		AddCfgItem(CCfgItem::Bool("ShowHUD", &bShowHUD));
		AddCfgItem(CCfgItem::Bool("ShowHUDTitle", &bShowHUDTitle));
		AddCfgItem(CCfgItem::Bool("ShowMessages", &bShowMessages));
		AddCfgItem(CCfgItem::Float("HUDTransparency", &fHUDTransparency));

		// General
		AddCfgItem(CCfgItem::Float("Crossfading", &fCrossfading));
		AddCfgItem(CCfgItem::Float("FadeIn", &fFadeIn));
		AddCfgItem(CCfgItem::Float("FadeOut", &fFadeOut));
		AddCfgItem(CCfgItem::Bool("AvoidBigReactions", &bAvoidBigReactions));

		// Effects
		AddCfgItem(CCfgItem::Float("SelectionFairness", &fFairness));
		AddCfgItem(CCfgItem::Int("FlowmapW", &nFlowmapW, true));
		AddCfgItem(CCfgItem::Int("FlowmapH", &nFlowmapH, true));
		for(int i = 0; i < ( int )pvpEffectList->size(); i++)
		{
			AddCfgItem(CCfgItem::String((*pvpEffectList)[i]->GetCfgItemName().c_str(), &psEffect[i]));
		}
	}
}

void UpdateCfgItems(bool bInit)
{
	for(unsigned int i = 0; i < pppCfgItem->size(); i++)
	{
		(*pppCfgItem)[i]->Update(bInit);
	}
	for(unsigned int i = 0; i < pvpEffectList->size(); i++)
	{
		for(int j = 0; j < 5; j++) (*pvpEffectList)[i]->pfSetting[j] = 0.0f;
		CCfgItem *pItem = FindCfgItem((*pvpEffectList)[i]->GetCfgItemName().c_str());
		pItem->GetFloatArray(5, (*pvpEffectList)[i]->pfSetting);
	}
	// filtering -> num
}

CCfgItem *FindCfgItem(const char *sName)
{
	for(unsigned int i = 0; i < pppCfgItem->size(); i++)
	{
		if(!_stricmp((*pppCfgItem)[i]->sName.c_str(), sName)) return (*pppCfgItem)[i];
	}
//	assert(false);
	return NULL;
}

static bool bLoadedCfg = false;
void LoadCfgItems()
{
	if(bLoadedCfg) return;

	UpdateCfgItems();

	map< string, vector< CCfgItem* >, CI_STR_CMP >::iterator it;
	for(it = pmpCfgItem->begin(); it != pmpCfgItem->end(); it++)
	{
		string sKey = it->first;
//			HKEY hKey = RegCreateKey(HKEY_CURRENT_USER, it->first.c_str(), KEY_READ);
		for(int j = 0; j < (int)it->second.size(); j++)
		{
				CCfgItem *pItem = it->second[j];
//				if(!pItem->Load(hKey))
//				{
				switch(pItem->nType)
				{
				case CCfgItem::CIT_INT:
					pItem->SetInt(GetDefaultInt(pItem->sName.c_str()));
					break;
				case CCfgItem::CIT_BOOL:
					pItem->SetBool(!!GetDefaultInt(pItem->sName.c_str()));
					break;
				case CCfgItem::CIT_FLOAT:
					pItem->SetFloat(GetDefaultInt(pItem->sName.c_str()) / 1000.0f);
					break;
				case CCfgItem::CIT_STRING:
					pItem->SetString(GetDefaultStr(pItem->sName.c_str()));
					break;
				default:
					_ASSERT(false);
					break;
				}
				pItem->bSave = false;
//				}
		}
//			RegCloseKey(hKey);
	}

	bLoadedCfg = true;
}
void SaveCfgItems()
{
	if(!bLoadedCfg) return;
	UpdateCfgItems();

//		for(map< string, vector< auto_ptr< CCfgItem > > >::iterator it = mpCfgItem.begin(); it != mpCfgItem.end(); it++)
//		{
//			string s = it->first;
//			HKEY hKey = NULL;
//			for(int j = 0; j < it->second.size(); j++)
//			{
//				if(it->second[j]->bSave && hKey == NULL)
//				{
//					hKey = RegCreateKey(HKEY_CURRENT_USER, it->first.c_str(), KEY_WRITE);
//				}
//				it->second[j]->Save(hKey);
//			}
//			RegCloseKey(hKey);
//		}
}
