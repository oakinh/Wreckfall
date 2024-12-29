#pragma once

#include <glm/glm.hpp>
#include <core.h>

struct Enemy {
	glm::vec2 position;		// Center of sprite
	float radius = 8.0f;	// Collision radius
	int health = 100;
	float movementSpeed = 300;
	bool isAlive = true;
};

extern Animation snatcherAnimation;
extern Animation snatcherDeathAnimation;