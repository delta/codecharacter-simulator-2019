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
#include "state/path_planner/path_planner.h"
#include "state/utilities.h"

#include <array>
#include <memory>
#include <vector>

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
	 * Path Planner instance
	 */
	std::unique_ptr<PathPlanner> path_planner;

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

	/**
	 * Model villager that is used to create new villager clones
	 */
	Villager model_villager;

	/**
	 * Model soldier that is used to create new soldier clones
	 */
	Soldier model_soldier;

	/**
	 * Model factory that is used to create new factory clones
	 */
	Factory model_factory;

	/**
	 * Get pointer to Factory given map offset
	 *
	 * @param player_id
	 * @param offset
	 * @return Factory*
	 */
	Factory *FindFactoryByOffset(PlayerId player_id, Vec2D offset);

	/**
	 * Create a new factory at the given offset
	 *
	 * @param p_player_id
	 * @param offset
	 * @return std::unique_ptr<Factory>
	 */
	std::unique_ptr<Factory> FactoryBuilder(PlayerId p_player_id, Vec2D offset);

	/**
	 * Create a new villager at the given position
	 *
	 * @param p_player_id
	 * @param position
	 * @return std::unique_ptr<Villager>
	 */
	std::unique_ptr<Villager> VillagerBuilder(PlayerId p_player_id,
	                                          DoubleVec2D position);

	/**
	 * Create a new soldier at the given position
	 *
	 * @param p_player_id
	 * @param position
	 * @return std::unique_ptr<Soldier>
	 */
	std::unique_ptr<Soldier> SoldierBuilder(PlayerId p_player_id,
	                                        DoubleVec2D position);

	/**
	 * Function to create a unit. This function is accessible by every factory,
	 * and is used for producing a new unit.
	 */
	void ProduceUnit(PlayerId player_id, ActorType actor_type,
	                 DoubleVec2D position);

  public:
	/**
	 * Constructor
	 */
	State(std::unique_ptr<Map> map, std::unique_ptr<GoldManager> gold_manager,
	      std::unique_ptr<PathPlanner> path_planner,
	      std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
	      std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
	      std::array<std::vector<std::unique_ptr<Factory>>, 2> factories,
	      Villager model_villager, Soldier model_soldier,
	      Factory model_factory);

	/**
	 * @see ICommandTaker#MoveUnit
	 */
	void MoveUnit(PlayerId player_id, ActorId actor_id,
	              Vec2D position) override;

	/**
	 * Get pointer to Actor by ActorId
	 *
	 * @param player_id PlayerId
	 * @param actor_id ActorId
	 * @return Actor*
	 */
	Actor *FindActorById(PlayerId player_id, ActorId actor_id) override;

	/**
	 * @see ICommandTaker#MineLocation
	 */
	void MineLocation(PlayerId player_id, ActorId villager_id,
	                  Vec2D mine_location) override;

	/**
	 * @see ICommandTaker#AttackActor
	 */
	void AttackActor(PlayerId player_id, ActorId unit_id,
	                 ActorId enemy_actor_id) override;

	/**
	 * @see ICommandTaker#CreateFactory
	 */
	void CreateFactory(PlayerId player_id, ActorId villager_id,
	                   Vec2D offset) override;

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
	const std::array<int64_t, 2> GetGold() override;

	/**
	 * @see ICommandTaker#GetScores
	 */
	const std::array<int64_t, 2> GetScores() override;

	/**
	 * @see ICommandTaker#IsGameOver
	 */
	bool IsGameOver(PlayerId &winner) override;

	/**
	 * State's update method, to call updates on all actors
	 */
	void Update() override;
};
} // namespace state
