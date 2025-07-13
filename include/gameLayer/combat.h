#include <unordered_map>

enum class Weapons {
	MainRifle,
};

namespace std {
	template <>
	struct hash<Weapons> {
		std::size_t operator()(const Weapons& w) const {
			return static_cast<std::size_t>(w);
		}
	};
}

std::unordered_map<Weapons, int> DamageMap {
	{ Weapons::MainRifle, 30 },
}