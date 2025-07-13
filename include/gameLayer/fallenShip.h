#pragma once
#include <glm/glm.hpp>

struct FallenShip {
	int health{ 300 };
	glm::vec2 position{};		// Center of sprite
	float radius { 12.0f };		// For collision
	bool onGround { false };	// If it's finished falling or not
	bool isActive { false };
	int fallSpeed { 300 };
	int distanceToFall {};
	float spriteScale{ 3.0f };
};

FallenShip* dropShip(glm::vec2 position);

FallenShip& destroyShip(FallenShip& ship);

