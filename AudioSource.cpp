#include "AudioSource.h"
#include <assert.h>

AudioSource::~AudioSource()
{
}

///////////////////////////////////////////

void RandomAudioSource::Read(int16* data, size_t num_samples)
{
	for (size_t idx = 0; idx < num_samples * NUM_CHANNELS; idx++)
	{
		data[idx] = rand();
	}
}
