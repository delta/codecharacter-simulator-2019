#pragma once

#include "state/actor/actor.fwd.h"
#include "state/actor/actor.h"
#include "state/gold_manager/gold_mine.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

class STATE_EXPORT IGoldManager {
  public:
	/**
	 * Destructor
	 */
	virtual ~IGoldManager() {}

	/**
	 * Function to increase a player's gold
	 */
	virtual void Increase(PlayerId player_id, int64_t amount) = 0;

	/**
	 * Function to decrease a player's gold
	 */
	virtual void Decrease(PlayerId player_id, int64_t amount) = 0;

	/**
	 * Reward the player for killing an enemy actor
	 *
	 * @param[in]  enemy_actor  Pointer to the killed enemy
	 */
	virtual void RewardKill(Actor *enemy_actor) = 0;

	/**
	 * Returns the cost for creating a particular unit
	 *
	 * @param[in]  actor_type  ActorType to fetch cost
	 */
	virtual int64_t GetCreateUnitCost(ActorType unit_type) = 0;

	/**
	 * Decreases player's gold for creating specific actor type
	 *
	 * @param[in]  actor_type Pointer to the actor which player wants to build
	 */
	virtual void DeductUnitCreateCost(PlayerId player_id, Actor *actor) = 0;

	/**
	 * Get the current balance amount of the PlayerId passed
	 *
	 * @param[in]  player_id  The player identifier
	 *
	 * @return     The balance.
	 */
	virtual int64_t GetBalance(PlayerId player_id) = 0;

	/**
	 * Gets the maximum balance.
	 *
	 * @return     The maximum possible balance.
	 */
	virtual int64_t GetMaxGold() = 0;

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	virtual void DeductFactorySuicidePenalty(PlayerId player_id) = 0;

	/**
	 * Penalty for player triggering suicide
	 *
	 * @param[in]  player_id Player who triggered the suicide
	 */
	virtual void RewardMineGold(PlayerId player_id, GoldMine *gold_mine,
	                            int64_t mining_reward) = 0;

	/**
	 * Function to add build request to current requests
	 */
	virtual void AddMineRequest(PlayerId player_id, Vec2D offset) = 0;

	/**
	 * Function to assign amount of gold to be given to each player
	 */
	virtual void AssignGold() = 0;
};

} // namespace state