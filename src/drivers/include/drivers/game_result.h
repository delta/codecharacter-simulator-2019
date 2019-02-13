/**
 * @file game_result.h
 * Final result of the game
 */
#pragma once

#include "drivers/drivers_export.h"
#include "drivers/player_result.h"
#include "state/utilities.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

namespace drivers {

/**
 * Final result of a player
 */
struct DRIVERS_EXPORT GameResult {
	enum class Winner {
		/**
		 * Player 1 wins
		 */
		PLAYER1,

		/**
		 * Player 2 wins
		 */
		PLAYER2,

		/**
		 * The game is a draw
		 */
		TIE,

		/**
		 * Undefined
		 */
		NONE
	};

	enum class WinType {
		/**
		 * A player wins by killing all enemy units
		 */
		DEATHMATCH,

		/**
		 * A player wins by having a higher score than the opponent
		 */
		SCORE,

		/**
		 * The game ended by one or both of the players exceeding the
		 * instruction limit
		 */
		EXCEEDED_INSTRUCTION_LIMIT,

		/**
		 * Undefined
		 */
		NONE
	};

	/**
	 * Who won the game
	 */
	Winner winner;

	/**
	 * How did they win the game
	 */
	WinType win_type;

	/**
	 * Metric of how exciting a game is
	 */
	int64_t interestingness;

	/**
	 * Player results from both players
	 */
	std::array<PlayerResult, 2> player_results;
};

DRIVERS_EXPORT inline std::ostream &
operator<<(std::ostream &os, const GameResult::Winner winner) {
	switch (winner) {
	case GameResult::Winner::PLAYER1:
		os << "PLAYER1";
		break;
	case GameResult::Winner::PLAYER2:
		os << "PLAYER2";
		break;
	case GameResult::Winner::TIE:
		os << "TIE";
		break;
	case GameResult::Winner::NONE:
		os << "NONE";
		break;
	}

	return os;
}

DRIVERS_EXPORT inline std::ostream &
operator<<(std::ostream &os, const GameResult::WinType win_type) {
	switch (win_type) {
	case GameResult::WinType::DEATHMATCH:
		os << "DEATHMATCH";
		break;
	case GameResult::WinType::SCORE:
		os << "SCORE";
		break;
	case GameResult::WinType::EXCEEDED_INSTRUCTION_LIMIT:
		os << "EXCEEDED_INSTRUCTION_LIMIT";
		break;
	case GameResult::WinType::NONE:
		os << "NONE";
		break;
	}

	return os;
}

DRIVERS_EXPORT inline std::ostream &operator<<(std::ostream &os,
                                               const GameResult game_result) {

	// Write Winner, Win Type, and Interestingness
	os << game_result.winner << " " << game_result.win_type << " "
	   << game_result.interestingness << " ";

	// Write Results
	auto &pr = game_result.player_results;
	os << pr[0].score << " " << pr[0].status << " " << pr[1].score << " "
	   << pr[1].status;

	return os;
}
} // namespace drivers
