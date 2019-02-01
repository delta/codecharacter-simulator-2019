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

enum FactoryState {
	FACTORY_UNBUILT = 0,
	FACTORY_IDLE = 1,
	FACTORY_VILLAGER_PRODUCTION = 2,
	FACTORY_SOLDIER_PRODUCTION = 3,
	FACTORY_DESTROYED = 4
};

/**
 * Entry for one factory in the logger
 */
struct FactoryLogEntry {
	int64_t id;
	int64_t hp;
	int build_percent;
	FactoryState state;
};

/**
 * Logger class that takes the current game state and logs information
 * Writes log to file system after game is complete
 */
class LOGGER_EXPORT Logger : public ILogger {
  private:
	/**
	 * State instance to get game information
	 */
	state::ICommandTaker *state;

	/**
	 * Number of turns since the start of the game
	 */
	int64_t turn_count;

	/**
	 * Maintains state of factories at the previous turn
	 */
	std::array<std::vector<FactoryLogEntry>, 2> factory_logs;

	/**
	 * Protobuf object holding complete game logs
	 */
	std::unique_ptr<proto::Game> logs;

	/**
	 * Stores the instruction counts until they are written into the log along
	 * with the remaininig state data, every turn
	 */
	std::vector<int64_t> instruction_counts;

	/**
	 * Map holding mapping of error strings to error codes
	 */
	std::unordered_map<std::string, int64_t> error_map;

	/**
	 * Holds an incrementing value to assign each error a unique code
	 */
	int64_t current_error_code;

	/**
	 * Holds the errors that occured in a particular move, indexed by player_id
	 */
	std::array<std::vector<int64_t>, 2> errors;

	/**
	 * Number of instructions exceeding which the turn is forfeit
	 */
	int64_t player_instruction_limit_turn;

	/**
	 * Number of instructions exceeding which the game is forfeit
	 */
	int64_t player_instruction_limit_game;

	/**
	 * Holds maximum Hp of soldier
	 */
	int64_t soldier_max_hp;

	/**
	 * Holds maximum Hp of villager
	 */
	int64_t villager_max_hp;

	/**
	 * Holds maximum Hp of factory
	 */
	int64_t factory_max_hp;

  public:
	/**
	 * Constructor for the Logger class
	 */
	Logger(state::ICommandTaker *state, int64_t player_instruction_limit_turn,
	       int64_t player_instruction_limit_game, int64_t soldier_max_hp,
	       int64_t villager_max_hp, int64_t factory_max_hp);

	/**
	 * @see ILogger#LogState
	 */
	void LogState() override;

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
