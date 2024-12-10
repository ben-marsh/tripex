#include "stdafx.h"
#include <mmeapi.h>
#include "Vis.h"

#define USE_XBOX_DLL_LOADER

struct VISUALISATION g_vis;
IDirect3D9 *g_pd3d = NULL;
IDirect3DDevice9 *g_pd3dDevice = NULL;
BOOL g_bStarted = FALSE;
HWAVEIN g_hWaveIn = NULL;
WAVEHDR g_aWaveHdr[ 2 ];
UINT8 *g_apnWaveBuf[ 2 ] = { NULL, };
WAVEFORMATEX g_wfex;

/*---------------------------------
* CreateD3D( )
-----------------------------------*/

BOOL CreateD3D( HWND hWnd )
{
	g_pd3d = Direct3DCreate9( D3D_SDK_VERSION );
	if( g_pd3d == NULL ) return FALSE;

	D3DDISPLAYMODE d3ddm;
	HRESULT hRes = g_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	if( FAILED( hRes ) ) return FALSE;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.hDeviceWindow = hWnd;
  
	hRes = g_pd3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice );
	if( FAILED( hRes ) ) return FALSE;

	return TRUE;
}

/*---------------------------------
* DestroyD3D( )
-----------------------------------*/

void DestroyD3D( )
{
	if( g_pd3dDevice != NULL )
	{
		g_pd3dDevice->Release( );
		g_pd3dDevice = NULL;
	}
	if( g_pd3d != NULL )
	{
		g_pd3d->Release( );
		g_pd3d = NULL;
	}
}

/*---------------------------------------------
* OutputWaveInError( ):
---------------------------------------------*/

void OutputWaveInError( MMRESULT mRes )
{
	char sBuf[ 256 ];
	waveInGetErrorTextA( mRes, sBuf, sizeof( sBuf ) );
	printf( "%s", sBuf );
}

/*---------------------------------------------
* AddWaveInBuffer( ):
---------------------------------------------*/

MMRESULT AddWaveInBuffer( int nIdx )
{
	if( g_hWaveIn == NULL ) return S_OK;

	MMRESULT mRes = waveInPrepareHeader( g_hWaveIn, &g_aWaveHdr[ nIdx ], sizeof( WAVEHDR ) );
	if( FAILED( mRes ) ) return mRes;

	mRes = waveInAddBuffer( g_hWaveIn, &g_aWaveHdr[ nIdx ], sizeof( WAVEHDR ) );
	if( FAILED( mRes ) ) return mRes;

	return mRes;
}

/*---------------------------------------------
* RemoveWaveInBuffer( ):
---------------------------------------------*/

MMRESULT RemoveWaveInBuffer( int nIdx )
{
	if( g_hWaveIn == NULL ) return S_OK;

	MMRESULT mRes = waveInUnprepareHeader( g_hWaveIn, &g_aWaveHdr[ nIdx ], sizeof( g_aWaveHdr[ nIdx ] ) );
	if( FAILED( mRes ) ) return mRes;

	return S_OK;
}

/*---------------------------------------------
* CreateWaveIn( ):
---------------------------------------------*/

