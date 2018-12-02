/**
 * @file i_state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_state_syncer.h"
#include "state/utilities.h"

#include <array>

namespace state {

class STATE_EXPORT StateSyncer : public IStateSyncer {

	/**
	 * Command giver instance that makes calls to update game state
	 */
	ICommandGiver *command_giver;

	/**
	 * State instance, to give commands to
	 */
	ICommandTaker *state;

  public:
	StateSyncer(ICommandGiver *command_giver, ICommandTaker *state);

	/**
	 * @see IStateSyncer#UpdateMainState
	 */
	void UpdateMainState(
	    const std::array<player_state::State, 2> &player_states) override;

	/**
	 * @see IStateSyncer#UpdatePlayerStates
	 */
	void UpdatePlayerStates(
	    std::array<player_state::State, 2> &player_states) override;

	/**
	 * @see IStateSyncer#GetScores
	 */
	std::array<int64_t, 2> GetScores() override;
};

} // namespace state
