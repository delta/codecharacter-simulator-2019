#include "constants/constants.h"
#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "player_code/player_code.h"
#include "player_wrapper/player_code_wrapper.h"
#include "simulator_constants/constants.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>

using namespace drivers;
using namespace player_wrapper;
using namespace player_code;

const std::string player_debug_log_ext = ".dlog";
const std::string debug_logs_turn_prefix =
    ">>>>>>>>>>>>>>>>>>>START OF TURN LOG<<<<<<<<<<<<<<<<<<<<\n";
const std::string debug_logs_truncate_message =
    "(logs truncated due to excessive size)\n";
const int64_t max_debug_logs_turn_length = 10000;

std::unique_ptr<PlayerDriver>
BuildPlayerDriver(std::string shm_name, std::string player_debug_log_file) {
	auto shm_player = std::make_unique<SharedMemoryPlayer>(shm_name);

	auto player_code_wrapper =
	    std::make_unique<PlayerCodeWrapper>(std::make_unique<PlayerCode>());

	return std::make_unique<PlayerDriver>(
	    std::move(player_code_wrapper), std::move(shm_player), NUM_TURNS,
	    Timer::Interval(GAME_DURATION_MS), player_debug_log_file,
	    debug_logs_turn_prefix, debug_logs_truncate_message,
	    max_debug_logs_turn_length);
}

std::string GetKeyFromFile(std::string file_name) {
	std::ifstream key_file(file_name, std::ifstream::in);
	std::string read_buffer;
	getline(key_file, read_buffer);
	return read_buffer;
}

bool FileExists(const std::string &name) {
	std::ifstream f(name.c_str());
	return f.good();
}

int main(int argc, char *argv[]) {
	// Get the player number. The process will be named './player_1' or
	// './player_2'
	auto process_name = std::string{argv[0]};
	auto player_number = process_name[process_name.size() - 1] - '0';

	// Read the SHM file to get the SHM name
	auto shm_file_name = SHM_FILE_NAMES[player_number - 1];
	if (not FileExists(shm_file_name)) {
		std::cerr << "Could not find SHM file. Aborting...\n";
		return EXIT_FAILURE;
	}

	std::string shm_name(GetKeyFromFile(shm_file_name));

	// We've read the SHM file name, remove the file
	auto remote_result = std::remove(shm_file_name.c_str());

	std::cout << "Running " << argv[0] << " ..." << std::endl;
	auto driver = BuildPlayerDriver(shm_name, std::string(argv[0]) +
	                                              player_debug_log_ext);

	driver->Start();
	std::cout << argv[0] << " Done!" << std::endl;
	return 0;
}
