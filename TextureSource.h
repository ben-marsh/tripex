#pragma once

#include "Texture.h"
#include <string>

class TextureSource
{
public:
	bool internal;
	int internal_id;
	std::string filename;
	std::set<int> classes;
	Texture *texture;

	static TextureSource *Internal(int id, const char *usage = "");
	static TextureSource *External(const char *filename, const char *usage = "");

	std::string GetPath();
	std::string GetFile();

	bool ParseUsageString(const char *sText);
	std::string MakeUsageString();
};

extern const uint32 *internal_textures[];
extern const int num_internal_textures;
