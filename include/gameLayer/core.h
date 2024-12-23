#pragma once
#include <glm/vec2.hpp>

struct Animation {
	glm::vec2 frameSize;	// Size of each frame in the sprite sheet
	int totalFrames;		// Total number of frames in the animation
	float frameTime;		// Time per frame
	float currentFrameTime = 0.0f;	// Accumulator for frame timing
	int currentFrame = 0;			// Current frame index
};