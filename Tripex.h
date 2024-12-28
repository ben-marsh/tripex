#pragma once

#include "ConfigItem.h"
#include "TextureFont.h"
#include "AudioData.h"
#include "Effect.h"
#include "TextureLibrary.h"
#include "GeometryBuffer.h"
#include <bitset>

class Tripex
{
public:
	Tripex(Renderer& renderer);
	~Tripex();

	Error* Startup();
	Error* Render();
	void Shutdown();

	void SetAudioData(int num_channels, int sample_rate, int sample_bits, const void* data, size_t data_len);

	void ChangeEffect();
	void MoveToPrevEffect();
	void MoveToNextEffect();
	void ReconfigureEffect();
	void ToggleHoldingEffect();

	void ToggleAudioInfo();

private:
	static const int MSG_DISPLAY_TIME = 4000;
	static const int MSG_FADEOUT_TIME = 1000;

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

	std::bitset<TXS_LAST> txs;
	std::unique_ptr<AudioData> audio;

	Effect* pEffectBlank;

	std::vector< ConfigItem* >* pppCfgItem = NULL;

	std::string* psEffect;

	TextureLibrary texture_library;

	GeometryBuffer overlay_background;
	GeometryBuffer overlay_text;
	GeometryBuffer overlay_foreground;

	Renderer& renderer;
	TextureFont tef;
	float effect_frames, fade_pos;
	char status_msg[256];
	DWORD status_time;
	unsigned int id;
	int effect_idx;
	int next_effect_idx;
	std::shared_ptr<Texture> gui;
	DWORD last_time;

	std::vector<std::shared_ptr<Effect>> effects;
	std::vector<std::shared_ptr<Effect>> enabled_effects;

	class CI_STR_CMP : public std::less<std::string>
	{
	public:
		bool operator()(const std::string& s1, const std::string& s2) const
		{
			return _stricmp(s1.c_str(), s2.c_str()) < 0;
		}
	};

	// TODO: make sure these CfgItems are free'd
	std::map< std::string, std::vector< ConfigItem* >, CI_STR_CMP >* name_to_config_item = NULL;

	void ShowStatusMsg(const char* format, ...);
	DWORD WINAPI InitialiseThread(void* param);

	void DrawMessage(const TextureFont& font, int y, const char* text, float brightness, float back_brightness);
	int GetClippedLineLength(const TextureFont& font, const char* text, int clip_width);

	void AddEffect(std::shared_ptr<Effect>(*fn)(), const char* name, int draw_order, float startup_weight);
	void CreateEffectList();

	ConfigItem* AddCfgItem(ConfigItem* item);
	void CreateCfgItems();
	void UpdateCfgItems(bool init = false);
	ConfigItem* FindCfgItem(const char* name);

	bool bLoadedCfg = false;
	void LoadCfgItems();
	void SaveCfgItems();
};
