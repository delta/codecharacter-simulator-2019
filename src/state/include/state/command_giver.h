/**
 * @file command_giver.h
 * Command giver class that calls command taker methods
 */

#pragma once

#include "logger/error_type.h"
#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/map/map.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * CommandGiver class to call command taker methods
 */
class STATE_EXPORT CommandGiver : public ICommandGiver {
  private:
	/**
	 * Instance of overall state that player can't modify
	 */
	ICommandTaker *state;

	/**
	 * Instance of logger to log user errors
	 */
	logger::ILogger *logger;

	/**
	 * Helper method that validates input and calls AttackActor
	 */
	void AttackActor(PlayerId player_id, ActorId unit_id,
	                 ActorId enemy_actor_id);
	/**
	 * Helper method that validates whether the actor who the unit is attacking
	 * is an enemy actor or not
	 */
	bool IsValidTarget(int64_t player_id, int64_t enemy_actor_id,
	                   ActorType &target_type, bool &found);

	/**
	 * Helper method that validates input and calls BuildFactory
	 */
	void CreateFactory(PlayerId player_id, ActorId villager_id, Vec2D offset,
	                   ActorType unit_type);

	/**
	 * Helper method that validates input and calls BuildFactory
	 */
	void BuildFactory(PlayerId player_id, ActorId villager_id,
	                  ActorId factory_id);

	/**
	 * Helper method that validates input and calls MoveUnit
	 */
	void MoveUnit(PlayerId player_id, ActorId actor_id, Vec2D position);

	/**
	 * Helper method that validates input and calls SetFactoryProduction
	 */
	void SetFactoryProduction(PlayerId player_id, ActorId actor_id,
	                          ActorType actor_type);

	/**
	 * Helper method that validates input and calls StopOrStartFactory
	 */
	void StopOrStartFactory(PlayerId player_id, ActorId factory_id,
	                        bool should_stop);

	/**
	 * Helper method that validates input and calls MineLocation
	 */
	void MineLocation(PlayerId player_id, ActorId villager_id,
	                  Vec2D mine_location);

	/**
	 * Helper function to check whether the position is valid
	 */
	bool IsValidPosition(Vec2D position) const;

	/**
	 * Helper function to check whether the position is valid
	 */
	bool IsValidOffset(Vec2D position) const;

	/**
	 * Helper function to check whether the target actor to attack is dead
	 */
	bool IsDeadTarget(int64_t player_id, int64_t enemy_actor_id,
	                  ActorType &enemy_type);

	/**
	 * Helper function to check whether a position already has a factory in it's
	 * position, by the given player_id
	 *
	 * If occupied, returns actor id of that factory through occupied_actor_id
	 */
	bool IsOccupied(int64_t player_id, Vec2D offset,
	                std::array<std::vector<Factory *>, 2> state_factories,
	                ActorId &occupied_actor_id);

	/**
	 * Helper function to flip a given position
	 *
	 * @param map Pointer to map
	 * @param position Input position
	 * @return DoubleVec2D Output Player2 position
	 */
	DoubleVec2D FlipPosition(const Map *map, DoubleVec2D position);

	/**
	 * Helper function to flip a given offset
	 *
	 * @param map Pointer to map
	 * @param offset Input offset
	 * @return Vec2D Output Player2 offset
	 */
	Vec2D FlipOffset(const Map *map, Vec2D offset);

  public:
	CommandGiver();

	CommandGiver(ICommandTaker *state, logger::ILogger *logger);

	/**
	 * @see ICommandGiver#RunCommands
	 */
	void RunCommands(const std::array<player_state::State, 2> &player_states,
	                 std::array<bool, 2> skip_player_turn);
};

} // namespace state
