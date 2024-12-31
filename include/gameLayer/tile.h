#pragma once
#include <core.h>

struct Tile {
	int tileID;
	bool passable;
	std::string type;
	std::vector<float> uv; // For static tiles
	std::vector<std::vector<float>> uvFrames; // For animated tiles
	bool animated;
	int frames;
};

extern Animation riverAnimation;

using Map = std::vector<std::vector<Tile>>;