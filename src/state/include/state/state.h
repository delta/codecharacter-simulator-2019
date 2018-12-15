/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/gold_manager/gold_manager.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/utilities.h"

#include <memory>

namespace state {

class STATE_EXPORT State : public ICommandTaker {
  private:
	/**
	 * Map instance that maintains game terrain
	 */
	std::unique_ptr<Map> map;

	/**
	 * Gold Manager instance to maintain player gold
	 */
	std::unique_ptr<GoldManager> gold_manager;

	/**
	 * List of soldiers, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers;

	/**
	 * List of villagers, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers;

	/**
	 * List of factories, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Factory>>, 2> factories;

  public:
	/**
	 * Constructor
	 */
	State(std::unique_ptr<Map> map, std::unique_ptr<GoldManager> gold_manager,
	      std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
	      std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
	      std::array<std::vector<std::unique_ptr<Factory>>, 2> factories);

	/**
	 * @see ICommandTaker#MoveUnit
	 */
	void MoveUnit(PlayerId player_id, ActorId actor_id,
	              physics::Vector<int64_t> position) override;

	/**
	 * @see ICommandTaker#MineLocation
	 */
	void MineLocation(PlayerId player_id, ActorId villager_id,
	                  physics::Vector<int64_t> mine_location) override;

	/**
	 * @see ICommandTaker#AttackActor
	 */
	void AttackActor(PlayerId player_id, ActorId unit_id,
	                 ActorId enemy_actor_id) override;

	/**
	 * @see ICommandTaker#BuildFactory
	 */
	void CreateFactory(PlayerId player_id, ActorId villager_id,
	                   physics::Vector<int64_t> offset) override;

	/**
	 * @see ICommandTaker#BuildFactory
	 */
	void BuildFactory(PlayerId player_id, ActorId villager_id,
	                  ActorId factory_id) override;

	/**
	 * @see ICommandTaker#SetFactoryProduction
	 */
	void SetFactoryProduction(PlayerId player_id, ActorId factory_id,
	                          ActorType production) override;

	/**
	 * @see ICommandTaker#StopOrStartFactory
	 */
	void StopOrStartFactory(PlayerId player_id, ActorId factory_id,
	                        bool should_stop) override;

	/**
	 * @see ICommandTaker#GetSoldiers
	 */
	const std::array<std::vector<Soldier *>, 2> GetSoldiers() override;

	/**
	 * @see ICommandTaker#GetVillagers
	 */
	const std::array<std::vector<Villager *>, 2> GetVillagers() override;

	/**
	 * @see ICommandTaker#GetFactories
	 */
	const std::array<std::vector<Factory *>, 2> GetFactories() override;

	/**
	 * @see ICommandTaker#GetMap
	 */
	const Map *GetMap() override;

	/**
	 * @see ICommandTaker#GetMoney
	 */
	const std::array<int64_t, 2> GetMoney() override;

	/**
	 * @see ICommandTaker#GetScores
	 */
	const std::array<int64_t, 2> GetScores() override;

	/**
	 * State's update method, to call updates on all actors
	 */
	void Update() override;
};
} // namespace state
