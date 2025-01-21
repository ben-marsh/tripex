#include "Platform.h"
#include "AudioData.h"

#include "stdafx.h"
#include <mmeapi.h>
#include "Tripex.h"
#include "RendererDirect3d.h"
#include "effect.h"

#define AUDIO_SOURCE_RANDOM 0
#define AUDIO_SOURCE_WAVEIN 1
#define AUDIO_SOURCE_WAVFILE 2

#define AUDIO_SOURCE AUDIO_SOURCE_WAVFILE // 0 = random, 1 = wave in, 2 = wav file

IDirect3D9* g_pd3d = nullptr;
IDirect3DDevice9* g_pd3dDevice = nullptr;
Tripex* g_pTripex;
RendererDirect3d* g_direct3d;

#if AUDIO_SOURCE == AUDIO_SOURCE_WAVEIN

HWAVEIN g_hWaveIn = nullptr;
WAVEHDR g_aWaveHdr[2];
uint8* g_apnWaveBuf[2] = { nullptr, };
WAVEFORMATEX g_wfex;

#endif
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE

std::unique_ptr<uint8[]> wav_file_data;

const WAVEFORMATEX* wav_file_format = nullptr;

const uint8* wav_data = nullptr;
uint32 wav_data_pos = 0;
uint32 wav_data_len = 0;

#endif

void DestroyD3D()
{
	if (g_pd3dDevice != nullptr)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
	if (g_pd3d != nullptr)
	{
		g_pd3d->Release();
		g_pd3d = nullptr;
	}
}

#if AUDIO_SOURCE == AUDIO_SOURCE_RANDOM

uint16 random_audio_data[4096];

#endif

#if AUDIO_SOURCE == AUDIO_SOURCE_WAVEIN

void OutputWaveInError(MMRESULT mRes)
{
	char sBuf[256];
	waveInGetErrorTextA(mRes, sBuf, sizeof(sBuf));
	printf("%s", sBuf);
}

MMRESULT AddWaveInBuffer(int nIdx)
{
	if (g_hWaveIn == nullptr) return S_OK;

	MMRESULT mRes = waveInPrepareHeader(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(WAVEHDR));
	if (FAILED(mRes)) return mRes;

	mRes = waveInAddBuffer(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(WAVEHDR));
	if (FAILED(mRes)) return mRes;

	return mRes;
}

MMRESULT RemoveWaveInBuffer(int nIdx)
{
	if (g_hWaveIn == nullptr) return S_OK;

	MMRESULT mRes = waveInUnprepareHeader(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(g_aWaveHdr[nIdx]));
	if (FAILED(mRes)) return mRes;

	return S_OK;
}

MMRESULT CreateWaveIn(HWND hWnd)
{
	ZeroMemory(&g_wfex, sizeof(g_wfex));
	g_wfex.wFormatTag = WAVE_FORMAT_PCM;
	g_wfex.nChannels = 1;
	g_wfex.wBitsPerSample = 16;
	g_wfex.nSamplesPerSec = 44100;
	g_wfex.nBlockAlign = (g_wfex.nChannels * g_wfex.wBitsPerSample) / 8;
	g_wfex.nAvgBytesPerSec = g_wfex.nSamplesPerSec * g_wfex.nChannels * g_wfex.wBitsPerSample / 8;

	MMRESULT mRes = waveInOpen(&g_hWaveIn, WAVE_MAPPER, &g_wfex, (DWORD_PTR)hWnd, 0, CALLBACK_WINDOW);
	if (mRes != S_OK) return mRes;

	DWORD nBufSize = g_wfex.nChannels * (g_wfex.nSamplesPerSec / 50) * g_wfex.wBitsPerSample / 8;
	for (int i = 0; i < 2; i++)
	{
		DWORD nAlign = g_wfex.nBlockAlign;
		g_apnWaveBuf[i] = new unsigned char[nBufSize + nAlign];

		ZeroMemory(&g_aWaveHdr[i], sizeof(g_aWaveHdr[i]));
		g_aWaveHdr[i].dwBufferLength = nBufSize;
		g_aWaveHdr[i].lpData = (LPSTR)(((SIZE_T)g_apnWaveBuf[i] + (nAlign - 1)) & ~((SIZE_T)nAlign - 1));

		AddWaveInBuffer(i);
	}

	return waveInStart(g_hWaveIn);
}

