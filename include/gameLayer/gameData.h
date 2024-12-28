#pragma once
#include <mainCharacter.h>
#include <enemy.h>
#include <gun.h>
#include <array>
#include <bullet.h>

struct GameData
{
	Player player;
	Enemy snatcher;
	Gun gun;
	static const int maxBullets = 50;
	std::array<Bullet, maxBullets> bullets;

};

extern GameData gameData;