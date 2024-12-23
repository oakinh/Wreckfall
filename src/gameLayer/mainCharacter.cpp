#include <mainCharacter.h>

Animation idleMcAnimation = {
	glm::vec2(16.0f, 16.0f),
	33,
	0.25f
};

void updateAnimation(Animation& animation, float deltaTime) {
	animation.currentFrameTime += deltaTime;

	if (animation.currentFrameTime >= animation.frameTime) {
		animation.currentFrameTime = 0.0f;
		animation.currentFrame = (animation.currentFrame + 1) % animation.totalFrames;
	}
}

