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

enum MovementDirection {
	NORTH = 0,
	NORTHEAST,
	EAST,
	SOUTHEAST,
	SOUTH,
	SOUTHWEST,
	WEST,
	NORTHWEST,
	NONE // For no movement
};

const glm::vec2 directionVectors[8] = {
    glm::vec2(0.0f, -1.0f),          // NORTH (dx=0,   dy<0)
    glm::vec2(0.7071f, -0.7071f),    // NORTHEAST
    glm::vec2(1.0f, 0.0f),           // EAST
    glm::vec2(0.7071f, 0.7071f),     // SOUTHEAST
    glm::vec2(0.0f, 1.0f),           // SOUTH
    glm::vec2(-0.7071f, 0.7071f),    // SOUTHWEST
    glm::vec2(-1.0f, 0.0f),          // WEST
    glm::vec2(-0.7071f, -0.7071f)    // NORTHWEST
};

const glm::ivec2 directionInts[8] = {
	{0, -1},   // NORTH
	{1, -1},   // NE
	{1, 0},    // EAST
	{1, 1},    // SE
	{0, 1},    // SOUTH
	{-1, 1},   // SW
	{-1, 0},   // WEST
	{-1, -1},  // NW
};

const int directionRotations[8] = {
    90,   // NORTH
    45,   // NORTHEAST
    0,    // EAST
    315,  // SOUTHEAST
    270,  // SOUTH
    225,  // SOUTHWEST
    180,  // WEST
    135   // NORTHWEST
};

glm::vec2 getTopLeft(glm::vec2 centerPos, float width, float height, float spriteScale);
glm::vec2 getMouseWorldPosition(const glm::ivec2& mouseScreenPos, const gl2d::Camera& camera, float windowWidth, float windowHeight);
int calculateRotation(const glm::vec2& characterPosition, const glm::vec2& mouseWorldPos);
void updateAnimation(Animation& animation, float deltaTime);
glm::ivec2 worldToGrid(const glm::vec2& worldPos, int tileWidth, int tileHeight, float spriteScale);