MMRESULT DestroyWaveIn()
{
	for (int i = 0; i < 2; i++)
	{
		if (g_apnWaveBuf[i] != nullptr)
		{
			while (!(((volatile WAVEHDR&)(g_aWaveHdr[i])).dwFlags & WHDR_DONE))
			{
				Sleep(10);
			}

			MMRESULT mRes = RemoveWaveInBuffer(i);
			if (FAILED(mRes)) OutputWaveInError(mRes);

			delete g_apnWaveBuf[i];
			g_apnWaveBuf[i] = nullptr;
		}
	}

	if (g_hWaveIn != nullptr)
	{
		MMRESULT mRes = waveInClose(g_hWaveIn);
		if (FAILED(mRes)) return mRes;

		g_hWaveIn = nullptr;
	}

	return S_OK;
}

#endif

#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE

HWAVEOUT g_hWaveOut;

Error* WaveOutWriteNext();

Error* CreateWaveOutError(MMRESULT res)
{
	char buffer[256];
	waveOutGetErrorTextA(res, buffer, sizeof(buffer));
	return new Error(std::string("Wave out error: ") + std::string(buffer));
}

Error* CreateWaveOut(HWND hWnd)
{
	WAVEFORMATEX format = {};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = wav_file_format->nChannels;
	format.wBitsPerSample = wav_file_format->wBitsPerSample;
	format.nSamplesPerSec = wav_file_format->nSamplesPerSec;
	format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nChannels * format.wBitsPerSample / 8;

	MMRESULT res = waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &format, (DWORD_PTR)hWnd, 0, CALLBACK_WINDOW);
	if (res != S_OK) return CreateWaveOutError(res);

	for (int idx = 0; idx < 4; idx++)
	{
		Error* error = WaveOutWriteNext();
		if (error) return error;
	}

	return nullptr;
}

Error* DestroyWaveOut()
{
	MMRESULT res = waveOutReset(g_hWaveOut);
	if (res != S_OK) return CreateWaveOutError(res);

	res = waveOutClose(g_hWaveOut);
	if (res != S_OK) return CreateWaveOutError(res);

	return nullptr;
}

Error* WaveOutFreeBuffer(WAVEHDR* hdr)
{
	MMRESULT res = waveOutUnprepareHeader(g_hWaveOut, hdr, sizeof(WAVEHDR));
	if (res != S_OK)
	{
		return CreateWaveOutError(res);
	}

	delete hdr;
	return nullptr;
}

Error* WaveOutWriteNext()
{
	uint32 buffer_size = wav_file_format->nChannels * 1024 * (wav_file_format->wBitsPerSample / 8);
	buffer_size = std::min(buffer_size, wav_data_len - wav_data_pos);

	WAVEHDR* header = new WAVEHDR();
	memset(header, 0, sizeof(*header));
	header->lpData = (LPSTR)(wav_data + wav_data_pos);
	header->dwBufferLength = buffer_size;

	MMRESULT res = waveOutPrepareHeader(g_hWaveOut, header, sizeof(*header));
	if (res != S_OK) return CreateWaveOutError(res);

	res = waveOutWrite(g_hWaveOut, header, sizeof(*header));
	if (res != S_OK) return CreateWaveOutError(res);

	g_pTripex->WriteAudioData(wav_file_format->nChannels, wav_file_format->nSamplesPerSec, wav_file_format->wBitsPerSample, header->lpData, header->dwBufferLength);

	wav_data_pos += buffer_size;
	if (wav_data_pos == wav_data_len)
	{
		wav_data_pos = 0;
	}

	return nullptr;
}

#endif

static const int TICK_TIMER_ID = 0x1234;

void HandleError(HWND hWnd, Error* error)
{
	KillTimer(hWnd, TICK_TIMER_ID);
	MessageBoxA(hWnd, error->GetDescription().c_str(), nullptr, MB_OK);
	CloseWindow(hWnd);
}

