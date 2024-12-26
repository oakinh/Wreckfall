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

struct GameData
{
	glm::vec2 rectPos = {100,100};

}gameData;

gl2d::Renderer2D renderer;
gl2d::Texture idleMcSpriteSheet;
gl2d::Texture movingMcSpriteSheet;
float zoomLevel = 1.0f;
float spriteScale = 4.0f;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();
	//platform::setFullScreen(true);

	//loading the saved data. Loading an entire structure like this makes saving game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));
	
	idleMcSpriteSheet.loadFromFile(RESOURCES_PATH "mainCharacterIdleAnimation.png", true);
	movingMcSpriteSheet.loadFromFile(RESOURCES_PATH "mcMovingAnimation.png", true);

	int w = platform::getFrameBufferSizeX();
	int h = platform::getFrameBufferSizeY();
	
	/*glm::vec2 screenCenter = glm::vec2(w, h) * 0.5f;
	glm::vec2 halfSprite = glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) * 0.5f;*/

	gameData.rectPos = glm::vec2(1000, 1000);

	std::cout << "Initialized Player Position: " << gameData.rectPos.x << ", " << gameData.rectPos.y << std::endl;

	
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
		gameData.rectPos.x -= deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Right))
	{
		playerIsMoving = true;
		gameData.rectPos.x += deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Up))
	{
		playerIsMoving = true;
		gameData.rectPos.y -= deltaTime * 300;
	}
	if (platform::isButtonHeld(platform::Button::Down))
	{
		playerIsMoving = true;
		gameData.rectPos.y += deltaTime * 300;
	}

	glm::ivec2 mouseScreenPos = platform::getRelMousePosition();
	glm::vec2 mouseWorldPos = getMouseWorldPosition(mouseScreenPos, camera, w, h);
	int mcRotation = calculateRotation(gameData.rectPos, mouseWorldPos);

	float offScreenMargin = 20.0f;

	// Clamp character to window bounds
	glm::vec2 halfSpriteSize = {
		idleMcAnimation.frameSize.x * 0.5f,
		idleMcAnimation.frameSize.y * 0.5f
	};
	glm::vec2 minBounds = glm::vec2(-offScreenMargin) - halfSpriteSize;
	glm::vec2 maxBounds = glm::vec2(w + offScreenMargin, h + offScreenMargin) + halfSpriteSize;
	gameData.rectPos = glm::clamp(gameData.rectPos, minBounds, maxBounds);
	
	// gameData.rectPos = glm::clamp(gameData.rectPos, glm::vec2{ 0,0 }, glm::vec2{ w - 100,h - 100 });



	// Render main character

	if (playerIsMoving) {
		int xCount = 12;	// Number of columns in the sprite sheet
		int yCount = 1;		// Number of rows
		int x = movingMcAnimation.currentFrame % xCount;
		int y = movingMcAnimation.currentFrame / xCount;
		glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);
		
		std::cout << "Moving Anim Current Frame: " << movingMcAnimation.currentFrame << std::endl;

		renderer.renderRectangle(
			gl2d::Rect{
				gameData.rectPos.x,
				gameData.rectPos.y,
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
				gameData.rectPos.x,
				gameData.rectPos.y,
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

	

	/*renderer.renderRectangle({gameData.rectPos, 100, 100},
		gl2d::Color4f{ 1, 0, 0, 1});*/

	renderer.flush();


	//ImGui::ShowDemoWindow();
	ImGui::Begin("Rectangle Control");
	//ImGui::SliderFloat("Zoom level", &zoomLevel, 0.1f, 5.0f);

	ImGui::DragFloat2("Positions", &gameData.rectPos[0]);


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
