#include <iostream>
#include <mainCharacter.h>
#include <enemy.h>
#include <variant>
#include <tile.h>

bool isPlayerCollidingWithTile(Map map, Player player, glm::vec2 tileSize, float spriteScale) {
	glm::vec2 currentTilePos = player.position / (tileSize * spriteScale);
	Tile currentTile = map[currentTilePos.y][currentTilePos.x];
	//std::cout << "Player is currently on a tile type: " << currentTile.type << std::endl;
	return true;
}