#include "StdAfx.h"
#include "ZEffect.h"
#include <d3d9.h>

class ZEffectBlank : public ZEffectBase
{
public:
	ZError* Calculate( FLOAT32, FLOAT32, ZAudio* )
	{
		return nullptr;
	};
	ZError* Render( )
	{
		return nullptr;
	}
};

EXPORT_EFFECT( Blank, ZEffectBlank )
