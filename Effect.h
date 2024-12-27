#pragma once

#include <string>
#include <set>
#include <memory>
#include "error.h"
#include "AudioData.h"
#include "Renderer.h"
#include "TextureLibrary.h"

#define EFFECT_MAX_TEXTURES 2

#define EFFECT_CHANGE_FRAMES 400.0f

enum
{
	// z buffering
	ZORDER_BUMPMAPPING,
	ZORDER_METABALLS,
	ZORDER_ANALYSER,
	ZORDER_WATERGLOBE,
	ZORDER_OSCILLOSCOPE,
	ZORDER_RINGS,
	ZORDER_TUBE,

	// alpha only
	ZORDER_BEZIERCUBE,
	ZORDER_COLLAPSINGSPHERE,
	ZORDER_DISTORTION,
	ZORDER_DISTORTION2,
	ZORDER_DISTORTION2COL,
	ZORDER_DOTSTAR,
	ZORDER_FLOWMAP, 
	ZORDER_LIGHTRING,
	ZORDER_LIGHTSPHERE,
	ZORDER_LIGHTSTAR,
	ZORDER_LIGHTTENTACLES,
	ZORDER_MORPHINGSPHERE,
	ZORDER_MOTIONBLUR,
	ZORDER_MOTIONBLUR2,
	ZORDER_MOTIONBLUR3,
	ZORDER_MOTIONBLUR3ALT,
	ZORDER_PHASED,
	ZORDER_SUN,
	ZORDER_TUNNEL,
	ZORDER_BLANK,
};

class EffectBase
{
public:
	struct CalculateParams
	{
		float brightness;
		float elapsed;
		const AudioData& audio_data;
		Renderer& renderer;

		CalculateParams(float brightness, float elapsed, const AudioData& audio_data, Renderer& renderer)
			: brightness(brightness)
			, elapsed(elapsed)
			, audio_data(audio_data)
			, renderer(renderer)
		{
		}
	};

	struct ReconfigureParams
	{
		const AudioData& audio_data;
		const TextureLibrary& texture_library;

		ReconfigureParams(const AudioData& audio_data, const TextureLibrary& texture_library)
			: audio_data(audio_data)
			, texture_library(texture_library)
		{
		}
	};

	struct RenderParams
	{
		Renderer& renderer;

		RenderParams(Renderer& renderer)
			: renderer(renderer)
		{
		}
	};

	class EffectHandler* pEffectPtr;

	EffectBase();
	virtual ~EffectBase();

	virtual Error* Calculate(const CalculateParams& params) = 0;
	virtual Error* Reconfigure(const ReconfigureParams& params);
	virtual Error* Render(const RenderParams& params) = 0;
	virtual bool CanRender(float fElapsed);
};

class EffectHandler
{
protected:
	std::unique_ptr<EffectBase> pEffect;

public:
	float fBr;
	bool bValid;
	int nLastUsed;
	float fProb;

	std::string name;
	int draw_order;
	float startup_weight;

	union
	{
		float pfSetting[5];
		struct
		{
			float fPreference;
			float fChange;
			float fSensitivity;
			float fActivity;
			float fSpeed;
		};
	};

	EffectHandler();
	~EffectHandler();
	virtual void Create() = 0;
	virtual void Destroy();

	Error* Calculate(float elapsed, AudioData& audio_data, Renderer& renderer);
	Error* Reconfigure(const AudioData& audio_data, const TextureLibrary& texture_library);
	Error* Render(Renderer& renderer);
	bool CanRender(float fElapsed);

	float GetElapsed(float fFrames);
	std::string GetCfgItemName() const;
};

template < class T > class EffectHandlerT : public EffectHandler
{
public:
	void Create()
	{
		pEffect = std::make_unique<T>();
		pEffect->pEffectPtr = this;
	}
};

#define IMPORT_EFFECT(name) extern std::shared_ptr<EffectHandler> CreateEffect_##name( );
#define EXPORT_EFFECT(name, type) std::shared_ptr<EffectHandler> CreateEffect_##name( ){ return std::make_shared<EffectHandlerT<type>>(); }
