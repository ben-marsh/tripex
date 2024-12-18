#include "StdAfx.h"
#include "effect.h"
#include "ZDirect3D.h"
#include "ZEffect.h"

/******** ZEffectBase ************************************/
ZEffectBase::ZEffectBase()
{
	pEffectPtr = nullptr;
}
ZEffectBase::~ZEffectBase()
{
}
HRESULT ZEffectBase::Reconfigure(ZAudio*)
{
	return D3D_OK;
}
bool ZEffectBase::CanRender(FLOAT32 fElapsed)
{
	return true;
}

/******** ZEffectPtr **************************************/
ZEffectPtr::ZEffectPtr()
{
	bValid = false;
	fBr = 0.0f;
	fProb = 0.0f;
	fStartupWeight = 0.0f;
	nDrawOrder = 0;
	nLastUsed = 0;



	memset(pfSetting, 0, sizeof(pfSetting));
}
ZEffectPtr::~ZEffectPtr()
{
}
void ZEffectPtr::Destroy()
{
	pEffect.reset();
}
HRESULT ZEffectPtr::Calculate(float fElapsed, ZAudio* pAudio)
{
	_ASSERT(pEffect != NULL);

	pAudio->SetIntensityBeatScale( fSensitivity * 3.0f );
	HRESULT hRes = pEffect->Calculate(fBr, GetElapsed(fElapsed), pAudio);
	pAudio->SetIntensityBeatScale( 0.0f );
	return hRes;
}
HRESULT ZEffectPtr::Reconfigure(ZAudio* pAudio)
{
	_ASSERT(pEffect != NULL);
	return pEffect->Reconfigure(pAudio);
}
HRESULT ZEffectPtr::Render()
{
	_ASSERT(pEffect != NULL);
	return pEffect->Render();
}
bool ZEffectPtr::CanRender(float fFrames)
{
	_ASSERT(pEffect != NULL);

	if(fFrames > 3.8) return true;
	else return pEffect->CanRender(GetElapsed(fFrames));
}
float ZEffectPtr::GetElapsed(float fFrames)
{
	return fFrames * ((1.8f * fSpeed) + 0.1f);
}
std::string ZEffectPtr::GetCfgItemName() const
{
	std::string sCfgName = "Effects\\";
	for(int i = 0; sName[i] != 0; i++)
	{
		if(isalnum(sName[i])) sCfgName += sName[i];
	}
	return sCfgName;
}
