/**
 * @file path_planner.cpp
 * Defines the PathPlanner class
 */

#include "state/path_planner/path_planner.h"
#include "state/path_planner/matrix.h"

#include <algorithm>
#include <cmath>

namespace state {

PathPlanner::PathPlanner(Map *map) : map(map) {
	auto map_size = map->GetSize();
	auto element_size = map->GetElementSize();

	// Create a map copy with bools. Land => true, Water => false
	auto map_graph = Matrix<bool>{};
	map_graph.resize(map_size);
	for (int i = 0; i < map_size; ++i) {
		map_graph[i].resize(map_size);
		for (int j = 0; j < map_size; ++j) {
			switch (map->GetTerrainTypeByOffset(i, j)) {
			case TerrainType::LAND:
			case TerrainType::GOLD_MINE:
				map_graph[i][j] = true;
				break;
			case TerrainType::WATER:
				map_graph[i][j] = false;
				break;
			}
		}
	}

	path_graph = PathGraph(map_graph, element_size);
}

DoubleVec2D PathPlanner::GetNextPosition(DoubleVec2D source,
                                         DoubleVec2D destination,
                                         int64_t speed) {
	auto map_size = map->GetSize();
	auto element_size = map->GetElementSize();

	// Final position to move to, end result
	auto new_position = DoubleVec2D::null;

	// If the destination is in reach...
	if (source.distance(destination) <= speed) {
		new_position = destination;
	} else {
		// Convert to offsets
		auto start_offset = (source / element_size).to_int();
		auto target_offset = (destination / element_size).to_int();

		// Find the next offset to go to
		auto next_offset = Vec2D::null;
		if (start_offset == target_offset) {
			next_offset = start_offset;
		} else {
			next_offset = path_graph.GetNextNode(start_offset, target_offset);
		}

		// If no valid path exists...
		if (next_offset == Vec2D::null) {
			return DoubleVec2D::null;
		}

		// Use that offset to find a target position
		auto next_dest = DoubleVec2D::null;

		// If we're close to the destination (at most a grid away)...
		if (next_offset == target_offset) {
			// Move directly towards the destination, and not the tile center
			next_dest = destination;
		} else {
			// Convert next_dest to position from offset, at the tile center
			next_dest =
			    (next_offset * element_size).to_double() + (element_size / 2);
		}

		// Find the unit vector along the direction of next_dest
		auto direction_vector = next_dest - source;
		auto unit_vector = direction_vector / direction_vector.magnitude();

		// Multiply speed with the unit vector to get the displacement vector.
		// Add the displacement with the current position to get position
		new_position = source + (unit_vector * speed);
	}

	return new_position;
}

} // namespace state
