#include "Platform.h"
#include "ZDirect3D.h"
#include "Effect.h"

/******** ZEffectBase ************************************/
EffectBase::EffectBase()
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
	fStartupWeight = 0.0f;
	nDrawOrder = 0;
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
Error* EffectHandler::Calculate(float fElapsed, AudioData* pAudio)
{
	_ASSERT(pEffect != NULL);

	pAudio->SetIntensityBeatScale( fSensitivity * 3.0f );

	EffectBase::CalculateParams params(fBr, GetElapsed(fElapsed), *pAudio);
	Error* error = pEffect->Calculate(params);

	pAudio->SetIntensityBeatScale( 0.0f );
	return error;
}
Error* EffectHandler::Reconfigure(AudioData* audio_data, const TextureLibrary& texture_library)
{
	_ASSERT(pEffect != NULL);

	EffectBase::ReconfigureParams params(*audio_data, texture_library);
	return pEffect->Reconfigure(params);
}
Error* EffectHandler::Render()
{
	_ASSERT(pEffect != NULL);

	EffectBase::RenderParams params;

	return pEffect->Render(params);
}
bool EffectHandler::CanRender(float fFrames)
{
	_ASSERT(pEffect != NULL);

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
	for(int i = 0; sName[i] != 0; i++)
	{
		if(isalnum(sName[i])) sCfgName += sName[i];
	}
	return sCfgName;
}
