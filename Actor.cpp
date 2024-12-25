#include "Platform.h"
#include "Actor.h"
#include "error.h"

const Point<float> Actor::sprite_tex_coords[4] =
{
	Point<float>(0.0f, 0.0f),
	Point<float>(1.0f, 0.0f),
	Point<float>(1.0f, 1.0f),
	Point<float>(0.0f, 1.0f)
};

const float Actor::OBJECT_NORMAL_LENGTH = 0.5f;

////// Actor::TextureEntry //////

Actor::TextureEntry::TextureEntry()
{
	pitch = yaw = roll = 0.0f;
	texture = NULL;
	type = TextureType::Unused;
}

void Actor::TextureEntry::Set(TextureType type, Texture* texture)
{
	this->type = type;
	this->texture = texture;
}

////// Actor::Light //////

Actor::Light::Light(LightType nType, WideColorRgb cColour)
{
	type = nType;
	attenuate = false;
	attenuation_factor = 0.0f;
	color = cColour;
}

////// Actor //////

Actor::Actor()
{
	clip_mask = CLIP_PLANE_MIN_X | CLIP_PLANE_MAX_X | CLIP_PLANE_MIN_Y | CLIP_PLANE_MAX_Y | CLIP_PLANE_MIN_Z | CLIP_FLAG_MAX_Z;
	ambient_light_color = ColorRgb::White();
	frame_history = 1.0f;
	frame_time = 0.3f;
	roll = pitch = yaw = 0.0f;
	position = Vector3(0, 0, 0);
	exposure = 1;
	transformed_vertices.nStep = 1000;
	delay_history = 0;
	//	pClippedFace.nStep = 50;
	reflectivity = 1.0;
	sprite_size = 7.5f;
	sprite_history_length = 0.5;
	exposure_light_delta = ColorRgb(0, 0, 0);
	max_history_length = 15;
	clip_min_x = 0.0f;
	clip_max_x = 0.0f;
	clip_min_y = 0.0f;
	clip_max_y = 0.0f;
	clip_min_z = 0.0f;
	clip_max_z = 0.0f;
	num_textures = 0;
	rotation_history = 0;
}

Actor::~Actor()
{
	for (int i = 0; i < vertex_face_list.GetLength(); i++)
	{
		free(vertex_face_list[i]);
	}
}

Vector3 Actor::GetCentre()
{
	Vector3 v(0, 0, 0);
	for (int i = 0; i < vertices.GetLength(); i++)
	{
		v += vertices[i].position;
	}
	v /= (float)vertices.GetLength();
	return v;
}

void Actor::TransformVertices(Matrix44& m)
{
	for (int i = 0; i < vertices.GetLength(); i++)
	{
		vertices[i].position *= m;
	}
}

void Actor::FindFaceOrder(const Vector3& vIntPoint)
{
	for (int i = 0; i < faces.GetLength(); i++)
	{
		Face face = faces[i];
		Vector3 normal = (vertices[face[1]].position - vertices[face[0]].position).Cross(vertices[face[2]].position - vertices[face[0]].position);
		if ((vertices[faces[i][0]].position - vIntPoint).Dot(normal) > 0)
		{
			faces[i].SwapCull();
		}
	}
}

void Actor::FindVertexFaceList()
{
	for (int i = 0; i < vertex_face_list.GetLength(); i++)
	{
		free(vertex_face_list[i]);
	}
	vertex_face_list.SetLength(vertices.GetLength());
	for (int i = 0; i < vertices.GetLength(); i++)
	{
		vertex_face_list[i] = NULL;
	}
	for (int i = 0; i < faces.GetLength(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			uint16*& face_list = vertex_face_list[faces[i][j]];

			int k = 0;
			if (face_list == NULL)
			{
				face_list = (uint16*)malloc(4 * sizeof(uint16));
			}
			else
			{
				while (face_list[k] != WORD_INVALID_INDEX)
				{
					k++;
				}
				if (k >= 3)
				{
					face_list = (uint16*)realloc(face_list, (k + 2) * sizeof(uint16));
				}
			}
			face_list[k] = i;
			face_list[k + 1] = WORD_INVALID_INDEX;
		}
	}
	flags.set(F_VALID_VERTEX_FACE_LIST);
}

void Actor::FindVertexNormals()
{
	if (!flags.test(F_VALID_VERTEX_FACE_LIST))
	{
		FindVertexFaceList();
	}

	face_normals.SetLength(faces.GetLength());
	for (int i = 0; i < faces.GetLength(); i++)
	{
		Face face = faces[i];

		Vector3 edge1 = vertices[face[1]].position - vertices[face[0]].position;
		Vector3 edge2 = vertices[face[2]].position - vertices[face[0]].position;

		face_normals[i] = edge1.Cross(edge2).Normal(OBJECT_NORMAL_LENGTH);
	}

	for (int i = 0; i < vertices.GetLength(); i++)
	{
		Vector3 normal = Vector3(0, 0, 0);

		uint16* face_list = vertex_face_list[i];
		for (int j = 0; face_list[j] != WORD_INVALID_INDEX; j++)
		{
			normal += face_normals[face_list[j]];
		}

		vertices[i].normal = normal.Normal(OBJECT_NORMAL_LENGTH);
	}

	flags.set(F_VALID_VERTEX_NORMALS);
}

void Actor::FindMeshEdges()
{
	Edge e;
	for (int i = 0; i < faces.GetLength(); i++)
	{
		e = Edge(faces[i][0], faces[i][1]).OrderIndices();
		if (edges.IndexOf(e) == -1)
		{
			edges.Add(e);
		}

		e = Edge(faces[i][1], faces[i][2]).OrderIndices();
		if (edges.IndexOf(e) == -1)
		{
			edges.Add(e);
		}

		e = Edge(faces[i][2], faces[i][0]).OrderIndices();
		if (edges.IndexOf(e) == -1)
		{
			edges.Add(e);
		}
	}
}

