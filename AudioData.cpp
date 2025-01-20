#include "Platform.h"
#include <assert.h>
#include "AudioData.h"
#include "Misc.h"
#include "Renderer.h"
#include "effect.h"

const float AudioData::FREQ_BAND_POWER = 2.0f;

extern float fHUDTransparency;

void DrawLineBar(GeometryBuffer& geom, int x, int y, int h, float p)
{
	int n = (int)(h * Bound<float>(p, 0.0f, 1.0f));
	geom.AddSprite(Point<int>(x, y), Rect<int>(0, 0, 1, n), 0.1f);
	geom.AddSprite(Point<int>(x, y + n), Rect<int>(0, 0, 1, h - n), 1.0f);
}

void DrawHorizontalBar(GeometryBuffer& geom, int x, int y, int w, int h, float p)
{
	p = std::min(1.0f, std::max(0.0f, p));
	geom.AddSprite(Point<int>(x, y), Rect<int>(0, 0, w, h), ColorRgb::Grey(20));
	geom.AddSprite(Point<int>(x, y), Rect<int>(0, 0, (int)(w * p), h), ColorRgb::White());
}


/*---------------------------------------------
* ZAudio:
---------------------------------------------*/

/*---------------------------------------------
* Constructor:
---------------------------------------------*/

AudioData::AudioData(int num_samples) : fourier(num_samples), num_samples(num_samples)
{
	mono_samples.resize(num_samples);
	stereo_samples[0].resize(num_samples);
	stereo_samples[1].resize(num_samples);

	memset(freq_history, 0, sizeof(freq_history));

	elapsed = 0.0f;
	intensity = 0.0f;
	beat = 0.0f;
	is_beat = FALSE;
	intensity_beat_scale = 0.0f;

	static const float power = 2.0f;
	static const float mul = 1.0f;

	float min_t = 0.0f, mid_t, max_t;
	for (int i = 0; i < FREQ_BANDS; i++)
	{
		mid_t = powf((i + 0.5f) / FREQ_BANDS, power);
		max_t = powf((i + 1.0f) / FREQ_BANDS, power);

		band_idx[i] = std::min((int)((num_samples / 2) * mid_t), (num_samples / 2) - 1);
		band_mul[i] = mul * ((max_t - min_t) * (num_samples / 2));

		min_t = max_t;
	}
}

AudioData::~AudioData()
{
}

/*---------------------------------------------
* SetDataFormat( ):
---------------------------------------------*/

void AudioData::SetDataFormat(int nChannels, int nFrequency, int nSampleBits)
{
	num_data_channels = nChannels;
	data_freq_shift = IntegerLog2(44100 / nFrequency);
	data_sample_size = nSampleBits / 8;
}

/*---------------------------------------------
* AddData( ):
---------------------------------------------*/

void AudioData::AddData(const void* data, size_t data_size)
{
	uint8* data_pos = (uint8*)data;

	size_t nDstIdx = 0;
	int nMaxSize = (num_samples * data_sample_size * num_data_channels) >> data_freq_shift;
	if (data_size > nMaxSize)
	{
		data_pos += data_size - nMaxSize;
	}
	else if (data_size < nMaxSize)
	{
		nDstIdx = num_samples - (data_size << data_freq_shift) / (num_data_channels * data_sample_size);
		memmove(stereo_samples[0].data(), stereo_samples[0].data() + (num_samples - nDstIdx), nDstIdx * sizeof(short int));
		memmove(stereo_samples[1].data(), stereo_samples[1].data() + (num_samples - nDstIdx), nDstIdx * sizeof(short int));
	}

	size_t nIdx = nDstIdx;
	int nStep = 1 << data_freq_shift;

	//	int nDstIdx = 0;
	//	int nDstSize = &m_anStereo[ m_nSamples * 2 ] - pnDst;
	//	int nDstStep = ( 2 * ( 1 << m_nDataFreqShift ) );
	switch (data_sample_size)
	{
	case 1:
		for (; nIdx < num_samples; nIdx += nStep)
		{
			stereo_samples[0][nIdx] = ((int)*data_pos) << 8;
			data_pos += num_data_channels - 1;
			stereo_samples[1][nIdx] = ((int)*data_pos) << 8;
			data_pos++;
		}
		break;
	case 2:
		for (; nIdx < num_samples; nIdx += nStep)
		{
			stereo_samples[0][nIdx] = *((short int*)data_pos);
			data_pos += (num_data_channels - 1) * 2;
			stereo_samples[1][nIdx] = *((short int*)data_pos);
			data_pos += 2;
		}
		break;
	default:
		assert(false);
		break;
	}

	//	short int *pnMonoDst = &m_anMono[ nDstIdx / 2 ];
	//	for( int i = nDstIdx; i < m_nSamples * 2; i += 2 )
	//	{
	//		*( pnMonoDst++ ) = ( ( int )( m_anStereo[ i + 0 ] ) + ( int )( m_anStereo[ i + 1 ] ) ) >> 1;
	//	}
}

/*---------------------------------------------
* Update( ):
---------------------------------------------*/

