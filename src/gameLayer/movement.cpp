#include <tile.h>
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <iostream>

struct Node {
	glm::ivec2 position;    // Grid position
	float g = 0;            // Cost from start to current node
	float h = 0;            // Heuristic cost to target
	float f = 0;            // Total cost (f = g + h)
	Node* parent = nullptr; // Parent node for path reconstruction

	Node(int x, int y) : position(x, y) {}

	bool operator==(const Node& other) const {
		return position == other.position;
	}
};

struct CompareNode {
	bool operator()(Node* a, Node* b) {
		return a->f > b->f;
	}
};

// A* heuristic: Diagonal distance
float heuristic(glm::ivec2 start, glm::ivec2 target) {
	int dx = std::abs(start.x - target.x);
	int dy = std::abs(start.y - target.y);
	return static_cast<float>(std::max(dx, dy) + (std::sqrt(2.0f) - 1) * std::min(dx, dy));
}

// Check if a tile is valid and passable
// Typical row-major layor => map[row][col] => map[y][x]
bool isValidTile(const glm::ivec2& pos, const std::vector<std::vector<Tile>>& map) {
	/*return pos.x >= 0 && pos.y >= 0 &&
		pos.x < static_cast<int>(map.size()) &&
		pos.y < static_cast<int>(map[0].size()) &&
		map[pos.x][pos.y].passable;*/
	return pos.y >= 0 && pos.x >= 0 &&
		pos.y < static_cast<int>(map.size()) &&
		pos.x < static_cast<int>(map[0].size()) &&
		map[pos.y][pos.x].passable; // row=y, col=x
}

std::vector<MovementDirection> reconstructPath(Node* target, const glm::ivec2& start) {
	std::vector<MovementDirection> path;
	Node* current = target;

	while (current->parent != nullptr) {
		std::cout << "Current->parent != nullptr" << std::endl;
		glm::ivec2 diff = current->position - current->parent->position;

		for (int i = 0; i < 8; ++i) {
			if (diff == directionInts[i]) {
				path.push_back(static_cast<MovementDirection>(i));
				break;
			}
		}

		current = current->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

// A* algorithm
std::vector<MovementDirection> aStar(
	const Map& map,
	const glm::ivec2& start,
	const glm::ivec2& target
) {
	std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
	std::unordered_map<int, Node*> allNodes;

	auto getNode = [&](const glm::ivec2& pos) -> Node* {
		int key = pos.y * static_cast<int>(map[0].size()) + pos.x;
		if (allNodes.find(key) == allNodes.end()) {
			Node* newNode = new Node(pos.x, pos.y);
			newNode->g = std::numeric_limits<float>::infinity();
			newNode->h = 0.0f;
			newNode->f = std::numeric_limits<float>::infinity();
			allNodes[key] = newNode;
		}
		return allNodes[key];
		};

	Node* startNode = getNode(start);
	startNode->g = 0;
	startNode->h = heuristic(start, target);
	startNode->f = startNode->g + startNode->h;
	openSet.push(startNode);

	while (!openSet.empty()) {
		//std::cout << "Open set isn't empty, running logic" << std::endl;
		Node* current = openSet.top();
		openSet.pop();

		if (current->position == target) {
			std::cout << "current->position == target" << std::endl;
			std::vector<MovementDirection> path = reconstructPath(current, start);
			for (auto& [_, node] : allNodes) {
				delete node;
			}
			std::cout << "Returning path..." << std::endl;
			return path;
		}

		for (int i = 0; i < 8; i++) {
			glm::ivec2 neighborPos = current->position + glm::ivec2(directionInts[i]);

			if (isValidTile(neighborPos, map)) {
				Node* neighbor = getNode(neighborPos);
				float movementCost = (directionInts[i].x != 0 && directionInts[i].y != 0) ? std::sqrt(2.0f) : 1.0f;

				float tentativeG = current->g + movementCost;

				//if (tentativeG < neighbor->g || neighbor->g == 0) {
				if (tentativeG < neighbor->g) {
					neighbor->g = tentativeG;
					neighbor->h = heuristic(neighborPos, target);
					neighbor->f = neighbor->g + neighbor->h;
					neighbor->parent = current;
					openSet.push(neighbor);
				}
			}
		}
	}

	for (auto& [_, node] : allNodes) {
		delete node;
	}
	//std::cout << "No path found" << std::endl;
	return {}; // No path found
}