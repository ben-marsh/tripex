#include "Platform.h"
#include "Renderer.h"
#include "Effect.h"

/******** ZEffectBase ************************************/
EffectBase::EffectBase()
{
	pEffectPtr = nullptr;
}
EffectBase::EffectBase(std::initializer_list<const TextureClass*> textures)
	: textures(textures)
{
	pEffectPtr = nullptr;
}
EffectBase::~EffectBase()
{
}
Error* EffectBase::Reconfigure(const ReconfigureParams& params)
{
	return nullptr;
}
bool EffectBase::CanRender(float fElapsed)
{
	return true;
}

/******** ZEffectPtr **************************************/
EffectHandler::EffectHandler()
{
	bValid = false;
	fBr = 0.0f;
	fProb = 0.0f;
	startup_weight = 0.0f;
	draw_order = 0;
	nLastUsed = 0;



	memset(pfSetting, 0, sizeof(pfSetting));
}
EffectHandler::~EffectHandler()
{
}
void EffectHandler::Destroy()
{
	pEffect.reset();
}
Error* EffectHandler::Calculate(float fElapsed, AudioData& audio_data, Renderer& renderer)
{
	assert(pEffect != NULL);

	audio_data.SetIntensityBeatScale( fSensitivity * 3.0f );

	EffectBase::CalculateParams params(fBr, GetElapsed(fElapsed), audio_data, renderer);
	Error* error = pEffect->Calculate(params);

	audio_data.SetIntensityBeatScale( 0.0f );
	return error;
}
Error* EffectHandler::Reconfigure(const AudioData& audio_data, const TextureLibrary& texture_library)
{
	assert(pEffect != NULL);

	EffectBase::ReconfigureParams params(audio_data, texture_library);
	return pEffect->Reconfigure(params);
}
Error* EffectHandler::Render(Renderer& renderer)
{
	assert(pEffect != NULL);

	EffectBase::RenderParams params(renderer);
	return pEffect->Render(params);
}
bool EffectHandler::CanRender(float fFrames)
{
	assert(pEffect != NULL);

	if(fFrames > 3.8) return true;
	else return pEffect->CanRender(GetElapsed(fFrames));
}
float EffectHandler::GetElapsed(float fFrames)
{
	return fFrames * ((1.8f * fSpeed) + 0.1f);
}
std::string EffectHandler::GetCfgItemName() const
{
	std::string sCfgName = "Effects\\";
	for(int i = 0; name[i] != 0; i++)
	{
		if(isalnum(name[i])) sCfgName += name[i];
	}
	return sCfgName;
}