void Actor::FindDelayValues()
{
	vertex_delay_factor.SetLength(vertices.GetLength());

	float dist_max = FLT_MIN;
	float dist_min = FLT_MAX;
	for (int i = 0; i < vertices.GetLength(); i++)
	{
		vertex_delay_factor[i] = vertices[i].position.Length();
		dist_min = std::min(dist_min, vertex_delay_factor[i]);
		dist_max = std::max(dist_max, vertex_delay_factor[i]);
	}

	for (int i = 0; i < vertex_delay_factor.GetLength(); i++)
	{
		vertex_delay_factor[i] = (vertex_delay_factor[i] - dist_min) / (dist_max - dist_min);
	}
}

Vector3 Actor::GetDelayedPosition(int vertex, ExposureData* exposure_data)
{
	float time = exposure_data->time + (vertex_delay_factor[vertex] * delay_history);
	float total_elapsed = 0;
	for (int k = exposure_data->frame; ; k++)
	{
		if (!(k + 1 < frames.GetLength()))
		{
			return frames[k]->positions[vertex];
		}
		float next_elapsed = total_elapsed + frames[k]->elapsed;
		if (next_elapsed > time)
		{
			float pos = (time - total_elapsed) / frames[k]->elapsed;
			return (frames[k]->positions[vertex] * (1 - pos)) + (frames[k + 1]->positions[vertex] * pos);
		}
		total_elapsed = next_elapsed;
	}
}

