/**
 * @file map.h
 * Declares the Map class that specifies the game map terrain
 */

#pragma once

#include "state/utilities.h"

#include <cstddef>
#include <vector>

namespace state {

/**
 * Map class that holds a 2D matrix of terrain elements, with the map data
 */
class Map {
  private:
	/**
	 * 2D vector containing terrain types for the map
	 */
	std::vector<std::vector<TerrainType>> map;

	/**
	 * Map size, the number of grids per side on the map
	 */
	size_t map_size;

	/**
	 * Element size, the width of each tile
	 */
	size_t element_size;

  public:
	/**
	 * Constructor
	 *
	 * @param element_size
	 */
	Map(std::vector<std::vector<TerrainType>> map, size_t map_size,
	    size_t element_size);

	/**
	 * Get the size of the map (returns the template param)
	 *
	 * @return Width of the map in grid elements
	 */
	size_t GetSize() const;

	/**
	 * Get the size of one element of the map
	 *
	 * @return Size of one map element
	 */
	size_t GetElementSize() const;

	/**
	 * Get the Terrain type of an element at a particular grid offset
	 *
	 * @param x
	 * @param y
	 * @return the tile's terrain type
	 */
	TerrainType GetTerrainTypeByOffset(int64_t x, int64_t y) const;

	/**
	 * Get the Terrain type of the element from any position on the map
	 *
	 * @param x
	 * @param y
	 * @return the tile's terrain type
	 */
	TerrainType GetTerrainTypeByPosition(int64_t x, int64_t y) const;
};

} // namespace state

