
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
     * @param[in] state main game state that to which commands must be give
     * @param[in] player_states Player state from which we get commands to run
     */
    virtual void
    RunCommands(ICommandTaker *state,
                const std::array<player_state::State, 2> &player_states) = 0;
};

} // namespace state