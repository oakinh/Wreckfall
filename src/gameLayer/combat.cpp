#include <glm/glm.hpp>
#include <cmath>
#include "combat.h"

template <typename T>
void takeDamage(T& entity, int damage) {
	entity.health - damage;
}

template <typename T>
int calculateBulletDamage(T& target, Weapon weapon, glm::vec2 mouseWorldPos) {
	constexpr minimumDamage{ 5 };
	const int distance { static_cast<int>(std::round(glm::distance(target.position, mouseWorldPos))) };
	const int damage { damageMap[weapon] - distance }
	return (damage > 0) ? damage : minimumDamage; // We don't want to inadvertently give health to the target entity
}

template <typename T>
T& checkAndHandleBulletCollisions(const GameData& gameData, )