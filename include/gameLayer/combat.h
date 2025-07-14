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