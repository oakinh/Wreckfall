#include <unordered_map>

enum class Weapon {
	MainRifle,
};

namespace std {
	template <>
	struct hash<Weapon> {
		std::size_t operator()(const Weapon& w) const {
			return static_cast<std::size_t>(w);
		}
	};
}

std::unordered_map<Weapon, int> damageMap {
	{ Weapon::MainRifle, 30 },
}

template <typename T>
void takeDamage(T& entity, int damage) {
	damage = damage > 0 ? damage : 0;
	entity.health - damage;
}

template <typename T>
int calculateBulletDamage(T& target, Weapon weapon, glm::vec2 mouseWorldPos) {
	constexpr minimumDamage{ 5 };
	const int distance{ static_cast<int>(std::round(glm::distance(target.position, mouseWorldPos))) };
	const int damage{ damageMap[weapon] - distance }
	return (damage > 0) ? damage : minimumDamage; // We don't want to inadvertently give health to the target entity
}

template <typename T>
T& checkAndHandleBulletCollisions(const GameData& gameData, )