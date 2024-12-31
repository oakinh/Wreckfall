#include <core.h>
#include <enemy.h>
#include <glm/glm.hpp>
#include <iostream>

Animation snatcherAnimation = {
	glm::vec2{ 16, 16 },
	6,
	0.25f,
};

Animation snatcherDeathAnimation = {
	glm::vec2{ 16, 16},
	9,
	0.05f,
};

MovementDirection getDirectionTowards(const glm::vec2& enemyPos, const glm::vec2& targetPos) {
	float dx = targetPos.x - enemyPos.x;
	float dy = targetPos.y - enemyPos.y;

	if (dx == 0.0f && dy == 0.0f) {
		return NONE;
	}

	MovementDirection dir;

	// Determine primary axis
	if (std::abs(dy) > std::abs(dx)) {
		dir = (dy > 0.0f) ? SOUTH : NORTH; // Positive Y is downward
	}
	else {
		dir = (dx > 0.0f) ? EAST : WEST;
	}

	// Determine if it's a diagonal
	const float threshold = 45.0f; // Adjust based on sensitivity
	if (std::abs(dx) > threshold && std::abs(dy) > threshold) {
		if (dx > 0.0f && dy < 0.0f) {      // +x, -y => NE
			dir = NORTHEAST;
		}
		else if (dx > 0.0f && dy > 0.0f) { // +x, +y => SE
			dir = SOUTHEAST;
		}
		else if (dx < 0.0f && dy > 0.0f) { // -x, +y => SW
			dir = SOUTHWEST;
		}
		else if (dx < 0.0f && dy < 0.0f) { // -x, -y => NW
			dir = NORTHWEST;
		}
	}

	return dir;
}

void moveEnemy(Enemy& enemy, float deltaTime, const glm::vec2& targetPos) {
	enemy.currentDirection = getDirectionTowards(enemy.position, targetPos);
	
	if (enemy.currentDirection == NONE) {
		// No movement
		return;
	}

	glm::vec2 dir = directionVectors[enemy.currentDirection];
	//std::cout << "Snatcher position pre-update: (" << enemy.position.x << ", " << enemy.position.y << ")" << std::endl;
	//std::cout << "Dir is: " << dir.x << ", " << dir.y << std::endl;
	std::cout << "Movement speed is: " << enemy.movementSpeed << std::endl;
	//std::cout << "Delta Time is: " << deltaTime << std::endl;
	enemy.position += dir * enemy.movementSpeed * deltaTime;

	std::cout << "Snatcher position post-update: (" << enemy.position.x << ", " << enemy.position.y << ")" << std::endl;


	enemy.rotation = directionRotations[enemy.currentDirection];
}