MMRESULT CreateWaveIn( HWND hWnd )
{
	ZeroMemory( &g_wfex, sizeof( g_wfex ) );
	g_wfex.wFormatTag = WAVE_FORMAT_PCM;
	g_wfex.nChannels = 1;//2;
	g_wfex.wBitsPerSample = 16;
	g_wfex.nSamplesPerSec = 44100;
	g_wfex.nBlockAlign = ( g_wfex.nChannels * g_wfex.wBitsPerSample ) / 8;
	g_wfex.nAvgBytesPerSec = g_wfex.nSamplesPerSec * g_wfex.nChannels * g_wfex.wBitsPerSample / 8;
	MMRESULT mRes = waveInOpen( &g_hWaveIn, WAVE_MAPPER, &g_wfex, ( DWORD_PTR )hWnd, 0, CALLBACK_WINDOW );
	if( mRes != S_OK ) return mRes;

	DWORD nBufSize = g_wfex.nChannels * ( g_wfex.nSamplesPerSec / 50 ) * g_wfex.wBitsPerSample / 8;
	for( int i = 0; i < 2; i++ )
	{
		DWORD nAlign = g_wfex.nBlockAlign;
		g_apnWaveBuf[ i ] = new unsigned char[ nBufSize + nAlign ];

		ZeroMemory( &g_aWaveHdr[ i ], sizeof( g_aWaveHdr[ i ] ) );
		g_aWaveHdr[ i ].dwBufferLength = nBufSize;
		g_aWaveHdr[ i ].lpData = ( LPSTR )( ( ( SIZE_T )g_apnWaveBuf[ i ] + ( nAlign - 1 ) ) & ~( (SIZE_T)nAlign - 1 ) );

		AddWaveInBuffer( i );
	}

	return waveInStart( g_hWaveIn );
}

/*---------------------------------------------
* DestroyWaveIn( ):
---------------------------------------------*/

MMRESULT DestroyWaveIn( )
{
	for( int i = 0; i < 2; i++ )
	{
		if( g_apnWaveBuf[ i ] != NULL )
		{
			while( !( ( ( volatile WAVEHDR & )( g_aWaveHdr[ i ] ) ).dwFlags & WHDR_DONE ) )
			{
				Sleep( 10 );
			}

			MMRESULT mRes = RemoveWaveInBuffer( i );
			if( FAILED( mRes ) ) OutputWaveInError( mRes );

			delete g_apnWaveBuf[ i ];
			g_apnWaveBuf[ i ] = NULL;
		}
	}

	if( g_hWaveIn != NULL )
	{
		MMRESULT mRes = waveInClose( g_hWaveIn );
		if( FAILED( mRes ) ) return mRes;

		g_hWaveIn = NULL;
	}

	return S_OK;
}

/*---------------------------------
* TxWndProc( )
-----------------------------------*/

LRESULT CALLBACK TxWndProc( HWND hWnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam )
{
	switch( nMsg )
	{
	case WM_CREATE:
		{
			MMRESULT mRes = CreateWaveIn( hWnd );
			if( FAILED( mRes ) )
			{
				OutputWaveInError( mRes );
				DestroyWaveIn( );
			}
			else if( !CreateD3D( hWnd ) )
			{
				printf( "couldn't create device\n" );
				DestroyWindow( hWnd );
			}
			else
			{
				g_vis.Create( g_pd3dDevice );
				g_vis.Start( 2, 44100, 16 );
				g_bStarted = TRUE;
				SetTimer( hWnd, 0x1234, 10, NULL );
			}
		}
		return FALSE;
	case MM_WIM_OPEN:
		printf( "wave in opened\n" );
		return FALSE;
	case MM_WIM_CLOSE:
		printf( "wave in closed\n" );
		return FALSE;
	case MM_WIM_DATA:
		{
			int nIdx = ( int )( ( WAVEHDR* )lParam - g_aWaveHdr );
			RemoveWaveInBuffer( nIdx );

			g_vis.AudioData( ( short* )g_aWaveHdr[ nIdx ].lpData, g_aWaveHdr[ nIdx ].dwBytesRecorded, NULL, 0 );

			AddWaveInBuffer( nIdx );
		}
		return FALSE;
	case WM_TIMER:
		if( wParam == 0x1234 )
		{
			g_vis.Render( );
		}
		break;
	case WM_DESTROY:
		{
			MMRESULT mRes = DestroyWaveIn( );
			if( FAILED( mRes ) ) OutputWaveInError( mRes );
		}

		KillTimer(hWnd, 1);

		if( g_bStarted )
		{
			g_vis.Stop( );
			g_bStarted = FALSE;
		}

		DestroyD3D( );
		return FALSE;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
//		case VK_F2:
//			txs.flip(TXS_VISIBLE_BEATS);
//			break;
//		case VK_LEFT:
//			if(nEffect > 1) txs[TXS_EFFECT_LEFT] = true;
//			break;
//		case VK_RIGHT:
//			if(nEffect + 1 < vpEffect.size()) txs[TXS_EFFECT_RIGHT] = true;
//			break;
//		case 'R':
//			txs[TXS_RECONFIGURE] = true;
//			break;
//		case 'E':
//			txs[TXS_CHANGE_EFFECT] = true;
//			break;
//		case 'H':
//			txs.flip(TXS_HOLD);
//			ShowStatusMsg("Effect %s", txs[TXS_HOLD]? "held" : "not held");
//			break;
		}
		return FALSE;
	}

	return DefWindowProc( hWnd, nMsg, wParam, lParam );
}

