#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <core.h>
#include <mainCharacter.h>
#include <cmath>
#include <circleCollision.h>
#include <enemy.h>
#include <bullet.h>
#include <gun.h>
#include <gameData.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

glm::vec2 getMouseWorldPosition(const glm::ivec2& mouseScreenPos, const gl2d::Camera& camera, float windowWidth, float windowHeight) {
	return gl2d::internal::convertPoint(camera, glm::vec2(mouseScreenPos), windowWidth, windowHeight);
}

int calculateRotation(const glm::vec2& characterPosition, const glm::vec2& mouseWorldPos) {
	glm::vec2 spriteSize = { 16, 16 };
	glm::vec2 characterCenter = characterPosition + spriteSize;
	glm::vec2 direction = mouseWorldPos - characterCenter;
	float angleRadians = std::atan2(direction.y, direction.x);
	float angleDegrees = -angleRadians * 180.0f / M_PI;
	return static_cast<int>(std::fmod(angleDegrees + 360.0f, 360.0f));
}

gl2d::Renderer2D renderer;
gl2d::Texture idleMcSpriteSheet;
gl2d::Texture movingMcSpriteSheet;
gl2d::Texture snatcherSpriteSheet;
gl2d::Texture gunSprite;
gl2d::Texture gunFireSprite;
float zoomLevel = 1.0f;
float spriteScale = 4.0f;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	std::cout << "Number of bullets: " << gameData.bullets.size() << std::endl;

	//loading the saved data. Loading an entire structure like this makes saving game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));
	
	idleMcSpriteSheet.loadFromFile(RESOURCES_PATH "mainCharacterIdleAnimation.png", true);
	movingMcSpriteSheet.loadFromFile(RESOURCES_PATH "mcMovingAnimation.png", true);
	snatcherSpriteSheet.loadFromFile(RESOURCES_PATH "snatcherWalkingAnimation.png", true);
	gunSprite.loadFromFile(RESOURCES_PATH "dmrStatic.png", true);
	gunFireSprite.loadFromFile(RESOURCES_PATH "gunfire.png", true);

	int w = platform::getFrameBufferSizeX();
	int h = platform::getFrameBufferSizeY();
	
	/*glm::vec2 screenCenter = glm::vec2(w, h) * 0.5f;
	glm::vec2 halfSprite = glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) * 0.5f;*/

	std::cout << "Initialized Player Position: " << gameData.player.position.x << ", " << gameData.player.position.y << std::endl;
	
	// Setup snatcher
	gameData.snatcher.position = glm::vec2{ 1100, 1100 };
	gameData.snatcher.isAlive = true;
	
	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	static int prevWindowWidth = 0;
	static int prevWindowHeight = 0;
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion
	renderer.clearScreen({ 1.0f, 1.0f, 1.0f, 1.0f });
	glm::vec2 viewportSizeInWorld = glm::vec2(w, h) / zoomLevel;
	gl2d::Camera camera;

	camera.position.x = 1000;
	camera.position.y = 1000;
	camera.position -= viewportSizeInWorld * 0.5f;
	renderer.setCamera(camera);

	bool playerIsMoving = false;
	if (platform::isButtonHeld(platform::Button::Left))
	{
		playerIsMoving = true;
		gameData.player.position.x -= deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Right))
	{
		playerIsMoving = true;
		gameData.player.position.x += deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Up))
	{
		playerIsMoving = true;
		gameData.player.position.y -= deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Down))
	{
		playerIsMoving = true;
		gameData.player.position.y += deltaTime * 300;
	}
	bool playerIsFiring = false;
	if (platform::isLMousePressed()) {
		playerIsFiring = true;
	}

	glm::ivec2 mouseScreenPos = platform::getRelMousePosition();
	glm::vec2 mouseWorldPos = getMouseWorldPosition(mouseScreenPos, camera, w, h);
	int mcRotation = calculateRotation(gameData.player.position, mouseWorldPos);

	//std::cout << "Snatcher is alive? - " << std::boolalpha << gameData.snatcher.isAlive << std::endl;

	float offScreenMargin = 20.0f;

	// Clamp character to window bounds
	glm::vec2 halfSpriteSize = {
		idleMcAnimation.frameSize.x * 0.5f,
		idleMcAnimation.frameSize.y * 0.5f
	};
	glm::vec2 minBounds = glm::vec2(-offScreenMargin) - halfSpriteSize;
	glm::vec2 maxBounds = glm::vec2(w + offScreenMargin, h + offScreenMargin) + halfSpriteSize;
	gameData.player.position = glm::clamp(gameData.player.position, minBounds, maxBounds);
	
	// gameData.player.position = glm::clamp(gameData.player.position, glm::vec2{ 0,0 }, glm::vec2{ w - 100,h - 100 });

	// Render main character
	if (playerIsMoving) {
		int xCount = 12;	// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = movingMcAnimation.currentFrame % xCount;
		int y = movingMcAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);
		renderer.renderRectangle(
			gl2d::Rect{
				gameData.player.position.x,
				gameData.player.position.y,
				movingMcAnimation.frameSize.x * spriteScale,
				movingMcAnimation.frameSize.y * spriteScale
			},
			movingMcSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			mcRotation,
			uvCoords
		);
		updateAnimation(movingMcAnimation, deltaTime);
	} else if (!playerIsMoving) {
		int xCount = 33;	// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = idleMcAnimation.currentFrame % xCount;
		int y = idleMcAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);

		renderer.renderRectangle(
			gl2d::Rect{
				gameData.player.position.x,
				gameData.player.position.y,
				idleMcAnimation.frameSize.x * spriteScale,
				idleMcAnimation.frameSize.y * spriteScale
			},
			idleMcSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			mcRotation,
			uvCoords
		);

		updateAnimation(idleMcAnimation, deltaTime);
	}
	// Calculate gun sprite position
	glm::vec2 characterCenter = gameData.player.position + glm::vec2(8, 8) * spriteScale;

	float gunOffsetDistance = 9.0f * spriteScale; // How far from the center the gun is
	glm::vec2 gunOffset(
		gunOffsetDistance * std::cos(glm::radians(float(mcRotation))),
		-gunOffsetDistance * std::sin(glm::radians(float(mcRotation)))
	);
	glm::vec2 gunPosition = characterCenter + gunOffset;

	float sideDist = -2.0f * spriteScale;

	glm::vec2 sideOffset(
		sideDist * std::cos(glm::radians(float(mcRotation + 90))),
		-sideDist * std::sin(glm::radians(float(mcRotation + 90)))
	);

	gunPosition += sideOffset;

	glm::vec2 gunSize = { 16.0f, 16.0f };
	glm::vec2 gunScaledSize = gunSize * spriteScale;

	glm::vec2 gunPivot = { 0.0f, 0.0f };
	gameData.gun.position = gunPosition - gunScaledSize * 0.5f;

	// Render gun
	renderer.renderRectangle(
		gl2d::Rect{
			gameData.gun.position.x,
			gameData.gun.position.y,
			gunScaledSize.x,
			gunScaledSize.y
		},
		gunSprite,
		gl2d::Color4f{1, 1, 1, 1},
		gunPivot,
		mcRotation
	);
	if (playerIsFiring) {
		std::cout << "Player is firing, spawning bullet" << std::endl;
		glm::vec2 muzzlePos = calculateMuzzlePos(mcRotation, spriteScale);
		Bullet bullet = fireBullet(muzzlePos, mcRotation, gameData.gun, spriteScale);
		std::cout << "Bullet position x, y: " << bullet.position.x << ", " << bullet.position.y << std::endl;
		renderer.renderRectangle(gl2d::Rect{
			bullet.position.x,
			bullet.position.y,
			6.0f * spriteScale,
			1.0f * spriteScale
			},
			gunFireSprite,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 3.0f * spriteScale, 0.5f * spriteScale },
			bullet.rotation
		);
	}

	updateBullets(deltaTime, w, h);

	for (const Bullet& bullet : gameData.bullets) {
		if (bullet.isActive) {
			renderer.renderRectangle(gl2d::Rect{
				bullet.position.x,
				bullet.position.y,
				6.0f * spriteScale,
				1.0f * spriteScale
				},
				gunFireSprite,
				gl2d::Color4f{ 1, 1, 1, 1 },
				glm::vec2{ 3.0f * spriteScale, 0.5f * spriteScale },
				bullet.rotation
			);
		}
	}

	// Render snatcher
	if (gameData.snatcher.isAlive) {
		int xCount = 6;		// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = idleMcAnimation.currentFrame % xCount;
		int y = idleMcAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);
		
		/*renderer.renderRectangle(
			gl2d::Rect{
				gameData.snatcher.position.x,
				gameData.snatcher.position.y,
				snatcherAnimation.frameSize.x * spriteScale,
				snatcherAnimation.frameSize.y * spriteScale
			},
			snatcherSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			0,
			uvCoords
		);*/
		// std::cout << "Snatcher rendered" << std::endl;

		renderer.renderRectangle(gl2d::Rect{gameData.snatcher.position.x, gameData.snatcher.position.y, 64, 64},
		gl2d::Color4f{ 1, 0, 0, 1});
	}
	else {
		std::cout << "Snatcher detected as NOT alive" << std::endl;
	}
	//std::cout << "Checking collision" << std::endl;
	//std::cout << "Player radius: " << gameData.player.radius << std::endl;
	//std::cout << "Snatcher radius: " << gameData.snatcher.radius << std::endl;

	if (isCircleColliding(gameData.player.position,
		gameData.player.radius,
		gameData.snatcher.position,
		gameData.snatcher.radius, spriteScale)) // Change radius
	{
		resolveCircleCollision(gameData.player.position, gameData.player.radius,
			gameData.snatcher.position,
			gameData.snatcher.radius, spriteScale);
		std::cout << "Collision detected and resolved!" << std::endl;
	}

	renderer.flush();


	//ImGui::ShowDemoWindow();
	ImGui::Begin("Rectangle Control");
	//ImGui::SliderFloat("Zoom level", &zoomLevel, 0.1f, 5.0f);

	ImGui::DragFloat2("Positions", &gameData.player.position[0]);


	ImGui::End();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{

	//saved the data.
	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
