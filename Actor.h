#pragma once

#include "ZDirect3D.h"
#include "Camera.h"
#include "Vector3.h"
#include "Face.h"
#include "Edge.h"
#include "ZArray.h"
#include "ColorRgb.h"

#define WORD_INVALID_INDEX ((uint16)0x0ffff)
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

class Actor
{
public:
	struct ExposureData
	{
		Matrix44 mTransform;
		float m_fTime;
		int32 m_nFrame;
		float m_fPos;
	};

	struct Frame
	{
		DWORD m_nTimeToLive;
		ZArray<Vector3> m_pvPosition;
		ZArray<float> m_pfDistance;
		float m_fElapsed;
		float m_fPitch, m_fYaw, m_fRoll;
	};

	class TextureEntry
	{
		friend class Actor;
	protected:
		Matrix44 m_mRotation;

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
		Texture *m_pTexture;
		Type m_nType;

		// Constructor:
		TextureEntry( );

		// Set( ):
		void Set( Type nType, Texture *pTexture );
	};

	class Light
	{
		friend class Actor;
	protected:
		Matrix44 m_mRotation;

	public:
		enum Type
		{
			T_DIRECTIONAL,
			T_POINT,
		};

		Type m_nType;
		bool m_bAttenuate;
		float m_fAttenuate;
		Vector3 m_vPosition, m_vDirection;
		WideColorRgb m_cColour;

		// Constructor:
		Light( Type nType, WideColorRgb cColour = ( WideColorRgb )ColorRgb::White( ) );
	};

protected:
	ZArray<Vector3> pvFaceNormal; // buffer for calculating vertex normals
	int nTextures;

	Vector3 GetDelayedPosition(int nVertex, ExposureData *pData);

	void Project(ZArray<ZVertexTL> &pVertex, Camera *pCamera);

	WORD GetClipFlag(WORD wClipMask, Vector3 &v);
	void AddClippedFace(Face &f, WORD wPlaneMask, ZArray<Face> &);
	int GetClippedIndex(Face *pf, int nIn, int nOut, WORD wPlaneFlag, WORD wClipRequired);
	void Clip(ZArray<Face> &pfSrc, ZArray<Face> &pfDst, WORD wClipMask);

public:
	WideColorRgb wcAmbientLight;
	WideColorRgb wcExposureLightChange;
	ZArray<Frame*> ppFrame;
	ZArray<Frame*> ppUnusedFrame;

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
		
		F_LAST,
	};

	std::bitset< F_LAST > m_bsFlag;

	uint16 wClipMask;

	std::map< D3DRENDERSTATETYPE, DWORD > mpState;

	int nExposure;
	float fDelayHistory, fFrameHistory, fRotationHistory, fFrameTime;
	float fSpriteSize;
	float fReflectivity;
	float fSpriteHistoryLength;
	int nMaxHistoryLength;

	float fPitch, fYaw, fRoll;
	Vector3 vPosition;

	Actor();
	~Actor();

	TextureEntry pTexture[MAX_TEXTURES];
	ZArray<Light*> ppLight;
	ZArray<ZVertex> pVertex;
	ZArray<ZVertexTL> pTransVertex;
//	ZArray<ZVertexTL> pClippedVertex;
	ZArray<Face> pFace, pClippedFace;//pClippedFace;
	ZArray<Edge> pEdge, pClippedEdge;//, peClipped;//Edge;
//	ZArray<int> pnClipped;

	ZArray<uint16*> ppwVertexFaceList;
	ZArray<float> pfDelay;

	void FindFaceOrder(const Vector3 &vIntPoint);
	void FindVertexNormals();
	void FindMeshEdges();
	void FindVertexFaceList();
	void FindDelayValues();

	void TransformVertices(Matrix44 &m);
	Vector3 GetCentre();

	// pipeline functions
	void Calculate(Camera *pCamera, float fElapsed = 1.0f);
	Error* Render( );

	// stock objects
	void CreateCube(float fSize);
	void CreateGeosphere(float fRadius, int nVertices);
	void CreateTetrahedronGeosphere(float fRadius, int nIterations);
	void CreateTorus(float fRadius, float fTubeRadius, int nPoints, int nTubePoints);
	void CreateTetrahedron(float fRadius);
};
