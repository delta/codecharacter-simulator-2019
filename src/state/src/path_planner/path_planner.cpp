/**
 * @file path_planner.cpp
 * Defines the PathPlanner class
 */

#include "state/path_planner/path_planner.h"

namespace state {

PathPlanner::PathPlanner(Map *map) : map(map) {}

Vec2D PathPlanner::GetNextPosition(Vec2D source, Vec2D destination,
                                   int64_t speed) {
	// TODO
	return Vec2D{};
}

}; // namespace state
