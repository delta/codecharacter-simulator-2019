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
	virtual void UpdateMainState(
	    const std::array<player_state::State, 2> &player_states) = 0;

	/**
	 * Method to update the two player state instances with the new values from
	 * the updates state.
	 *
	 * @param[inout] player_states Reference to two player states
	 */
	virtual void UpdatePlayerStates(
	    std::array<player_state::State, 2> &player_states) = 0;

	/**
	 * Check if the game is over
	 *
	 * @param[out] winner If the game is over, who is the winner
	 *             PLAYER1 if Player 1 wins
	 *             PLAYER2 if Player 2 wins
	 *             PLAYER_NULL if it's a draw (both teams die simultaneously)
	 *
	 * @return true If the game is over
	 * @return false If the game is not over
	 */
	virtual bool IsGameOver(PlayerId &winner) = 0;

	/**
	 * Method to get both players' scores
	 */
	virtual std::array<int64_t, 2> GetScores() = 0;

	/**
	 * Returns game interestingness factor
	 */
	virtual int64_t GetInterestingness() = 0;
};

} // namespace state
