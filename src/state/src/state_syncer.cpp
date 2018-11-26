/**
 * @file state_syncer.cpp
 * Defines the StateSyncer class
 */

#include "state/state_syncer.h"

namespace state {

StateSyncer::StateSyncer(ICommandGiver *command_giver, ICommandTaker *state)
    : command_giver(command_giver), state(state) {}

void StateSyncer::UpdateMainState(
    std::array<player_state::State, 2> &player_states) {

	// Call the CommandGiver
	command_giver->RunCommands(this->state, player_states);

	// Update main state
	state->Update();
}

void StateSyncer::UpdatePlayerStates(
    std::array<player_state::State, 2> &player_states) {

	// TODO: Get information from main state and build new player states
}

} // namespace state
