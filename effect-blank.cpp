#include "StdAfx.h"
#include "ZEffect.h"
#include <d3d9.h>

class ZEffectBlank : public ZEffectBase
{
public:
	HRESULT Calculate( FLOAT32, FLOAT32, ZAudio* )
	{
		return D3D_OK;
	};
	HRESULT Render( )
	{
		return D3D_OK;
	}
};

EXPORT_EFFECT( Blank, ZEffectBlank )
