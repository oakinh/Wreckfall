#pragma once
#include <glm/glm.hpp>
#include <gun.h>

struct Bullet {
	glm::vec2 position;
	glm::vec2 velocity;	// Velocity
	float radius;
	int rotation;
	int damage;	// Damage dealt
	bool isActive;	// Active state
};

glm::vec2 calculateMuzzlePos(int rotation, float spriteScale);

Bullet fireBullet(glm::vec2 muzzlePos, int rotation, Gun gun, float spriteScale);

void updateBullets(float deltaTime, float gameWidth, float gameHeight);