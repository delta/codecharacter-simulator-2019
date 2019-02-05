#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "game/game.h"
#include "logger/logger.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/command_giver.h"
#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_syncer.h"
#include "state/utilities.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>

using namespace std;
using namespace drivers;
using namespace state;
using namespace logger;
using namespace physics;

// The map file contains the terrain layout for initializing the game map
const auto MAP_FILE_NAME = "map.txt";

// The security key file contains a single string, which is prefixed to the
// output scores so that the player cannot directly print a score
const auto KEY_FILE_NAME = "key.txt";

auto shm_names = vector<string>(2);

unique_ptr<Map> BuildMap() {
	auto map_elements = vector<vector<TerrainType>>{};

	auto map_file = ifstream(MAP_FILE_NAME, ifstream::in);

	// Compute file size
	map_file.seekg(0, map_file.end);
	auto map_file_size = map_file.tellg();
	map_file.seekg(0);

	auto map_file_input = vector<char>(map_file_size, '\0');
	map_file.read(&map_file_input[0], map_file_size);

	auto map_row = vector<TerrainType>{};
	for (auto character : map_file_input) {
		switch (character) {
		case 'L':
			map_row.push_back(TerrainType::LAND);
			break;
		case 'W':
			map_row.push_back(TerrainType::WATER);
			break;
		case 'G':
			map_row.push_back(TerrainType::GOLD_MINE);
			break;
		case ' ':
			break;
		case '\n':
			map_elements.push_back(map_row);
			map_row.clear();
			break;
		}
	}
	map_file.close();

	// TODO : Put map element size here
	return make_unique<Map>(map_elements, map_elements.size(), 10);
}

int main(int argc, char *argv[]) {
	std::cout << "Hello World" << std::endl;
	return 0;
}
