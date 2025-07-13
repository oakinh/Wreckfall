#include <glm/glm.hpp>
#include <gameData.h>
#include <array>
#include <core.h>

const std::array<glm::vec2, 4> shipSpawnLocations {
	glm::vec2{ 300, 300 }, 
	glm::vec2{ 200, 200 },
	glm::vec2{ 150, 150 },
	glm::vec2{ 125, 125 }
};

void orchestrateShips() {
	for (size_t i = 0; i < shipSpawnLocations.size(); ++i) {
		FallenShip* ship = dropShip(shipSpawnLocations[i]);
	}
}

void renderShips(gl2d::Renderer2D& renderer) {
	for (FallenShip& ship : gameData.fallenShips) {
		const glm::vec2 shipTopLeft = getTopLeft(ship.position, ship.radius, ship.spriteScale;
		renderer.renderRectangle(
			gl2d::Rect{
				shipTopLeft.x,
				shipTopLeft.y,
			},
		);
	}

}

FallenShip* dropShip(glm::vec2 position) {
	for (FallenShip& ship : gameData.fallenShips) {
		if (!ship.isActive) {
			resetShip(ship);
			ship.position = position;
			return &ship;
		}
	}
	return nullptr;
}

FallenShip& resetShip(FallenShip& ship) {
	ship.health = { 300 };
	ship.onGround = { false };
	ship.isActive = { true };
	ship.distanceToFall = { 1000 };
}

FallenShip& destroyShip(FallenShip& ship) {
	ship.onGround = { false };
	ship.isActive = { false };
}