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
	                      physics::Vector<int64_t> position) = 0;

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
	 * Handles factory build
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  villager_id   villager to build
	 * @param[in]  offset        grid location to build the factory
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void BuildFactory(PlayerId player_id, ActorId villager_id,
	                          physics::Vector<int64_t> offset) = 0;

	/**
	 * Overload on BuildFactory to accept a Factory id instead of offset
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
	virtual const std::array<int64_t, 2> GetMoney() = 0;

	/**
	 * Get game scores from state
	 *
	 * @return scores
	 */
	virtual const std::array<int64_t, 2> GetScores() = 0;
};
} // namespace state
