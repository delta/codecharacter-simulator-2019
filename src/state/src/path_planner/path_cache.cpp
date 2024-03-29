/**
 * @file path_cache.cpp
 * Defines path caching related methods for the PathGraph class
 */

#include "state/path_planner/path_graph.h"

#include <queue>

namespace state {

void PathGraph::GeneratePathCache() {
	path_cache = InitMatrix(InitMatrix(Vec2D::null, size), size);
	is_path_computed = InitMatrix(InitMatrix(false, size), size);

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			ComputeAllPathsFromNode(Vec2D{i, j});
		}
	}
}

void PathGraph::ComputeAllPathsFromNode(Vec2D node) {
	std::queue<Vec2D> queue;

	auto &current_path_cache = path_cache[node.x][node.y];
	current_path_cache[node.x][node.y] = node;

	auto visited = InitMatrix(false, size);

	// BFS All Nodes
	// Add the first node
	queue.push(node);
	visited[node.x][node.y] = true;

	// Variable to store the node currently being searched on
	Vec2D current;

	// While there are no new nodes to visit
	while (!queue.empty()) {
		// Get the next node from the queue
		current = queue.front();
		queue.pop();
		// Iterate through all the neighbours of the current node
		for (const auto &neighbour : GetNeighbours(current)) {
			if (visited[neighbour.x][neighbour.y])
				continue;
			// Visit the neighbour
			visited[neighbour.x][neighbour.y] = true;
			current_path_cache[neighbour.x][neighbour.y] = current;
			queue.push(neighbour);
		}
	}
}

// void PathGraph::GeneratePathCache() {
// 	path_cache = InitMatrix(InitMatrix(Vec2D::null, size), size);
// 	is_path_computed = InitMatrix(InitMatrix(false, size), size);

// 	// We generate all pairs of possible pairs of paths between any two nodes on
// 	// the map, and for each such pair, store the next node to travel to

// 	// For each source...
// 	for (int source_x = 0; source_x < size; ++source_x) {
// 		for (int source_y = 0; source_y < size; ++source_y) {
// 			auto source = Vec2D{source_x, source_y};

// 			// For each destination...
// 			for (int dest_x = size - 1; dest_x >= 0; --dest_x) {
// 				for (int dest_y = size - 1; dest_y >= 0; --dest_y) {
// 					auto dest = Vec2D{dest_x, dest_y};

// 					// Skip if already computed
// 					if (is_path_computed[source.x][source.y][dest.x][dest.y]) {
// 						continue;
// 					}

// 					// Compute path
// 					auto path = GetPath(source, dest);
// 					path.push_back(source);
// 					AddPathToCache(path, 0, path.size() - 1);
// 				}
// 			}
// 		}
// 	}
// }

// void PathGraph::AddPathToCache(std::vector<Vec2D> &path, size_t start_i,
//                                size_t end_i) {
// 	// If there's just a single node...
// 	if (start_i == end_i) {
// 		auto elem = path[start_i];
// 		path_cache[elem.x][elem.y][elem.x][elem.y] = elem;
// 		is_path_computed[elem.x][elem.y][elem.x][elem.y] = true;
// 	} else {
// 		auto source = path[start_i];
// 		auto dest = path[end_i];

// 		// Skip if already computed
// 		if (is_path_computed[source.x][source.y][dest.x][dest.y]) {
// 			return;
// 		}

// 		// Set next nodes
// 		path_cache[source.x][source.y][dest.x][dest.y] = path[start_i + 1];
// 		path_cache[dest.x][dest.y][source.x][source.y] = path[end_i - 1];

// 		// Flag as computed
// 		is_path_computed[source.x][source.y][dest.x][dest.y] = true;
// 		is_path_computed[dest.x][dest.y][source.x][source.y] = true;

// 		// Recursively set all subpaths
// 		AddPathToCache(path, start_i, end_i - 1);
// 		AddPathToCache(path, start_i + 1, end_i);
// 	}
// }

} // namespace state
