#pragma once

#include "main.h"
#include "CCfgItem.h"
#include "ZTextureFont.h"
#include "ZAudio.h"
#include <bitset>

class ZTripex
{
public:
	std::bitset<TXS_LAST> txs;

	ZTextureFont tef;
	ZTexture *ptefTexture;
	std::vector< std::unique_ptr< ZTexture > > vpTexture;
	float fEffectFrames, fFadePos;
	char sStatusMsg[ 256 ];
	DWORD dwStatusTime;
	unsigned int id;
	int nEffect;
	int nNextEffect;
	std::unique_ptr< ZTexture > gui;
	DWORD dwLastTime;
	std::unique_ptr<ZAudio> pAudio;

	std::vector< ZEffectPtr* >* pvpEffect;
	std::vector< ZEffectPtr* >* pvpEffectList;

	ZTripex();
	void ShowStatusMsg( const char *sFormat, ... );
	DWORD WINAPI InitialiseThread(void *pParam);
	HRESULT Startup( );
	HRESULT Render( );
	void Shutdown( );

private:
	ZEffectPtr* pEffectBlank;

	std::vector< CCfgItem* >* pppCfgItem = NULL;

	std::string* psEffect;

	class CI_STR_CMP : public std::less<std::string>
	{
	public:
		bool operator()(const std::string& s1, const std::string& s2) const
		{
			return _stricmp(s1.c_str(), s2.c_str()) < 0;
		}
	};

	// TODO: make sure these CfgItems are free'd
	std::map< std::string, std::vector< CCfgItem* >, CI_STR_CMP >* pmpCfgItem = NULL;

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
