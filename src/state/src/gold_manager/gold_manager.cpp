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

GoldManager::GoldManager(
    std::array<int64_t, 2> players_gold, int64_t max_gold,
    int64_t soldier_kill_reward_amount, int64_t villager_kill_reward_amount,
    int64_t factory_kill_reward_amount, int64_t factory_suicide_penalty_amount,
    int64_t villager_cost, int64_t soldier_cost, int64_t factory_cost,
    int64_t mining_reward, std::vector<GoldMine> gold_mines,
    std::array<std::map<GoldMine, int64_t, GoldMineCompare>, 2> mine_requests)
    : players_gold(players_gold), max_gold(max_gold),
      soldier_kill_reward_amount(soldier_kill_reward_amount),
      villager_kill_reward_amount(villager_kill_reward_amount),
      factory_kill_reward_amount(factory_kill_reward_amount),
      factory_suicide_penalty_amount(factory_suicide_penalty_amount),
      villager_cost(villager_cost), soldier_cost(soldier_cost),
      factory_cost(factory_cost), mining_reward(mining_reward),
      gold_mines(gold_mines), mine_requests(mine_requests) {}

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

void GoldManager::RewardMineGold(PlayerId player_id, GoldMine gold_mine,
                                 int64_t mining_reward) {
	// Extracting gold from the gold mine
	int64_t extracted_amount = gold_mine.ExtractGold(mining_reward);

	// Increasing the player's gold
	Increase(player_id, extracted_amount);
}

void GoldManager::AddBuildRequest(PlayerId player_id, Vec2D offset) {
	int64_t id = static_cast<int64_t>(player_id);
	auto &player_requests = this->mine_requests[id];
	auto const gold_mine = GetGoldMine(offset);

	// If the gold mine dosen't already exist in the hash map, then we add an
	// entry with count 1
	if (player_requests.empty()) {
		player_requests.insert(std::pair<GoldMine, int64_t>(gold_mine, 1));
	} else if (player_requests.find(gold_mine) != player_requests.end()) {
		player_requests.insert(std::pair<GoldMine, int64_t>(gold_mine, 1));
	} else {
		auto request = player_requests.find(gold_mine);
		request->second += 1;
	}
}

void GoldManager::AssignGold() {
	// Iterating through all the player's requests
	for (int id = 0; id < 2; ++id) {
		int64_t enemy_id =
		    (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);
		auto player_requests = this->mine_requests[id];
		auto enemy_requests = this->mine_requests[enemy_id];
		for (auto const &request : player_requests) {
			auto gold_mine = request.first;
			auto no_player_requests = request.second;
			PlayerId player_id = static_cast<PlayerId>(id);

			// If enemy is trying to mine in the same gold mine, gold is split
			if (enemy_requests.find(gold_mine) != enemy_requests.end()) {
				int64_t no_enemy_requests =
				    enemy_requests.find(gold_mine)->second;

				// Checking if the gold mine has enough gold to accomodate both
				// the users
				int64_t total_gold_required =
				    this->mining_reward *
				    (no_player_requests + no_enemy_requests);

				// If the gold mine has enough gold to accomodate both the
				// player's requests
				if (gold_mine.value >= total_gold_required) {
					// Deducting the player's requested gold
					int64_t mining_reward =
					    this->mining_reward * no_player_requests;
					RewardMineGold(player_id, gold_mine, mining_reward);
				}

				// If the gold mine has lesser gold than what is asked
				// NOTE: One gold coin may get lost
				else {
					// Finding the ratio of gold that this player will recieve
					float gold_ratio = float(no_player_requests) /
					                   (no_enemy_requests + no_enemy_requests);
					int gold_recieved = gold_ratio * gold_mine.value;
					RewardMineGold(player_id, gold_mine, gold_recieved);
				}
			}
			// Gold is not split between the villagers
			else {
				int gold_recieved = no_player_requests * this->mining_reward;

				// Rewarding the user gold
				RewardMineGold(player_id, gold_mine, gold_recieved);
			}
		}
	}
}

const GoldMine GoldManager::GetGoldMine(Vec2D offset) {
	for (auto gold_mine : this->gold_mines) {
		if (gold_mine.offset == offset) {
			return gold_mine;
		}
	}
	// TODO : Return null value
	// return null;
}

} // namespace state
