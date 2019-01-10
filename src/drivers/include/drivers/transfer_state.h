/**
 * @file transfer_state.h
 * Defines a version of the player state that can be written to SHM
 */

// SHM cannot handle vectors, so this version of the player state uses arrays
// (stack-allocated) instead

#pragma once

#include "state/player_state.h"

#include <algorithm>

namespace transfer_state {

using player_state::FactoryProduction;
using player_state::FactoryState;
using player_state::SoldierState;
using player_state::VillagerState;

using player_state::Factory;
using player_state::Soldier;
using player_state::TerrainType;
using player_state::Villager;

struct State {
	std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> map;

	std::array<Soldier, MAX_NUM_SOLDIERS> soldiers;
	std::array<Soldier, MAX_NUM_SOLDIERS> enemy_soldiers;
	size_t num_soldiers;
	size_t num_enemy_soldiers;

	std::array<Villager, MAX_NUM_VILLAGERS> villagers;
	std::array<Villager, MAX_NUM_VILLAGERS> enemy_villagers;
	size_t num_villagers;
	size_t num_enemy_villagers;

	std::array<Factory, MAX_NUM_FACTORIES> factories;
	std::array<Factory, MAX_NUM_FACTORIES> enemy_factories;
	size_t num_factories;
	size_t num_enemy_factories;

	std::array<Vec2D, TOTAL_MAP_TILES> gold_mine_locations;
	size_t num_gold_mine_locations;

	int64_t score;
	int64_t gold;
};

template <typename T, size_t _>
inline std::vector<T> arrayToVector(const std::array<T, _> &arr,
                                    const size_t size) {
	return std::vector<T>(arr.begin(), arr.begin() + size);
}

inline player_state::State
ConvertToPlayerState(const transfer_state::State &ts) {
	auto ps = player_state::State{};

	// Copy Map
	ps.map = ts.map;

	// Copy Soldiers
	ps.soldiers = arrayToVector(ts.soldiers, ts.num_soldiers);
	ps.enemy_soldiers = arrayToVector(ts.enemy_soldiers, ts.num_enemy_soldiers);

	// Copy Villagers
	ps.villagers = arrayToVector(ts.villagers, ts.num_villagers);
	ps.enemy_villagers =
	    arrayToVector(ts.enemy_villagers, ts.num_enemy_villagers);

	// Copy Factories
	ps.factories = arrayToVector(ts.factories, ts.num_factories);
	ps.enemy_factories =
	    arrayToVector(ts.enemy_factories, ts.num_enemy_factories);

	// Copy gold mine locations
	ps.gold_mine_locations =
	    arrayToVector(ts.gold_mine_locations, ts.num_gold_mine_locations);

	// Copy gold and scores
	ps.score = ts.score;
	ps.gold = ts.gold;

	return ps;
}

template <size_t N, typename T>
std::array<T, N> vectorToArray(const std::vector<T> &vec) {
	auto arr = std::array<T, N>{};
	std::copy_n(vec.begin(), vec.size(), arr.begin());
	return arr;
}

inline transfer_state::State
ConvertToTransferState(const player_state::State &ps) {
	auto ts = transfer_state::State{};

	// Copy map
	ts.map = ps.map;

	// Copy soldiers
	ts.soldiers = vectorToArray<MAX_NUM_SOLDIERS>(ps.soldiers);
	ts.enemy_soldiers = vectorToArray<MAX_NUM_SOLDIERS>(ps.enemy_soldiers);

	// Copy Villagers
	ts.villagers = vectorToArray<MAX_NUM_VILLAGERS>(ps.villagers);
	ts.enemy_villagers = vectorToArray<MAX_NUM_VILLAGERS>(ps.enemy_villagers);

	// Copy Factories
	ts.factories = vectorToArray<MAX_NUM_FACTORIES>(ps.factories);
	ts.enemy_factories = vectorToArray<MAX_NUM_FACTORIES>(ps.enemy_factories);

	// Copy Gold Mine Locations
	ts.gold_mine_locations =
	    vectorToArray<TOTAL_MAP_TILES>(ps.gold_mine_locations);

	// Copy score and gold
	ts.score = ps.score;
	ts.gold = ps.gold;

	// Copy sizes
	ts.num_soldiers = ps.soldiers.size();
	ts.num_enemy_soldiers = ps.soldiers.size();
	ts.num_villagers = ps.villagers.size();
	ts.num_enemy_villagers = ps.villagers.size();
	ts.num_factories = ps.factories.size();
	ts.num_enemy_factories = ps.factories.size();
	ts.num_gold_mine_locations = ps.gold_mine_locations.size();

	return ts;
}

} // namespace transfer_state
