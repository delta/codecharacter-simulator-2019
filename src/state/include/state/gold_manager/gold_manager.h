/**
 * @file gold_manager.h
 * Manages dat gold
*/

#ifndef STATE_INCLUDE_STATE_GOLD_MANAGER_H
#define STATE_INCLUDE_STATE_GOLD_MANAGER_H

#include <cstdint>
#include <state/actor/actor.h>
#include <state/utilities.h>
#include <vector>

namespace state {

class STATE_EXPORT GoldManager {

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
	 * Reward amount for a successful factory kill
	 */
	int64_t factory_kill_reward_amount;

	/**
	 * Penalty amount for a suicide of a factory
	 */
	int64_t factory_suicide_penalty_amount;

	/**
	 * Reward amount for a successful soldier kill
	 */
	int64_t soldier_kill_reward_amount;

	/**
	 * Reward amount for a successful soldier kill
	 */
	int64_t villager_kill_reward_amount;

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
	 * Returns the cost for creating a particular unit
	 *
	 * @param[in]  actor   Pointer to the type of actor that is trying to be
	 * created
	 */
	int64_t GetCreateUnitCost(Actor *actor);

  public:
	/**
	* Constructor for Gold Manager class
	*/
	GoldManager();

	GoldManager(std::array<int64_t, 2> player_gold, int64_t max_gold,
	            int64_t soldier_kill_reward_amount,
	            int64_t villager_kill_reward_amount, int64_t factory_kill_reward_amount,
	            int64_t factory_suicide_penalty_amount, int64_t villager_cost,
	            int64_t soldier_cost, int64_t soldier_factory_cost,
	            int64_t mining_reward);

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
	void Increase(PlayerId player_id, int64_t amount);

	/**
	 * Method to decrease player money
	 *
	 * @param[in]  player_id  The player identifier
	 * @param[in]  amount     The amount of money to deduct
	 *
	 * @throw      std::out_of_range If the amount is not positive
	 *                               player has insufficuent balance
	 */
	void Decrease(PlayerId player_id, int64_t amount);

	/**
	 * Reward the player for killing an enemy actor
	 *
	 * @param[in]  enemy_actor  Pointer to the killed enemy
	 */
	void RewardKill(Actor *enemy_actor);

	/**
	 * Decreases player's gold for creating specific actor type
	 *
	 * @param[in]  actor_type Pointer to the actor which player wants tp build
	 */
	void GetCreateCost(PlayerId player_id, Actor *actor);

	/**
	* Get the current balance amount of the PlayerId passed
	 *
	* @param[in]  player_id  The player identifier
	*
	* @return     The balance.
	 */
	int64_t GetBalance(PlayerId player_id);

	/**
	 * Gets the maximum balance.
	 *
	 * @return     The maximum possible balance.
	 */
	int64_t GetMaxGold();

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	void GetFactorySuicideCost(PlayerId player_id);

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	void RewardMineGold(PlayerId player_id);
};
}

#endif
				