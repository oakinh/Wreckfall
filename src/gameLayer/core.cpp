#include <glm/glm.hpp>
#include <gl2d/gl2d.h>
#include <core.h>
#include <tile.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Node {
	glm::ivec2 position;    // Grid position
	float g = 0;            // Cost from start to current node
	float h = 0;            // Heuristic cost to target
	float f = 0;            // Total cost (f = g + h)
	Node* parent = nullptr; // Parent node for path reconstruction

	Node(int x, int y) : position(x, y) {}

	bool operator==(const Node& other) const {
		return position == other.position;
	}
};

struct CompareNode {
	bool operator()(Node* a, Node* b) {
		return a->f > b->f;
	}
};

glm::vec2 getTopLeft(glm::vec2 centerPos, float width, float height, float spriteScale) {
	float scaledWidth = width * spriteScale;
	float scaledHeight = height * spriteScale;

	return { centerPos.x - scaledWidth / 2, centerPos.y - scaledHeight / 2 };
}

glm::vec2 getTopLeft(glm::vec2 centerPos, float radius, float spriteScale) {
	float scaledRadius = radius * spriteScale;
	return { centerPos.x - scaledRadius, centerPos.y - scaledRadius };
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

void updateAnimation(Animation& animation, float deltaTime) {
	animation.currentFrameTime += deltaTime;

	if (animation.currentFrameTime >= animation.frameTime) {
		animation.currentFrameTime = 0.0f;
		animation.currentFrame = (animation.currentFrame + 1) % animation.totalFrames;
	}
}

// A* heuristic: Diagonal distance
float heuristic(glm::ivec2 start, glm::ivec2 target) {
	int dx = std::abs(start.x - target.x);
	int dy = std::abs(start.y - target.y);
	return static_cast<float>(std::max(dx, dy) + (std::sqrt(2.0f) - 1) * std::min(dx, dy));
}

// Check if a tile is valid and passable
bool isValidTile(const glm::ivec2& pos, const std::vector<std::vector<Tile>>& map) {
	return pos.x >= 0 && pos.y >= 0 &&
		pos.x < static_cast<int>(map.size()) &&
		pos.y < static_cast<int>(map[0].size()) &&
		map[pos.x][pos.y].passable;
}

std::vector<MovementDirection> reconstructPath(Node* target, const glm::ivec2& start) {
	std::vector<MovementDirection> path;
	Node* current = target;

	while (current->parent != nullptr) {
		glm::ivec2 diff = current->position - current->parent->position;

		for (int i = 0; i < 8; ++i) {
			if (glm::vec2(diff) == directionVectors[i]) {
				path.push_back(static_cast<MovementDirection>(i));
				break;
			}
		}

		current = current->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

template <typename T>
glm::vec2 getTopLeft(T ) {

}



