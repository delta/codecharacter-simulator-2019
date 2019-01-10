/**
 * @file shared_buffer.h
 * Declaration for buffer object in shared memory
 */

#pragma once

#include "drivers/drivers_export.h"
#include "drivers/transfer_state.h"
#include <atomic>

namespace drivers {

/**
 * Struct for using as buffer in shared memory
 */
struct DRIVERS_EXPORT SharedBuffer {
	SharedBuffer(bool is_player_running, int64_t instruction_counter,
	             const transfer_state::State &transfer_state);

	/**
	 * True if the player process is executing its turn, false otherwise
	 */
	std::atomic_bool is_player_running;

	/**
	 * Count of the number of instructions executed in the present turn
	 */
	std::atomic<int64_t> instruction_counter;

	/**
	 * Player's copy of the state with limited information
	 */
	transfer_state::State transfer_state;
};
} // namespace drivers
