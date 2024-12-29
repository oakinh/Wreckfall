#include <glm/glm.hpp>

glm::vec2 getTopLeft(glm::vec2 centerPos, float width, float height, float spriteScale) {
	float scaledWidth = width * spriteScale;
	float scaledHeight = height * spriteScale;

	return { centerPos.x - scaledWidth / 2, centerPos.y - scaledHeight / 2 };
}
