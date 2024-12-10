#include "StdAfx.h"
#include "effect.h"
#include "ZDirect3D.h"
#include "ZEffect.h"

/******** ZEffectBase ************************************/
ZEffectBase::ZEffectBase()
{
}
ZEffectBase::~ZEffectBase()
{
}
HRESULT ZEffectBase::Reconfigure()
{
	return D3D_OK;
}
bool ZEffectBase::CanRender(float fElapsed)
{
	return true;
}

/******** ZEffectPtr **************************************/
//	ZEffectPtr::ZEffectPtr()
//	{
//		pEffect = NULL;
//	}
//	ZEffectPtr::~ZEffectPtr()
//	{
//		_ASSERT(pEffect == NULL);
//		Destroy();
//	}
void ZEffectPtr::Destroy()
{
	if(pEffect != NULL)
	{
		delete pEffect;
		pEffect = NULL;
	}
}
HRESULT ZEffectPtr::Calculate(float fElapsed)
{
	_ASSERT(pEffect != NULL);

	g_pAudio->SetIntensityBeatScale( fSensitivity * 3.0f );
	HRESULT hRes = pEffect->Calculate(fBr, GetElapsed(fElapsed));
	g_pAudio->SetIntensityBeatScale( 0.0f );
	return hRes;
}
HRESULT ZEffectPtr::Reconfigure()
{
	_ASSERT(pEffect != NULL);
	return pEffect->Reconfigure();
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
string ZEffectPtr::GetCfgItemName() const
{
	string sCfgName = "Effects\\";
	for(int i = 0; sName[i] != 0; i++)
	{
		if(isalnum(sName[i])) sCfgName += sName[i];
	}
	return sCfgName;
}
