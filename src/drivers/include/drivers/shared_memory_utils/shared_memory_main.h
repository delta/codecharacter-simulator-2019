/**
 * @file shared_memory_main.h
 * Declaration for owner of shared memory
 */

#pragma once

#define BOOST_DATE_TIME_NO_LIB

#include "boost/interprocess/managed_shared_memory.hpp"
#include "drivers/drivers_export.h"
#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/transfer_state.h"

namespace drivers {

/**
 * Wrapper for shared memory owner
 */
class DRIVERS_EXPORT SharedMemoryMain {
  private:
	/**
	 * Name associated with this shared memory
	 */
	std::string shared_memory_name;

	/**
	 * Mapped region to write to and read from
	 */
	boost::interprocess::managed_shared_memory shared_memory;

  public:
	/**
	 * Creates new shm with given name
	 *
	 * @param[in]  shared_memory_name  The shared memory name
	 *
	 * @throw      std::exception      If shm already exists
	 */
	SharedMemoryMain(std::string shared_memory_name, bool is_player_running,
	                 bool is_game_complete, int64_t instruction_counter,
	                 const transfer_state::State &transfer_state);

	/**
	 * Removes shm
	 */
	~SharedMemoryMain();

	/**
	 * Gets pointer to shared memory
	 *
	 * @return     The pointer
	 */
	SharedBuffer *GetBuffer();
};
} // namespace drivers