void AudioData::Update(float elapsed, float sensitivity)
{
	for (int i = 0; i < num_samples; i++)
	{
		mono_samples[i] = ((int)stereo_samples[0][i] + (int)stereo_samples[1][i]) / 2;
	}

	fourier.Update(mono_samples.data());

	for (int i = 0; i < FREQ_BANDS; i++)
	{
		bands[i] = band_mul[i] * fourier.GetAmplitude(band_idx[i]);
	}

	elapsed = std::min(elapsed + elapsed, 1.0f);
	for (; elapsed > 0.5f; elapsed -= 0.5f)
	{
		for (size_t idx = FREQ_HISTORY_SIZE - 1; idx > 0; idx--)
		{
			memmove(freq_history[idx], freq_history[idx - 1], sizeof(freq_history[idx]));
		}
		memset(freq_history[0], 0, sizeof(freq_history[0]));

		for (int i = 0; i < 256; i++)
		{
			freq_history[0][i >> 4] += std::min(1.0f, bands[i] * 4.0f) / 16.0f;
		}

		float sum = 0.0f;
		for (int i = 0; i < num_samples; i++)
		{
			sum += abs(mono_samples[i]);
		}

		intensity = (576.0f / 512.0f) * sum / ( /*10.0f **/ 10.0f * num_samples * 256.0f);//* 256.0f );// / ( 256.0f * 10.0f ); 
		//		pastaverage[0] /= 10.0 * 2 * 576.0;//10.0 * 288.0;
		///		pastaverage[0] *= (1 / (pastaverage[0] + 0.6));
		//		pastaverage[0] = min(max(pastaverage[0], 0), 1);


		//		m_fIntensity /= ( 1 << 16 ) * m_nSamples;
		//		m_fIntensity /= 10.0f * 2.0f * 2.0f /*512.0f */* 2.0f;
		intensity *= 1.0f / (intensity + 0.6f);
		intensity = std::min(std::max(intensity, 0.0f), 1.0f);

		const bool disable_beats = false;
		if (disable_beats)
		{
			beat = 0.0f;
		}
		else
		{
			float value = (fourier.GetAmplitude(0) + fourier.GetAmplitude(1)) * 1.5f;

			float size = 0.0f;
			for (int i = 0; i < BEATHISTORY; i++)
			{
				size = std::max(size, value - beat_history[i]);
			}

			float limit = 500.0f - 4.8f * sensitivity;
			if (size > limit)
			{
				is_beat = TRUE;
				beat += 0.5f * size / limit;
			}
			else
			{
				is_beat = FALSE;
			}

			memmove(&beat_history[1], &beat_history[0], sizeof(beat_history[0]) * (BEATHISTORY - 1));
			beat_history[0] = beat;

			beat = std::min(beat, 1.0f);
			beat *= 0.6f;
		}
	}
}

/*---------------------------------------------
* Render( ):
---------------------------------------------*/

void AudioData::Render(GeometryBuffer& overlay_back, GeometryBuffer& overlay, float overlay_back_mult) const
{
	overlay_back.AddSprite(Point<int>(10, 30), Rect<int>(0, 0, 600, 400), overlay_back_mult);

	DrawHorizontalBar(overlay, 20, 40, 256, 10, intensity);
	DrawHorizontalBar(overlay, 20, 60, 256, 10, beat);

	for (int i = 0; i < 256; i++)
	{
		DrawLineBar(overlay, 20 + i, 140, 200, 1 - bands[i]);
	}

	for (int i = 0; i < 16; i++)
	{
		DrawHorizontalBar(overlay, 300, 280 + (i * 5), 256, 5, GetDampenedBand(1.0f, i / 16.0f, (i + 1.0f) / 16.0f));
	}
}

/*---------------------------------------------
* GetRandomSample( ):
---------------------------------------------*/

float AudioData::GetRandomSample() const
{
	return mono_samples[(rand() * num_samples) / RAND_MAX] * (1.0f / (1 << 16));
}

/*---------------------------------------------
* GetDampenedBand( ):
---------------------------------------------*/

float AudioData::GetDampenedBand(float dampen, float min, float max) const
{
	int history = (int)(dampen * FREQ_HISTORY_SIZE);
	history = std::min(std::max(history, 1), FREQ_HISTORY_SIZE - 1);

	float peak = 0.0f;

	int min_idx = std::min(std::max((int)(min * 16.0f), 0), 15);
	int max_idx = std::min(std::max((int)(max * 16.0f), 0), 15);
	for (int i = min_idx; i < max_idx; i++)
	{
		float sum = 0.0f;
		for (int j = 0; j < history; j++)
		{
			sum += freq_history[j][i];
		}
		peak = std::max(peak, sum);
	}
	return peak / history;
}

float AudioData::GetIntensity() const
{
	return intensity + (beat * intensity_beat_scale);
}

float AudioData::GetBeat() const
{
	return beat;
}

bool AudioData::IsBeat() const
{
	return is_beat;
}

void AudioData::SetIntensityBeatScale(float scale)
{
	intensity_beat_scale = scale;
}

float AudioData::GetSample(int sample_idx) const
{
	assert(sample_idx >= 0 && sample_idx < num_samples);
	return ((float)mono_samples[sample_idx]) / (1 << 15);
}

float AudioData::GetSample(int channel_idx, int sample_idx) const
{
	assert(sample_idx >= 0 && sample_idx < num_samples);
	assert(channel_idx >= 0 && channel_idx < 2);
	return ((float)stereo_samples[channel_idx][sample_idx]) / (1 << 15);
}

float AudioData::GetBand(int band_idx) const
{
	assert(band_idx >= 0 && band_idx < num_samples);
	return bands[band_idx];
}
