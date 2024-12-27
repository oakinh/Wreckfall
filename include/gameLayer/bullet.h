#pragma once

struct Bullet {
	glm::vec2 position;
	float radius;
	float dx, dy;	// Velocity
	float damage;	// Damage dealt
	bool isActive;	// Active state
};

glm::vec2 calculateMuzzlePos(int rotation, float spriteScale);