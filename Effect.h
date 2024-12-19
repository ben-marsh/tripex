#pragma once

#include <string>
#include <set>
#include <memory>
#include "error.h"
#include "AudioData.h"

#define EFFECT_MAX_TEXTURES 2

#define EFFECT_CHANGE_FRAMES 400.0f

//#include "tripex2.h"

enum // TC_WT (wrap), TC_EM (envmap), TC_LB (lightbob)
{
	TC_GUI,

	// bezier cube
	TC_LBBEZIERCUBE,
	TC_WTBEZIERCUBE,

	// collapsing sphere
	TC_LBCOLLAPSINGSPHERE,
	TC_WTCOLLAPSINGSPHERE,

	// distortion
	TC_WTDISTORTION,

	// distortion 2
	TC_WTDISTORTION2,

	// dot star
	TC_LBDOTSTAR,
	TC_WTDOTSTAR,

	// light ring
	TC_LBLIGHTRING,
	TC_WTLIGHTRING,

	// light sphere
	TC_LBLIGHTSPHERE,
	TC_WTLIGHTSPHERE,

	// light star
	TC_LBLIGHTSTAR,
	TC_WTLIGHTSTAR,

	// light tentacles
	TC_LBLIGHTTENTACLES,
	TC_WTLIGHTTENTACLES,

	// metaballs
	TC_EMMETABALLS,

	// morphing sphere
	TC_EMMORPHINGSPHERE,

	// motion blur
	TC_EMMOTIONBLUR,

	// motion blur 2
	TC_EMMOTIONBLUR2,

	// phased
	TC_LBPHASED,

	// rings
	TC_EMRINGS,

	// bumpmapping
	TC_WTBUMPMAPBACK,
	TC_EMBUMPMAPTENTACLES,

	// tube
	TC_EMTUBE,

	// tunnel
	TC_WTTUNNEL,

	// water globe
	TC_EMWATERGLOBE,

	// spectrum
	TC_EMANALYSER,

	// motion blur 3
	TC_EMMOTIONBLUR3,

	// motion blur 3 alt
	TC_EMMOTIONBLUR3ALT,

	// distortion 2 col
	TC_WTDISTORTION2COL,

	TC_END,
};

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
	class EffectHandler* pEffectPtr;

	EffectBase();
	virtual ~EffectBase();

	virtual Error* Calculate(FLOAT32 fBr, FLOAT32 fElapsed, AudioData* pAudio) = 0;
	virtual Error* Reconfigure(AudioData* pAudio);
	virtual Error* Render() = 0;
	virtual bool CanRender(FLOAT32 fElapsed);
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

	std::string sName;
	std::set<int> snTexture;
	int nDrawOrder;
	float fStartupWeight;

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

	Error* Calculate(float fElapsed, AudioData* pAudio);
	Error* Reconfigure(AudioData* pAudio);
	Error* Render();
	bool CanRender(float fElapsed);

	float GetElapsed(float fFrames);
	std::string GetCfgItemName() const;
};

template < class T > class EffectFactoryT : public EffectHandler
{
public:
	void Create()
	{
		//		_ASSERT(pEffect == NULL);
		//		Destroy();
		pEffect = std::make_unique<T>();
		pEffect->pEffectPtr = this;
	}
};

//#define DECLARE_EFFECT_PTR(type, varname) static ZEffectPtrT<type> pTemp_##varname; ZEffectPtr *varname = &pTemp_##varname;
#define IMPORT_EFFECT(name) extern EffectHandler *CreateEffect_##name( );
#define EXPORT_EFFECT(name, type) EffectHandler *CreateEffect_##name( ){ return (EffectHandler*)new EffectFactoryT< type >( ); }
