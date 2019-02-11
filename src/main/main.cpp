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
			// Ensure that size of the row matches MAP_SIZE
			if (map_row.size() != MAP_SIZE) {
				std::cerr << "Bad map file! Match MAP_SIZE " << MAP_SIZE
				          << '\n';
				exit(EXIT_FAILURE);
			}

			map_elements.push_back(map_row);
			map_row.clear();
			break;
		}
	}
	map_file.close();

	// Ensure that number of rows matches MAP_SIZE
	if (map_elements.size() != MAP_SIZE) {
		std::cerr << "Bad map file! Match MAP_SIZE " << MAP_SIZE << '\n';
		exit(EXIT_FAILURE);
	}

	return make_unique<Map>(map_elements, MAP_SIZE, ELEMENT_SIZE);
}

unique_ptr<GoldManager> BuildGoldManager() {
	return make_unique<GoldManager>(
	    array<int64_t, 2>{GOLD_START, GOLD_START}, GOLD_MAX,
	    SOLDIER_KILL_REWARD_AMOUNT, VILLAGER_KILL_REWARD_AMOUNT,
	    FACTORY_KILL_REWARD_AMOUNT, FACTORY_SUICIDE_PENALTY, VILLAGER_COST,
	    SOLDIER_COST, FACTORY_COST, MINING_REWARD);
}

unique_ptr<PathPlanner> BuildPathPlanner(Map *map) {
	return make_unique<PathPlanner>(map);
}

unique_ptr<Soldier> BuildSoldier(PlayerId player_id, PathPlanner *path_planner,
                                 GoldManager *gold_manager) {
	return make_unique<Soldier>(
	    Actor::GetNextActorId(), player_id, ActorType::SOLDIER, SOLDIER_MAX_HP,
	    SOLDIER_MAX_HP, ACTOR_START_POSITIONS[static_cast<size_t>(player_id)],
	    gold_manager, path_planner, SOLDIER_SPEED, SOLDIER_ATTACK_RANGE,
	    SOLDIER_ATTACK_DAMAGE);
}

Soldier BuildModelSoldier(PathPlanner *path_planner,
                          GoldManager *gold_manager) {
	return Soldier(0, PlayerId::PLAYER1, ActorType::SOLDIER, SOLDIER_MAX_HP,
	               SOLDIER_MAX_HP, ACTOR_START_POSITIONS[0], gold_manager,
	               path_planner, SOLDIER_SPEED, SOLDIER_ATTACK_RANGE,
	               SOLDIER_ATTACK_DAMAGE);
}

unique_ptr<Villager> BuildVillager(PlayerId player_id,
                                   PathPlanner *path_planner,
                                   GoldManager *gold_manager) {
	return make_unique<Villager>(
	    Actor::GetNextActorId(), player_id, ActorType::VILLAGER,
	    VILLAGER_MAX_HP, VILLAGER_MAX_HP,
	    ACTOR_START_POSITIONS[static_cast<size_t>(player_id)], gold_manager,
	    path_planner, VILLAGER_SPEED, VILLAGER_ATTACK_RANGE,
	    VILLAGER_ATTACK_DAMAGE, VILLAGER_BUILD_EFFORT, VILLAGER_BUILD_RANGE,
	    VILLAGER_MINE_RANGE);
}

Villager BuildModelVillager(PathPlanner *path_planner,
                            GoldManager *gold_manager) {
	return Villager(0, PlayerId::PLAYER1, ActorType::VILLAGER, VILLAGER_MAX_HP,
	                VILLAGER_MAX_HP, ACTOR_START_POSITIONS[0], gold_manager,
	                path_planner, VILLAGER_SPEED, VILLAGER_ATTACK_RANGE,
	                VILLAGER_ATTACK_DAMAGE, VILLAGER_BUILD_EFFORT,
	                VILLAGER_BUILD_RANGE, VILLAGER_MINE_RANGE);
}

Factory BuildModelFactory(GoldManager *gold_manager) {
	return Factory(0, PlayerId::PLAYER1, ActorType::FACTORY, FACTORY_BASE_HP,
	               FACTORY_MAX_HP, ACTOR_START_POSITIONS[0], gold_manager, 0,
	               FACTORY_CONSTRUCTION_TOTAL, ActorType::VILLAGER,
	               FACTORY_VILLAGER_FREQUENCY, FACTORY_SOLDIER_FREQUENCY,
	               UnitProductionCallback{});
}

