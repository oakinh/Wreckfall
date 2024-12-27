#pragma once
#include <mainCharacter.h>
#include <enemy.h>
#include <gun.h>

struct GameData
{
	Player player;
	Enemy snatcher;
	Gun gun;

};

extern GameData gameData;