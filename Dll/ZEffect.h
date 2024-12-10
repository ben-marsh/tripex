#pragma once

class ZEffectBase
{
public:
	class ZEffectPtr *pEffectPtr;

	ZEffectBase();
	virtual ~ZEffectBase();

	virtual HRESULT Calculate( FLOAT32 fBr, FLOAT32 fElapsed ) = 0;
	virtual HRESULT Reconfigure( );
	virtual HRESULT Render( ) = 0;
	virtual bool CanRender( FLOAT32 fElapsed );
};

class ZEffectPtr
{
protected:
	ZEffectBase *pEffect;

public:
	float fBr;
	bool bValid;
	int nLastUsed;
	float fProb;

	string sName;
	set<int> snTexture;
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

//	ZEffectPtr();
//	~ZEffectPtr();
	virtual void Create() = 0;
	virtual void Destroy();

	HRESULT Calculate(float fElapsed);
	HRESULT Reconfigure( );
	HRESULT Render( );
	bool CanRender(float fElapsed);

	float GetElapsed(float fFrames);
	string GetCfgItemName() const;
};

template < class T > class ZEffectPtrT : public ZEffectPtr
{
public:
	void Create()
	{
//		_ASSERT(pEffect == NULL);
//		Destroy();
		pEffect = static_cast<ZEffectBase*>(new T);
		pEffect->pEffectPtr = this;
	}
};

//#define DECLARE_EFFECT_PTR(type, varname) static ZEffectPtrT<type> pTemp_##varname; ZEffectPtr *varname = &pTemp_##varname;
#define EXPORT_EFFECT(name, type) ZEffectPtr *CreateEffect_##name( ){ return (ZEffectPtr*)new ZEffectPtrT< type >( ); }
