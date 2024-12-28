#include <mainCharacter.h>
#include <bullet.h>
#include <gun.h>
#include <cmath>
#include <glm/glm.hpp>
#include <gameData.h>
#include <iostream>

glm::vec2 calculateMuzzlePos(int rotation, float spriteScale) {
	// Calculate muzzlePosition for gunfire
	glm::vec2 barrelLocalOffset = glm::vec2(7.0f, -1.0f) * spriteScale; // From Gun's center pos(8,8) to barrel pos(15,7)
	float r = glm::radians(float(rotation));
	glm::vec2 rotatedBarrelOffset(
		barrelLocalOffset.x * std::cos(r) - barrelLocalOffset.y * std::sin(r),
		-barrelLocalOffset.x * std::sin(r) + barrelLocalOffset.y * std::cos(r)
	);
	return gameData.gun.position + rotatedBarrelOffset;
};

Bullet fireBullet(glm::vec2 muzzlePos, int rotation, Gun gun, float spriteScale) {
	for (Bullet& bullet : gameData.bullets) {
		if (!bullet.isActive) {
			bullet.isActive = true;
			bullet.damage = 50;
			bullet.radius = 1.0f;
			float r = glm::radians(float(rotation));
			glm::vec2 direction(std::cos(r), -std::sin(r)); // Negative due to coordinate system
			bullet.rotation = rotation;
			bullet.position = muzzlePos + direction * spriteScale;
			bullet.velocity = direction * gun.bulletSpeed;
			return bullet;
		}
	}
	std::cerr << "No inactive bullets available to reutilize for firing" << std::endl;
};

void updateBullets(float deltaTime, float gameWidth, float gameHeight) {
	for (Bullet& bullet : gameData.bullets) {
		if (bullet.isActive) {
			bullet.position += bullet.velocity * deltaTime;
			// Deactivate bullet if it goes out of bounds
			if (bullet.position.x < 0 || bullet.position.y < 0
				|| bullet.position.x > gameWidth || bullet.position.y > gameHeight) {
				bullet.isActive = false;
			}
		}
	};
}