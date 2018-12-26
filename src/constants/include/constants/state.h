/**
 * @file state.h
 * Constants related to game state
 */

#pragma once

#include <cstddef>
#include <cstdint>

// Maximum number of soldiers per player
const size_t MAX_NUM_SOLDIERS = 100;

// Maximum number of villagers per player
const size_t MAX_NUM_VILLAGERS = 100;

// Maximum number of factories per player
const size_t MAX_NUM_FACTORIES = 50;

// Size of the map in grid elements
const size_t MAP_SIZE = 50;

// Max Starting HP for Soldier
const int64_t SOLDIER_MAX_HP = 200;

// Max Starting HP for Villager
const int64_t VILLAGER_MAX_HP = 80;

// Max Starting HP for Factory
const int64_t FACTORY_MAX_HP = 3000;
