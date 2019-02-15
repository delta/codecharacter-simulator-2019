/**
 * @file actor.h
 * Declares the Base Unit class that all mobile actors derive from
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/path_planner/path_planner.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <memory>

namespace state {
/**
 * Unit base class
 */
class STATE_EXPORT Unit : public Actor {
  protected:
	/**
	 * Soldier speed, in map-units/turn
	 */
	int64_t speed;

	/**
	 * Distance in units around the soldier where actors can be attacked
	 */
	int64_t attack_range;

	/**
	 * Damage to inflict in one attack
	 */
	int64_t attack_damage;

	/**
	 * Path Planner instance to perform transactions
	 */
	PathPlanner *path_planner;

	/**
	 * Actor that this soldier targets to attack
	 * nullptr if unset
	 */
	Actor *attack_target;

	/**
	 * Destination that the soldier needs to navigate to
	 * Valid only if is_destination_set is true
	 */
	Vec2D destination;

	/**
	 * true if the Soldier is currently moving to the set destination
	 * false otherwise
	 */
	bool is_destination_set;

	/**
	 * Position where the soldier should move to, if the soldier must move
	 * Serves as a temp for updating soldier positions
	 */
	DoubleVec2D new_position;

	/**
	 * true if new_position is set
	 * false otherwise
	 */
	bool is_new_position_set;

  public:
	Unit();

	Unit(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	     int64_t max_hp, DoubleVec2D position, GoldManager *gold_manager,
	     PathPlanner *path_planner, int64_t speed, int64_t attack_range,
	     int64_t attack_damage);

	virtual ~Unit() {}

	/**
	 * Get the unit's speed stat
	 *
	 * @return     Unit Speed
	 */
	int64_t GetSpeed();

	/**
	 * Get the unit's attack range
	 *
	 * @return     Unit's Attack Range
	 */
	int64_t GetAttackRange();

	/**
	 * Get the unit's attack damage
	 *
	 * @return     Unit's Attack Damage
	 */
	int64_t GetAttackDamage();

	/**
	 * Get the unit's attack target
	 *
	 * @return     attack_target  Unit's Attack Target
	 */
	Actor *GetAttackTarget();

	/**
	 * Get the unit's PathPlanner Pointer
	 *
	 * @return     path_planner  Unit's PathPlanner Pointer
	 */
	PathPlanner *GetPathPlanner();

	/**
	 * Set the soldier's attack target
	 *
	 * @param[in]  attack_target  Soldier's Attack Target
	 */

	void SetAttackTarget(Actor *attack_target);

	/**
	 * Check if the attack target is in range
	 *
	 * @return     true if target is in range, false otherwise
	 */
	bool IsAttackTargetInRange();

	/**
	 * Get the unit's destination
	 *
	 * @return     Unit's Destination
	 */
	Vec2D GetDestination();

	/**
	 * Set the unit's destination
	 *
	 * @param[in]  Unit's Destination
	 */
	void SetDestination(Vec2D destination);

	/**
	 * Check if the destination parameter is set
	 *
	 * @return     the destination_set field
	 */
	bool IsDestinationSet();

	/**
	 * Clear the current destination
	 * Set destination_set to false
	 */
	void ClearDestination();

	/**
	 * Get the unit's new_position
	 *
	 * @return     Unit's new_position
	 */
	DoubleVec2D GetNewPosition();

	/**
	 * Set the unit's new_position
	 *
	 * @param[in]  Unit's new_position
	 */
	void SetNewPosition(DoubleVec2D new_position);

	/**
	 * Check if the new_position parameter is set
	 *
	 * @return     the is_new_position_set field
	 */
	bool IsNewPositionSet();

	/**
	 * Clear the current new_position
	 * Set is_new_position_set to false
	 */
	void ClearNewPosition();

	/**
	 * Check if the attack target is set
	 *
	 * @return     the attack_target field
	 */
	bool IsAttackTargetSet();

	/**
	 * Set the actor's position
	 *
	 * @param[in]  Actor's new position
	 */
	void SetPosition(DoubleVec2D position);

	/**
	 * Method to give the move command to this soldier
	 *
	 * @param[in]     destination    The destination to go to
	 */
	void Move(Vec2D destination);

	/**
	 * Method to give the attack command to this soldier
	 *
	 * @param[in]     attack_target  The target to attack
	 */
	void Attack(Actor *attack_target);

	/**
	 * Late Update function of the soldier
	 */
	virtual void LateUpdate() = 0;

	/**
	 * Late Update function of the soldier
	 */
	virtual void Update() override = 0;
};
} // namespace state
