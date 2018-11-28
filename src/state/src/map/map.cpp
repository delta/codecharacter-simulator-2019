/**
 * @file map.cpp
 * Defines the Map class
 */

#include "state/map/map.h"

#include <vector>

namespace state {

Map::Map(std::vector<std::vector<TerrainType>> map, size_t map_size,
         size_t element_size)
    : map(map), map_size(map_size), element_size(element_size) {}

size_t Map::GetSize() { return map_size; }
size_t Map::GetElementSize() { return element_size; }

TerrainType Map::GetTerrainTypeByOffset(int64_t x, int64_t y) {
	return map[x][y];
}

TerrainType Map::GetTerrainTypeByPosition(int64_t x, int64_t y) {
	auto offset_x = x / element_size;
	auto offset_y = y / element_size;

	return map[offset_x][offset_y];
}

} // namespace state
