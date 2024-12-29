#include <json.hpp>
#include <fstream>
#include <iostream>
#include <tile.h>
#include <gl2d/gl2d.h>

using json = nlohmann::json;

Map loadMap(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open " << filename << std::endl;
		return Map{};
	}


	json mapData;
	file >> mapData;

	Map map;

	for (const auto& row : mapData) {
		std::vector<Tile> mapRow;
		for (const auto& tile : row) {
			int tileID = tile["tile_id"];
			bool passable = tile["passable"];
			std::string type = tile["type"];
			
			std::vector<float> uv = tile.contains("uv")
				? tile["uv"].get<std::vector<float>>()
				: std::vector<float>{};

			std::vector<std::vector<float>> uvFrames = tile.contains("uv_frames")
				? tile["uv_frames"].get<std::vector<std::vector<float>>>()
				: std::vector<std::vector<float>>{};

			bool animated = tile.contains("animated")
				? tile["animated"].get<bool>()
				: false;
			int frames = tile.contains("frames")
				? tile["frames"].get<int>()
				: 0;

			mapRow.emplace_back(Tile{ tileID, passable, type, uv, uvFrames, animated, frames });
			// Debug log: Tile details
			//std::cout << "    Tile (" << rowIndex << ", " << colIndex << "):\n";
			std::cout << "      Tile ID: " << tileID << "\n";
			std::cout << "      Passable: " << (passable ? "true" : "false") << "\n";
			std::cout << "      Type: " << type << "\n";

			if (!uv.empty()) {
				std::cout << "      UV Coords: ["
					<< uv[0] << ", " << uv[1] << ", " << uv[2] << ", " << uv[3] << "]\n";
			}

			if (!uvFrames.empty()) {
				std::cout << "      Animated: true\n";
				std::cout << "      Frames: " << frames << "\n";
				std::cout << "      UV Frames:\n";
				for (size_t frameIndex = 0; frameIndex < uvFrames.size(); ++frameIndex) {
					const auto& frame = uvFrames[frameIndex];
					std::cout << "        Frame " << frameIndex << ": ["
						<< frame[0] << ", " << frame[1] << ", " << frame[2] << ", " << frame[3] << "]\n";
				}
			}
			else {
				std::cout << "      Animated: false\n";
			}
		}
		map.push_back(mapRow);
	}
	std::cout << "Map loaded successfully!\n";
	return map;
}

void renderMap(const Map& map, gl2d::Renderer2D& renderer, gl2d::Texture& tilesetTexture, gl2d::Texture& animatedTexture, int tileWidth, int tileHeight) {
	for (size_t row = 0; row < map.size(); ++row) {
		for (size_t col = 0; col < map[row].size(); ++col) {
			const Tile& tile = map[row][col];

			float x = col * tileWidth;
			float y = row * tileHeight;

			if (tile.animated && !tile.uvFrames.empty()) {
				// Use the first frame for now
				const auto& uvFrame = tile.uvFrames[0];
				renderer.renderRectangle(
					gl2d::Rect{
						x,
						y,
						(float)tileWidth,
						(float)tileHeight
					},
					animatedTexture,
					gl2d::Color4f{ 1, 1, 1, 1 },
					glm::vec2{ 0, 0 },
					0,
					glm::vec4{uvFrame[0], uvFrame[1], uvFrame[2], uvFrame[3]}
				);
			}
			else if (!tile.uv.empty()) {
				const auto& uv = tile.uv;
				renderer.renderRectangle(
					gl2d::Rect{
						x,
						y,
						(float)tileWidth,
						(float)tileHeight
					},
					tilesetTexture,
					gl2d::Color4f{ 1, 1, 1, 1 },
					glm::vec2{ 0, 0 },
					0,
					glm::vec4{ tile.uv[0], tile.uv[1], tile.uv[2], tile.uv[3]}
				);
			}
		}
	}
}