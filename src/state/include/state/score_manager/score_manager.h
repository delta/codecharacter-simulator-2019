/**
 * @file score_manager.h
 * Manages dat score
 */

#pragma once

#include "state/state_export.h"
#include "state/utilities.h"

#include <array>
#include <cstdint>
#include <vector>

namespace state {

/**
 * Score Manager class that handles player scoring
 */
class STATE_EXPORT ScoreManager {
  private:
	/**
	 * Current game scores
	 */
	std::array<int64_t, 2> scores;

	/**
	 * Reward for killing one villager
	 */
	int64_t villager_kill_reward;

	/**
	 * Reward for killing one soldier
	 */
	int64_t soldier_kill_reward;

	/**
	 * Reward for killing one factory
	 */
	int64_t factory_kill_reward;

	/**
	 * Reward for constructing one factory
	 */
	int64_t factory_construction_reward;

	/**
	 * Ages at which units are rewarded bonuses
	 */
	std::vector<int64_t> unit_age_levels;

	/**
	 * Bonus to villagers for each age level
	 */
	std::vector<int64_t> villager_age_rewards;

	/**
	 * Bonus to soldiers for each age level
	 */
	std::vector<int64_t> soldier_age_rewards;

	/**
	 * Ages at which factories are rewarded bonuses
	 */
	std::vector<int64_t> factory_age_levels;

	/**
	 * Bonus to factories for each age level
	 */
	std::vector<int64_t> factory_age_rewards;

	/**
	 * Fraction of final code that gets added to the score
	 */
	double gold_reward_ratio;

  public:
	ScoreManager() = default;
	ScoreManager(std::array<int64_t, 2> scores, int64_t villager_kill_reward,
	             int64_t soldier_kill_reward, int64_t factory_kill_reward,
	             int64_t factory_construction_reward,
	             std::vector<int64_t> unit_age_levels,
	             std::vector<int64_t> villager_age_rewards,
	             std::vector<int64_t> soldier_age_rewards,
	             std::vector<int64_t> factory_age_levels,
	             std::vector<int64_t> factory_age_rewards,
	             double gold_reward_ratio);

	/**
	 * Rewards for every kill
	 *
	 * @param player_id
	 * @param actor_type
	 */
	void ScoreActorKill(PlayerId player_id, ActorType actor_type);

	/**
	 * Rewards for every factory constructed
	 *
	 * @param player_id
	 */
	void ScoreFactoryConstructionCompletion(PlayerId player_id);

	/**
	 * Reward current actors for age. Keep your actors alive longer for a score
	 * bonus
	 *
	 * @param player_id
	 * @param actor_type
	 */
	void ScoreActorAge(PlayerId player_id, ActorType actor_type, int64_t age);

	/**
	 * Reward player for amount of gold left in reserves after game is over
	 *
	 * @param player_id
	 * @param gold
	 */
	void ScoreWealth(PlayerId player_id, int64_t gold);

	/**
	 * Get the current scores
	 *
	 * @return std::array<int64_t, 2> scores
	 */
	std::array<int64_t, 2> GetScores();
};
} // namespace state
