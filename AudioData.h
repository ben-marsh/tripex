#pragma once

#include "Fourier.h"
#include "GeometryBuffer.h"

class AudioData
{
protected:
	static const int FREQ_HISTORY_SIZE = 20;
	static const int FREQ_BANDS = 256;
	static const int BEATHISTORY = 5;

	static const float FREQ_BAND_POWER;

	std::vector<int16> mono_samples;
	std::vector<int16> stereo_samples[2];
	float freq_history[FREQ_HISTORY_SIZE][16];
	Fourier fourier;

	int band_idx[FREQ_BANDS];
	float band_mul[FREQ_BANDS];
	float bands[FREQ_BANDS];

	float beat_history[BEATHISTORY];

	float elapsed;
	float intensity;
	float beat;
	float intensity_beat_scale;
	bool is_beat;

	int num_data_channels;
	int data_sample_size;
	int data_freq_shift;

public:
	const int num_samples;

	AudioData(int num_samples);
	~AudioData();

	void SetDataFormat(int num_channels, int frequency, int num_sample_bits);
	void AddData(const void* data, size_t data_size);

	void Update(float elapsed, float sensitivity);
	void Render(GeometryBuffer& overlay_back, GeometryBuffer& overlay) const;

	float GetIntensity() const;
	float GetBeat() const;
	bool IsBeat() const;
	float GetRandomSample() const;
	void SetIntensityBeatScale(float scale);
	float GetDampenedBand(float dampen, float min, float max) const;

	float GetSample(int sample_idx) const;
	float GetSample(int channel_idx, int sample_idx) const;

	float GetBand(int band_idx) const;
};