void Actor::Calculate(const Renderer& renderer, Camera* camera, float elapsed)
{
	int i, j;
	flags.set(F_VALID_TRANSFORMED_DATA, false);

	Matrix44 mTransform = Matrix44::Rotate(yaw, pitch, roll) * Matrix44::Translate(position) * camera->GetTransform();

	// set up the lighting
	int min_light = std::min(ambient_light_color.GetMinComponent(), (ambient_light_color + (exposure * exposure_light_delta)).GetMinComponent());
	int max_light = std::max(ambient_light_color.GetMaxComponent(), (ambient_light_color + (exposure * exposure_light_delta)).GetMaxComponent());
	if (lights.size() > 0)
	{
		if (!flags.test(F_VALID_VERTEX_NORMALS))
		{
			FindVertexNormals();
		}
		for (const Light& light : lights)
		{
			max_light = std::max(max_light, light.color.GetMaxComponent());
		}
	}

	flags.set(F_USE_DIFFUSE, (min_light < 255) || flags.test(F_VALID_VERTEX_DIFFUSE));
	flags.set(F_USE_SPECULAR, (max_light > 255) || flags.test(F_VALID_VERTEX_SPECULAR));

	// set up the texturing
	ZArray<TextureEntry*> use_textures;
	if (flags.test(F_DRAW_VERTEX_SPRITES))
	{
		_ASSERT(textures[0].type == TextureType::Sprite);
		num_textures = 1;
	}
	else
	{
		for (i = 0; i < MAX_TEXTURES; i++)
		{
			switch (textures[i].type)
			{
			case TextureType::Normal:
				use_textures.Add(&textures[i]);
				break;
			case TextureType::Lightmap:
			case TextureType::Envmap:
				if (!flags.test(F_VALID_VERTEX_NORMALS))
				{
					FindVertexNormals();
				}

				if (textures[i].type == TextureType::Lightmap)
				{
					textures[i].rotation = Matrix44::Rotate(textures[i].pitch, textures[i].yaw, textures[i].roll);
				}
				else
				{
					textures[i].rotation = Matrix44::Rotate(yaw, pitch, roll);
				}
				use_textures.Add(&textures[i]);
				break;
			}
		}
		num_textures = use_textures.GetLength();
	}

	// set up the frame array
	Frame* store_frame = NULL;
	for (i = 0; i < unused_frames.GetLength();)
	{
		if (unused_frames[i]->time_to_live > timeGetTime())
		{
			delete unused_frames[i];
			unused_frames.Remove(i);
		}
		else
		{
			i++;
		}
	}

	if (flags.test(F_DRAW_VERTEX_SPRITE_HISTORY) ||
		flags.test(F_DO_POSITION_DELAY) ||
		flags.test(F_DO_FRAME_HISTORY) ||
		exposure > 1)
	{
		if (frames.GetLength() == 0 || frames[0]->elapsed > frame_time)
		{
			float total_elapsed = elapsed;
			for (int i = 0;; i++)
			{
				if (total_elapsed > frame_history)
				{
					// remove i + 1 and upwards
					while (frames.GetLength() > i + 1)
					{
						int frame_index = frames.GetLength() - 1;
						frames[frame_index]->time_to_live = timeGetTime() + FRAME_TTL;
						unused_frames.Add(frames[frame_index]);
						frames.Remove(frame_index);
					}
				}
				if (!(i + 1 < frames.GetLength()))
				{
					if (unused_frames.GetLength() > 0)
					{
						frames.Insert(0, unused_frames[0]);
						unused_frames.Remove(0);
					}
					else
					{
						frames.Insert(0, new Frame());
					}
					break;
				}
				total_elapsed += frames[i]->elapsed;
			}
			frames[0]->elapsed = 0;
		}
		frames[0]->elapsed += elapsed;
		if (flags.test(F_DO_ROTATION_HISTORY))
		{
			frames[0]->roll = roll;
			frames[0]->pitch = pitch;
			frames[0]->yaw = yaw;
		}
		else
		{
			store_frame = frames[0];
			store_frame->positions.SetLength(vertices.GetLength());
			if (flags.test(F_DRAW_VERTEX_SPRITE_HISTORY))
			{
				store_frame->distances.SetLength(vertices.GetLength());
			}
		}
	}

	// create 
//	pnClipped.SetLength(nExposure * pVertex.GetLength());
//	pClippedEdge.SetLength(0);
//	pClippedFace.SetLength(0);
	transformed_vertices.SetLength(0);
	//	pClippedVertex.SetLength(0);
	bool clipped = false;

	//	ZTexturePosition pTex[8];
	Vector3 trans_normal;
	ZArray<Vector3> pos;
	float angle;
	Vector3 trans_direction;
	ColorRgb init_diffuse(ambient_light_color);
	ColorRgb init_specular(ambient_light_color - ColorRgb(255, 255, 255));

	ZArray<ExposureData> exp_base;
	ZArray<Matrix44> exposure_matrix;
	for (int nExp = 0; nExp < exposure; nExp++)
	{
		if (nExp == 0)
		{
			ExposureData* pData = exp_base.AddEmptyPtr();
			pData->time = 0;
			pData->frame = 0;
			pData->pos = 0;
		}
		else
		{
			float fTime = nExp * (frame_history - delay_history) / (exposure - 1);
			float fTotalElapsed = 0;
			int k;
			for (k = 0; ; k++)
			{
				if (!(k + 1 < frames.GetLength())) break;
				float fNextElapsed = fTotalElapsed + frames[k]->elapsed;
				if (fNextElapsed > fTime)
				{
					ExposureData* pData = exp_base.AddEmptyPtr();
					pData->time = fTime;
					pData->frame = k;
					pData->pos = (pData->time - fTotalElapsed) / frames[k]->elapsed;
					if (flags.test(F_DO_ROTATION_HISTORY))
					{
						float fThisYaw = (frames[k]->yaw * (1 - pData->pos)) + (frames[k + 1]->yaw * pData->pos);
						float fThisPitch = (frames[k]->pitch * (1 - pData->pos)) + (frames[k + 1]->pitch * pData->pos);
						float fThisRoll = (frames[k]->roll * (1 - pData->pos)) + (frames[k + 1]->roll * pData->pos);
						pData->transform = Matrix44::Rotate(fThisYaw, fThisPitch, fThisRoll) * Matrix44::Translate(position) * camera->GetTransform();
					}
					break;
				}
				fTotalElapsed = fNextElapsed;
			}
			if (!(k + 1 < frames.GetLength())) break;
		}
	}

	if (vertices.GetLength() == 0) return;

	int max_history_length = MAX_VERTICES / (4 * vertices.GetLength());

	// transform
	for (i = 0; i < vertices.GetLength(); i++)
	{
		VertexTL* pVert = transformed_vertices.AddEmptyPtr();
		if (flags.test(F_NO_TRANSFORM)) pVert->position = vertices[i].position;
		else pVert->position = vertices[i].position * mTransform;

		if (store_frame) store_frame->positions[i] = pVert->position;
	}

	if (exposure == 0) return;

	// light
	if (lights.size() > 0)
	{
		for (i = 0; i < vertices.GetLength(); i++)
		{
			WideColorRgb wcLight = ambient_light_color;
			for (const Light& light : lights)
			{
				switch (light.type)
				{
				case LightType::Directional:
					trans_direction = (light.direction * (Matrix44::Rotate(-pitch, -yaw) * Matrix44::RotateAroundZ(-roll))).Normal();
					angle = 4.0f * vertices[i].normal.Dot(trans_direction);
					if (angle > 0)
					{
						wcLight += light.color * angle;
					}
					break;
				case LightType::Point:
					Vector3 vDirection = light.position - transformed_vertices[i].position;
					angle = vDirection.Dot(vertices[i].normal);
					if (angle > 0)
					{
						float fDistance = vDirection.Length();
						if (light.attenuate)
						{
							angle *= light.attenuation_factor / (fDistance * fDistance);
						}
						else
						{
							angle /= fDistance;
						}
						wcLight += light.color * angle;
					}
					break;
				}
			}
			transformed_vertices[i].diffuse = wcLight;
			transformed_vertices[i].specular = wcLight - ColorRgb(255, 255, 255);
		}
	}
	else
	{
		for (i = 0; i < vertices.GetLength(); i++)
		{
			transformed_vertices[i].diffuse = init_diffuse;
			transformed_vertices[i].specular = init_specular;
		}
	}
	if (flags.test(F_VALID_VERTEX_DIFFUSE))
	{
		for (i = 0; i < vertices.GetLength(); i++)
		{
			transformed_vertices[i].diffuse.r = (uint8)(transformed_vertices[i].diffuse.r * vertices[i].diffuse.r / 255.0f);
			transformed_vertices[i].diffuse.g = (uint8)(transformed_vertices[i].diffuse.g * vertices[i].diffuse.g / 255.0f);
			transformed_vertices[i].diffuse.b = (uint8)(transformed_vertices[i].diffuse.b * vertices[i].diffuse.b / 255.0f);
		}
	}
	if (flags.test(F_VALID_VERTEX_SPECULAR))
	{
		for (i = 0; i < vertices.GetLength(); i++)
		{
			transformed_vertices[i].specular = transformed_vertices[i].specular + vertices[i].specular;
		}
	}

	// texture
	for (j = 0; j < use_textures.GetLength(); j++)
	{
		if (use_textures[j]->type == TextureType::Normal)
		{
			for (i = 0; i < vertices.GetLength(); i++)
			{
				transformed_vertices[i].tex_coords[j] = vertices[i].tex_coord[j];
			}
		}
		else
		{
			for (i = 0; i < vertices.GetLength(); i++)
			{
				trans_normal = vertices[i].normal * use_textures[j]->rotation;
				transformed_vertices[i].tex_coords[j].x = (trans_normal.x * 1.0f) + 0.5f;
				transformed_vertices[i].tex_coords[j].y = (trans_normal.y * 1.0f) + 0.5f;
			}
		}
	}

	if (!flags.test(F_DRAW_VERTEX_SPRITE_HISTORY))
	{
		VertexTL* exposure_vertex = transformed_vertices.GetBuffer(vertices.GetLength() * exp_base.GetLength());
		for (int exp = 0;;)
		{
			if (flags.test(F_DO_POSITION_DELAY))
			{
				for (int i = 0; i < vertices.GetLength(); i++)
				{
					exposure_vertex[i].position = GetDelayedPosition(i, &exp_base[exp]);
				}
			}

			exp++;
			if (!(exp < exp_base.GetLength()))
			{
				break;
			}

			VertexTL* new_exposure_vertex = exposure_vertex + vertices.GetLength();
			for (i = 0; i < vertices.GetLength(); i++)
			{
				new_exposure_vertex[i].diffuse = exposure_vertex[i].diffuse + exposure_light_delta;
				new_exposure_vertex[i].specular = exposure_vertex[i].specular + exposure_light_delta;
				new_exposure_vertex[i].tex_coords[0] = exposure_vertex[i].tex_coords[0];
			}

			if (!flags.test(F_DO_ROTATION_HISTORY))
			{
				for (i = 0; i < vertices.GetLength(); i++)
				{
					new_exposure_vertex[i].position = (frames[exp_base[exp].frame]->positions[i] * (1 - exp_base[exp].pos)) + (frames[exp_base[exp].frame + 1]->positions[i] * exp_base[exp].pos);
				}
			}
			else
			{
				for (i = 0; i < vertices.GetLength(); i++)
				{
					new_exposure_vertex[i].position = vertices[i].position * exp_base[exp].transform;
				}
			}

			exposure_vertex = new_exposure_vertex;
		}
	}
	else if (frames.GetLength() > 1)
	{
		_ASSERT(exposure == 1);
		transformed_vertices.SetLength(0);

		for (int i = 0; i < vertices.GetLength(); i++)
		{
			frames[0]->distances[i] = (frames[1]->positions[i] - frames[0]->positions[i]).Length();

			ColorRgb diffuse = init_diffuse;
			ColorRgb specular = init_specular;

			int num_frames = 0;
			float position = 0;
			int length = 0;
			for (int j = 0; j < frames.GetLength() - 1; j++)
			{
				if (frames[j]->distances[i] > 0)
				{
					for (; position < frames[j]->distances[i]; position += sprite_history_length)
					{
						float fMult = position / frames[j]->distances[i];
						Vector3 vPos = (frames[j]->positions[i] * (1 - fMult)) + (frames[j + 1]->positions[i] * fMult);

						VertexTL* pVert = transformed_vertices.AddEmptyPtr();
						pVert->position = vPos;
						pVert->diffuse = diffuse;
						pVert->specular = specular;
						pVert->tex_coords[0] = sprite_tex_coords[0];

						//						AddSprite(pCamera, vPos, cDiffuse, cSpecular, pvSprite);
						diffuse = diffuse + exposure_light_delta;
						specular = specular + exposure_light_delta;
						length++;
						if (length > max_history_length) break;
					}
					position -= frames[j]->distances[i];
				}
				if (length > max_history_length) break;
			}
		}
	}

	// projection stuff
	float mult_z = 1 / clip_max_z;
	if (camera->flags.test(Camera::F_SCREEN_TRANSFORM))
	{
		int width = renderer.GetWidth();
		int height = renderer.GetHeight();
		camera->screen_x = (width / 2.0f) - 0.25f;
		camera->screen_y = (height / 2.0f) - 0.25f;
		camera->scale = std::min(width / 640.0f, height / 480.0f) * camera->perspective;
	}
	else
	{
		camera->screen_x = 0;
		camera->screen_y = 0;
		camera->scale = camera->perspective;
	}

	clipped_faces.Empty();

	if (flags.test(F_DRAW_VERTEX_SPRITES))
	{
		// Make a copy of the transformed vertices, and clear the array. The new sprite vertices will be written into it.
		ZArray<VertexTL> input_vertices = transformed_vertices;

		// Resize the output buffers
		clipped_faces.SetLength(input_vertices.GetLength() * 2);
		transformed_vertices.SetLength(input_vertices.GetLength() * 4);

		// Generate sprites for each input vertex
		int output_face_idx = 0;
		int output_vertex_idx = 0;

		float offsets_x[4] = { -sprite_size, +sprite_size, +sprite_size, -sprite_size };
		float offsets_y[4] = { -sprite_size, -sprite_size, +sprite_size, +sprite_size };

		for (int i = 0; i < input_vertices.GetLength(); i++)
		{
			const VertexTL& input_vertex = input_vertices[i];

			// Create two triangles
			clipped_faces[output_face_idx++].Set(output_vertex_idx + 0, output_vertex_idx + 1, output_vertex_idx + 2);
			clipped_faces[output_face_idx++].Set(output_vertex_idx + 2, output_vertex_idx + 3, output_vertex_idx + 0);

			// Create four vertices
			float fRHW = 1.0f / input_vertex.position.z;
			float fZ = input_vertex.position.z * mult_z;
			float fMult = camera->scale * fRHW;

			for (int j = 0; j < 4; j++)
			{
				VertexTL& output_vertex = transformed_vertices[output_vertex_idx++];
				output_vertex.position.x = ((input_vertex.position.x + offsets_x[j]) * fMult) + camera->screen_x;
				output_vertex.position.y = ((input_vertex.position.y + offsets_y[j]) * fMult) + camera->screen_y;
				output_vertex.position.z = fZ;
				output_vertex.rhw = fRHW;
				output_vertex.diffuse = input_vertex.diffuse;
				output_vertex.specular = input_vertex.specular;
				output_vertex.tex_coords[0] = sprite_tex_coords[j];
			}
		}
	}
	else
	{
		if (exp_base.GetLength() <= 1)
		{
			clipped_faces = faces;
			Clip(renderer, clipped_faces, clip_mask & (CLIP_PLANE_MIN_Z | CLIP_FLAG_MAX_Z));
		}
		else
		{
			int ofs = 0;
			for (int j = 0; j < exp_base.GetLength(); j++)
			{
				Face* pExpFace = clipped_faces.AddEmptyPtr(faces.GetLength());
				for (int i = 0; i < faces.GetLength(); i++)
				{
					pExpFace[i][0] = faces[i][0] + ofs;
					pExpFace[i][1] = faces[i][1] + ofs;
					pExpFace[i][2] = faces[i][2] + ofs;
				}
				ofs += vertices.GetLength();
			}

			if (flags.test(F_DO_MIX_EXPOSURE_FACES))
			{
				ZArray<int> lookup;
				lookup.SetLength(transformed_vertices.GetLength());

				for (int i = 0; i < transformed_vertices.GetLength(); i++)
				{
					uint16 vertex = i % vertices.GetLength();
					uint16 exposure = i / vertices.GetLength();
					int old_index = (vertex * exp_base.GetLength()) + exposure;
					lookup[old_index] = i;
				}

				for (int i = 0; i < clipped_faces.GetLength(); i++)
				{
					for (int j = 0; j < 3; j++)
					{
						uint16& wv = clipped_faces[i][j];
						wv = lookup[wv];
					}
				}
			}

			Clip(renderer, clipped_faces, clip_mask & (CLIP_PLANE_MIN_Z | CLIP_FLAG_MAX_Z));
		}

		// Do the camera transform
		for (i = 0; i < transformed_vertices.GetLength(); i++)
		{
			Vector3& v = transformed_vertices[i].position;

			transformed_vertices[i].rhw = 1 / v.z;

			float mult = camera->scale * transformed_vertices[i].rhw;
			v.x = (v.x * mult) + camera->screen_x;
			v.y = (v.y * mult) + camera->screen_y;
			v.z = v.z * mult_z;
		}
	}

	// Clip to the screen boundaries
	Clip(renderer, clipped_faces, clip_mask & ~(CLIP_PLANE_MIN_Z | CLIP_FLAG_MAX_Z));

	flags.set(F_VALID_TRANSFORMED_DATA);
}

