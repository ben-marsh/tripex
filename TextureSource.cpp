#include "Platform.h"
#include "TextureSource.h"
#include "ConfigItem.h"
#include "TextureData.h"

extern const uint32 *internal_textures[ ] =
{
	g_anTexRawGUI,
	g_anTexBlank,
	g_anTexAlienEgg,
	g_anTexLight,
	g_anTexEyes,
	g_anTexFlesh,
	g_anTexForest,
	g_anTexShinySand,
	g_anTexBrightLight,
};

const int num_internal_textures = sizeof( internal_textures ) / sizeof( internal_textures[ 0 ] );

TextureSource *TextureSource::Internal(int id, const char *usage)
{
	if(id >= num_internal_textures) return NULL;
	else
	{
		TextureSource *source = new TextureSource();
		source->internal = true;
		source->internal_id = id;
		source->filename = "";//psIntTexture[nID];
		source->ParseUsageString(usage);
		return source;
	}
}

TextureSource *TextureSource::External(const char *path, const char *usage)
{
	TextureSource *source = new TextureSource();
	source->internal = false;
	source->filename = path;
	source->ParseUsageString(usage);
	return source;
}

bool TextureSource::ParseUsageString(const char *text)
{
	classes.clear();

	std::vector<int> vn;
	if (!ConfigItem::ParseArrayString(text, vn))
	{
		return false;
	}
	for (int value : vn)
	{
		classes.insert((TextureClass)value);
	}
	return true;
}

std::string TextureSource::MakeUsageString()
{
	std::vector<int> vn;
	for (TextureClass value : classes)
	{
		vn.push_back((int)value);
	}
	return ConfigItem::MakeArrayString(',', vn);
}

std::string TextureSource::GetPath()
{
	if (internal)
	{
		return "[ Internal ]";
	}
	else
	{
		size_t pos = filename.find_last_of("\\");
		if (pos != std::string::npos)
		{
			pos++;
		}
		return filename.substr(0, pos);
	}
}

std::string TextureSource::GetFile()
{
	if (internal)
	{
		return filename;
	}
	else
	{
		size_t pos = filename.find_last_of("\\");
		if (pos != std::string::npos)
		{
			pos++;
		}
		return filename.substr(pos, std::string::npos);
	}
}