LRESULT CALLBACK TxWndProc(HWND hWnd, uint32 nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CREATE:
		{
			Error* error;
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVEIN
			MMRESULT mRes = CreateWaveIn(hWnd);
			if (FAILED(mRes))
			{
				OutputWaveInError(mRes);
				DestroyWaveIn();
				return FALSE;
			}
#endif
			g_direct3d = new RendererDirect3d();

			error = g_direct3d->Open(hWnd);
			if (error)
			{
				HandleError(hWnd, error);
				return FALSE;
			}

			g_pTripex = new Tripex(*g_direct3d);

			error = g_pTripex->Startup();
			if (error)
			{
				HandleError(hWnd, error);
				return FALSE;
			}

#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE
			error = CreateWaveOut(hWnd);
			if (error)
			{
				HandleError(hWnd, error);
				//				OutputWaveInError(mRes);
				//				DestroyWaveIn();
				return FALSE;
			}
#endif

			SetTimer(hWnd, TICK_TIMER_ID, 10, nullptr);
		}
		return FALSE;
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVEIN
	case MM_WIM_OPEN:
		return FALSE;
	case MM_WIM_CLOSE:
		return FALSE;
	case MM_WIM_DATA:
		{
			int nIdx = (int)((WAVEHDR*)lParam - g_aWaveHdr);
			RemoveWaveInBuffer(nIdx);

			g_pTripex->SetAudioData(g_wfex.nChannels, g_wfex.nSamplesPerSec, g_wfex.wBitsPerSample, g_aWaveHdr[nIdx].lpData, g_aWaveHdr[nIdx].dwBytesRecorded);

			AddWaveInBuffer(nIdx);
		}
		return FALSE;
#endif
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE
	case MM_WOM_OPEN:
		return FALSE;
	case MM_WOM_CLOSE:
		return FALSE;
	case MM_WOM_DONE:
		{
			WAVEHDR* hdr = (WAVEHDR*)lParam;

			Error* error = WaveOutFreeBuffer(hdr);
			if (error)
			{
				HandleError(hWnd, error);
				return FALSE;
			}

			error = WaveOutWriteNext();
			if (error)
			{
				HandleError(hWnd, error);
				return FALSE;
			}
		}
		return FALSE;
#endif
	case WM_TIMER:
		if (wParam == TICK_TIMER_ID)
		{
#if AUDIO_SOURCE == AUDIO_SOURCE_RANDOM
			for (int idx = 0; idx < sizeof(random_audio_data) / sizeof(random_audio_data[0]); idx++)
			{
				random_audio_data[idx] = rand();
			}
			g_pTripex->SetAudioData(2, 44100, 16, random_audio_data, sizeof(random_audio_data));
#endif
			Error* error = g_pTripex->Render();
			if (error)
			{
				HandleError(hWnd, error);
			}
		}
		break;
	case WM_DESTROY:
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVEIN
		{
			MMRESULT mRes = DestroyWaveIn();
			if (FAILED(mRes)) OutputWaveInError(mRes);
		}
#endif
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE
		{
			Error* error = DestroyWaveOut();
			if (error) HandleError(hWnd, error);
		}
#endif

		KillTimer(hWnd, TICK_TIMER_ID);

		if (g_pTripex != nullptr)
		{
			g_pTripex->Shutdown();

			delete g_pTripex;
			g_pTripex = nullptr;
		}

		if (g_direct3d != nullptr)
		{
			g_direct3d->Close();

			delete g_direct3d;
			g_direct3d = nullptr;
		}

		DestroyD3D();
		return FALSE;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_F2:
			g_pTripex->ToggleAudioInfo();
			break;
		case VK_LEFT:
			g_pTripex->MoveToPrevEffect();
			break;
		case VK_RIGHT:
			g_pTripex->MoveToNextEffect();
			break;
		case 'R':
			g_pTripex->ReconfigureEffect();
			break;
		case 'E':
			g_pTripex->ChangeEffect();
			break;
		case 'H':
			g_pTripex->ToggleHoldingEffect();
			break;
		}
		return FALSE;
	}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR sCmdLine, _In_ int nCmd)
{
#if AUDIO_SOURCE == AUDIO_SOURCE_WAVFILE

	FILE* file = fopen("C:\\dump\\yardact.wav", "rb");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	wav_file_data = std::make_unique<uint8[]>(length);
	fread(wav_file_data.get(), 1, length, file);
	fclose(file);

	const uint8* wav_header = wav_file_data.get();
	assert(memcmp(wav_header, "RIFF", 4) == 0);
	assert(memcmp(wav_header + 8, "WAVE", 4) == 0);

	for (long pos = 12; pos < length; )
	{
		const uint8* chunk_header = wav_header + pos;
		uint32 chunk_len = *((const uint32*)(chunk_header + 4));

		const uint8* chunk_data = chunk_header + 8;
		if (memcmp(chunk_header, "fmt ", 4) == 0)
		{
			const WAVEFORMAT* wav_format = (const WAVEFORMAT*)chunk_data;
			assert(wav_format->wFormatTag == WAVE_FORMAT_PCM);

			wav_file_format = (const WAVEFORMATEX*)wav_format;
		}
		else if (memcmp(chunk_header, "data", 4) == 0)
		{
			wav_data  = chunk_data;
			wav_data_len = chunk_len;
		}

		pos += 8 + chunk_len;
	}

	assert(wav_file_format != nullptr);
	assert(wav_data != nullptr);

#endif

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = L"TripexWndClass";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)TxWndProc;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(wc.lpszClassName, L"Tripex", WS_OVERLAPPEDWINDOW, 50, 50, 800, 600, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (IsWindow(hWnd))
	{
		DWORD dwRes = GetMessage(&msg, nullptr, 0, 0);
		if (dwRes == 0 || dwRes == -1) break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