Error* Actor::Render(Renderer& renderer)
{
	if (!flags.test(F_VALID_TRANSFORMED_DATA))
	{
		return nullptr;
	}

//	ZDirect3D::TextureStage tsDefault;
	//	tsDisable.AddState(D3DTSS_COLOROP, D3DTOP_DISABLE);

	//	assert(pTransVertex.GetLength() < 16384 * 3);
	_ASSERT(clipped_faces.GetLength() * 3 < 0x0ffff);//D3DMAXNUMVERTICES);//16384);
	//	assert(pClippedEdge.GetLength() < 16384);

	bool stencil_z = flags.test(F_DRAW_Z_BUFFER) && flags.test(F_DRAW_TRANSPARENT);
	for (int i = stencil_z ? -1 : 0; i < 1; i++)
	{
		RenderState render_state;

		// calculate the target vertex format
		if (exposure > 0)
		{
			// build the state list
			if (!flags.test(F_DRAW_Z_BUFFER))
			{
				render_state.enable_zbuffer = false;
			}
			if (flags.test(F_DRAW_TRANSPARENT))
			{
				render_state.dst_blend = D3DBLEND_ONE;
			}
			if (flags.test(F_NO_CULL))
			{
				render_state.enable_culling = false;
			}
			if (lights.size() == 0 && !flags.test(F_VALID_VERTEX_DIFFUSE) && !flags.test(F_VALID_VERTEX_SPECULAR))
			{
				render_state.enable_shading = false;
			}
			if (flags.test(F_USE_SPECULAR))
			{
				render_state.enable_specular = true;
			}
		}


//		g_pD3D->ResetRenderState();
//		for (std::map< D3DRENDERSTATETYPE, DWORD >::iterator it = state.begin(); it != state.end(); it++)
//		{
//			g_pD3D->SetRenderState(it->first, it->second);
//		}

//		g_pD3D->ResetTextureState();
		if (i == -1)
		{
			render_state.enable_specular = false;
			render_state.src_blend = D3DBLEND_ZERO;
			render_state.dst_blend = D3DBLEND_ONE;
		}
		else
		{
			if (stencil_z)
			{
				render_state.zfunc = D3DCMP_EQUAL;
				render_state.enable_zbuffer_write = false;
			}

			render_state.texture_stages[0].texture = textures[0].texture;

			if (textures[0].type == TextureType::Envmap || textures[0].type == TextureType::Lightmap)
			{
				render_state.texture_stages[0].address_u = D3DTADDRESS_CLAMP;
				render_state.texture_stages[0].address_v = D3DTADDRESS_CLAMP;
			}
		}

		if (clipped_faces.GetLength() > 0)
		{
			Error* error = renderer.DrawIndexedPrimitive(render_state, transformed_vertices, clipped_faces);
			if (error) return TraceError(error);
		}
	}

	return nullptr;
}

