/**
 * @file player_result.h
 * Final result of a player
 */
#pragma once

#include "drivers/drivers_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <iostream>
#include <vector>

namespace drivers {

/**
 * Final result of a player
 */
struct DRIVERS_EXPORT PlayerResult {
	/**
	 * Status of the player
	 */
	enum class Status {
		/**
		 * The player's result is not valid (possible due to game timeout)
		 */
		UNDEFINED,

		/**
		 * Normal result
		 */
		NORMAL,

		/**
		 * The player has exceeded the game instruction limit and thus
		 * forfeited the game
		 */
		EXCEEDED_INSTRUCTION_LIMIT,

		/**
		 * The player's code had a runtime error
		 */
		RUNTIME_ERROR,

		/**
		 * A player took too long to finish their turn
		 */
		TIMEOUT
	};

	/**
	 * The score of the player
	 */
	int64_t score;

	/**
	 * The status of the player
	 */
	Status status;
};

DRIVERS_EXPORT inline std::ostream &
operator<<(std::ostream &ostream, const PlayerResult::Status &status) {
	switch (status) {
	case PlayerResult::Status::UNDEFINED:
		ostream << "UNDEFINED";
		break;
	case PlayerResult::Status::NORMAL:
		ostream << "NORMAL";
		break;
	case PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT:
		ostream << "EXCEEDED_INSTRUCTION_LIMIT";
		break;
	case PlayerResult::Status::RUNTIME_ERROR:
		ostream << "RUNTIME_ERROR";
		break;
	case PlayerResult::Status::TIMEOUT:
		ostream << "TIMEOUT";
		break;
	}
	return ostream;
}
} // namespace drivers
