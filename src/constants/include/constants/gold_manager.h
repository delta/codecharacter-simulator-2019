/**
 * @file gold_manager.h
 * Constants related to gold
 */

#ifndef CONSTANTS_INCLUDE_CONSTANTS_MONEY_MANAGER_H
#define CONSTANTS_INCLUDE_CONSTANTS_MONEY_MANAGER_H

#include <cstddef>

// Amount of gold each player has at the start of the game
const int64_t GOLD_START = 300;

// Maxiumm amount of gold each player can have
const int64_t GOLD_MAX = INT64_MAX;

// Reward for killing one soldier
const int64_t SOLDIER_KILL_REWARD_AMOUNT = 200;

// Reward for killing one villager
const int64_t VILLAGER_KILL_REWARD_AMOUNT = 100;

// Amount of gold to create soldier
const int64_t SOLDIER_COST = 200;

// Amount of gold to create soldier
const int64_t VILLAGER_COST = 200;

// Amount of gold to create soldier
const int64_t FACTORY_COST = 500;

#endif
