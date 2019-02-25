/**
 * @file score_manager.h
 * Constants related to score
 */

#pragma once

#include <cstdint>
#include <vector>

namespace score_constants {

// Reward for killing a villager
const int64_t VILLAGER_KILL_REWARD = 12;

// Reward for killing a soldier
const int64_t SOLDIER_KILL_REWARD = 25;

// Reward for killing a factory
const int64_t FACTORY_KILL_REWARD = 45;

// Reward for constructing a factory
const int64_t FACTORY_CONSTRUCTION_REWARD = 100;

// Age levels at which units receive rewards
const auto UNIT_AGE_LEVELS = std::vector<int64_t>{50, 100, 200, 500};

// Rewards for incremental ages of a villager
const auto VILLAGER_AGE_REWARDS = std::vector<int64_t>{4, 9, 19, 55};

// Rewards for incremental ages of a soldier
const auto SOLDIER_AGE_REWARDS = std::vector<int64_t>{3, 7, 16, 43};

// Age levels at which factories receive rewards
const auto FACTORY_AGE_LEVELS = std::vector<int64_t>{100, 200, 400, 800};

// Rewards for incremental ages of a factory
const auto FACTORY_AGE_REWARDS = std::vector<int64_t>{21, 42, 84, 168};

// Fraction of final gold to reward the player as score
const double GOLD_REWARD_RATIO = 0.02;

} // namespace score_constants
