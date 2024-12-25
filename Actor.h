#pragma once

#include "ZDirect3D.h"
#include "Camera.h"
#include "Vector3.h"
#include "Face.h"
#include "Edge.h"
#include "ZArray.h"
#include "ColorRgb.h"

#define WORD_INVALID_INDEX ((uint16)0x0ffff)

class Actor
{
public:
	static const int MAX_VERTICES = 32768;
	static const float OBJECT_NORMAL_LENGTH;

	static const int FRAME_TTL = 1000;

	static const uint16 CLIP_PLANES = 6;

	static const uint16 CLIP_PLANE_MIN_X = 1 << 0;
	static const uint16 CLIP_PLANE_MAX_X = 1 << 1;
	static const uint16 CLIP_PLANE_MIN_Y = 1 << 2;
	static const uint16 CLIP_PLANE_MAX_Y = 1 << 3;
	static const uint16 CLIP_PLANE_MIN_Z = 1 << 4;
	static const uint16 CLIP_FLAG_MAX_Z = 1 << 5;

	struct ExposureData
	{
		Matrix44 transform;
		float time;
		int32 frame;
		float pos;
	};

	struct Frame
	{
		DWORD time_to_live;
		ZArray<Vector3> positions;
		ZArray<float> distances;
		float elapsed;
		float pitch, yaw, roll;
	};

	enum class TextureType
	{
		Unused,
		Normal,
		Lightmap,
		Envmap,
		Sprite
	};

	class TextureEntry
	{
		friend class Actor;
	protected:
		Matrix44 rotation;

	public:
		float pitch, yaw, roll;
		Texture* texture;
		TextureType type;

		TextureEntry();
		void Set(TextureType type, Texture* texture);
	};

	enum class LightType
	{
		Directional,
		Point
	};

	class Light
	{
		friend class Actor;
	protected:
		Matrix44 rotation;

	public:
		LightType type;
		bool attenuate;
		float attenuation_factor;
		Vector3 position, direction;
		WideColorRgb color;

		Light(LightType type, WideColorRgb color = (WideColorRgb)ColorRgb::White());
	};

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

	std::bitset< F_LAST > flags;

	WideColorRgb ambient_light_color;
	WideColorRgb exposure_light_delta;
	ZArray<Frame*> frames;
	ZArray<Frame*> unused_frames;

	float clip_min_x;
	float clip_max_x;

	float clip_min_y;
	float clip_max_y;

	float clip_min_z;
	float clip_max_z;

	uint16 clip_mask;

	int exposure;
	float delay_history, frame_history, rotation_history, frame_time;
	float sprite_size;
	float reflectivity;
	float sprite_history_length;
	int max_history_length;

	float pitch, yaw, roll;
	Vector3 position;

	TextureEntry textures[MAX_TEXTURES];
	std::vector<Light> lights;

	ZArray<Vertex> vertices;
	ZArray<VertexTL> transformed_vertices;

	ZArray<Face> faces;
	ZArray<Face> clipped_faces;

	ZArray<Edge> edges;
	ZArray<Edge> clipped_edges;

	ZArray<uint16*> vertex_face_list;
	ZArray<float> vertex_delay_factor;

	Actor();
	~Actor();

	void FindFaceOrder(const Vector3& vIntPoint);
	void FindVertexNormals();
	void FindMeshEdges();
	void FindVertexFaceList();
	void FindDelayValues();

	void TransformVertices(Matrix44& m);
	Vector3 GetCentre();

	// pipeline functions
	void Calculate(const Renderer& renderer, Camera* pCamera, float fElapsed = 1.0f);
	Error* Render(Renderer& renderer);

	// stock objects
	void CreateCube(float fSize);
	void CreateGeosphere(float fRadius, int nVertices);
	void CreateTetrahedronGeosphere(float fRadius, int nIterations);
	void CreateTorus(float fRadius, float fTubeRadius, int nPoints, int nTubePoints);
	void CreateTetrahedron(float fRadius);

protected:
	static const Point<float> sprite_tex_coords[4];

	struct VertexInfo
	{
		uint16 clip_planes;
		uint16 index;
	};

	ZArray<Vector3> face_normals; // buffer for calculating vertex normals
	int num_textures;

	Vector3 GetDelayedPosition(int nVertex, ExposureData* pData);

	void Clip(const Renderer& renderer, ZArray<Face>& faces, WORD plane_mask);
	bool IsClipRequired(const Face& face, uint16 plane_mask, const ZArray<VertexInfo>& vertex_info) const;
	uint16 GetRequiredClipPlanes(const Vector3& v) const;
	void AddClippedFace(Face& f, WORD wPlaneMask, ZArray<Face>&, const ZArray<VertexInfo>& vertex_info, ZArray<Face>& pfOut, ZArray<int>& unused_faces);
	int ClipEdge(const Face& face, int nIn, int nOut, WORD wPlaneFlag, WORD wClipRequired, ZArray<VertexInfo>& vertex_info, ZArray<VertexTL>& pvOut, ZArray<int>& unused_vertices);
};
