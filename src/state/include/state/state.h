/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#ifndef STATE_STATE_H
#define STATE_STATE_H

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/utilities.h"

#include <memory>

namespace state {

class STATE_EXPORT State {
  private:
	/**
	 * Map instance that maintains game terrain
	 */
	std::unique_ptr<Map> map;

  public:
	/**
	 * Constructor
	 */
	State(std::unique_ptr<Map> map);
};
} // namespace state

#endif