void Actor::Clip(const Renderer& renderer, ZArray<Face>& faces, uint16 plane_mask)
{
	ZArray<Face> output_faces;
	output_faces.Empty();

	ZArray<VertexTL> temp_vertices;
	ZArray<Face> temp_faces;
	ZArray<int> unused_faces;
	ZArray<int> unused_vertices;

	ZArray<VertexInfo> vertex_info;
	vertex_info.SetLength(transformed_vertices.GetLength());

	if (!plane_mask)
	{
		return;
	}

	if (!flags.test(F_VALID_CLIP_PLANES))
	{
		Rect<float> clip_rect = renderer.GetClipRect();

		clip_min_x = clip_rect.left;
		clip_max_x = clip_rect.right;
		clip_min_y = clip_rect.top;
		clip_max_y = clip_rect.bottom;
		clip_min_z = 0.001f;
		clip_max_z = 1500.0f;
	}

	uint16 required_clip_planes = 0;
	for (int i = 0; i < transformed_vertices.GetLength(); i++)
	{
		vertex_info[i].clip_planes = GetRequiredClipPlanes(transformed_vertices[i].position) & plane_mask;
		if (vertex_info[i].clip_planes != 0)
		{
			vertex_info[i].index = (uint16)temp_vertices.Add(transformed_vertices[i]);
			unused_vertices.Add(i);
		}
		else
		{
			vertex_info[i].index = i;
		}
		required_clip_planes |= vertex_info[i].clip_planes;
	}

	for (int i = 0; i < faces.GetLength(); i++)
	{
		for (int j = 0;; j++)
		{
			if (j >= 3)
			{
				output_faces.Add(faces[i]);
				break;
			}
			else if (vertex_info[faces[i][j]].clip_planes != 0)
			{
				temp_faces.Add(faces[i]);
				break;
			}
		}
	}

	int nFirstClip = output_faces.GetLength();

	uint16 face_vertex_index[3];
	bool is_out[3];
	for (int clip_plane_idx = CLIP_PLANES - 1; clip_plane_idx >= 0; clip_plane_idx--)
	{
		uint16 plane_flag = (uint16)(1 << clip_plane_idx);
		if (!(required_clip_planes & plane_flag))
		{
			continue;
		}

		for (int face_idx = temp_faces.GetLength() - 1; face_idx >= 0; face_idx--)
		{
			Face* face_ptr = &temp_faces[face_idx];
			if ((*face_ptr)[0] == WORD_INVALID_INDEX)
			{
				continue;
			}

			int num_verticies_out = 0;
			for (int k = 0; k < 3; k++)
			{
				face_vertex_index[k] = k;
				is_out[k] = (vertex_info[(*face_ptr)[k]].clip_planes & plane_flag) != 0;
				if (is_out[k])
				{
					num_verticies_out++;
				}
			}

			if (num_verticies_out == 0)
			{
				continue;
			}

			if (num_verticies_out != 3)
			{
				// make sure that no realloc is required (changes pointers)
				transformed_vertices.MakeExtraSpace(100);
				temp_vertices.MakeExtraSpace(100);
				temp_faces.MakeExtraSpace(100);

				face_ptr = &temp_faces[face_idx];

				if (num_verticies_out == 1)
				{
					// Reorder the indices such that 0 is out, and 1 & 2 are in
					if (is_out[1])
					{
						std::swap(face_vertex_index[0], face_vertex_index[1]);
					}
					else if (is_out[2])
					{
						std::swap(face_vertex_index[0], face_vertex_index[2]);
					}

					Face new_face = *face_ptr;
					new_face[face_vertex_index[0]] = ClipEdge(*face_ptr, face_vertex_index[1], face_vertex_index[0], plane_flag, required_clip_planes, vertex_info, temp_vertices, unused_vertices);
					AddClippedFace(new_face, plane_flag - 1, output_faces, vertex_info, temp_faces, unused_faces);

					new_face[face_vertex_index[1]] = new_face[face_vertex_index[0]];
					new_face[face_vertex_index[0]] = ClipEdge(*face_ptr, face_vertex_index[2], face_vertex_index[0], plane_flag, required_clip_planes, vertex_info, temp_vertices, unused_vertices);
					AddClippedFace(new_face, plane_flag - 1, output_faces, vertex_info, temp_faces, unused_faces);
				}
				else // if(nOut == 2)
				{
					// 0 = in, 1, 2 = out
					if (!is_out[1])
					{
						std::swap(face_vertex_index[0], face_vertex_index[1]);
					}
					else if (!is_out[2])
					{
						std::swap(face_vertex_index[0], face_vertex_index[2]);
					}

					Face new_face;
					new_face[face_vertex_index[0]] = (*face_ptr)[face_vertex_index[0]];
					new_face[face_vertex_index[1]] = ClipEdge(*face_ptr, face_vertex_index[0], face_vertex_index[1], plane_flag, required_clip_planes, vertex_info, temp_vertices, unused_vertices);
					new_face[face_vertex_index[2]] = ClipEdge(*face_ptr, face_vertex_index[0], face_vertex_index[2], plane_flag, required_clip_planes, vertex_info, temp_vertices, unused_vertices);
					AddClippedFace(new_face, plane_flag - 1, output_faces, vertex_info, temp_faces, unused_faces);
				}
			}
			(*face_ptr)[0] = WORD_INVALID_INDEX;
			unused_faces.Add(face_idx);
		}
	}

	for (int i = nFirstClip; i < output_faces.GetLength(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			uint16 index = vertex_info[output_faces[i][j]].index;
			output_faces[i][j] = index;
		}
	}

	faces.SwapBuffer(output_faces);
}