unique_ptr<State> BuildState() {
	Actor::SetActorIdIncrement();

	auto map = BuildMap();
	auto path_planner = BuildPathPlanner(map.get());
	auto gold_manager = BuildGoldManager();

	auto model_factory = BuildModelFactory(gold_manager.get());
	auto model_villager =
	    BuildModelVillager(path_planner.get(), gold_manager.get());
	auto model_soldier =
	    BuildModelSoldier(path_planner.get(), gold_manager.get());

	auto soldiers = array<vector<unique_ptr<Soldier>>, 2>{};
	auto factories = array<vector<unique_ptr<Factory>>, 2>{};

	// Create and initialize villagers list
	auto villagers = array<vector<unique_ptr<Villager>>, 2>{};
	for (int player_id = 0; player_id < 2; ++player_id) {
		auto &player_villagers = villagers.at((int)player_id);
		player_villagers.reserve(NUM_VILLAGERS_START);

		for (int i = 0; i < NUM_VILLAGERS_START; ++i) {
			player_villagers.push_back(
			    BuildVillager(static_cast<PlayerId>(player_id),
			                  path_planner.get(), gold_manager.get()));
		}
	}

	return make_unique<State>(move(map), move(gold_manager), move(path_planner),
	                          move(soldiers), move(villagers), move(factories),
	                          move(model_villager), move(model_soldier),
	                          move(model_factory));
}

unique_ptr<MainDriver> BuildMainDriver() {
	auto state = BuildState();
	auto logger =
	    make_unique<Logger>(state.get(), PLAYER_INSTRUCTION_LIMIT_TURN,
	                        PLAYER_INSTRUCTION_LIMIT_GAME, SOLDIER_MAX_HP,
	                        VILLAGER_MAX_HP, FACTORY_MAX_HP);

	auto command_giver = make_unique<CommandGiver>(state.get(), logger.get());
	auto state_syncer = make_unique<StateSyncer>(move(command_giver),
	                                             move(state), logger.get());

	vector<unique_ptr<SharedMemoryMain>> shm_mains;
	for (int i = 0; i < 2; ++i) {
		shm_names[i] = Game::GenerateRandomString(64) + to_string(i);
		shm_mains.push_back(make_unique<SharedMemoryMain>(
		    shm_names[i], false, 0, transfer_state::State()));
	}

	return make_unique<MainDriver>(
	    move(state_syncer), move(shm_mains), PLAYER_INSTRUCTION_LIMIT_TURN,
	    PLAYER_INSTRUCTION_LIMIT_GAME, NUM_TURNS,
	    Timer::Interval(GAME_DURATION_MS), move(logger), GAME_LOG_FILE_NAME);
}

string GetKeyFromFile() {
	ifstream key_file(KEY_FILE_NAME, ifstream::in);
	string read_buffer;
	getline(key_file, read_buffer);
	return read_buffer;
}

bool FileExists(const std::string &name) {
	ifstream f(name.c_str());
	return f.good();
}

void WriteToFile(string file_name, string content) {
	ofstream file(file_name, ofstream::out);
	file << content;
	file.close();
}

int main(int argc, char *argv[]) {
	// Check if map file exists
	if (not FileExists(MAP_FILE_NAME)) {
		cerr << "Error! Could not open map file " << MAP_FILE_NAME << '\n';
		return EXIT_FAILURE;
	}

	// Handle prefix security key
	string prefix_key = "codecharacter";
	if (not FileExists(KEY_FILE_NAME)) {
		cerr << "Warning! Could not open key file. Using default key...\n"
		     << "Please add a security key in file " << KEY_FILE_NAME << '\n';
	} else {
		prefix_key = GetKeyFromFile();

		// We've read the key, delete the file for safety
		auto result = remove(KEY_FILE_NAME);
	}

	// Build main driver
	auto driver = BuildMainDriver();

	// Write the SHM names to file, to be read by the player process
	for (int i = 0; i < 2; ++i) {
		WriteToFile(SHM_FILE_NAMES[i], shm_names[i]);
	}

	// Build game object
	auto game = std::make_unique<Game>(move(driver));

	// Start the game
	cout << "Starting game...\n";
	auto results = game->Start();

	// Game has finished
	cout << prefix_key << " " << results << endl;

	return 0;
}
