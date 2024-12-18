#pragma once

//#include "ZFlexibleVertexArray.h"
#include "ZDirect3D.h"
#include "ZCamera.h"
#include "ZVector.h"
#include "ZFace.h"
#include "ZEdge.h"
#include "ZArray.h"
#include "ZColour.h"

#define WORD_INVALID_INDEX ((WORD)0x0ffff)
#define FRAME_TTL 1000

#define CLIP_FLAG(x) (1L << x)
#define CLIP_FLAG_MIN_X CLIP_FLAG(0)
#define CLIP_FLAG_MAX_X CLIP_FLAG(1)
#define CLIP_FLAG_MIN_Y CLIP_FLAG(2)
#define CLIP_FLAG_MAX_Y CLIP_FLAG(3)
#define CLIP_FLAG_MIN_Z CLIP_FLAG(4)
#define CLIP_FLAG_MAX_Z CLIP_FLAG(5)

const int nFrameTTL = 20;
const int nMaxVertices = 32768;
const float fObjectNormal = 0.5f;
//extern float fClipMinZ;
//extern float fClipMaxZ;

class ZExposureData
{
public:
	ZMatrix mTransform;
	FLOAT32 m_fTime;
	SINT32 m_nFrame;
	FLOAT32 m_fPos;
};

class ZObject
{
public:
	class Frame
	{
	public:
		DWORD m_nTimeToLive;
		ZArray<ZVector> m_pvPosition;
		ZArray<float> m_pfDistance;
		float m_fElapsed;
		float m_fPitch, m_fYaw, m_fRoll;
	};

	class Texture
	{
		friend class ZObject;
	protected:
		ZMatrix m_mRotation;

	public:
		enum Type
		{
			T_UNUSED,
			T_USER,
			T_LIGHTMAP,
			T_ENVMAP,
			T_SPRITE,
		};

		float m_fPitch, m_fYaw, m_fRoll;
		ZTexture *m_pTexture;
		Type m_nType;

		// Constructor:
		Texture( );

		// Set( ):
		void Set( Type nType, ZTexture *pTexture );
	};

	class Light
	{
		friend class ZObject;
	protected:
		ZMatrix m_mRotation;

	public:
		enum Type
		{
			T_DIRECTIONAL,
			T_POINT,
		};

		Type m_nType;
		bool m_bAttenuate;
		float m_fAttenuate;
		ZVector m_vPosition, m_vDirection;
		ZWideColour m_cColour;

		// Constructor:
		Light( Type nType, ZWideColour cColour = ( ZWideColour )ZColour::White( ) );
	};

protected:
//	ZArray<WORD> pwClipIndex;
//	ZArray<WORD> pwClippedIndex;
//	ZFlexibleVertexArray pClipVertex;
//	ZArray<ZWideColour> pwcVertex;
//	ZArray<ZVector> pvPositionBuffer;
	ZArray<ZVector> pvFaceNormal; // buffer for calculating vertex normals
	int nTextures;

//	inline void AddTransSprite(ZCamera *pCamera, ZVector &vPos, ZColour cDiffuse, ZColour cSpecular);
//	inline void AddTransVertex(ZCamera *pCamera, ZVector &vPos, ZColour cDiffuse, ZColour cSpecular, ZTexturePosition *pTex);


//	void AddVertexPosition(BYTE *&pbData, ZCamera *pCamera, ZVector &vPos);
//	void AddVertexColour(BYTE *&pbData, ZWideColour &wcLight);
//	void AddVertexColour(BYTE *&pbData, ZFlexibleVertex &pVertex, ZWideColour &wcLight);
//	void AddVertexTextures(BYTE *&pbData, ZTexturePosition *pTex);

//	inline void AddVertexData(ZVector &vPosition, float fRHW, ZWideColour &wcAmbientLight, int nVertexIndex, ZTexturePosition *pTexturePosition);
//	inline void AddVertexData(ZVector &vPosition, ZWideColour &wcAmbientLight, int nVertexIndex, ZTexturePosition *pTexturePosition);
//	inline void AddVertexColourTexture(BYTE *&pbData, ZWideColour &wcAmbientLight, int nVertexIndex, ZTexturePosition *pTexturePosition);
//	inline void AddClippedVertex(ZCamera &cCamera, int nVertexIn, int nVertexOut, ZArray<ZVector> &arvPosition, ZWideColour &cAmbientLight); 
//	void AddVertexSprites(ZArray<ZVector> &pvPosition, ZWideColour cAmbientLight);
//	void AddFacesEdges(ZCamera &cCamera, ZArray<ZVector> &pvPosition, ZWideColour cAmbientLight);