bool Actor::IsClipRequired(const Face& face, uint16 plane_mask, const ZArray<VertexInfo>& vertex_info) const
{
	for (int j = 0; j < 3; j++)
	{
		if (vertex_info[face[j]].clip_planes & plane_mask)
		{
			return true;
		}
	}
	return false;
}

uint16 Actor::GetRequiredClipPlanes(const Vector3& v) const
{
	uint16 clip_planes = 0;
	if (v.x < clip_min_x)
	{
		clip_planes |= CLIP_PLANE_MIN_X;
	}
	if (v.x > clip_max_x)
	{
		clip_planes |= CLIP_PLANE_MAX_X;
	}
	if (v.y < clip_min_y)
	{
		clip_planes |= CLIP_PLANE_MIN_Y;
	}
	if (v.y > clip_max_y)
	{
		clip_planes |= CLIP_PLANE_MAX_Y;
	}
	if (v.z < clip_min_z)
	{
		clip_planes |= CLIP_PLANE_MIN_Z;
	}
	if (v.z > clip_max_z)
	{
		clip_planes |= CLIP_FLAG_MAX_Z;
	}
	return clip_planes;
}

int Actor::ClipEdge(const Face& face, int src_idx, int dst_idx, uint16 plane_flag, uint16 all_plane_flags, ZArray<VertexInfo>& vertex_info, ZArray<VertexTL>& temp_vertices, ZArray<int>& unused_vertices)
{
	VertexInfo* pci = &vertex_info[face[src_idx]];
	VertexTL& src = (pci->clip_planes != 0) ? temp_vertices[pci->index] : transformed_vertices[pci->index];
	VertexTL& dst = temp_vertices[vertex_info[face[dst_idx]].index];
	//	ZVertexTL &vIn = pTransVertex[pClip[pf->v[nIn]].wIndex];
	//	ZVertexTL &vOut = pvOut[pClip[pf->v[nOut]].wIndex];

	float pos;
	switch (plane_flag)
	{
	case CLIP_PLANE_MIN_X:
		pos = (src.position.x - clip_min_x) / (src.position.x - dst.position.x);
		break;
	case CLIP_PLANE_MAX_X:
		pos = (clip_max_x - src.position.x) / (dst.position.x - src.position.x);
		break;
	case CLIP_PLANE_MIN_Y:
		pos = (src.position.y - clip_min_y) / (src.position.y - dst.position.y);
		break;
	case CLIP_PLANE_MAX_Y:
		pos = (clip_max_y - src.position.y) / (dst.position.y - src.position.y);
		break;
	case CLIP_PLANE_MIN_Z:
		pos = (src.position.z - clip_min_z) / (src.position.z - dst.position.z);
		break;
	case CLIP_FLAG_MAX_Z:
		pos = (clip_max_z - src.position.z) / (dst.position.z - src.position.z);
		break;
	default:
		__assume(0);
		break;
	}

	Vector3 new_position = (src.position * (1 - pos)) + (dst.position * pos);

	int new_vertex_index;
	VertexTL* new_vertex;

	uint16 required_clip_planes = GetRequiredClipPlanes(new_position) & (all_plane_flags & (plane_flag - 1));
	if (required_clip_planes == 0)
	{
		int length = unused_vertices.GetLength();
		if (length > 0)
		{
			new_vertex_index = unused_vertices[length - 1];
			unused_vertices.SetLength(length - 1);
		}
		else
		{
			new_vertex_index = transformed_vertices.AddEmpty();
		}
		new_vertex = &transformed_vertices[new_vertex_index];
	}
	else
	{
		new_vertex_index = temp_vertices.AddEmpty();
		new_vertex = &temp_vertices[new_vertex_index];
	}

	int new_info_index = vertex_info.AddEmpty();
	vertex_info[new_info_index].index = new_vertex_index;
	vertex_info[new_info_index].clip_planes = required_clip_planes;

	new_vertex->position = new_position;
	new_vertex->diffuse = (src.diffuse * (1 - pos)) + (dst.diffuse * pos);
	new_vertex->specular = (src.specular * (1 - pos)) + (dst.specular * pos);

	if (plane_flag & ~(CLIP_FLAG_MAX_Z | CLIP_PLANE_MIN_Z))
	{
		// work out texture position with texture perspective
		new_vertex->rhw = (src.rhw * (1 - pos)) + (dst.rhw * pos);
		new_vertex->tex_coords[0].x = ((src.tex_coords[0].x * src.rhw * (1 - pos)) + (dst.tex_coords[0].x * dst.rhw * pos)) / new_vertex->rhw;
		new_vertex->tex_coords[0].y = ((src.tex_coords[0].y * src.rhw * (1 - pos)) + (dst.tex_coords[0].y * dst.rhw * pos)) / new_vertex->rhw;
	}
	else
	{
		new_vertex->tex_coords[0].x = (src.tex_coords[0].x * (1 - pos)) + (dst.tex_coords[0].x * pos);
		new_vertex->tex_coords[0].y = (src.tex_coords[0].y * (1 - pos)) + (dst.tex_coords[0].y * pos);
	}
	return new_info_index;
}

