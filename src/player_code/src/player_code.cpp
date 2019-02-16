#include "player_code/player_code.h"
#include "constants/gold_manager.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <vector>

namespace player_code {

using namespace player_state;
using namespace std;

// Declaring some useful hyperparameters
int64_t tick = 0;
Vec2D closest_mine = Vec2D::null;
Vec2D base_factory = Vec2D(1, 1);
bool beg_strat = true;
int64_t spacing = 2;
int64_t created_facs = 0;
int64_t build_fac_id = 0;
int64_t built_facs = 0;
int64_t enemy_count = -1;
player_state::Soldier enemy_soldier;

// Function to find the distance between 2 points
int64_t distance_vecs(Vec2D pos1, Vec2D pos2) {
	return ((pos1.x - pos2.x) * (pos1.x - pos2.x)) +
	       ((pos1.y - pos2.y) * (pos1.y - pos2.y));
}

// Function to find closest gold mine location to a given position
Vec2D ClosestGoldMine(Vec2D ref_pos, vector<Vec2D> gold_mine_locations,
                      int64_t map_size) {
	int64_t min_dist = map_size * map_size;
	Vec2D closest_pos = gold_mine_locations[0];
	for (auto gold_mine : gold_mine_locations) {
		int64_t dist = distance_vecs(gold_mine, ref_pos);
		if (dist < min_dist) {
			min_dist = dist;
			closest_pos = gold_mine;
		}
	}
	return closest_pos;
};

TerrainType GetTerrainType(array<array<TerrainType, MAP_SIZE>, MAP_SIZE> map,
                           Vec2D position) {
	return map[position.x][position.y];
}

State PlayerCode::Update(State state) {
	// Incrementing tick at every turn
	++tick;

	cout << "The timer shows " << tick << endl;
	cout << "The number of villagers is " << state.villagers.size() << "\n";

	// Building soldiers in the beginning rounds in the beginning starts
	if (tick == 1) {
		// Finding and setting the closest gold mine and flocking all other
		// villager to that position
		closest_mine =
		    ClosestGoldMine(base_factory, state.gold_mine_locations, MAP_SIZE);

		// Assinging half the villagers to find the closest gold mine and
		// mine gold
		for (int villager_index = 1;
		     villager_index < state.villagers.size() / 2; ++villager_index) {
			auto &villager = state.villagers[villager_index];
			villager.mine(closest_mine);
		}
	}

	if (created_facs > built_facs) {
		// Assinging the first factory id
		build_fac_id = state.factories[built_facs].id;

		// Assinging the other half villagers to build factories
		for (int villager_index = state.villagers.size() / 2;
		     villager_index < state.villagers.size(); ++villager_index) {
			auto &villager = state.villagers[villager_index];
			villager.build(build_fac_id);
		}

		// Checking to see if the factory that the villagers are
		// building is built, then changing target factory id
		auto &target_factory = state.factories[built_facs];
		if (target_factory.built) {
			// Switching his production type to soldier for alternate factories
			if (built_facs % 2 == 0) {
				target_factory.produce_soldiers();
			} else {
				target_factory.produce_villagers();
			}

			// Setting the target factories production to start
			target_factory.start();
			++built_facs;
		}
	}

	// Creating factories whenever we have the money but only after the present
	// factories have been built
	if (state.gold >= FACTORY_COST && created_facs == built_facs) {
		// Making the last villager go to create a factory
		auto &villager = state.villagers[0];

		// Creating a new factory
		villager.create(base_factory, FactoryProduction::VILLAGER);

		// Incrementing create factory number
		created_facs++;

		// Choosing the next position to build a factory
		if (tick % 2 == 0) {
			base_factory = Vec2D(base_factory.x + spacing, base_factory.y);
		} else {
			base_factory = Vec2D(base_factory.x, base_factory.y + spacing);
		}
	}

	// Making the soldiers move around the map randomly
	// for (auto &soldier : state.soldiers) {
	// 	soldier.destination = Vec2D(MAP_SIZE * ELEMENT_SIZE / 2 + rand() % 5,
	// 	                            MAP_SIZE * ELEMENT_SIZE / 2 + rand() % 5);
	// }
	for (auto &soldier : state.soldiers) {
		if (state.enemy_villagers.size() > 0) {
			soldier.attack(state.enemy_villagers[0]);
		} else if (state.enemy_soldiers.size() > 0) {
			soldier.attack(state.enemy_soldiers[0]);
		}
	}
	return state;
}
} // namespace player_code
