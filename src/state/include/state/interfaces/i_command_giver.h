/**
 * @file i_command_giver.h
 * Interface for class that calls command taker methods
 */

#pragma once

#include "state/interfaces/i_command_taker.h"
#include "state/player_state.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * ICommandGiver interface to call command taker methods
 */
class STATE_EXPORT ICommandGiver {
  public:
	virtual ~ICommandGiver() {}

	/**
	 * Runs the necessary commands on the command taker (state)
	 *
	 * @param[in] player_states Player state from which we get commands to run
	 * @param[in] skip_player_turn If true for a player, turn is not processed
	 */
	virtual void
	RunCommands(const std::array<player_state::State, 2> &player_states,
	            std::array<bool, 2> skip_player_turn) = 0;
};

} // namespace state
