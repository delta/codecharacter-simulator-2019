/**
 * @file i_state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

#include "state/player_state.h"
#include "state/utilities.h"

#include <array>

namespace state {

class STATE_EXPORT IStateSyncer {
  public:
	virtual ~IStateSyncer() {}

	/**
	 * Method to call updates on the command taker after the player states
	 * have been processed.
	 *
	 * @param[in] player_states Reference to two player states
	 */
	virtual void
	    UpdateMainState(std::array<player_state::State, 2> &player_states) = 0;

	/**
	 * Method to update the two player state instances with the new values from
	 * the updates state.
	 *
	 * @param[inout] player_states Reference to two player states
	 */
	virtual void UpdatePlayerStates(
	    std::array<player_state::State, 2> &player_states) = 0;
};

} // namespace state
