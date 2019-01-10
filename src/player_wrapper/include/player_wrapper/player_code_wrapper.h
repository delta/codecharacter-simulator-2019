/**
 * @file player_code_wrapper.h
 * Declaration for class that wraps the players' code
 */

#pragma once

#include "drivers/transfer_state.h"
#include "player_wrapper/interfaces/i_player_code.h"
#include <memory>

namespace player_wrapper {

/**
 * Class that wraps the players' code
 */
class PLAYER_WRAPPER_EXPORT PlayerCodeWrapper {
  private:
	/**
	 * An instance of the player code
	 */
	std::unique_ptr<IPlayerCode> player_code;

  public:
	/**
	 * Constructor
	 *
	 * @param      player_code  The player code
	 */
	PlayerCodeWrapper(std::unique_ptr<IPlayerCode> player_code);

	/**
	 * Runs the player's update and returns the player's debug logs
	 *
	 * @return     The debug logs
	 */
	std::string Update(transfer_state::State &transfer_state);
};
} // namespace player_wrapper
