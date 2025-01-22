#pragma once

#include "Platform.h"
#include "AudioSource.h"
#include "Error.h"
#include <deque>
#include <mmeapi.h>

class AudioDevice : public AudioSource
{
public:
	virtual Error* Tick(float elapsed) = 0;
	virtual Error* Destroy() = 0;
};

class WaveOutAudioDevice : public AudioDevice
{
public:
	Error* CreateDefault();
	Error* Open(const char* filename);

	virtual Error* Tick(float elapsed) override;
	virtual Error* Destroy() override;

	virtual void Read(int16* data, size_t num_samples) override;

private:
	static const int BUFFERED_PACKETS = 4;

	HWAVEOUT waveout_handle = nullptr;
	bool closing = false;

	std::unique_ptr<uint8[]> wav_file_data;
	std::deque<std::unique_ptr<WAVEHDR>> waveout_packets;

	WAVEFORMATEX wav_format = {};

	const uint8* wav_data = nullptr;
	uint32 wav_data_pos = 0;
	uint32 wav_data_len = 0;

	float sample_pos = 0.0;

	Error* CreateWaveOutError(MMRESULT res);

	static void CALLBACK WaveOutStaticCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	Error* CreateWaveOut();
	Error* DestroyWaveOut();
	Error* WriteNextPacket();
};