void Actor::AddClippedFace(Face& face, uint16 plane_mask, ZArray<Face>& output_faces, const ZArray<VertexInfo>& vertex_info, ZArray<Face>& temp_faces, ZArray<int>& unused_faces)
{
	if (IsClipRequired(face, plane_mask, vertex_info))
	{
		int length = unused_faces.GetLength();
		if (length > 0)
		{
			int new_index = unused_faces[length - 1];
			unused_faces.SetLength(length - 1);
			temp_faces[new_index] = face;
		}
		else
		{
			temp_faces.Add(face);
		}
	}
	else
	{
		output_faces.Add(face);
	}
}

void Actor::CreateCube(float size)
{
	vertices.SetLength(8);
	vertices[0].position.Set(-size, size, -size);
	vertices[1].position.Set(size, size, -size);
	vertices[2].position.Set(size, -size, -size);
	vertices[3].position.Set(-size, -size, -size);
	vertices[4].position.Set(-size, size, size);
	vertices[5].position.Set(size, size, size);
	vertices[6].position.Set(size, -size, size);
	vertices[7].position.Set(-size, -size, size);

	for (int i = 0; i < 8; i++)
	{
		vertices[i].normal = vertices[i].position.Normal(OBJECT_NORMAL_LENGTH);
	}
	flags.set(F_VALID_VERTEX_NORMALS);

	faces.SetLength(12);
	faces[0].Set(0, 3, 2);
	faces[1].Set(0, 2, 1);
	faces[2].Set(1, 2, 6);
	faces[3].Set(1, 6, 5);
	faces[4].Set(4, 5, 7);
	faces[5].Set(5, 6, 7);
	faces[6].Set(0, 4, 7);
	faces[7].Set(0, 7, 3);
	faces[8].Set(4, 0, 1);
	faces[9].Set(4, 1, 5);
	faces[10].Set(7, 6, 2);
	faces[11].Set(7, 2, 3);
}
void Actor::CreateTetrahedron(float radius)
{
	float sqrt2 = sqrtf(2.0f);
	float sqrt3 = sqrtf(3.0f);
	float x = 2.0f * sqrt2 * radius / 3.0f;
	float y = radius / 3.0f;
	float p = sqrt2 * radius / 3.0f;
	float q = sqrt2 * radius / sqrt3;

	vertices.SetLength(4);
	vertices[0].position = Vector3(0, radius, 0);
	vertices[1].position = Vector3(x, -y, 0);
	vertices[2].position = Vector3(-p, -y, q);
	vertices[3].position = Vector3(-p, -y, -q);
	faces.SetLength(4);
	faces[0] = Face(0, 1, 3);
	faces[1] = Face(0, 2, 1);
	faces[2] = Face(0, 3, 2);
	faces[3] = Face(3, 1, 2);
	for (int i = 0; i < 4; i++)
	{
		vertices[i].normal = vertices[i].position.Normal(OBJECT_NORMAL_LENGTH);
	}

	flags.set(F_VALID_VERTEX_NORMALS);
}
void Actor::CreateGeosphere(float radius, int num_vertices)
{
	struct GeoEdge : public Edge
	{
		float length;
		uint16 faces[2];

		void FindLength(Actor* pObj) { length = (pObj->vertices[(*this)[0]].position - pObj->vertices[(*this)[1]].position).Length(); }
	};

	CreateCube(radius / sqrtf(3.0f));
	ZArray<GeoEdge> geo_edges;
	vertices.SetSize(num_vertices);

	for (int i = 0; i < faces.GetLength(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Edge e = faces[i].GetEdge(j).OrderIndices();
			for (int k = 0;; k++)
			{
				if (k < geo_edges.GetLength())
				{
					if (geo_edges[k] == e)
					{
						geo_edges[k].faces[1] = i;
						break;
					}
				}
				else
				{
					k = geo_edges.AddEmpty();
					geo_edges[k].Set(e);
					geo_edges[k].FindLength(this);
					geo_edges[k].faces[0] = i;
					break;
				}
			}
		}
	}

	while (vertices.GetLength() < num_vertices)
	{
		int split_edge = 0;
		float longest_edge = geo_edges[split_edge].length;
		for (int j = 1; j < geo_edges.GetLength(); j++)
		{
			if (geo_edges[j].length > longest_edge)
			{
				split_edge = j;
				longest_edge = geo_edges[j].length;
			}
		}

		GeoEdge te = geo_edges[split_edge];

		// and split it
		float fScale = (0.5f * radius) / sqrtf((radius * radius) - (longest_edge * longest_edge / 4.0f));
		int new_vertex = vertices.AddEmpty();
		vertices[new_vertex].position = (vertices[te[0]].position + vertices[te[1]].position) * fScale;
		////////////////////////////////////////////

		// add the main edges
		geo_edges[split_edge][0] = te[0];
		geo_edges[split_edge][1] = new_vertex;
		geo_edges[split_edge].length /= 2.0;

		int new_edge = geo_edges.AddEmpty();
		geo_edges[new_edge][0] = te[1];
		geo_edges[new_edge][1] = new_vertex;
		geo_edges[new_edge].length = geo_edges[split_edge].length;

		// add two more edges (one may be the same)
		for (int i = 0; i < 2; i++)
		{
			int face1 = te.faces[i];
			int face2 = faces.AddEmpty();

			faces[face2] = faces[face1];

			int vertex;
			for (int k = 0; k < 3; k++)
			{
				if (faces[face1][k] != te[0] && faces[face1][k] != te[1])
				{
					vertex = faces[face1][k];
				}

				if (faces[face1][k] == te[1])
				{
					faces[face1][k] = new_vertex;
				}
				if (faces[face2][k] == te[0])
				{
					faces[face2][k] = new_vertex;
				}
			}
			geo_edges[split_edge].faces[i] = face1;
			geo_edges[new_edge].faces[i] = face2;

			int new_split = geo_edges.AddEmpty();
			geo_edges[new_split].Set(Edge(vertex, new_vertex).OrderIndices());
			geo_edges[new_split].FindLength(this);
			geo_edges[new_split].faces[0] = face1;
			geo_edges[new_split].faces[1] = face2;

			Edge ce = Edge(te[1], vertex).OrderIndices();
			for (int k = 0; k < geo_edges.GetLength(); k++)
			{
				if (geo_edges[k] == ce)
				{
					if (geo_edges[k].faces[0] == face1)
					{
						geo_edges[k].faces[0] = face2;
					}
					if (geo_edges[k].faces[1] == face1)
					{
						geo_edges[k].faces[1] = face2;
					}
				}
			}
		}
	}

	_ASSERT(vertices.GetLength() == num_vertices);

	for (int i = 0; i < vertices.GetLength(); i++)
	{
		vertices[i].normal = vertices[i].position.Normal(OBJECT_NORMAL_LENGTH);
	}
	flags.set(F_VALID_VERTEX_NORMALS);
}

