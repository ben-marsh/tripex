#include "Platform.h"
#include "Effect.h"
#include <d3d9.h>

class EffectBlank : public EffectBase
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

EXPORT_EFFECT( Blank, EffectBlank )
