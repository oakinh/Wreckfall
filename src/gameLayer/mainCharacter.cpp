#include <mainCharacter.h>

Animation idleMcAnimation = {
	glm::vec2(16.0f, 16.0f),
	33,
	0.25f
};

Animation movingMcAnimation = {
	glm::vec2(16.0f, 16.0f),
	12,
	0.05f
};

Entity mainCharacter = {
	100,
	300,
	50,
};


void updateAnimation(Animation& animation, float deltaTime) {
	animation.currentFrameTime += deltaTime;

	if (animation.currentFrameTime >= animation.frameTime) {
		animation.currentFrameTime = 0.0f;
		animation.currentFrame = (animation.currentFrame + 1) % animation.totalFrames;
	}
}

