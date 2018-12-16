/**
 * @file path_planner.h
 * Declares the PathPlanner class
 */

#pragma once

#include "state/map/map.h"
#include "state/path_planner/interfaces/i_path_planner.h"

namespace state {

class PathPlanner : public IPathPlanner {
	/**
	 * Map instance to determine terrain
	 */
	Map *map;

  public:
	PathPlanner(Map *map);

	/**
	 * @see IPathPlanner#GetNextPosition
	 */
	Vec2D GetNextPosition(Vec2D source, Vec2D destination, int64_t speed);
};

} // namespace state
