#pragma once

#include <d3d9.h>
#include "ZArray.h"
#include "ColorRgb.h"
#include "Vector3.h"
#include "Face.h"
#include "Edge.h"
#include "Point.h"
#include "Rect.h"
#include <map>
#include <set>
#include "error.h"
#include "Vertex.h"

class Texture;
class ZPalette;

enum class TextureClass
{
	Invalid = -1,
	Gui = 0,

	// bezier cube
	BezierCubeSprite = 1,
	BezierCubeBackground = 2,

	// collapsing sphere
	CollapsingSphereSprite = 3,
	CollapsingSphereBackground = 4,

	// distortion
	DistortionBackground = 5,

	// distortion 2
	Distortion2Background = 6,

	// dot star
	DotStarSprite = 7,
	DotStarBackground = 8,

	// light ring
	LightRingSprite = 9,
	LightRingBackground = 10,

	// light sphere
	LightSphereSprite = 11,
	LightSphereBackground = 12,

	// light star
	LightStarSprite = 13,
	LightStarBackground = 14,

	// light tentacles
	LightTentaclesSprite = 15,
	LightTentaclesBackground = 16,

	// metaballs
	MetaballsEnvMap = 17,

	// morphing sphere
	MorphingSphereEnvMap = 18,

	// motion blur
	MotionBlurEnvMap = 19,

	// motion blur 2
	MotionBlur2EnvMap = 20,

	// phased
	PhasedSprite = 21,

	// rings
	RingsEnvMap = 22,

	// bumpmapping
	BumpMapBackground = 23,
	BumpMapTentaclesEnvMap = 24,

	// tube
	TubeEnvMap = 25,

	// tunnel
	TunnelBackground = 26,

	// water globe
	WaterGlobeEnvMap = 27,

	// spectrum
	AnalyzerEnvMap = 28,

	// motion blur 3
	MotionBlur3EnvMap = 29,

	// motion blur 3 alt
	MotionBlur3AltEnvMap = 30,

	// distortion 2 col
	Distortion2ColBackground = 31,
};

class ZDirect3D
{
public:
	typedef std::map< DWORD, std::pair< DWORD, DWORD > > StateBuffer;
	typedef std::map< DWORD, DWORD > NewStateBuffer;
	typedef std::pair< StateBuffer::iterator, bool > StateBufferChange;

	class TextureStage
	{
		friend ZDirect3D;

	protected:
		StateBuffer mpState;
		NewStateBuffer mpNewState;
		StateBuffer mpSamplerState;
		NewStateBuffer mpNewSamplerState;
		Texture *pTexture;
		Texture *pNewTexture;

	public:
		TextureStage();
	};

	enum
	{
		Transparent = (1 << 0L),
		Multiply = (1 << 1L),
		Perspective = (1 << 2L),
		Shade = (1 << 3L),
		ZBuffer = (1 << 4L),
		PerspectiveCorrect = (1 << 5L),
		LuminanceOpacity = (1 << 6L),
		InverseMultiply = (1 << 7L),
	};

	IDirect3DDevice9 *g_pDevice;
	D3DCAPS9 g_Caps;
	unsigned int nWidth, nHeight;
	StateBuffer mpRender;
	NewStateBuffer mpNewRender;
	TextureStage pTextureStage[8];
	std::set< std::pair< uint16, uint16 > > g_spnFreePalette;
	std::set< Texture* > g_spTexture;

	ZDirect3D( );

	Error* Open( );
	Error* Close( );

	Error* DrawIndexedPrimitive(ZArray<VertexTL>& vertices, ZArray<Face>& faces);
	Error* DrawIndexedPrimitive(const std::vector<VertexTL>& vertices, const std::vector<Face>& faces);
	Error* DrawIndexedPrimitive(uint32_t num_vertices, const VertexTL* vertices, uint32_t num_faces, const Face* faces);

	Error* AddTexture( Texture *pTexture );
	Error* UploadTexture( Texture *pTexture );
	void GetStateChanges(NewStateBuffer &mp_new, StateBuffer &mp_current, std::vector< StateBufferChange > &vc);

	Error* CreateTexture(Texture *pTexture);
	void DestroyTexture(Texture *pTexture);

	Texture *Find(TextureClass nType);

	int GetWidth( );
	int GetHeight( );

	// render state
	void SetState(int nFlags);
	void ResetRenderState();
	void SetRenderState(DWORD dwKey, DWORD dwValue);
	Error* FlushRenderState();

	// texture state
	void ResetTextureState();
	void ResetTextureStageState(DWORD dwStage);
	void SetTexture(DWORD dwStage, Texture *pTexture, DWORD dwOp = D3DTOP_MODULATE, DWORD dwArg2 = D3DTA_CURRENT);
	void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwKey, DWORD dwValue);
	void SetSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE dwState, DWORD dwValue);
	Error* FlushTextureState();

	Error* DrawSprite(const Point<int> &p, const Rect<int> &spr, ColorRgb cDiffuse = ColorRgb::White(), ColorRgb cSpecular = ColorRgb::Black());
};

extern ZDirect3D *g_pD3D;
