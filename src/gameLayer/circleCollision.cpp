#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <bullet.h>
#include <enemy.h>
#include <circleCollision.h>
#include <glm/gtx/norm.hpp>

bool isCircleColliding(const glm::vec2& pos1, float r1, const glm::vec2& pos2, float r2) {
	glm::vec2 center1 = pos1 + glm::vec2(r1, r2);
	glm::vec2 center2 = pos2 + glm::vec2(r2, r2);
	float distanceSquared = glm::length2(center1 - center2);
	float radiusSum = r1 + r2;
	return distanceSquared <= radiusSum * radiusSum;
};

void checkBulletEnemyCollisions(Bullet* bullets, int bulletCount, Enemy* enemies, int enemyCount) {
	for (int i = 0; i < bulletCount; ++i) {
		if (!bullets[i].isActive) continue;

		for (int j = 0; j < enemyCount; ++j) {
			if (!enemies[j].isAlive) continue;

			if (isCircleColliding(bullets[i].position, bullets[i].radius, enemies[j].position, enemies[j].radius)) {
				// Handle collision
				bullets[i].isActive = false;
				enemies[j].isAlive = false;
				break;
			}
		}
	}
};