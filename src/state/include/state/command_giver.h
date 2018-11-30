/**
 * @file command_giver.h
 * Command giver class that calls command taker methods
 */

#pragma once

#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * CommandGiver class to call command taker methods
 */
class STATE_EXPORT CommandGiver {
  public:
	CommandGiver();

	/**
	 * @see ICommandGiver#RunCommands
	 */
	void RunCommands(ICommandTaker *state,
	                 std::array<player_state::State, 2> player_states);
};

} // namespace state
