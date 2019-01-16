/**
 * @file path_graph.cpp
 * Defines the PathGraph class
 */

#include "state/path_planner/path_graph.h"

#include <algorithm>

namespace state {

PathGraph::PathGraph() {}

PathGraph::PathGraph(Matrix<bool> graph, size_t element_size)
    : graph(graph), size(graph.size()), element_size(element_size) {

	// Compute paths for all nodes
	GeneratePathCache();
}

template <typename T> inline void printVector(const std::vector<T> &vec) {
	using std::cout;
	using std::endl;
	for (const auto &elem : vec) {
		cout << elem << endl;
	}
	cout << endl;
}

inline DoubleVec2D GetTileCenter(Vec2D offset, int64_t element_size) {
	auto position = DoubleVec2D{};
	position.x = offset.x * element_size + (element_size / 2);
	position.y = offset.y * element_size + (element_size / 2);
	return position;
}

Vec2D PathGraph::GetNextNode(Vec2D source, Vec2D destination) {
	// If source or destination are out of bounds...
	if (source.x < 0 || source.x >= size || source.y < 0 || source.y >= size ||
	    destination.x < 0 || destination.x >= size || destination.y < 0 ||
	    destination.y >= size) {
		return Vec2D::null;
	}

	// If source or destination are invalid locations (water)...
	if (not graph[source.x][source.y] ||
	    not graph[destination.x][destination.y]) {
		return {};
	}

	// If the source and destination are the same...
	if (source == destination) {
		return {};
	}

	// Build the path
	auto source_matrix = GetAt(path_cache, destination);
	auto next_node = GetAt(source_matrix, source);

	auto path = std::vector<Vec2D>{source, next_node};
	while (next_node != destination) {
		next_node = GetAt(source_matrix, next_node);
		path.push_back(next_node);
	}

	auto smoothed_path = SmoothenPath(path);

	return smoothed_path.at(1);
}

std::vector<Vec2D> PathGraph::GetNeighbours(Vec2D offset) {
	auto neighbours = std::vector<Vec2D>{};
	neighbours.reserve(4);

	auto rel_offsets = std::vector<Vec2D>{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
	auto current_offset = Vec2D::null;

	// For each of the 4 positions around the current offset...
	for (const auto &rel_offset : rel_offsets) {
		auto current_offset = offset + rel_offset;

		// If the location is within bounds and a valid location...
		if (current_offset.x >= 0 && current_offset.x < size &&
		    current_offset.y >= 0 && current_offset.y < size &&
		    graph[current_offset.x][current_offset.y]) {
			neighbours.push_back(current_offset);
		}
	}

	return neighbours;
}

std::vector<Vec2D> PathGraph::SmoothenPath(const std::vector<Vec2D> &path) {
	if (path.size() == 1 || path.size() == 2) {
		return path;
	}

	// The source node is always a concrete part of the path
	// Take that as the first smoothened node
	auto smoothed_path = std::vector<Vec2D>{path.at(0)};

	auto current_elem = path.at(0);
	auto prev_elem = Vec2D::null;

	// For each element (except the dest) in the path after the source...
	for (int i = 1; i < path.size() - 1; ++i) {

		// Set the previous element and get the new current element
		prev_elem = current_elem;
		current_elem = path.at(i);

		// If there's no straight path (i.e. walkable) between the last node in
		// the smoothed path and the current node, then the previously checked
		// node is a necessary waypoint in the smoothed path
		if (not Walkable(smoothed_path.back(), current_elem)) {
			smoothed_path.push_back(prev_elem);
		}
	}

	// The destination will always be part of the smoothed path
	smoothed_path.push_back(path.back());

	return smoothed_path;
}

bool PathGraph::Walkable(Vec2D source_int, Vec2D destination_int) {
	// Take the source and destination as the tile centers
	auto source = GetTileCenter(source_int, element_size);
	auto destination = GetTileCenter(destination_int, element_size);

	// Find the direction vector, and the unit direction vector
	auto direction = destination - source;
	auto unit_direction = direction / direction.magnitude();

	// Set a sampling size, with which we step along the path to test for water
	auto sample_size = element_size / 5;
	auto sample_point = source;

	while (true) {
		if (sample_point.distance(destination) < sample_size) {
			// We're at the destination. Everything is clear.
			break;
		}

		// Take another step towards the destination
		sample_point = sample_point + (unit_direction * sample_size);

		// If we are on water, this path is not walkable. Else, continue...
		auto sample_offset = (sample_point / element_size).to_int();
		auto is_land = graph[sample_offset.x][sample_offset.y];
		if (not is_land) {
			// The path is not clear!
			return false;
		}
	}

	return true;
}

} // namespace state
