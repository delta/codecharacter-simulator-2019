/**
 * @file i_command_taker.h
 * Interface for game state changing calls
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/state_export.h"
#include "state/utilities.h"

#include <array>
#include <vector>

namespace state {

/**
 * ICommandTaker interface to define the game action methods
 */
class STATE_EXPORT ICommandTaker : public IUpdatable {
  public:
	virtual ~ICommandTaker() {}

	/**
	 * Handles unit movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier to act upon
	 * @param[in]  destination   new position to move the unit to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void MoveUnit(PlayerId player_id, ActorId actor_id,
	                      Vec2D position) = 0;

	/**
	 * Command villgaer to mine a mine
	 *
	 * @param player_id      player to act upon
	 * @param villager_id    villager to act upon
	 * @param mine_location  Vector indicating mine location
	 */
	virtual void MineLocation(PlayerId player_id, ActorId villager_id,
	                          Vec2D mine_location) = 0;

	/**
	 * Handles attack on actor
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  unit_id       unit to initiate attack
	 * @param[in]  actor_id      actor to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void AttackActor(PlayerId player_id, ActorId unit_id,
	                         ActorId enemy_actor_id) = 0;

	/**
	 * Adds mine request which will later be handled by HandleBuildRequests
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  villager_id   villager to build
	 * @param[in]  offset        grid location to build the factory
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void CreateFactory(PlayerId player_id, ActorId villager_id,
	                           Vec2D offset, ActorType produce_unit) = 0;

	/**
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  villager_id   villager to build
	 * @param[in]  factory_id    factory to build (assuming building has begun)
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void BuildFactory(PlayerId player_id, ActorId villager_id,
	                          ActorId factory_id) = 0;

	/**
	 * Command factory to change production
	 *
	 * @param player_id      player to act upon
	 * @param factory_id     factory to act upon
	 * @param production     actor type to produce
	 */
	virtual void SetFactoryProduction(PlayerId player_id, ActorId factory_id,
	                                  ActorType production) = 0;

	/**
	 * Command factory to stop or start factory
	 *
	 * @param player_id      player to act upon
	 * @param factory_id     factory to act upon
	 * @param should_stop    true to stop, false to start
	 */
	virtual void StopOrStartFactory(PlayerId player_id, ActorId factory_id,
	                                bool should_stop) = 0;

	/**
	 * Get all soldiers from state
	 *
	 * @return soldiers
	 */
	virtual const std::array<std::vector<Soldier *>, 2> GetSoldiers() = 0;

	/**
	 * Get all villagers from state
	 *
	 * @return villagers
	 */
	virtual const std::array<std::vector<Villager *>, 2> GetVillagers() = 0;

	/**
	 * Get all factories from state
	 *
	 * @return factories
	 */
	virtual const std::array<std::vector<Factory *>, 2> GetFactories() = 0;

	/**
	 * Get map from state
	 *
	 * @return map
	 */
	virtual const Map *GetMap() = 0;

	/**
	 * Get money from state
	 *
	 * @return money
	 */
	virtual const std::array<int64_t, 2> GetGold() = 0;

	/**
	 * Get game scores from state
	 *
	 * @return scores
	 */
	virtual const std::array<int64_t, 2> GetScores(bool game_over) = 0;

	/**
	 * Get the game Interestingness
	 *
	 * @return int64_t
	 */
	virtual int64_t GetInterestingness() = 0;

	/**
	 * Check if the game is over
	 *
	 * @param[out] winner If the game is over, who is the winner
	 *             PLAYER1 if Player 1 wins
	 *             PLAYER2 if Player 2 wins
	 *             PLAYER_NULL if it's a draw (both teams die simultaneously)
	 *
	 * @return true If the game is over
	 * @return false If the game is not over
	 */
	virtual bool IsGameOver(PlayerId &winner) = 0;

	/**
	 * Get pointer to Actor by ActorId
	 *
	 * @param player_id PlayerId
	 * @param actor_id ActorId
	 * @return Actor*
	 */
	virtual Actor *FindActorById(PlayerId player_id, ActorId actor_id) = 0;
};
} // namespace state
