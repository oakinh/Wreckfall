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
#include <renderMap.h>
#include <tileCollision.h>

gl2d::Renderer2D renderer;
gl2d::Texture idleMcSpriteSheet;
gl2d::Texture movingMcSpriteSheet;
gl2d::Texture snatcherSpriteSheet;
gl2d::Texture snatcherDeathSpriteSheet;
gl2d::Texture gunSprite;
gl2d::Texture gunFireSprite;
gl2d::Texture tileset;
gl2d::Texture animatedTiles;

Map map;
float zoomLevel = 1.0f;
float spriteScale = 4.0f;
int tileWidth = 32;
int tileHeight = 32;
int mapColumns = 10;
int mapRows = 10;
float mapWidth = (tileWidth * mapColumns * spriteScale);
float mapHeight = (tileHeight * mapRows * spriteScale);
float mapCenterX = mapWidth / 2;
float mapCenterY = mapHeight / 2;

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
	snatcherSpriteSheet.loadFromFile(RESOURCES_PATH "snatcher.walking.png", true);
	snatcherDeathSpriteSheet.loadFromFile(RESOURCES_PATH "snatcher.death.animation.png", true);
	gunSprite.loadFromFile(RESOURCES_PATH "dmrStatic.png", true);
	gunFireSprite.loadFromFile(RESOURCES_PATH "gunfire.png", true);
	tileset.loadFromFile(RESOURCES_PATH "wreckfall.tilemap.png", true);
	animatedTiles.loadFromFile(RESOURCES_PATH "wreckfall.water.animation.png", true);

	map = loadMap(RESOURCES_PATH "processed_map_inverted.json");
	writeMapToFile(map, RESOURCES_PATH "loaded_map.json");

	int w = platform::getFrameBufferSizeX();
	int h = platform::getFrameBufferSizeY();
	
	//glm::vec2 halfSprite = glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) * 0.5f;

	std::cout << "Initialized Player Position: " << gameData.player.position.x << ", " << gameData.player.position.y << std::endl;
	
	// Setup snatcher
	gameData.snatcher.position = glm::vec2{ 300, 300 };
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
	// renderer.clearScreen({ 1.0f, 1.0f, 1.0f, 1.0f }); // To make the background white
	glm::vec2 viewportSizeInWorld = glm::vec2(w, h) / zoomLevel;
	gl2d::Camera camera;

	camera.position.x = mapCenterX - (w / 2);
	camera.position.y = mapCenterY - (h / 2);
	renderer.setCamera(camera);
	//gameData.player.position = { 200, 200 };
	renderMap(map, renderer, tileset, animatedTiles, 32, 32, spriteScale);

	bool playerIsMoving = false;
	if (platform::isButtonHeld(platform::Button::Left))
	{
		playerIsMoving = true;
		gameData.player.position.x -= deltaTime * gameData.player.movementSpeed;
	}
	if (platform::isButtonHeld(platform::Button::Right))
	{
		playerIsMoving = true;
		gameData.player.position.x += deltaTime * gameData.player.movementSpeed;
	}
	if (platform::isButtonHeld(platform::Button::Up))
	{
		playerIsMoving = true;
		gameData.player.position.y -= deltaTime * gameData.player.movementSpeed;
	}
	if (platform::isButtonHeld(platform::Button::Down))
	{
		playerIsMoving = true;
		gameData.player.position.y += deltaTime * gameData.player.movementSpeed;
	}
	bool playerIsFiring = false;
	if (platform::isLMousePressed()) {
		playerIsFiring = true;
	}

	glm::ivec2 mouseScreenPos = platform::getRelMousePosition();
	glm::vec2 mouseWorldPos = getMouseWorldPosition(mouseScreenPos, camera, w, h);
	gameData.player.rotation = calculateRotation(gameData.player.position, mouseWorldPos);

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

	glm::vec2 playerTopLeft = getTopLeft(gameData.player.position, 16, 16, spriteScale);

	// Render main character
	if (playerIsMoving) {
		int xCount = 12;	// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = movingMcAnimation.currentFrame % xCount;
		int y = movingMcAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);

		renderer.renderRectangle(
			gl2d::Rect{
				playerTopLeft.x,
				playerTopLeft.y,
				movingMcAnimation.frameSize.x * spriteScale,
				movingMcAnimation.frameSize.y * spriteScale
			},
			movingMcSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			gameData.player.rotation,
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
				playerTopLeft.x,
				playerTopLeft.y,
				idleMcAnimation.frameSize.x * spriteScale,
				idleMcAnimation.frameSize.y * spriteScale
			},
			idleMcSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			gameData.player.rotation,
			uvCoords
		);

		updateAnimation(idleMcAnimation, deltaTime);
	}
	// Calculate gun sprite position
	//glm::vec2 characterCenter = gameData.player.position + (glm::vec2(8, 8) * spriteScale);
	glm::vec2 characterCenter = gameData.player.position;
	gameData.gun.position = characterCenter;

	float forwardDist = 9.0f * spriteScale; // How far in front I want the gun center
	float r = glm::radians(float(gameData.player.rotation));
	glm::vec2 forwardOffset(
		forwardDist * std::cos(r),
		-forwardDist * std::sin(r)
	);
	gameData.gun.position += forwardOffset;

	float sideDist = -3.0f * spriteScale;

	glm::vec2 sideOffset(
		sideDist * std::cos(glm::radians(float(gameData.player.rotation + 90))),
		-sideDist * std::sin(glm::radians(float(gameData.player.rotation + 90)))
	);

	gameData.gun.position += sideOffset;

	glm::vec2 gunSize = { 16.0f * spriteScale, 16.0f * spriteScale};
	glm::vec2 gunPivot = { 8.0f * spriteScale, 8.0f * spriteScale};

	glm::vec2 gunTopLeft = gameData.gun.position - gunPivot;

	// Render gun
	renderer.renderRectangle(
		gl2d::Rect{
			gunTopLeft.x,
			gunTopLeft.y,
			gunSize.x,
			gunSize.y
		},
		gunSprite,
		gl2d::Color4f{1, 1, 1, 1},
		glm::vec2{0, 0}, // Pivot around center (8,8) in local coords
		gameData.player.rotation
	);
	if (playerIsFiring) {
		std::cout << "Player is firing, spawning bullet" << std::endl;
		glm::vec2 muzzlePos = calculateMuzzlePos(gameData.player.rotation, spriteScale);
		Bullet bullet = fireBullet(muzzlePos, gameData.player.rotation, gameData.gun, spriteScale);
		std::cout << "Bullet position x, y: " << bullet.position.x << ", " << bullet.position.y << std::endl;
		renderer.renderRectangle(gl2d::Rect{
			bullet.position.x - (3.0f * spriteScale),
			bullet.position.y - (0.5f * spriteScale),
			6.0f * spriteScale,
			1.0f * spriteScale
			},
			gunFireSprite,
			gl2d::Color4f{ 1, 1, 1, 1 },
			//glm::vec2{ 3.0f * spriteScale, 0.5f * spriteScale },
			glm::vec2{0, 0},
			bullet.rotation
		);
	}

	updateBullets(deltaTime, w, h);

	for (const Bullet& bullet : gameData.bullets) {
		if (bullet.isActive) {
			renderer.renderRectangle(gl2d::Rect{
				bullet.position.x - (3.0f * spriteScale),
				bullet.position.y - (0.5f * spriteScale),
				6.0f * spriteScale,
				1.0f * spriteScale
				},
				gunFireSprite,
				gl2d::Color4f{ 1, 1, 1, 1 },
				//glm::vec2{ 3.0f * spriteScale, 0.5f * spriteScale },
				glm::vec2{ 0, 0 },
				bullet.rotation
			);
		}
	}

	// Render snatcher
	if (gameData.snatcher.isAlive) {
		int xCount = 9;		// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = snatcherAnimation.currentFrame % xCount;
		int y = snatcherAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);

		glm::vec2 snatcherTopLeft = getTopLeft(gameData.snatcher.position, 16, 16, spriteScale);
		
		renderer.renderRectangle(
			gl2d::Rect{
				snatcherTopLeft.x,
				snatcherTopLeft.y,
				snatcherAnimation.frameSize.x * spriteScale,
				snatcherAnimation.frameSize.y * spriteScale
			},
			snatcherSpriteSheet,
			gl2d::Color4f{ 1, 1, 1, 1 },
			glm::vec2{ 0, 0 },
			0,
			uvCoords
		);
		 //updateAnimation(snatcherAnimation, deltaTime);
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

	isCollidingWithTile(map, gameData.player.position, gameData.player.radius, glm::vec2{ tileWidth, tileHeight }, glm::ivec2{ mapColumns, mapRows }, spriteScale);
	glm::vec2 extraMapBorderAllowance = glm::vec2(tileWidth * spriteScale, tileHeight * spriteScale);

	if (gameData.player.position.x < -extraMapBorderAllowance.x ||
		gameData.player.position.x > mapWidth + extraMapBorderAllowance.x ||
		gameData.player.position.y < -extraMapBorderAllowance.y ||
		gameData.player.position.y > mapHeight + extraMapBorderAllowance.y)
	{
		std::cout << "Player exited map boundaries. Teleporting to (200, 200)." << std::endl;
		gameData.player.position = glm::vec2(200.0f, 200.0f);
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
