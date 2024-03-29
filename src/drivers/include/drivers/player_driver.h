/**
 * @file player_driver.h
 * Declaration for player driver class that drives the game
 */

#pragma once

#include "drivers/drivers_export.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "player_wrapper/player_code_wrapper.h"
#include "state/player_state.h"
#include <atomic>
#include <cstdint>
#include <memory>

namespace drivers {

/**
 * Drives a player's AI code
 */
class DRIVERS_EXPORT PlayerDriver {
  private:
	/**
	 * Number of LLVM IR instructions executed by the player
	 */
	static std::atomic<uint64_t> instruction_count;

	/**
	 * An instance of the player code wrapper
	 */
	std::unique_ptr<player_wrapper::PlayerCodeWrapper> player_code_wrapper;

	/**
	 * Shared memory of the player to write count to
	 */
	std::unique_ptr<SharedMemoryPlayer> shm_player;

	/**
	 * Access pointer to the shared memory
	 */
	SharedBuffer *shared_buffer;

	/**
	 * Number of turns in the game
	 */
	int64_t max_no_turns;

	/**
	 * true if the game has timed out without completing and Start has been
	 * called, false otherwise
	 */
	std::atomic_bool is_game_timed_out;

	/**
	 * Timer for the game
	 */
	Timer game_timer;

	/**
	 * Time limit for the game.
	 *
	 * Game must be completed within this time to be considered valid.
	 */
	Timer::Interval game_duration;

	/**
	 * File path to player's debug logs
	 */
	std::string player_debug_log_file;

	/**
	 * Debug logs of player, dumped into a debug log file at the end of the
	 * game
	 */
	std::ostringstream player_debug_logs;

	/**
	 * Used as delimiter between turns in player's debug logs
	 */
	std::string debug_logs_turn_prefix;

	/**
	 * Message to use in logs when player exceeds debug log limit per turn
	 */
	std::string debug_logs_truncate_message;

	/**
	 * Maximum number of characters allowed in player's debug logs per turn
	 */
	int64_t max_debug_logs_turn_length;

	/**
	 * Writes the count to shared memory
	 */
	void WriteCountToShm();

	/**
	 * Blocking function that runs the player's code
	 */
	void Run();

  public:
	/**
	 * Constructor
	 *
	 * @param[in]  player_code_wrapper          The player code wrapper
	 * @param[in]  shm_player                   The player's shared memory to
	 *                                          write the count to
	 * @param[in]  max_no_turns                 The number of turns in the game
	 * @param[in]  game_duration                The time limit for the game
	 * @param[in]  player_debug_log_file        The player debug log file path
	 * @param[in]  debug_logs_turn_prefix       Prefix for debug log every turn
	 * @param[in]  debug_logs_truncate_message  Message in debug logs if player
	 *                                          exceeded per turn limit
	 * @param[in]  max_debug_logs_turn_length   Maxiumum length of debug logs
	 *                                          per turn
	 */
	PlayerDriver(
	    std::unique_ptr<player_wrapper::PlayerCodeWrapper> player_code_wrapper,
	    std::unique_ptr<SharedMemoryPlayer> shm_player, int64_t max_no_turns,
	    Timer::Interval game_duration, std::string player_debug_log_file,
	    std::string debug_logs_turn_prefix,
	    std::string debug_logs_truncate_message,
	    int64_t max_debug_logs_turn_length);

	/**
	 * Increment instruction_count by count
	 *
	 * @param  count  The count
	 */
	static void IncrementCount(uint64_t count);

	/**
	 * Gets the instruction_count
	 *
	 * @return     The count.
	 */
	uint64_t GetCount();

	/**
	 * Starts the player's AI code in a loop
	 *
	 * Blocks until the game is over
	 */
	void Start();
};
} // namespace drivers
