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
Vec2D closest_mine1, closest_mine2 = Vec2D::null;
Vec2D base_factory1, base_factory2 = Vec2D::null;
bool beg_strat = true;
int64_t spacing1, spacing2 = 2;
int64_t build_villager_index = 0;
int64_t assinged_villager_count = NUM_VILLAGERS_START;
player_state::Soldier enemy_soldier;

// Function to find closest gold mine location to a given position
Vec2D ClosestGoldMine(Vec2D ref_pos, vector<Vec2D> gold_mine_locations,
                      int64_t map_size) {
	int64_t min_dist = map_size * map_size;
	Vec2D closest_pos = gold_mine_locations[0];
	for (auto gold_mine : gold_mine_locations) {
		int64_t dist = gold_mine.distance(ref_pos);
		if (dist < min_dist) {
			min_dist = dist;
			closest_pos = gold_mine;
		}
	}
	return closest_pos;
}

bool IsValidPosition(array<array<TerrainType, MAP_SIZE>, MAP_SIZE> map,
                     Vec2D position) {
	TerrainType terrain = map[position.x][position.y];
	return (terrain == TerrainType::LAND);
}

State PlayerCode::Update(State state) {
	// Incrementing tick at every turn
	++tick;

	//	cout << "The timer shows " << tick << endl;
	// cout << "The number of villagers is " << state.factories.size() << "\n";

	// Building soldiers in the beginning rounds in the beginning starts
	if (tick == 1) {
		// Creating 2 vectors of gold mines
		vector<Vec2D> first_half;
		vector<Vec2D> second_half;

		// Splittin the gold mines
		for (int i = 0; i < state.gold_mine_locations.size() / 2; ++i) {
			auto gold_mine = state.gold_mine_locations[i];
			first_half.push_back(gold_mine);
		}
		for (int i = state.gold_mine_locations.size() / 2;
		     i < state.gold_mine_locations.size(); ++i) {
			auto gold_mine = state.gold_mine_locations[i];
			second_half.push_back(gold_mine);
		}

		// Finding the 2 closest gold mines and split running
		closest_mine1 =
		    ClosestGoldMine(state.villagers[0].position, first_half, MAP_SIZE);
		closest_mine2 =
		    ClosestGoldMine(state.villagers[0].position, second_half, MAP_SIZE);

		// Assinging the base factory positions near the gold mines
		base_factory1 = closest_mine1;
		base_factory2 = closest_mine2;

		// Assinging half the villagers to find the closest gold mine and
		// mine gold
		for (int villager_index = 0;
		     villager_index < state.villagers.size() / 2; ++villager_index) {
			auto &villager = state.villagers[villager_index];
			villager.mine(closest_mine1);
		}

		for (int villager_index = state.villagers.size() / 2;
		     villager_index < state.villagers.size(); ++villager_index) {
			auto &villager = state.villagers[villager_index];
			villager.mine(closest_mine2);
		}
	}

	// // Creating factories whenever money is available
	if (state.gold >= FACTORY_COST && state.villagers.size() > 0 &&
	    build_villager_index < state.villagers.size() * 0.5) {
		// Making the last villager go to create a factory
		auto &villager = state.villagers[build_villager_index++];

		// New build position
		Vec2D new_position = Vec2D::null;
		FactoryProduction prod_type;

		// Choosing the next position to build a factory
		if (tick % 2 == 0) {
			do {
				new_position = Vec2D(base_factory1.x + spacing1,
				                     base_factory1.y + spacing2);
				spacing1 = rand() % 5 - 3;
				spacing2 = rand() % 5 - 3;
			} while (!IsValidPosition(state.map, new_position));
			prod_type = FactoryProduction::SOLDIER;
			base_factory1 = new_position;
		} else {
			do {
				new_position = Vec2D(base_factory2.x + spacing1,
				                     base_factory2.y + spacing2);
				spacing1 = rand() % 5 - 3;
				spacing2 = rand() % 5 - 3;
			} while (!IsValidPosition(state.map, new_position));
			prod_type = FactoryProduction::VILLAGER;
			base_factory2 = new_position;
		}

		// Creating a new factory
		villager.build(new_position, prod_type);
	}

	// When new villagers are created, assign them tasks
	if (state.villagers.size() > assinged_villager_count) {
		//	cout << "Here\n";
		// Choosing the new villager
		auto &villager = state.villagers[assinged_villager_count++];
		bool build = false;

		// Iterating through factories and chekcing if they are built properly
		// for (auto &factory : state.factories) {
		// 	// If any factory is not built
		// 	if (factory.build_percent == 100) {
		// 		cout << "Building\n";
		// 		villager.build(factory.id);
		// 		break;
		// 	}
		// }

		// If all the factories have been built, assigning the villager to mine
		// some new mine
		if (not build) {
			int64_t start, end;
			start = rand() % (state.gold_mine_locations.size() / 2);
			end = rand() % (state.gold_mine_locations.size() / 2) +
			      (state.gold_mine_locations.size() / 2);

			// Random gold mines
			vector<Vec2D> other_mines;
			for (; start <= end; ++start) {
				Vec2D position = state.gold_mine_locations[start];
				other_mines.push_back(position);
			}
			Vec2D closest =
			    ClosestGoldMine(villager.position, other_mines, MAP_SIZE);
			villager.mine(closest);
		}
	}

	// // As soon as the soldiers appear, start attacking enemy villagers
	int soldier_index = 0;
	for (; soldier_index < state.soldiers.size() * 0.5; ++soldier_index) {
		auto &soldier = state.soldiers[soldier_index];
		if (state.enemy_soldiers.size() > 0) {
			soldier.attack(state.enemy_soldiers[0]);
		} else {
			break;
		}
	}
	// for (; soldier_index < state.soldiers.size() * 0.75; ++soldier_index) {
	// 	auto &soldier = state.soldiers[soldier_index];
	// 	if (state.enemy_villagers.size() > 0) {
	// 		soldier.attack(state.enemy_villagers[0]);
	// 	} else {
	// 		break;
	// 	}
	// }
	// for (; soldier_index < state.soldiers.size(); ++soldier_index) {
	// 	auto &soldier = state.soldiers[soldier_index];
	// 	if (state.enemy_factories.size() > 0) {
	// 		soldier.attack(state.enemy_factories[0]);
	// 	} else {
	// 		break;
	// 	}
	// }
	return state;
}
} // namespace player_code
