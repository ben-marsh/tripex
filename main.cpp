#include "Platform.h"
#include "AudioData.h"

#include "stdafx.h"
#include <mmeapi.h>
#include "Tripex.h"
#include "RendererDirect3d.h"
#include "effect.h"

#define USE_RANDOM_AUDIO 1

IDirect3D9* g_pd3d = nullptr;
IDirect3DDevice9* g_pd3dDevice = nullptr;

HWAVEIN g_hWaveIn = nullptr;
WAVEHDR g_aWaveHdr[2];
uint8* g_apnWaveBuf[2] = { nullptr, };
WAVEFORMATEX g_wfex;

Tripex* g_pTripex;
RendererDirect3d* g_direct3d;

/*---------------------------------
* CreateD3D( )
-----------------------------------*/

/*---------------------------------
* DestroyD3D( )
-----------------------------------*/

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

/*---------------------------------------------
* OutputWaveInError( ):
---------------------------------------------*/

void OutputWaveInError(MMRESULT mRes)
{
	char sBuf[256];
	waveInGetErrorTextA(mRes, sBuf, sizeof(sBuf));
	printf("%s", sBuf);
}

/*---------------------------------------------
* AddWaveInBuffer( ):
---------------------------------------------*/

MMRESULT AddWaveInBuffer(int nIdx)
{
	if (g_hWaveIn == nullptr) return S_OK;

	MMRESULT mRes = waveInPrepareHeader(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(WAVEHDR));
	if (FAILED(mRes)) return mRes;

	mRes = waveInAddBuffer(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(WAVEHDR));
	if (FAILED(mRes)) return mRes;

	return mRes;
}

/*---------------------------------------------
* RemoveWaveInBuffer( ):
---------------------------------------------*/

MMRESULT RemoveWaveInBuffer(int nIdx)
{
	if (g_hWaveIn == nullptr) return S_OK;

	MMRESULT mRes = waveInUnprepareHeader(g_hWaveIn, &g_aWaveHdr[nIdx], sizeof(g_aWaveHdr[nIdx]));
	if (FAILED(mRes)) return mRes;

	return S_OK;
}

/*---------------------------------------------
* CreateWaveIn( ):
---------------------------------------------*/

MMRESULT CreateWaveIn(HWND hWnd)
{
	ZeroMemory(&g_wfex, sizeof(g_wfex));
	g_wfex.wFormatTag = WAVE_FORMAT_PCM;
	g_wfex.nChannels = 1;//2;
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

/*---------------------------------------------
* DestroyWaveIn( ):
---------------------------------------------*/

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

/*---------------------------------
* TxWndProc( )
-----------------------------------*/

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
			MMRESULT mRes = CreateWaveIn(hWnd);
			if (FAILED(mRes))
			{
				OutputWaveInError(mRes);
				DestroyWaveIn();
			}
			else
			{
				g_direct3d = new RendererDirect3d();

				Error* error = g_direct3d->Open(hWnd);
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

				SetTimer(hWnd, TICK_TIMER_ID, 10, nullptr);
			}
		}
		return FALSE;
	case MM_WIM_OPEN:
		return FALSE;
	case MM_WIM_CLOSE:
		return FALSE;
	case MM_WIM_DATA:
		{
			int nIdx = (int)((WAVEHDR*)lParam - g_aWaveHdr);
			RemoveWaveInBuffer(nIdx);

#if USE_RANDOM_AUDIO
			for (int idx = 0; idx < (int)g_aWaveHdr[nIdx].dwBytesRecorded; idx++)
			{
				g_aWaveHdr[nIdx].lpData[idx] = rand();
			}
#endif

			g_pTripex->SetAudioData(2, 44100, 16, g_aWaveHdr[nIdx].lpData, g_aWaveHdr[nIdx].dwBytesRecorded);

			AddWaveInBuffer(nIdx);
		}
		return FALSE;
	case WM_TIMER:
		if (wParam == TICK_TIMER_ID)
		{
			Error* error = g_pTripex->Render();
			if (error)
			{
				HandleError(hWnd, error);
			}
		}
		break;
	case WM_DESTROY:
		{
			MMRESULT mRes = DestroyWaveIn();
			if (FAILED(mRes)) OutputWaveInError(mRes);
		}

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

/*---------------------------------
* WinMain( )
-----------------------------------*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int nCmd)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = L"TripexWndClass";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)TxWndProc;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(wc.lpszClassName, L"Tripex", WS_OVERLAPPEDWINDOW, 50, 50, 700, 500, nullptr, nullptr, hInstance, nullptr);
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
