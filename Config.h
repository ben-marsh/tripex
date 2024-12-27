#pragma once

#include "Renderer.h"
#include <vector>

#define HKEY_ROOT (HKEY_LOCAL_MACHINE)
#define KEY_MAIN "Software\\-\\Tripex"
#define KEY_TEXTURES "Software\\-\\Tripex\\Textures"

class TextureSource;

bool LoadTextureSettings(std::vector< TextureSource* > &ppItem);
