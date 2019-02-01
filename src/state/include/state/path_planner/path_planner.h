/**
 * @file path_planner.h
 * Declares the PathPlanner class, and helper utilities
 */

#pragma once

#include "state/map/map.h"
#include "state/path_planner/interfaces/i_path_planner.h"
#include "state/path_planner/path_graph.h"

namespace state {

class PathPlanner : public IPathPlanner {
	/**
	 * Map instance to determine terrain
	 */
	Map *map;

	/**
	 * PathGraph class handles all path calculation algorithms
	 */
	PathGraph path_graph;

  public:
	PathPlanner(Map *map);

	/**
	 * @see IPathPlanner#GetNextPosition
	 */
	DoubleVec2D GetNextPosition(DoubleVec2D source, DoubleVec2D destination,
	                            int64_t speed) override;
};

} // namespace state
