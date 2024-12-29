#pragma once
#include <tile.h>

Map loadMap(const std::string& filename);
void renderMap(const Map& map, gl2d::Renderer2D& renderer, gl2d::Texture& tilesetTexture, gl2d::Texture& animatedTexture, int tileWidth, int tileHeight, float spriteScale);
void writeMapToFile(const Map& map, const std::string& filename);
void writeTileToStream(const Tile& tile, std::ofstream& out);