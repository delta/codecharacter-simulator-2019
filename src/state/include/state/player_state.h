/**
 * @file player_state.h
 * Player structure for accessing and game state and setting game actions
 */

#ifndef STATE_INCLUDE_STATE_PLAYER_STATE_H
#define STATE_INCLUDE_STATE_PLAYER_STATE_H

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/utilities.h"

#include <array>

namespace player_state {

enum class SoldierState {
	// Soldier is doing nothing
	IDLE,
	// Soldier is moving
	MOVE,
	// Soldier is currently attacking another actor
	ATTACK,
	// Soldier is dead
	DEAD
};

inline std::ostream &operator<<(std::ostream &os, SoldierState soldier_state) {
	switch (soldier_state) {
	case SoldierState::IDLE:
		os << "IDLE";
		break;
	case SoldierState::MOVE:
		os << "MOVE";
		break;
	case SoldierState::ATTACK:
		os << "ATTACK";
		break;
	case SoldierState::DEAD:
		os << "DEAD";
		break;
	}
	return os;
}

enum class VillagerState {
	// Villager is doing nothing
	IDLE,
	// Villager is moving
	MOVE,
	// Villager is currently attacking another actor
	ATTACK,
	// Villager is currently mining resources
	MINE,
	// Villager is currently building a factory
	BUILD,
	// Villager is dead
	DEAD
};

inline std::ostream &operator<<(std::ostream &os,
                                VillagerState villager_state) {
	switch (villager_state) {
	case VillagerState::IDLE:
		os << "IDLE";
		break;
	case VillagerState::MOVE:
		os << "MOVE";
		break;
	case VillagerState::ATTACK:
		os << "ATTACK";
		break;
	case VillagerState::BUILD:
		os << "BUILD";
		break;
	case VillagerState::MINE:
		os << "MINE";
		break;
	case VillagerState::DEAD:
		os << "DEAD";
		break;
	}
	return os;
}

enum class FactoryState {
	// Factory is not yet completely built
	UNBUILT,
	// Factory is built and idle
	IDLE,
	// Factory is currently producing villagers
	VILLAGER_PRODUCTION,
	// Factory is currently producing soldiers
	SOLDIER_PRODUCTION,
	// Factory is dead
	DEAD
};

enum class FactoryProduction {
	// Factory is producing villagers
	VILLAGER,
	// Factory is producing Soldiers
	SOLDIER
};

inline std::ostream &operator<<(std::ostream &os, FactoryState factory_state) {
	switch (factory_state) {
	case FactoryState::UNBUILT:
		os << "UNBUILT";
		break;
	case FactoryState::IDLE:
		os << "IDLE";
		break;
	case FactoryState::VILLAGER_PRODUCTION:
		os << "VILLAGER_PRODUCTION";
		break;
	case FactoryState::SOLDIER_PRODUCTION:
		os << "SOLDIER_PRODUCTION";
		break;
	case FactoryState::DEAD:
		os << "DEAD";
		break;
	}
	return os;
}

struct _Actor {
	int64_t id;
	physics::Vector<int64_t> position;
	int64_t hp;
};

struct _Unit : _Actor {
	// Player mutable
	physics::Vector<int64_t> destination;
	int64_t target;

	void move(physics::Vector<int64_t> p_destination) {
		destination = p_destination;
	}

	void attack(int64_t p_target) { target = p_target; }
};

struct Soldier : _Unit {
	//
};

struct Villager : _Unit {
	// Player mutable
	physics::Vector<int64_t> _build_position;
	FactoryProduction _build_factory_type;

	void
	build(physics::Vector<int64_t> build_position,
	      FactoryProduction build_factory_type = FactoryProduction::SOLDIER) {
		build_position = _build_position;
		build_factory_type = _build_factory_type;
	}
};

struct Factory : _Actor {
	double_t build_percent;
	bool built;

	// Player mutable
	FactoryProduction production_state;

	void toggle_production() {
		if (production_state == FactoryProduction::SOLDIER) {
			production_state = FactoryProduction::VILLAGER;
		} else {
			production_state = FactoryProduction::SOLDIER;
		}
	}

	void produce_soldiers() { production_state = FactoryProduction::SOLDIER; }

	void produce_villagers() { production_state = FactoryProduction::VILLAGER; }
};

struct MapElement {
	state::TerrainType type;
};

/**
 * Main player state object. One such struct is provided to each player
 */
struct State {
	std::array<std::array<MapElement, MAP_SIZE>, MAP_SIZE> map;

	std::array<Soldier, MAX_NUM_SOLDIERS> soldiers;
	std::array<Soldier, MAX_NUM_SOLDIERS> enemy_soldiers;
	int64_t num_soldiers;
	int64_t num_enemy_soldiers;

	std::array<Villager, MAX_NUM_VILLAGERS> villagers;
	std::array<Villager, MAX_NUM_VILLAGERS> enemy_villagers;
	int64_t num_villagers;
	int64_t num_enemy_villagers;

	std::array<Factory, MAX_NUM_FACTORIES> factories;
	std::array<Factory, MAX_NUM_FACTORIES> enemy_factories;
	int64_t num_factories;
	int64_t num_enemy_factories;

	int64_t score;
	int64_t gold;
};

} // namespace player_state

#endif
