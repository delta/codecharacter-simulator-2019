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
const size_t MAX_NUM_SOLDIERS = 40;

// Maximum number of villagers per player
const size_t MAX_NUM_VILLAGERS = 80;

// Maximum number of factories per player
const size_t MAX_NUM_FACTORIES = 30;

// Number of villagers that each player starts with
const size_t NUM_VILLAGERS_START = 5;

// Size of the map in grid elements
const size_t MAP_SIZE = 30;

// Size of each grid element on the map
const size_t ELEMENT_SIZE = 10;

// Max Starting HP for Soldier
const int64_t SOLDIER_MAX_HP = 150;

// Max Starting HP for Villager
const int64_t VILLAGER_MAX_HP = 90;

// HP of an unbuilt factory
const int64_t FACTORY_BASE_HP = 50;

// Max Starting HP for Factory
const int64_t FACTORY_MAX_HP = 2500;

// Total construction effort needed per factory
const int64_t FACTORY_CONSTRUCTION_TOTAL = 300;

// Frequency with which a factory produces villagers
const int64_t FACTORY_VILLAGER_FREQUENCY = 4;

// Frequency with which a factory produces soldiers
const int64_t FACTORY_SOLDIER_FREQUENCY = 6;

// Speed of a soldier
const int64_t SOLDIER_SPEED = 6;

// Speed of a villager
const int64_t VILLAGER_SPEED = 4;

// Attack range of a soldier
const int64_t SOLDIER_ATTACK_RANGE = 9;

// Attack range of a villager
const int64_t VILLAGER_ATTACK_RANGE = 9;

// Attack damage of a soldier
const int64_t SOLDIER_ATTACK_DAMAGE = 15;

// Attack damage of a villager
const int64_t VILLAGER_ATTACK_DAMAGE = 5;

// Build effort in one turn of the villager
const int64_t VILLAGER_BUILD_EFFORT = 6;

// Build range of the villager
const int64_t VILLAGER_BUILD_RANGE = 9;

// Mine range of the villager
const int64_t VILLAGER_MINE_RANGE = 9;

// Player 1 start position
// Player 2 is the same position flipped
const auto PLAYER1_START_POSITION = DoubleVec2D{5, 5};
const auto PLAYER2_START_POSITION =
    DoubleVec2D{(MAP_SIZE * ELEMENT_SIZE - 1) - PLAYER1_START_POSITION.x,
                (MAP_SIZE * ELEMENT_SIZE - 1) - PLAYER1_START_POSITION.y};

// Starting positions of actors for both players
const std::vector<DoubleVec2D> ACTOR_START_POSITIONS = {PLAYER1_START_POSITION,
                                                        PLAYER2_START_POSITION};

// Total number of units on the map
constexpr size_t TOTAL_MAP_TILES = MAP_SIZE * MAP_SIZE;