/*---------------------------------
* WinMain( )
-----------------------------------*/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int nCmd )
{
	if( !AllocConsole( ) )
	{
		MessageBoxA( NULL, "Couldn't create console", NULL, MB_OK );
		return 1;
	}

//	HANDLE hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
//	int hCrt = _open_osfhandle( ( intptr_t )hStdOut, _O_TEXT );
//	*stdout = *_fdopen( hCrt, "w" );
//	setvbuf( stdout, NULL, _IONBF, 0 );
/*
	FILE * file = fopen(sCmdLine, "rb" );//"c:\\projects\\Tripex\\Harness\\..\\Dll\\Debug\\vis-tx3.dll", "rb" );
	if( file == NULL )
	{
		printf( "no\n" );
	}
	else
	{
		printf( "yes\n" );
	}
*/

//	const char *sTempName = 
//	SetConsoleTitle( "afdsjkfnasdjnerlkerConsole" );
//	Sleep( 40 );
//	HWND hWndConsole = FindWindow( NULL, "Tripex Console" );
//	if( hWndConsole != NULL ) SetForegroundWindow( hWndConsole );

//	sCmdLine = "c:\\projects\\tripex\\dll\\release\\vis-tx3.dll";


//#define EXPORT_CRT_FN_NAMED( x, y ) new Exp2Dll( "PartialCRT.dll", #x, ( long )y )
//#define EXPORT_CRT_FN( x ) new Exp2Dll( "PartialCRT.dll", #x, ( long )&x )
/*
	typedef void (__cdecl *FnGetModule )( struct VISUALISATION* );
	FnGetModule GetModule;
#ifdef USE_XBOX_DLL_LOADER
	if( !pDllLoader->ResolveExport( "get_module", ( void** )&GetModule ) )
#else
	GetModule = ( FnGetModule )GetProcAddress( hDll, "get_module" );
	if( GetModule == NULL )
#endif
	{
		printf( "get_module( ) not found in %s\n", sCmdLine );
		_getch( );
		return 1;
	}

	GetModule( &g_vis );
	*/
	void get_module(VISUALISATION * vis);
	get_module(&g_vis);

	WNDCLASSA wc;
	ZeroMemory( &wc, sizeof( wc ) );
	wc.hCursor = LoadCursor( 0, IDC_ARROW );
	wc.lpszClassName = "TripexWndClass";
	wc.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = ( WNDPROC )TxWndProc;
	RegisterClassA( &wc );

	HWND hWnd = CreateWindowA( wc.lpszClassName, "Tripex", WS_OVERLAPPEDWINDOW, 50, 50, 700, 500, NULL, NULL, hInstance, NULL );
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	MSG msg;
	while( IsWindow( hWnd ) )
	{
		DWORD dwRes = GetMessage( &msg, NULL, 0, 0 );
		if( dwRes == 0 || dwRes == -1 ) break;

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#ifdef USE_XBOX_DLL_LOADER
//	delete pDllLoader;
#endif
	return 0;
}