void Actor::CreateTorus(float outer_rad, float inner_rad, int outer_points, int inner_points)
{
	vertices.Empty();
	faces.Empty();

	int num_vertices = outer_points * inner_points;
	for (int i = 0; i < outer_points; i++)
	{
		int base = vertices.GetLength();
		for (int j = 0; j < inner_points; j++)
		{
			double ang_outer = (i + 0.5) * PI2 / outer_points;
			double ang_inner = (j + 0.5) * PI2 / inner_points;

			Vertex* pv = vertices.AddEmptyPtr();
			pv->position.x = (float)(sin(ang_outer) * (outer_rad + (cos(ang_inner) * inner_rad)));
			pv->position.y = (float)(cos(ang_outer) * (outer_rad + (cos(ang_inner) * inner_rad)));
			pv->position.z = (float)(sin(ang_inner) * inner_rad);

			Face face;
			face[0] = base + j;
			face[2] = base + ((j + 1) % inner_points);
			face[1] = (base + ((j + 1) % inner_points) + inner_points) % num_vertices;
			faces.Add(face);

			face[0] = base + j;
			face[1] = (base + j + inner_points) % num_vertices;
			face[2] = (base + ((j + 1) % inner_points) + inner_points) % num_vertices;
			faces.Add(face);
		}
	}
	_ASSERT(vertices.GetLength() == num_vertices);
}

void Actor::CreateTetrahedronGeosphere(float radius, int num_iterations)
{
	struct TetraGeoEdge : public Edge
	{
		int split;
	};

	ZArray<TetraGeoEdge> geo_edges;
	ZArray<Face> geo_faces;

	CreateTetrahedron(radius);
	for (int n = 0; n < num_iterations; n++)
	{
		geo_edges.SetLength(0);
		geo_faces.SetLength(faces.GetLength());
		for (int i = 0; i < faces.GetLength(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				TetraGeoEdge e;
				ZeroMemory(&e, sizeof(e));
				e[0] = faces[i][j];
				e[1] = faces[i][(j + 1) % 3];
				for (int k = 0;; k++)
				{
					if (k < geo_edges.GetLength())
					{
						if (geo_edges[k].Contains(e[0]) && geo_edges[k].Contains(e[1]))
						{
							geo_faces[i][j] = k;
							break;
						}
					}
					else
					{
						int index = geo_edges.Add(e);
						geo_faces[i][j] = k;
						break;
					}
				}
			}
		}

		//		ZFlexibleVertex fv(pVertex);
		for (int i = 0; i < geo_edges.GetLength(); i++)
		{
			int vertex = vertices.AddEmpty();
			geo_edges[i].split = vertex;//pv;//p.SetNewEmpty();
			vertices[vertex].position = (vertices[geo_edges[i][0]].position + vertices[geo_edges[i][1]].position).Normal(radius);
		}
		faces.SetLength(0);
		for (int i = 0; i < geo_faces.GetLength(); i++)
		{
			Face& face = geo_faces[i];
			TetraGeoEdge& edge1 = geo_edges[face[0]];
			TetraGeoEdge& edge2 = geo_edges[face[1]];
			TetraGeoEdge& edge3 = geo_edges[face[2]];
			faces.Add(Face(edge1.split, edge2.split, edge3.split));
			faces.Add(Face(edge1.split, edge2.split, edge1.CommonIndex(edge2)));
			faces.Add(Face(edge1.split, edge3.split, edge1.CommonIndex(edge3)));
			faces.Add(Face(edge2.split, edge3.split, edge2.CommonIndex(edge3)));
		}
	}
	FindFaceOrder(Vector3::Origin());

	for (int i = 0; i < vertices.GetLength(); i++)
	{
		vertices[i].normal = vertices[i].position.Normal(OBJECT_NORMAL_LENGTH);
	}

	flags.set(F_VALID_VERTEX_NORMALS);
}
