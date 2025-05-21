#include <glm/glm.hpp>
#include <gameData.h>

FallenShip* dropShip(glm::vec2 position) {
	for (FallenShip& ship : gameData.fallenShips) {
		if (!ship.isActive) {
			ship.position = position;
			ship.isActive = true;
			return &ship;
		}
	}
	return nullptr;
}