/**
 * @file gold_manager.cpp
 * Defines the methods of the GoldManager Class
 */

#include "state/gold_manager/gold_manager.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include <stdexcept>

namespace state {

GoldManager::GoldManager() {}

GoldManager::GoldManager(std::array<int64_t, 2> players_gold, int64_t max_gold,
                         int64_t soldier_kill_reward_amount,
                         int64_t villager_kill_reward_amount,
                         int64_t factory_kill_reward_amount,
                         int64_t factory_suicide_penalty_amount,
                         int64_t villager_cost, int64_t soldier_cost,
                         int64_t factory_cost, int64_t mining_reward)
    : players_gold(players_gold), max_gold(max_gold),
      soldier_kill_reward_amount(soldier_kill_reward_amount),
      villager_kill_reward_amount(villager_kill_reward_amount),
      factory_kill_reward_amount(factory_kill_reward_amount),
      factory_suicide_penalty_amount(factory_suicide_penalty_amount),
      villager_cost(villager_cost), soldier_cost(soldier_cost),
      factory_cost(factory_cost), mining_reward(mining_reward) {}

void GoldManager::Increase(PlayerId player_id, int64_t amount) {

	auto current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range("Increase in gold amount should be positive");
	}
	players_gold[current_player_id] += amount;
	if (players_gold[current_player_id] > max_gold) {
		players_gold[current_player_id] = max_gold;
	}
}

void GoldManager::Decrease(PlayerId player_id, int64_t amount) {

	auto current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range(
		    "The amount of descrese in gold must be positive");
	}
	if (amount >= players_gold[current_player_id]) {
		throw std::out_of_range(
		    "The decrease in gold is greater than current balance");
	}
	players_gold[current_player_id] -= amount;
}

void GoldManager::RewardKill(Actor *enemy_actor) {

	// Finding player ID of player to increment gold
	PlayerId player_id;

	PlayerId enemy_player_id = enemy_actor->GetPlayerId();

	if (enemy_player_id == PlayerId::PLAYER2) {
		player_id = PlayerId::PLAYER1;
	} else {
		player_id = PlayerId::PLAYER2;
	}

	// Assigning reward amount
	ActorType enemy_actor_type = enemy_actor->GetActorType();
	int64_t reward_amount;

	if (enemy_actor_type == ActorType::SOLDIER) {
		reward_amount = soldier_kill_reward_amount;
	} else if (enemy_actor_type == ActorType::VILLAGER) {
		reward_amount = villager_kill_reward_amount;
	} else {
		reward_amount = factory_kill_reward_amount;
	}

	Increase(player_id, reward_amount);
}

int64_t GoldManager::GetCreateUnitCost(ActorType unit_type) {
	if (unit_type == ActorType::FACTORY) {
		return factory_cost;
	} else if (unit_type == ActorType::VILLAGER) {
		return villager_cost;
	} else {
		return soldier_cost;
	}
}

void GoldManager::DeductUnitCreateCost(PlayerId player_id, Actor *actor) {

	auto actor_cost = GetCreateUnitCost(actor->GetActorType());
	auto current_balance = GetBalance(player_id);

	// Decreasing player's gold if they can buy the unit
	if (current_balance < actor_cost) {
		throw std::out_of_range("Do not possess enough gold to buy unit");
	} else {
		Decrease(player_id, actor_cost);
	}
}

int64_t GoldManager::GetBalance(PlayerId player_id) {
	return players_gold[static_cast<int>(player_id)];
}

int64_t GoldManager::GetMaxGold() { return max_gold; }

void GoldManager::DeductFactorySuicidePenalty(PlayerId player_id) {
	auto suicide_penalty = factory_suicide_penalty_amount;
	Decrease(player_id, suicide_penalty);
}

void GoldManager::RewardMineGold(PlayerId player_id) {
	auto reward_mining = mining_reward;
	Increase(player_id, reward_mining);
}

void GoldManager::AddBuildRequest(PlayerId player_id, Vec2D offset) {}

void GoldManager::AssignGold() {}

GoldMine GoldManager::GetGoldMine(Vec2D offset) {}

} // namespace state
