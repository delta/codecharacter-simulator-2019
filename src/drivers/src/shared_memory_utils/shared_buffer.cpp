/**
 * @file shared_buffer.cpp
 * Definitions for buffer for shared memory
 */

#include "drivers/shared_memory_utils/shared_buffer.h"

namespace drivers {

SharedBuffer::SharedBuffer(bool is_player_running, bool is_game_complete,
                           int64_t instruction_counter,
                           const transfer_state::State &transfer_state)
    : is_player_running(is_player_running), is_game_complete(is_game_complete),
      instruction_counter(instruction_counter), transfer_state(transfer_state) {
}
} // namespace drivers