	inline void DoScreenTransformTemplate(ZCamera &camera, int nIdenticalZ, bool bPerspective, bool bScreenTransform);
	inline void DoScreenTransform(ZCamera &camera, int nIdenticalZ);

	DWORD GetDepthFlag(DWORD dwDepth);

	void AddClippedVertex(ZCamera *pCamera, WORD wOut, WORD wIn);
	void AddVertex(ZCamera *pCamera, ZVector &vPos, ZColour cDiffuse, ZColour cSpecular, ZPoint<float> *pTex, int nClipped, bool &bClipped);
	void AddSprite(ZCamera *pCamera, ZVector &vPos, ZColour cDiffuse, ZColour cSpecular, ZVector *pvSprite);
	void DoLighting();
	void DoTextures(ZCamera &camera);

	ZVector GetDelayedPosition(int nVertex, class ZExposureData *pData);

	void Project(ZArray<ZVertexTL> &pVertex, ZCamera *pCamera);

	WORD GetClipFlag(WORD wClipMask, ZVector &v);
	void AddClippedFace(ZFace &f, WORD wPlaneMask, ZArray<ZFace> &);
	int GetClippedIndex(ZFace *pf, int nIn, int nOut, WORD wPlaneFlag, WORD wClipRequired);
	void Clip(ZArray<ZFace> &pfSrc, ZArray<ZFace> &pfDst, WORD wClipMask);

public:
	ZWideColour wcAmbientLight;
	ZWideColour wcExposureLightChange;
//	WORD wVertices;
//	ZFlexibleVertexArray pTransVertex;
//	ZArray<BYTE> pbVertexData;
//	DWORD dwVertexFormat, dwItemSize;
//	ZArray<ZFace> pClippedFace;
//	ZArray<ZEdge> pClippedEdge;
	ZArray<Frame*> ppFrame;
	ZArray<Frame*> ppUnusedFrame;
//	Frame *pCurrentFrame;

	float m_fClipMinX;
	float m_fClipMaxX;
	
	float m_fClipMinY;
	float m_fClipMaxY; 
	
	float m_fClipMinZ;
	float m_fClipMaxZ;

	enum
	{
		F_VALID_CLIP_PLANES,
		F_NO_CULL,
		F_NO_TRANSFORM,
		
		F_USE_DIFFUSE,
		F_USE_SPECULAR,

		F_DO_POSITION_DELAY,
		F_DO_FRAME_HISTORY,
		F_DO_ROTATION_HISTORY,
		F_DO_MIX_EXPOSURE_FACES,

		F_DRAW_Z_BUFFER,
		F_DRAW_TRANSPARENT,
		F_DRAW_VERTEX_SPRITES,
		F_DRAW_VERTEX_SPRITE_HISTORY,

