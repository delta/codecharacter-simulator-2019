/**
 * @file score_manager.cpp
 * Defines the ScoreManager class for maintaining game score
 */

#include "state/score_manager/score_manager.h"

#include <cmath>
#include <vector>

namespace state {

ScoreManager::ScoreManager(
    std::array<int64_t, 2> scores, int64_t villager_kill_reward,
    int64_t soldier_kill_reward, int64_t factory_kill_reward,
    int64_t factory_construction_reward, std::vector<int64_t> unit_age_levels,
    std::vector<int64_t> villager_age_rewards,
    std::vector<int64_t> soldier_age_rewards,
    std::vector<int64_t> factory_age_levels,
    std::vector<int64_t> factory_age_rewards, double gold_reward_ratio)
    : scores(scores), villager_kill_reward(villager_kill_reward),
      soldier_kill_reward(soldier_kill_reward),
      factory_kill_reward(factory_kill_reward),
      factory_construction_reward(factory_construction_reward),
      unit_age_levels(unit_age_levels),
      villager_age_rewards(villager_age_rewards),
      soldier_age_rewards(soldier_age_rewards),
      factory_age_levels(factory_age_levels),
      factory_age_rewards(factory_age_rewards),
      gold_reward_ratio(gold_reward_ratio) {}

std::array<int64_t, 2> ScoreManager::GetScores() { return scores; }

void ScoreManager::ScoreActorKill(PlayerId player_id, ActorType actor_type) {
	auto player_index = static_cast<int64_t>(player_id);

	auto reward = 0;
	switch (actor_type) {
	case ActorType::VILLAGER:
		reward = villager_kill_reward;
		break;
	case ActorType::SOLDIER:
		reward = soldier_kill_reward;
		break;
	case ActorType::FACTORY:
		reward = factory_kill_reward;
		break;
	}

	scores[player_index] += reward;
}

void ScoreManager::ScoreFactoryConstructionCompletion(PlayerId player_id) {
	auto player_index = static_cast<int64_t>(player_id);

	auto reward = factory_construction_reward;

	scores[player_index] += reward;
}

void ScoreManager::ScoreActorAge(PlayerId player_id, ActorType actor_type,
                                 int64_t age) {
	auto player_index = static_cast<int64_t>(player_id);

	auto reward = 0;

	// If the age of this actor has reached a level mark for that actor type,
	// reward a bonus
	switch (actor_type) {
	case ActorType::VILLAGER:
		for (int i = 0; i < unit_age_levels.size(); ++i) {
			const auto AGE_LEVEL = unit_age_levels[i];
			if (age == AGE_LEVEL) {
				reward = villager_age_rewards[i];
				break;
			}
		}
		break;
	case ActorType::SOLDIER:
		for (int i = 0; i < unit_age_levels.size(); ++i) {
			const auto AGE_LEVEL = unit_age_levels[i];
			if (age == AGE_LEVEL) {
				reward = soldier_age_rewards[i];
				break;
			}
		}
		break;
	case ActorType::FACTORY:
		for (int i = 0; i < factory_age_levels.size(); ++i) {
			const auto AGE_LEVEL = factory_age_levels[i];
			if (age == AGE_LEVEL) {
				reward = factory_age_rewards[i];
				break;
			}
		}
		break;
	}

	scores[player_index] += reward;
}

void ScoreManager::ScoreWealth(PlayerId player_id, int64_t gold) {
	auto player_index = static_cast<int64_t>(player_id);

	scores[player_index] += floor(gold_reward_ratio * gold);
}

} // namespace state
