#pragma once

#include "Texture.h"
#include <set>
#include <map>
#include <vector>

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

class TextureLibrary
{
public:
	void Add(TextureClass tc, Texture* texture);
	Texture* Find(TextureClass tc) const;

private:
	std::set<Texture*> textures;
	std::map<TextureClass, std::vector<Texture*>> textures_by_class;
};
