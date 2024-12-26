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

struct GameData
{
	glm::vec2 rectPos = {100,100};

}gameData;

gl2d::Renderer2D renderer;
gl2d::Texture t;
gl2d::Texture idleMcSpriteSheet;
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

	/*camera.position.x = -(
		(static_cast<float>(w) / 2.0f) / zoomLevel);
	camera.position.y = +(
		(static_cast<float>(h) / 2.0f) / zoomLevel);*/

	//camera.zoom = zoomLevel;


	camera.position.x = 1000;
	camera.position.y = 1000;
		
	camera.position -= viewportSizeInWorld * 0.5f;

	
	//// For a camera that follows player
	//// camera.position = gameData.rectPos + glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) / 2.0f; // Center the camera
	renderer.setCamera(camera);


	if (platform::isButtonHeld(platform::Button::Left))
	{
		gameData.rectPos.x -= deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::Right))
	{
		gameData.rectPos.x += deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::Up))
	{
		gameData.rectPos.y -= deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::Down))
	{
		gameData.rectPos.y += deltaTime * 100;
	}

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

	int xCount = 33;	// Number of columns in the sprite sheet
	int yCount = 1;		// Number of rows
	int x = idleMcAnimation.currentFrame % xCount;
	int y = idleMcAnimation.currentFrame / xCount;
	glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);

	// Render main character
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
		0,
		uvCoords
	);

	updateAnimation(idleMcAnimation, deltaTime);

	glm::vec2 cameraTopLeft = camera.position - viewportSizeInWorld * 0.5f;
	glm::vec2 cameraBottomRight = camera.position + viewportSizeInWorld * 0.5f;
	glm::vec2 cameraCenter = (cameraTopLeft + cameraBottomRight) * 0.5f;

	std::cout << "Camera Center: " << cameraCenter.x << ", " << cameraCenter.y << std::endl;
	std::cout << "Player Position: " << gameData.rectPos.x << ", " << gameData.rectPos.y << std::endl;
	std::cout << "Camera View: (" << cameraTopLeft.x << ", " << cameraTopLeft.y << ") to ("
		<< cameraBottomRight.x << ", " << cameraBottomRight.y << ")" << std::endl;


	
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
