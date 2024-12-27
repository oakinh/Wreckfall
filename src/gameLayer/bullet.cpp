#include <mainCharacter.h>
#include <bullet.h>
#include <gun.h>
#include <cmath>
#include <glm/glm.hpp>
#include <gameData.h>

glm::vec2 calculateMuzzlePos(int rotation, float spriteScale) {
	// Calculate muzzlePosition for gunfire
	glm::vec2 barrelLocalOffset = glm::vec2(7.0f, -1.0f) * spriteScale; // From Gun's center pos(8,8) to barrel pos(15,7)
	float r = glm::radians(float(rotation));
	glm::vec2 rotatedBarrelOffset(
		barrelLocalOffset.x * std::cos(r) - barrelLocalOffset.y * std::sin(r),
		barrelLocalOffset.x * std::sin(r) + barrelLocalOffset.y * std::cos(r)
	);
	return gameData.gun.position + rotatedBarrelOffset;
}