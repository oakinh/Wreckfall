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
float zoomLevel = 5.0f;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();
	//platform::setFullScreen(true);

	//loading the saved data. Loading an entire structure like this makes savind game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

	//t.loadFromFile(RESOURCES_PATH "test.jpg", true);
	idleMcSpriteSheet.loadFromFile(RESOURCES_PATH "mainCharacterIdleAnimation.png", true);
	

	int w = platform::getFrameBufferSizeX();
	int h = platform::getFrameBufferSizeY();

	glm::vec2 screenCenter = glm::vec2(w, h) / 2.0f;
	gameData.rectPos = screenCenter / zoomLevel - glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) / 2.0f;
	
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

	if (w != prevWindowWidth || h != prevWindowHeight)
	{
		prevWindowWidth = w;
		prevWindowHeight = h;

		// Re-center the character
		glm::vec2 screenCenter = glm::vec2(w, h) / (2.0f * zoomLevel);
		gameData.rectPos = screenCenter - glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) / 2.0f;
	}

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion
	gl2d::Camera camera;
	camera.zoom = zoomLevel;
	camera.position = gameData.rectPos + glm::vec2(idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y) / 2.0f; // Center the camera
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

	gameData.rectPos = glm::clamp(gameData.rectPos, glm::vec2{ 0,0 }, glm::vec2{ w - 100,h - 100 });

	//float uWidth = 1.0f / idleMcAnimation.totalFrames;
	//float uStart = uWidth * idleMcAnimation.currentFrame;
	//float uEnd = uStart + uWidth;
	
	updateAnimation(idleMcAnimation, deltaTime);

	int xCount = 33;	// Number of columns in the sprite sheet
	int yCount = 1;		// Number of rows
	int x = idleMcAnimation.currentFrame % xCount;
	int y = idleMcAnimation.currentFrame / xCount;
	glm::vec4 uvCoords = gl2d::computeTextureAtlas(xCount, yCount, x, y, false);

	// Render main character
	renderer.renderRectangle(
		gl2d::Rect{ gameData.rectPos.x, gameData.rectPos.y, idleMcAnimation.frameSize.x, idleMcAnimation.frameSize.y },
		idleMcSpriteSheet,
		gl2d::Color4f{ 1, 1, 1, 1 },
		glm::vec2{ 0, 0 },
		0,
		uvCoords
	);
	
	//renderer.renderRectangle({gameData.rectPos, 100, 100}, t);

	renderer.flush();


	//ImGui::ShowDemoWindow();
	ImGui::Begin("Zoom Control");
	ImGui::SliderFloat("Zoom level", &zoomLevel, 0.1f, 5.0f);

	//ImGui::DragFloat2("Positions", &gameData.rectPos[0]);

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
