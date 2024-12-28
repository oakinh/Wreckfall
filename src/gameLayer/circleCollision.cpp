#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <bullet.h>
#include <enemy.h>
#include <circleCollision.h>
#include <glm/gtx/norm.hpp>

bool isCircleColliding(const glm::vec2& pos1, float r1, const glm::vec2& pos2, float r2, float spriteScale) {
	// Top left corner -> center
	float r1Adjusted = r1 * spriteScale;
	float r2Adjusted = r2 * spriteScale;
	glm::vec2 center1 = pos1 + glm::vec2(r1Adjusted, r1Adjusted);
	glm::vec2 center2 = pos2 + glm::vec2(r2Adjusted, r2Adjusted);

	// Distance between centers
	glm::vec2 diff = center1 - center2;
	float distanceSquared = glm::dot(diff, diff);

	float radiusSum = r1Adjusted + r2Adjusted;
	return distanceSquared <= (radiusSum * radiusSum);
};

void resolveCircleCollision(glm::vec2& pos1, float r1, glm::vec2& pos2, float r2, float spriteScale) {
	float r1Adjusted = r1 * spriteScale;
	float r2Adjusted = r2 * spriteScale;
	glm::vec2 center1 = pos1 + glm::vec2(r1Adjusted, r1Adjusted);
	glm::vec2 center2 = pos2 + glm::vec2(r2Adjusted, r2Adjusted);
	
	glm::vec2 diff = center2 - center1;
	float distance = glm::length(diff);

	float radiusSum = r1Adjusted + r2Adjusted;
	float overlap = radiusSum - distance;

	if (overlap > 0.0f) {
		glm::vec2 direction = glm::normalize(diff);

		pos1 -= direction * (overlap * 0.5f);
		pos2 += direction * (overlap * 0.5f);
	}
};

//void checkBulletEnemyCollisions(Bullet* bullets, int bulletCount, Enemy* enemies, int enemyCount) {
//	for (int i = 0; i < bulletCount; ++i) {
//		if (!bullets[i].isActive) continue;
//
//		for (int j = 0; j < enemyCount; ++j) {
//			if (!enemies[j].isAlive) continue;
//
//			if (isCircleColliding(bullets[i].position, bullets[i].radius, enemies[j].position, enemies[j].radius)) {
//				// Handle collision
//				bullets[i].isActive = false;
//				enemies[j].isAlive = false;
//				break;
//			}
//		}
//	}
//};