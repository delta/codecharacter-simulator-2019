/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "state/interfaces/i_command_taker.h"

#include <string>

using namespace state;

namespace logger {

Logger::Logger(int64_t player_instruction_limit_turn,
               int64_t player_instruction_limit_game)
    : player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game) {}

void Logger::LogState(ICommandTaker *state) {
	// TODO: Log the state into an in memory proto buffer
}

void Logger::LogInstructionCount(PlayerId player_id, int64_t count) {
	// TODO: Log the instruction counts to proto buffer
}

void Logger::LogError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
	// TODO: Log errors to proto buffer
}

void Logger::LogFinalGameParams() {
	// TODO: Write error map to logs
}

void Logger::WriteGame(std::ostream &write_stream) {
	// TODO: Write serialized game to stream
}
} // namespace logger
