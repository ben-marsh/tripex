//#include <windows.h>
//#include <d3dx8.h>
#include <H:\Microsoft Xbox SDK\xbox\include\xtl.h>

namespace XBoxDX8
{
	/*---------------------------------
	* D3DXCreateTextureFromFileInMemory( ):
	-----------------------------------*/

	extern "C" __declspec(dllexport) HRESULT WINAPI D3DXCreateTextureFromFileInMemory( LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcData, LPDIRECT3DTEXTURE8* ppTexture )
	{
		return ::D3DXCreateTextureFromFileInMemory( pDevice, pSrcData, SrcData, ppTexture );
	}

	/*---------------------------------
	* D3DXCreateTexture( ):
	-----------------------------------*/

	extern "C" __declspec(dllexport) HRESULT WINAPI D3DXCreateTexture( LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture )
	{
		return ::D3DXCreateTexture( pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture );
	}
};
