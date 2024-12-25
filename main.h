#pragma once

#include "Effect.h"
#include "AudioData.h"

enum
{
	TXS_STARTED,
	TXS_VISIBLE_BEATS,
	TXS_RESET_TIMING,
	TXS_CHANGE_EFFECT,
	TXS_IN_FADE,
	TXS_EFFECT_LEFT,
	TXS_EFFECT_RIGHT,
	TXS_RECONFIGURE,
	TXS_HOLD,
	TXS_RESET_TARGET,

	TXS_LAST,
};
