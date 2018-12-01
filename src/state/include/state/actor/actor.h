/**
 * @file actor.h
 * Declares the Base Actor class that all actors derive from
 */

#pragma once

#include "physics/vector.hpp"
#include "state/gold_manager/gold_manager.h"
#include "state/interfaces/i_updatable.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <memory>

namespace state {
/**
 * Actor base class
 */
class STATE_EXPORT Actor : public IUpdatable {
  protected:
	/**
	 * Unique ID of the actor
	 */
	ActorId id;

	/**
	 * Static counter that's used to assign a unique
	 * incrementing id to all actors
	 */
	static ActorId actor_id_increment;

	/**
	 * ID of the player that the actor belongs to
	 */
	PlayerId player_id;

	/**
	 * Actor Type
	 */
	ActorType actor_type;

	/**
	 * Current HP of the Actor
	 */
	int64_t hp;

	/**
	 * Maximum possible HP of the actor
	 */
	int64_t max_hp;

	/**
	 * Actor position
	 */
	physics::Vector<int64_t> position;

	/**
	 * Gold manager instance to perform transactions
	 */
	GoldManager *gold_manager;

  public:
	Actor();

	Actor(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	      int64_t max_hp, physics::Vector<int64_t> position,
	      GoldManager *gold_manager);

	virtual ~Actor() {}

	/**
	 * Gets the next actor id to assign to new actors
	 *
	 * @return     The new actor id to assign
	 */
	static ActorId GetNextActorId();

	/**
	 * Sets the auto incrementing actor id,
	 * Resets it to 0 when no parameter is passed
	 *
	 * @param[in]   actor_id ActorId to set
	 *
	 * @throw       std::out_of_range If actor_id is negative
	 */
	static void SetActorIdIncrement(ActorId actor_id = 0);

	/**
	 * Get actor id
	 *
	 * @return     ActorId of the actor
	 */
	ActorId GetActorId();

	/**
	 * Get the id of the player the actor belongs to
	 *
	 * @return     PlayerId of the actor
	 */
	PlayerId GetPlayerId();

	/**
	 * Get the actor type
	 *
	 * @return     PlayerType of the Actor
	 */
	ActorType GetActorType();

	/**
	 * Get the current HP of the Actor
	 *
	 * @return     Actor HP
	 */

	/**
	 * Get the unit's GoldManager Pointer
	 *
	 * @return     gold_manager  Unit's GoldManager Pointer
	 */
	GoldManager *GetGoldManager();

	int64_t GetHp();

	/**
	 * Get the maximum hp of the actor
	 *
	 * @return     Actor Max HP
	 */
	int64_t GetMaxHp();

	/**
	 * Set the HP of the Actor
	 *
	 * @param[in]  hp New HP to set
	 *
	 * @throw std::out_of_range If hp is negative or greater
	 *                          than the max_hp
	 */
	void SetHp(int64_t hp);

	/**
	 * Gets the latest version of HP
	 *
	 * Some Actors may buffer damage accumulated over a turn and apply it at
	 * the turn's end. This method returns HP after applying buffered damage
	 *
	 * @return     The latest HP
	 */
	virtual int64_t GetLatestHp() = 0;

	/**
	 * Reduces the Actor's HP by the specified amount
	 *
	 * Ensures that HP does not drop below zero
	 *
	 * @param[in]  damage_amount  The damage amount
	 */
	virtual void Damage(int64_t damage_amount) = 0;

	/**
	 * Get the position of the actor
	 *
	 * @return     Actor Position
	 */
	physics::Vector<int64_t> GetPosition();
};
} // namespace state
