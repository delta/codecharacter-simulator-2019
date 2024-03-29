#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <stdexcept>

using namespace std;
using namespace drivers;

const string shm_name = "ShmTest";

void RemoveShm() {
	boost::interprocess::shared_memory_object::remove(shm_name.c_str());
}

TEST(SharedMemoryUtilsTest, ValidWriteRead) {
	RemoveShm();
	// Create owner of shm
	SharedMemoryMain shm_main(shm_name, false, false, 0,
	                          transfer_state::State());

	// Write to shm
	SharedBuffer *buf = shm_main.GetBuffer();
	buf->is_player_running = true;
	buf->is_game_complete = false;
	buf->instruction_counter = 1;

	// Spawn shm client to read from shm
	string command = "./shm_client " + shm_name;
	ASSERT_EQ(system(command.c_str()), 0) << "If you are not running these "
	                                         "tests from inside "
	                                         "<install_location>/bin, cd there";
}

TEST(SharedMemoryUtilsTest, InvalidRead) {
	RemoveShm();
	// Create client without owner
	EXPECT_THROW((SharedMemoryPlayer(shm_name)), std::exception);

	// Test if destructor for owner is working
	{
		SharedMemoryMain shm_main(shm_name, false, false, 0,
		                          transfer_state::State());
	}

	EXPECT_THROW((SharedMemoryPlayer(shm_name)), std::exception);
}

TEST(SharedMemoryUtilsTest, InvalidCreate) {
	RemoveShm();
	SharedMemoryMain shm_main(shm_name, false, false, 0,
	                          transfer_state::State());
	// Cannot have two owners
	EXPECT_THROW(
	    (SharedMemoryMain(shm_name, false, false, 0, transfer_state::State())),
	    std::exception);
}
