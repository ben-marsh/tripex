#pragma once

#include "main.h"
#include "CCfgItem.h"
#include "ZTextureFont.h"

class ZTripex
{
public:
	bitset<TXS_LAST> txs;

	ZTextureFont tef;
	ZTexture *ptefTexture;
	vector< auto_ptr< ZTexture > > vpTexture;
	float fEffectFrames, fFadePos;
	char sStatusMsg[ 256 ];
	DWORD dwStatusTime;
	unsigned int id;
	int nEffect;
	int nNextEffect;
	auto_ptr< ZTexture > gui;
	DWORD dwLastTime;

	vector< ZEffectPtr* >* pvpEffect;
	vector< ZEffectPtr* >* pvpEffectList;

	ZTripex();
	void ShowStatusMsg( const char *sFormat, ... );
	DWORD WINAPI InitialiseThread(void *pParam);
	HRESULT Startup( );
	HRESULT Render( );
	void Shutdown( );

private:
	ZEffectPtr* pEffectBlank;

	vector< CCfgItem* >* pppCfgItem = NULL;

	string* psEffect;

	class CI_STR_CMP : public less<string>
	{
	public:
		bool operator()(const string& s1, const string& s2) const
		{
			return _stricmp(s1.c_str(), s2.c_str()) < 0;
		}
	};

	// TODO: make sure these CfgItems are free'd
	map< string, vector< CCfgItem* >, CI_STR_CMP >* pmpCfgItem = NULL;

	void AddEffect(ZEffectPtr* (*fn)(), const char* sName, int nDrawOrder, float fStartupWeight, int nTex, ...);
	void CreateEffectList();

	CCfgItem* AddCfgItem(CCfgItem* pItem);
	void CreateCfgItems();
	void UpdateCfgItems(bool bInit = false);
	CCfgItem* FindCfgItem(const char* sName);

	bool bLoadedCfg = false;
	void LoadCfgItems();
	void SaveCfgItems();
};
