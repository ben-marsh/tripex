#pragma once

#include <memory>
#include <vector>

class AudioSource
{
public:
	static const int NUM_CHANNELS = 2;
	static const int SAMPLE_RATE = 44100;
	static const int SAMPLE_DEPTH = 16;

	virtual ~AudioSource();
	virtual void Read(int16* data, size_t num_samples) = 0;
};

class RandomAudioSource : public AudioSource
{
public:
	virtual void Read(int16* data, size_t num_samples) override;
};

