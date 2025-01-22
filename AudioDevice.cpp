#include "AudioDevice.h"
#include "Misc.h"

Error* WaveOutAudioDevice::Tick(float elapsed)
{
	while (waveout_packets.size() > 0)
	{
		std::unique_ptr<WAVEHDR>& packet = waveout_packets.front();
		if ((packet->dwFlags & WHDR_DONE) == 0)
		{
			break;
		}

		MMRESULT res = waveOutUnprepareHeader(waveout_handle, packet.get(), sizeof(WAVEHDR));
		if (res != S_OK) return CreateWaveOutError(res);

		waveout_packets.pop_front();
	}

	while (waveout_packets.size() < BUFFERED_PACKETS)
	{
		Error* error = WriteNextPacket();
		if (error) return error;
	}

	return nullptr;
}

Error* WaveOutAudioDevice::Destroy()
{
	return DestroyWaveOut();
}

Error* WaveOutAudioDevice::CreateDefault()
{
	wav_format.wFormatTag = WAVE_FORMAT_PCM;
	wav_format.nChannels = 1;
	wav_format.wBitsPerSample = 16;
	wav_format.nSamplesPerSec = 44100;
	wav_format.nBlockAlign = (wav_format.nChannels * wav_format.wBitsPerSample) / 8;

	int length = 20;
	int num_samples = wav_format.nSamplesPerSec * length;

	wav_data_len = wav_format.nBlockAlign * num_samples;

	wav_file_data = std::make_unique<uint8[]>(wav_data_len);

	wav_data = wav_file_data.get();

	int16* samples = (int16*)wav_file_data.get();
	for (int idx = 0; idx < num_samples; idx++)
	{
		float time = (float)idx / wav_format.nSamplesPerSec;
		float frequency = ((((int)time) % 2) == 0) ? 256.0f : 512.0f;
		float angle = time * frequency * PI2;
		samples[idx] = (int16)(32760.0f * sinf(angle));
	}

	return CreateWaveOut();
}

Error* WaveOutAudioDevice::Open(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	wav_file_data = std::make_unique<uint8[]>(length);
	fread(wav_file_data.get(), 1, length, file);
	fclose(file);

	const uint8* wav_header = wav_file_data.get();
	if (memcmp(wav_header, "RIFF", 4) != 0)
	{
		return new Error("Missing RIFF bytes at start of WAV file");
	}
	if (memcmp(wav_header + 8, "WAVE", 4) != 0)
	{
		return new Error("Missing WAVE section in WAV file");
	}

	for (long pos = 12; pos < length; )
	{
		const uint8* chunk_header = wav_header + pos;
		uint32 chunk_len = *((const uint32*)(chunk_header + 4));

		const uint8* chunk_data = chunk_header + 8;
		if (memcmp(chunk_header, "fmt ", 4) == 0)
		{
			memcpy(&wav_format, chunk_data, std::min((size_t)chunk_len, sizeof(WAVEFORMATEX)));
		}
		else if (memcmp(chunk_header, "data", 4) == 0)
		{
			wav_data = chunk_data;
			wav_data_len = chunk_len;
		}

		pos += 8 + chunk_len;
	}

	if (wav_format.wFormatTag != WAVE_FORMAT_PCM)
	{
		return new Error("Unsupported WAV format");
	}
	if (wav_data == nullptr)
	{
		return new Error("No data section in WAV file");
	}

	return CreateWaveOut();
}

void WaveOutAudioDevice::Read(int16* data, size_t num_samples)
{
	uint32 wav_sample_size = wav_format.nChannels * (wav_format.wBitsPerSample / 8);
	uint32 wav_sample_count = wav_data_len / wav_sample_size;

	uint32 second_channel_offset = (wav_format.nChannels > 1) ? 1 : 0;

	float sample_step = (float)wav_format.nSamplesPerSec / SAMPLE_RATE;

	if (wav_format.wBitsPerSample == 16)
	{
		const int16* wav_samples = (const int16*)wav_data;
		for (size_t idx = 0; idx < num_samples; idx++)
		{
			int sample_idx = std::min((int)sample_pos, (int)wav_sample_count - 2);
			float sample_t = std::min(sample_pos - sample_idx, 1.0f);

			int wav_idx = sample_idx * wav_format.nChannels;
			*(data++) = wav_samples[wav_idx] + (wav_samples[wav_idx + wav_format.nChannels] - wav_samples[wav_idx]) * sample_t;
			wav_idx += second_channel_offset;
			*(data++) = wav_samples[wav_idx] + (wav_samples[wav_idx + wav_format.nChannels] - wav_samples[wav_idx]) * sample_t;

			sample_pos += sample_step;
			if (sample_pos >= wav_sample_count) sample_pos -= wav_sample_count;
		}
	}
	else
	{
		assert(false);
	}
}

Error* WaveOutAudioDevice::CreateWaveOutError(MMRESULT res)
{
	char buffer[256];
	waveOutGetErrorTextA(res, buffer, sizeof(buffer));
	return new Error(std::string("Wave out error: ") + std::string(buffer));
}

void CALLBACK WaveOutAudioDevice::WaveOutStaticCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
}

Error* WaveOutAudioDevice::CreateWaveOut()
{
	WAVEFORMATEX format = {};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = wav_format.nChannels;
	format.wBitsPerSample = wav_format.wBitsPerSample;
	format.nSamplesPerSec = wav_format.nSamplesPerSec;
	format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nChannels * format.wBitsPerSample / 8;

	MMRESULT res = waveOutOpen(&waveout_handle, WAVE_MAPPER, &format, (DWORD_PTR)&WaveOutStaticCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (res != S_OK) return CreateWaveOutError(res);

	for (int idx = 0; idx < 4; idx++)
	{
		Error* error = WriteNextPacket();
		if (error) return error;
	}

	return nullptr;
}

Error* WaveOutAudioDevice::DestroyWaveOut()
{
	closing = true;

	MMRESULT res = waveOutReset(waveout_handle);
	if (res != S_OK) return CreateWaveOutError(res);

	res = waveOutClose(waveout_handle);
	if (res != S_OK) return CreateWaveOutError(res);

	return nullptr;
}

Error* WaveOutAudioDevice::WriteNextPacket()
{
	uint32 buffer_size = wav_format.nChannels * 1024 * (wav_format.wBitsPerSample / 8);
	buffer_size = std::min(buffer_size, wav_data_len - wav_data_pos);

	std::unique_ptr<WAVEHDR> header = std::make_unique<WAVEHDR>();
	memset(header.get(), 0, sizeof(*header));
	header->lpData = (LPSTR)(wav_data + wav_data_pos);
	header->dwBufferLength = buffer_size;

	MMRESULT res = waveOutPrepareHeader(waveout_handle, header.get(), sizeof(*header));
	if (res != S_OK) return CreateWaveOutError(res);

	res = waveOutWrite(waveout_handle, header.get(), sizeof(*header));
	if (res != S_OK) return CreateWaveOutError(res);

	waveout_packets.push_back(std::move(header));

	wav_data_pos += buffer_size;
	if (wav_data_pos == wav_data_len)
	{
		wav_data_pos = 0;
	}

	return nullptr;
}
