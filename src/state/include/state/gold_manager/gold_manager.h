/**
 * @file gold_manager.h
 * Manages dat gold
 */

#pragma once

#include "state/interfaces/i_gold_manager.h"
#include "state/state_export.h"
#include "state/utilities.h"

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

namespace state {

/**
 * Forward Declare Actor class
 */
class Actor;

/**
 * Gold Manager class that handles player gold
 */
class STATE_EXPORT GoldManager : public IGoldManager {

  private:
	/**
	 * Vector representing the amount of gold each player has
	 */
	std::array<int64_t, 2> players_gold;

	/**
	 * Maximum possible gold for a player
	 */
	int64_t max_gold;

	/**
	 * Reward amount for a successful soldier kill
	 */
	int64_t soldier_kill_reward_amount;

	/**
	 * Reward amount for a successful soldier kill
	 */
	int64_t villager_kill_reward_amount;

	/**
	 * Reward amount for a successful factory kill
	 */
	int64_t factory_kill_reward_amount;

	/**
	 * Penalty amount for a suicide of a factory
	 */
	int64_t factory_suicide_penalty_amount;

	/**
	 * Amount of gold to create a villager
	 */
	int64_t villager_cost;

	/**
	 * Amount of gold to create a soldier
	 */
	int64_t soldier_cost;

	/**
	 * Amount of gold to create any factory
	 */
	int64_t factory_cost;

	/**
	 * Reward for 1 villager mining gold for 1 unit of time
	 */
	int64_t mining_reward;

	/**
	 * Vector of the gold mines
	 */
	std::vector<std::unique_ptr<GoldMine>> gold_mines;

	/**
	 * Map to manage user build requests
	 * It maps each gold mine to number of requests made by the player to mine
	 * that location
	 */
	std::array<std::map<GoldMine *, int64_t, GoldMineCompare>, 2> mine_requests;

	/**
	 * Function to return gold mine given the Vec2D offset
	 */

	GoldMine *GetGoldMine(Vec2D offset);

  public:
	/**
	 * Constructor for Gold Manager class
	 */
	GoldManager();

	GoldManager(std::array<int64_t, 2> player_gold, int64_t max_gold,
	            int64_t soldier_kill_reward_amount,
	            int64_t villager_kill_reward_amount,
	            int64_t factory_kill_reward_amount,
	            int64_t factory_suicide_penalty_amount, int64_t villager_cost,
	            int64_t soldier_cost, int64_t soldier_factory_cost,
	            int64_t mining_reward,
	            std::vector<std::unique_ptr<GoldMine>> gold_mines);

	/**
	 * Method to increase player money
	 *
	 * @param[in]  player_id  The player identifier
	 * @param[in]  amount     The amount of money to add
	 *                        Balance will cap at max_money even if increase
	 *                        amount is sufficient to exceed it
	 *
	 * @throw      std::out_of_range If the amount is not positive
	 */
	void Increase(PlayerId player_id, int64_t amount) override;

	/**
	 * Method to decrease player money
	 *
	 * @param[in]  player_id  The player identifier
	 * @param[in]  amount     The amount of money to deduct
	 *
	 * @throw      std::out_of_range If the amount is not positive
	 *                               player has insufficuent balance
	 */
	void Decrease(PlayerId player_id, int64_t amount) override;

	/**
	 * Reward the player for killing an enemy actor
	 *
	 * @param[in]  enemy_actor  Pointer to the killed enemy
	 */
	void RewardKill(Actor *enemy_actor) override;

	/**
	 * Returns the cost for creating a particular unit
	 *
	 * @param[in]  actor_type  ActorType to fetch cost
	 */
	int64_t GetCreateUnitCost(ActorType unit_type) override;

	/**
	 * Decreases player's gold for creating specific actor type
	 *
	 * @param[in]  actor_type Pointer to the actor which player wants to build
	 */
	void DeductUnitCreateCost(PlayerId player_id, Actor *actor) override;

	/**
	 * Get the current balance amount of the PlayerId passed
	 *
	 * @param[in]  player_id  The player identifier
	 *
	 * @return     The balance.
	 */
	int64_t GetBalance(PlayerId player_id) override;

	/**
	 * Gets the maximum balance.
	 *
	 * @return     The maximum possible balance.
	 */
	int64_t GetMaxGold() override;

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	void DeductFactorySuicidePenalty(PlayerId player_id) override;

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	void RewardMineGold(PlayerId player_id, GoldMine *gold_mine,
	                    int64_t mining_reward) override;

	/**
	 * Function to add build request to current requests
	 */
	void AddMineRequest(PlayerId player_id, Vec2D offset) override;

	/**
	 * Function to assign amount of gold to be given to each player
	 */
	void AssignGold() override;
};
} // namespace state