		F_VALID_TRANS_FORMAT,
		F_VALID_VERTEX_FACE_LIST,
		F_VALID_TRANSFORMED_DATA,
		F_VALID_VERTEX_NORMALS,
		F_VALID_VERTEX_DIFFUSE,
		F_VALID_VERTEX_SPECULAR,



/*		ValidClipPlanes = (1 << 27L),
			// 
//		NoClip = (1 << 1L),
		NoCull = (1 << 3L),
		NoTransform = (1 << 1L),

		UseDiffuse = (1 << 21L),
		UseSpecular = (1 << 22L),

		DoPositionDelay = (1 << 5L),
//		DoRotationDelay = (1 << 6L),
//		DoBuildVertexData = (1 << 7L),
		DoFrameHistory = (1 << 24L),
		DoRotationHistory = (1 << 6L),
		DoMixExposureFaces = (1 << 25L),

		DrawZBuffer = (1 << 9L),
		DrawTransparent = (1 << 10L),
		DrawVertexSprites = (1 << 11L),
		DrawVertexSpriteHistory = (1 << 12L),

		ValidTransFormat = (1 << 4L),
		ValidVertexFaceList = (1 << 14L),
		ValidTransformedData = (1 << 15L),
		ValidVertexNormals = (1 << 17L),
		ValidVertexDiffuse = (1 << 18L),
		ValidVertexSpecular = (1 << 23L),

*/
		
		F_LAST,
	};

	std::bitset< F_LAST > m_bsFlag;
/*		
	enum
	{
		ValidClipPlanes = (1 << 27L),
		// 
//		NoClip = (1 << 1L),
		NoCull = (1 << 3L),
		NoTransform = (1 << 1L),

		UseDiffuse = (1 << 21L),
		UseSpecular = (1 << 22L),

		DoPositionDelay = (1 << 5L),
//		DoRotationDelay = (1 << 6L),
//		DoBuildVertexData = (1 << 7L),
		DoFrameHistory = (1 << 24L),
		DoRotationHistory = (1 << 6L),
		DoMixExposureFaces = (1 << 25L),

		DrawZBuffer = (1 << 9L),
		DrawTransparent = (1 << 10L),
		DrawVertexSprites = (1 << 11L),
		DrawVertexSpriteHistory = (1 << 12L),

		ValidTransFormat = (1 << 4L),
		ValidVertexFaceList = (1 << 14L),
		ValidTransformedData = (1 << 15L),
		ValidVertexNormals = (1 << 17L),
		ValidVertexDiffuse = (1 << 18L),
		ValidVertexSpecular = (1 << 23L),


		// other combinations
		// number of vectors in the vertex position buffer changes during pipeline 
		// (so can't use same index into pVertex)
	};
*/
	WORD wClipMask;

	std::map< D3DRENDERSTATETYPE, DWORD > mpState;

	int nExposure;
	float fDelayHistory, fFrameHistory, fRotationHistory, fFrameTime;
	float fSpriteSize;
	float fReflectivity;
	float fSpriteHistoryLength;
	int nMaxHistoryLength;

	float fPitch, fYaw, fRoll;
	ZVector vPosition;

	ZObject();
	~ZObject();

	Texture pTexture[MAX_TEXTURES];
	ZArray<Light*> ppLight;
	ZArray<ZVertex> pVertex;
	ZArray<ZVertexTL> pTransVertex;
//	ZArray<ZVertexTL> pClippedVertex;
	ZArray<ZFace> pFace, pClippedFace;//pClippedFace;
	ZArray<ZEdge> pEdge, pClippedEdge;//, peClipped;//Edge;
//	ZArray<int> pnClipped;

	ZArray<WORD*> ppwVertexFaceList;
	ZArray<float> pfDelay;

	void FindFaceOrder(const ZVector &vIntPoint);
	void FindVertexNormals();
	void FindMeshEdges();
	void FindVertexFaceList();
	void FindDelayValues();

	void TransformVertices(ZMatrix &m);
	ZVector GetCentre();

	// pipeline functions
	void Calculate(ZCamera *pCamera, float fElapsed = 1.0f);
	HRESULT Render( );

	// stock objects
	void CreateCube(float fSize);
	void CreateGeosphere(float fRadius, int nVertices);
	void CreateTetrahedronGeosphere(float fRadius, int nIterations);
	void CreateTorus(float fRadius, float fTubeRadius, int nPoints, int nTubePoints);
	void CreateTetrahedron(float fRadius);
};
