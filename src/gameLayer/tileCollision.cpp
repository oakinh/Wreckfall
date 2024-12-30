#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <mainCharacter.h>
#include <enemy.h>
#include <variant>
#include <tile.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

bool isCircleCollidingWithTile(
	const glm::vec2& circleCenter, float circleRadius,
	const glm::vec2& tileCenter, const glm::vec2& squareHalfSize)
{
	// Compute the closest point on the square to the circle's center
	glm::vec2 closestPoint = glm::clamp(
		circleCenter,
		tileCenter - squareHalfSize, // Min corner
		tileCenter + squareHalfSize  // Max corner
	);

	// Calculate the squared distance from the circle's center to the closest point
	float distanceSquared = glm::distance2(circleCenter, closestPoint);

	// Check if the distance is less than or equal to the circle's radius squared
	return distanceSquared <= circleRadius * circleRadius;
}

void resolveTileCollision(glm::vec2 tileCenter, glm::vec2& playerPosition, float radius, glm::vec2 scaledTileSize) {
	glm::vec2 tileHalfSize = scaledTileSize * 0.5f;

	glm::vec2 closestPoint = glm::clamp(playerPosition, tileCenter - tileHalfSize, tileCenter + tileHalfSize);

	glm::vec2 delta = playerPosition - closestPoint;

	float distanceSquared = glm::dot(delta, delta);
	float radiusSquared = radius * radius;

	if (distanceSquared < radiusSquared) {
		float distance = (distanceSquared > 0.0f) ? std::sqrt(distanceSquared) : 0.0f;

		if (distance == 0.0f) {
			delta = glm::vec2(0.0f, 1.0f);
			distance = 0.001f;
		}

		float penetrationDepth = radius - distance;

		glm::vec2 collisionNormal = delta / distance;

		playerPosition += collisionNormal * penetrationDepth;
	}
}

bool isCollidingWithTile(Map map, glm::vec2& position, float radius, glm::vec2 tileSize, glm::ivec2 mapSize, float spriteScale) {
	float scaledRadius = radius * spriteScale;
	float scaledTileWidth = tileSize.x * spriteScale;
	float scaledTileHeight = tileSize.y * spriteScale;

	const int maxIterations = 5;
	bool collisionDetected = false;

	for (int iteration = 0; iteration < maxIterations; ++iteration) {
		float left = position.x - scaledRadius;
		float right = position.x + scaledRadius;
		float top = position.y - scaledRadius;
		float bottom = position.y + scaledRadius;

		int minTileX = static_cast<int>(std::floor(left / scaledTileWidth));
		int maxTileX = static_cast<int>(std::floor(right / scaledTileWidth));
		int minTileY = static_cast<int>(std::floor(top / scaledTileHeight));
		int maxTileY = static_cast<int>(std::floor(bottom / scaledTileHeight));

		minTileX = glm::clamp(minTileX, 0, mapSize.x - 1);
		maxTileX = glm::clamp(maxTileX, 0, mapSize.x - 1);
		minTileY = glm::clamp(minTileY, 0, mapSize.y - 1);
		maxTileY = glm::clamp(maxTileY, 0, mapSize.y - 1);

		bool anyCollision = false;

		for (int row = minTileY; row <= maxTileY; ++row) {
			for (int column = minTileX; column <= maxTileX; ++column) {
				Tile& tile = map[row][column];

				if (tile.passable) {
					continue;
				}

				glm::vec2 tileCenter = glm::vec2{
					column * scaledTileWidth + (scaledTileWidth * 0.5f),
					row * scaledTileHeight + (scaledTileHeight * 0.5f)
				};
				glm::vec2 tileHalfSize = tileSize * spriteScale * 0.5f;
				if (isCircleCollidingWithTile(position, scaledRadius, tileCenter, tileHalfSize)) {
					std::cout << "isColliding found to be true" << std::endl;
					resolveTileCollision(tileCenter, position, scaledRadius, glm::vec2{ scaledTileWidth, scaledTileHeight });
					collisionDetected = true;
					anyCollision = true;
				}
			}
		}
		if (!anyCollision) {
			break;
		}
	}
	//std::cout << "Player is currently on a tile type: " << currentTile.type << std::endl;
	return collisionDetected;
}