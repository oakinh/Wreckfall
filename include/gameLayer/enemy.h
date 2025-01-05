#pragma once
#include <glm/glm.hpp>
#include <core.h>
#include <tile.h>

struct Enemy {
	glm::vec2 position;		// Center of sprite
	float radius = 8.0f;	// Collision radius
	int health = 100;
	float movementSpeed = 300;
	int rotation = 0;
	MovementDirection currentDirection;
	bool isAlive = true;
};

extern Animation snatcherAnimation;
extern Animation snatcherDeathAnimation;

void moveEnemy(Enemy& enemy, float deltaTime, const glm::vec2& targetPos, const Map& map);