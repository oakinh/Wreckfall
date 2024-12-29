#include <glm/glm.hpp>
#include <gl2d/gl2d.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

glm::vec2 getTopLeft(glm::vec2 centerPos, float width, float height, float spriteScale) {
	float scaledWidth = width * spriteScale;
	float scaledHeight = height * spriteScale;

	return { centerPos.x - scaledWidth / 2, centerPos.y - scaledHeight / 2 };
}

glm::vec2 getMouseWorldPosition(const glm::ivec2& mouseScreenPos, const gl2d::Camera& camera, float windowWidth, float windowHeight) {
	return gl2d::internal::convertPoint(camera, glm::vec2(mouseScreenPos), windowWidth, windowHeight);
}

int calculateRotation(const glm::vec2& characterPosition, const glm::vec2& mouseWorldPos) {
	glm::vec2 direction = mouseWorldPos - characterPosition;
	float angleRadians = std::atan2(direction.y, direction.x);
	float angleDegrees = -angleRadians * 180.0f / M_PI;
	return static_cast<int>(std::fmod(angleDegrees + 360.0f, 360.0f));
}
