#include <tile.h>

std::vector<MovementDirection> aStar(
	const Map& map,
	const glm::ivec2& start,
	const glm::ivec2& target
);