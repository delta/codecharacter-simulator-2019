/**
 * @file logger.h
 * Declarations for the logger class
 */

#pragma once

#include "game.pb.h"
#include "logger/error_type.h"
#include "logger/interfaces/i_logger.h"
#include "logger/logger_export.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_command_taker.h"

#include <cstdint>
#include <ostream>

namespace logger {

/**
 * Logger class that takes the current game state and logs information
 * Writes log to file system after game is complete
 */
class LOGGER_EXPORT Logger : public ILogger {
  private:
	/**
	 * Number of instructions exceeding which the turn is forfeit
	 */
	int64_t player_instruction_limit_turn;

	/**
	 * Number of instructions exceeding which the game is forfeit
	 */
	int64_t player_instruction_limit_game;

  public:
	/**
	 * Constructor for the Logger class
	 */
	Logger(int64_t player_instruction_limit_turn,
	       int64_t player_instruction_limit_game);

	/**
	 * @see ILogger#LogState
	 */
	void LogState(state::ICommandTaker *state) override;

	/**
	 * @see ILogger#LogInstructionCount
	 */
	void LogInstructionCount(state::PlayerId player_id, int64_t count) override;

	/**
	 * @see ILogger#LogError
	 */
	void LogError(state::PlayerId player_id, ErrorType error_type,
	              std::string message) override;

	/**
	 * @see ILogger#LogFinalGameParams
	 */
	void LogFinalGameParams() override;

	/**
	 * @see ILogger#WriteGame
	 * Defaults to std::cout when no stream passed
	 */
	void WriteGame(std::ostream &write_stream = std::cout) override;
};
} // namespace logger
