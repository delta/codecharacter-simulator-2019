/**
 * @file player_state.h
 * Player structure for accessing and game state and setting game actions
 */

#pragma once

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/player_state_helpers.h"
#include "state/utilities.h"

#include <array>
#include <limits>
#include <vector>

namespace player_state {

enum class SoldierState {
	// Soldier is doing nothing
	IDLE,
	// Soldier is moving
	MOVE,
	// Soldier is currently attacking another actor
	ATTACK
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
	BUILD
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
	}
	return os;
}

enum class FactoryState {
	// Factory is not yet completely built
	UNBUILT,
	// Factory is built and idle
	IDLE,
	// Factory is currently producing units
	PRODUCTION
};

inline std::ostream &operator<<(std::ostream &os, FactoryState factory_state) {
	switch (factory_state) {
	case FactoryState::UNBUILT:
		os << "UNBUILT";
		break;
	case FactoryState::IDLE:
		os << "IDLE";
		break;
	case FactoryState::PRODUCTION:
		os << "PRODUCTION";
		break;
	}
	return os;
}

enum class FactoryProduction {
	// Factory is producing villagers
	VILLAGER,
	// Factory is producing Soldiers
	SOLDIER
};

inline std::ostream &operator<<(std::ostream &os,
                                FactoryProduction factory_production) {
	switch (factory_production) {
	case FactoryProduction::VILLAGER:
		os << "VILLAGER";
		break;
	case FactoryProduction::SOLDIER:
		os << "SOLDIER";
		break;
	}
	return os;
}

struct _Actor {
	int64_t id;
	Vec2D position;
	int64_t hp;
	int64_t age;

	_Actor() : id(0), position(Vec2D{0, 0}), hp(100), age(0) {}
};

struct Factory : _Actor {
	double_t build_percent;
	bool built;
	bool stopped;

	FactoryProduction production_state;
	FactoryState state;

	// Helper to quickly toggle prodution states
	void toggle_production() {
		if (production_state == FactoryProduction::SOLDIER) {
			production_state = FactoryProduction::VILLAGER;
		} else {
			production_state = FactoryProduction::SOLDIER;
		}
	}

	void produce_soldiers() { production_state = FactoryProduction::SOLDIER; }

	void produce_villagers() { production_state = FactoryProduction::VILLAGER; }

	void stop() { stopped = true; }

	void start() { stopped = false; }

	Factory()
	    : _Actor(), build_percent(0), built(false), stopped(false),
	      production_state(FactoryProduction::VILLAGER) {}
};

inline std::ostream &operator<<(std::ostream &os, Factory factory) {
	using std::endl;

	os << "Factory(id: " << factory.id << ") {" << endl;
	os << "    position: " << factory.position << endl;
	os << "    hp: " << factory.hp << endl;
	os << "    age: " << factory.age << endl;
	os << "    build_percent: " << factory.build_percent << endl;
	os << "    stopped: " << factory.stopped << endl;
	os << "    production_state: " << factory.production_state << endl;
	os << "    state: " << factory.state << endl;
	os << "}";

	return os;
}

struct _Unit : _Actor {
	Vec2D destination;
	int64_t target;

	virtual void clear() {
		destination = Vec2D::null;
		target = -1;
	}

	void move(Vec2D p_destination) {
		clear();
		destination = p_destination;
	}

	// Attack with actor id
	void attack(int64_t p_target) {
		clear();
		target = p_target;
	}

	// Attack with actor object
	void attack(_Actor &p_target) {
		clear();
		target = p_target.id;
	}

	_Unit() : _Actor(), destination(Vec2D::null), target(-1) {}
};

struct Soldier : _Unit {
	SoldierState state;

	Soldier() : _Unit() {}
};

inline std::ostream &operator<<(std::ostream &os, Soldier soldier) {
	using std::endl;

	os << "Soldier(id: " << soldier.id << ") {" << endl;
	os << "    position: " << soldier.position << endl;
	os << "    hp: " << soldier.hp << endl;
	os << "    age: " << soldier.age << endl;
	os << "    state: " << soldier.state << endl;
	os << "}";

	return os;
}

struct Villager : _Unit {
	int64_t target_factory_id;
	Vec2D mine_target;
	Vec2D build_offset;
	FactoryProduction build_factory_type; // Note: Defaults to villager if unset
	VillagerState state;

	void clear() override {
		destination = Vec2D::null;
		target = -1;
		target_factory_id = -1;
		mine_target = Vec2D::null;
		build_offset = Vec2D::null;
	}

