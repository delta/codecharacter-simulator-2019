/**
 * @file command_giver.cpp
 * Defines the CommandGiver class
 */

#include "state/command_giver.h"
#include "state/interfaces/i_command_taker.h"

namespace state {

CommandGiver::CommandGiver() {}

void CommandGiver::RunCommands(
    ICommandTaker *state, std::array<player_state::State, 2> player_states) {

	// TODO:
	// Validate and evaluate player moves, make calls to ICommandTaker methods
}

} // namespace state
