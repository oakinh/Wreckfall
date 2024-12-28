#include <json.hpp>
#include <fstream>
#include <iostream>
#include <tile.h>

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