	// Build a new factory
	void build(Vec2D p_build_offset, FactoryProduction p_build_factory_type) {
		clear();
		// Set build target
		build_offset = p_build_offset;
		build_factory_type = p_build_factory_type;
	}

	// Join build at an existing factory using position
	void build(Vec2D p_build_offset) {
		clear();
		// Set build position
		build_offset = p_build_offset;
	}

	// Join build at an existing factory using actor id
	void build(int64_t p_factory_id) {
		clear();
		// Set build target
		target_factory_id = p_factory_id;
	}

	// Join build at an existing factory using factory reference
	void build(Factory &factory) {
		clear();
		// Set build target
		target_factory_id = factory.id;
	}

	// Mine at a particular location
	void mine(Vec2D p_mine_target) {
		clear();
		// Set mine target
		mine_target = p_mine_target;
	}

	Villager()
	    : _Unit(), target_factory_id(-1), mine_target(Vec2D::null),
	      build_offset(Vec2D::null),
	      build_factory_type(FactoryProduction::VILLAGER) {}
};

inline std::ostream &operator<<(std::ostream &os, Villager villager) {
	using std::endl;

	os << "Villager(id: " << villager.id << ") {" << endl;
	os << "    position: " << villager.position << endl;
	os << "    hp: " << villager.hp << endl;
	os << "    age: " << villager.age << endl;
	os << "    state: " << villager.state << endl;
	os << "}";

	return os;
}

enum class TerrainType { LAND, WATER, GOLD_MINE };

inline std::ostream &operator<<(std::ostream &os, TerrainType &t) {
	switch (t) {
	case TerrainType::LAND:
		os << "L";
		break;
	case TerrainType::WATER:
		os << "W";
		break;
	case TerrainType::GOLD_MINE:
		os << "G";
		break;
	}

	return os;
}

/**
 * Main player state object. One such struct is provided to each player
 */
struct State {
	std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> map;

	std::vector<Soldier> soldiers;
	std::vector<Soldier> enemy_soldiers;

	std::vector<Villager> villagers;
	std::vector<Villager> enemy_villagers;

	std::vector<Factory> factories;
	std::vector<Factory> enemy_factories;

	std::vector<Vec2D> gold_mine_offsets;

	Vec2D closest_gold_mine(Vec2D position);

	int64_t score;
	int64_t gold;
};

inline std::ostream &operator<<(std::ostream &os, State state) {
	using std::endl;

	os << "Map:" << endl;
	for (auto const &row : state.map) {
		for (auto const &elem : row) {
			switch (elem) {
			case TerrainType::LAND:
				os << "L"
				   << " ";
				break;
			case TerrainType::WATER:
				os << "W"
				   << " ";
				break;
			case TerrainType::GOLD_MINE:
				os << "G"
				   << " ";
				break;
			}
		}
		os << endl;
	}

	os << "-- Villagers --" << endl;
	for (auto const &villager : state.villagers) {
		os << villager << endl;
	}
	os << "-- Enemy Villagers --" << endl;
	for (auto const &villager : state.enemy_villagers) {
		os << villager << endl;
	}

	os << "-- Soldiers --" << endl;
	for (auto const &villager : state.soldiers) {
		os << villager << endl;
	}
	os << "-- Enemy Soldiers --" << endl;
	for (auto const &villager : state.enemy_soldiers) {
		os << villager << endl;
	}

	os << "-- Factories --" << endl;
	for (auto const &villager : state.factories) {
		os << villager << endl;
	}
	os << "-- Enemy Factories --" << endl;
	for (auto const &villager : state.enemy_factories) {
		os << villager << endl;
	}

	os << "-- Gold Mine Locations --" << endl;
	for (auto const &gold_mine : state.gold_mine_offsets) {
		os << gold_mine << endl;
	}

	os << "-- Score --" << endl;
	os << state.score << endl;

	os << "-- Gold --" << endl;
	os << state.gold << endl;

	return os;
}

/// Player State helper methods
inline Vec2D State::closest_gold_mine(Vec2D position) {
	// Set start distances
	double min_distance = std::numeric_limits<double>::max();
	Vec2D closest_gold_mine =
	    gold_mine_offsets.size() ? gold_mine_offsets[0] : Vec2D::null;

	// If there's a closer gold mine, set it
	for (auto &gold_mine : gold_mine_offsets) {
		if (OffsetToPosition(gold_mine).distance(position) < min_distance) {
			min_distance = OffsetToPosition(gold_mine).distance(position);
			closest_gold_mine = gold_mine;
		}
	}

	return closest_gold_mine;
}

} // namespace player_state
