/**
 * @file i_state_syncer.h
 * Declares the StateSyncer class
 */

#ifndef STATE_STATE_SYNCER_H
#define STATE_STATE_SYNCER_H

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
	    std::array<player_state::State, 2> &player_states) override;

	/**
	 * @see IStateSyncer#UpdatePlayerStates
	 */
	void UpdatePlayerStates(
	    std::array<player_state::State, 2> &player_states) override;
};

} // namespace state

#endif
