#pragma once
#include <glm/vec2.hpp>
#include <gl2d/gl2d.h>

struct Animation {
	glm::vec2 frameSize;	// Size of each frame in the sprite sheet
	int totalFrames;		// Total number of frames in the animation
	float frameTime;		// Time per frame
	float currentFrameTime = 0.0f;	// Accumulator for frame timing
	int currentFrame = 0;			// Current frame index
};

glm::vec2 getTopLeft(glm::vec2 centerPos, float width, float height, float spriteScale);
glm::vec2 getMouseWorldPosition(const glm::ivec2& mouseScreenPos, const gl2d::Camera& camera, float windowWidth, float windowHeight);
int calculateRotation(const glm::vec2& characterPosition, const glm::vec2& mouseWorldPos);