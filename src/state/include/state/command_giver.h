/**
 * @file command_giver.h
 * Command giver class that calls command taker methods
 */

#pragma once

#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * CommandGiver class to call command taker methods
 */
class STATE_EXPORT CommandGiver {
  private:
	/**
	 * Instance of overall state that player can't modify
	 */
	ICommandTaker *state;

	/**
	 * @see ICommandTaker#AttackActor
	 */
	void AttackActor(PlayerId player_id, ActorId unit_id,
	                 ActorId enemy_actor_id);

	/**
	 * @see ICommandTaker#BuildFactory
	 */
	void CreateFactory(PlayerId player_id, ActorId villager_id,
	                   physics::Vector<int64_t> offset);

	/**
	 * @see ICommandTaker#BuildFactory
	 */
	void BuildFactory(PlayerId player_id, ActorId villager_id,
	                  ActorId factory_id);

	/**
	 * @see ICommandTaker#MoveUnit
	 */
	void MoveUnit(PlayerId player_id, ActorId actor_id,
	              physics::Vector<int64_t> position);

	/**
	 * @see ICommandTaker#SetFactoryProduction
	 */
	void SetFactoryProduction(PlayerId player_id, ActorId actor_id,
	                          ActorType actor_type);

	/**
	 * @see ICommandTaker#StopOrStartFactory
	 */
	void StopOrStartFactory(PlayerId player_id, ActorId factory_id,
	                        bool should_stop);

	/**
	 * @see ICommandTaker#MineLocation
	 */
	void MineLocation(PlayerId player_id, ActorId villager_id,
	                  physics::Vector<int64_t> mine_location);

  public:
	CommandGiver();

	CommandGiver(ICommandTaker *state);

	/**
	 * @see ICommandGiver#RunCommands
	 */
	void RunCommands(ICommandTaker *state,
	                 const std::array<player_state::State, 2> &player_states);
};

} // namespace state
