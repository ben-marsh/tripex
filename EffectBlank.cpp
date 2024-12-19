#include "StdAfx.h"
#include "ZEffect.h"
#include <d3d9.h>

class ZEffectBlank : public ZEffectBase
{
public:
	Error* Calculate( FLOAT32, FLOAT32, AudioData* )
	{
		return nullptr;
	};
	Error* Render( )
	{
		return nullptr;
	}
};

EXPORT_EFFECT( Blank, ZEffectBlank )
