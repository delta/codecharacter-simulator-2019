/**
 * @file state.h
 * Constants related to game state
 */

#pragma once

#include "physics/vector.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

// Maximum number of soldiers per player
const size_t MAX_NUM_SOLDIERS = 30;

// Maximum number of villagers per player
const size_t MAX_NUM_VILLAGERS = 30;

// Maximum number of factories per player
const size_t MAX_NUM_FACTORIES = 20;

// Number of villagers that each player starts with
const size_t NUM_VILLAGERS_START = 10;

// Size of the map in grid elements
const size_t MAP_SIZE = 50;

// Size of each grid element on the map
const size_t ELEMENT_SIZE = 10;

// Max Starting HP for Soldier
const int64_t SOLDIER_MAX_HP = 200;

// Max Starting HP for Villager
const int64_t VILLAGER_MAX_HP = 80;

// HP of an unbuilt factory
const int64_t FACTORY_BASE_HP = 1;

// Max Starting HP for Factory
const int64_t FACTORY_MAX_HP = 3000;

// Total construction effort needed per factory
const int64_t FACTORY_CONSTRUCTION_TOTAL = 300;

// Frequency with which a factory produces villagers
const int64_t FACTORY_VILLAGER_FREQUENCY = 10;

// Frequency with which a factory produces soldiers
const int64_t FACTORY_SOLDIER_FREQUENCY = 15;

// Speed of a soldier
const int64_t SOLDIER_SPEED = 5;

// Speed of a villager
const int64_t VILLAGER_SPEED = 3;

// Attack range of a soldier
const int64_t SOLDIER_ATTACK_RANGE = 5;

// Attack range of a villager
const int64_t VILLAGER_ATTACK_RANGE = 5;

// Attack damage of a soldier
const int64_t SOLDIER_ATTACK_DAMAGE = 10;

// Attack damage of a villager
const int64_t VILLAGER_ATTACK_DAMAGE = 5;

// Build effort in one turn of the villager
const int64_t VILLAGER_BUILD_EFFORT = 3;

// Build range of the villager
const int64_t VILLAGER_BUILD_RANGE = 5;

// Mine range of the villager
const int64_t VILLAGER_MINE_RANGE = 5;

// Starting positions of actors for both players
const std::vector<DoubleVec2D> ACTOR_START_POSITIONS = {{2, 2}, {40, 40}};

// Total number of units on the map
constexpr size_t TOTAL_MAP_TILES = MAP_SIZE * MAP_SIZE;
