/**
 * @file open_list.h
 * Declares an entry class to store node information in the A* algorithm
 */

#pragma once

#include "physics/vector.hpp"
#include "state/path_planner/matrix.h"

#include <cstdint>

namespace state {

struct OpenListEntry {
	/**
	 * Position that this entry represents
	 */
	Vec2D pos;

	/**
	 * Cost to get to this position
	 */
	double_t cost;

	/**
	 * Total cost to get to the goal
	 */
	double_t total_cost;

	/**
	 * The position before this one on the main path
	 */
	Vec2D parent;

	/**
	 * If true, this node is completely visited (is in the closed list)
	 */
	bool is_closed;

	/**
	 * The next element in the OpenList
	 */
	Vec2D next;

	/**
	 * The previous element in the OpenList
	 */
	Vec2D prev;

	/**
	 * Is this entry in use? Set if in open or closed lists.
	 * Data in the node is invalid if not set, so can be used to clear entry
	 */
	bool is_set;

	/**
	 * Convenient access to member is_set, to quickly evaluate the entry's
	 * validity in a boolean expression, for use like `if (this_entry) {}`
	 */
	explicit inline operator bool() const { return is_set; }
};

} // namespace state
