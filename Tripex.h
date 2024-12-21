#pragma once

#include "main.h"
#include "ConfigItem.h"
#include "TextureFont.h"
#include "AudioData.h"
#include <bitset>

class Tripex
{
public:
	std::bitset<TXS_LAST> txs;

	TextureFont tef;
	Texture *ptefTexture;
	std::vector< std::unique_ptr< Texture > > vpTexture;
	float fEffectFrames, fFadePos;
	char sStatusMsg[ 256 ];
	DWORD dwStatusTime;
	unsigned int id;
	int nEffect;
	int nNextEffect;
	std::unique_ptr< Texture > gui;
	DWORD dwLastTime;
	std::unique_ptr<AudioData> pAudio;

	std::vector<std::shared_ptr<EffectHandler>> effects;
	std::vector<std::shared_ptr<EffectHandler>> enabled_effects;

	Tripex();
	void ShowStatusMsg( const char *sFormat, ... );
	DWORD WINAPI InitialiseThread(void *pParam);
	Error* Startup( );
	Error* Render( );
	void Shutdown( );

private:
	EffectHandler* pEffectBlank;

	std::vector< ConfigItem* >* pppCfgItem = NULL;

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
	std::map< std::string, std::vector< ConfigItem* >, CI_STR_CMP >* pmpCfgItem = NULL;

	void DrawMessage(SpriteBuffer& sb, TextureFont* pFont, int y, const char* sText, float fBr, float fBackBr);
	int GetClippedLineLength(TextureFont* pFont, const char* sText, int nClipWidth);

	void AddEffect(std::shared_ptr<EffectHandler> (*fn)(), const char* sName, int nDrawOrder, float fStartupWeight, TextureClass nTex, ...);
	void CreateEffectList();

	ConfigItem* AddCfgItem(ConfigItem* pItem);
	void CreateCfgItems();
	void UpdateCfgItems(bool bInit = false);
	ConfigItem* FindCfgItem(const char* sName);

	bool bLoadedCfg = false;
	void LoadCfgItems();
	void SaveCfgItems();
};
