/**
 * @file player_state.h
 * Player structure for accessing and game state and setting game actions
 */

#pragma once

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/utilities.h"

#include <array>
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

	_Actor() : id(0), position(Vec2D{0, 0}), hp(100) {}
};

struct _Unit : _Actor {
	Vec2D destination;
	int64_t target;

	void move(Vec2D p_destination) { destination = p_destination; }

	// Attack with actor id
	void attack(int64_t p_target) { target = p_target; }

	// Attack with actor object
	void attack(_Actor &p_target) { target = p_target.id; }

	_Unit() : _Actor(), destination(Vec2D::null), target(-1) {}
};

struct Soldier : _Unit {
	SoldierState state;

	Soldier() : _Unit() {}
};

struct Villager : _Unit {
	int64_t target_factory_id;
	Vec2D mine_target;
	Vec2D build_position;
	FactoryProduction build_factory_type; // Note: Defaults to villager if unset
	VillagerState state;

	// Build a new factory
	void build(Vec2D p_build_position, FactoryProduction p_build_factory_type) {
		build_position = p_build_position;
		build_factory_type = p_build_factory_type;
	}

	// Join build at an existing factory using position
	void build(Vec2D p_build_position) { build_position = p_build_position; }

	// Join build at an existing factory using actor id
	void build(int64_t p_factory_id) { target_factory_id = p_factory_id; }

	// Mine at a particular location
	void mine(Vec2D p_mine_target) { mine_target = p_mine_target; }

	Villager()
	    : _Unit(), target_factory_id(-1), mine_target(Vec2D::null),
	      build_position(Vec2D::null),
	      build_factory_type(FactoryProduction::VILLAGER) {}
};

struct Factory : _Actor {
	double_t build_percent;
	bool built;
	bool stopped;

	// Suicide if set
	bool _suicide;

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

	// Self destruct
	void suicide() { _suicide = true; }

	Factory()
	    : _Actor(), build_percent(0), built(false), stopped(false),
	      production_state(FactoryProduction::VILLAGER) {}
};

enum class TerrainType { LAND, WATER, GOLD_MINE };

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

	std::vector<Vec2D> gold_mine_locations;

	int64_t score;
	int64_t gold;
};

} // namespace player_state
