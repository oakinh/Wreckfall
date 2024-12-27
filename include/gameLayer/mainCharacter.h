#ifndef MAIN_CHARACTER_H
#define MAIN_CHARACTER_H
#include <core.h>
#include <glm/vec2.hpp>

extern Animation idleMcAnimation;
extern Animation movingMcAnimation;

struct Player {
	glm::vec2 position = { 1000, 1000 };
	float radius = 8.0f;						// Collision radius
	int health = 100;
	int movementSpeed = 300;
};

void updateAnimation(Animation& animation, float deltaTime);

#endif // MAIN_CHARACTER_H