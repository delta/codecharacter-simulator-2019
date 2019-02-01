/**
 * @file i_path_planner.h
 * Declares an interface for the Path Planner
 */

#pragma once

#include "physics/vector.hpp"

namespace state {

class IPathPlanner {
  public:
	virtual ~IPathPlanner() {}

	/**
	 * Get the Next Position to move to, given the current position, the
	 * destination, and movement speed
	 *
	 * @param source Current position
	 * @param destination Target position
	 * @param speed Movement speed in one frame
	 * @return DoubleVec2D Next position to move to
	 */
	virtual DoubleVec2D GetNextPosition(DoubleVec2D source,
	                                    DoubleVec2D destination,
	                                    int64_t speed) = 0;
};

} // namespace state
