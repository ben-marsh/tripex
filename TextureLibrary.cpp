#include "TextureLibrary.h"

void TextureLibrary::Add(TextureClass tc, Texture* texture)
{
	std::map<TextureClass, std::vector<Texture*>>::iterator it = textures_by_class.find(tc);
	if (it == textures_by_class.end())
	{
		it = textures_by_class.emplace(tc, std::vector<Texture*>()).first;
	}
	it->second.push_back(texture);
}

Texture* TextureLibrary::Find(TextureClass tc) const
{
	std::map<TextureClass, std::vector<Texture*>>::const_iterator it = textures_by_class.find(tc);
	if (it == textures_by_class.end())
	{
		return nullptr;
	}

	const std::vector<Texture*>& textures = it->second;
	int index = (int)(((long long)rand() * textures.size()) / (RAND_MAX + 1));

	return textures[index];
}
