/**
 * @file actor.h
 * Declares the Base Unit class that all mobile actors derive from
 */

#ifndef STATE_ACTOR_UNIT_H
#define STATE_ACTOR_UNIT_H

#include "physics/vector.hpp"
#include "state/actor/actor.h"
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
	 * Actor that this soldier targets to attack
	 * nullptr if unset
	 */
	Actor *attack_target;

	/**
	 * Destination that the soldier needs to navigate to
	 * Valid only if is_destination_set is true
	 */
	physics::Vector<int64_t> destination;

	/**
	 * true if the Soldier is currently moving to the set destination
	 * false otherwise
	 */
	bool is_destination_set;

	/**
	 * Position where the soldier should move to, if the soldier must move
	 * Serves as a temp for updating soldier positions
	 */
	physics::Vector<int64_t> new_position;

	/**
	 * true if new_position is set
	 * false otherwise
	 */
	bool is_new_position_set;

	/**
	 * Amount of damage the soldier incurred in the current turn
	 * Applied to hp at the end of the turn
	 */
	int64_t damage_incurred;

  public:
	Unit();

	Unit(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	     int64_t max_hp, physics::Vector<int64_t> position, int64_t speed,
	     int64_t attack_range, int64_t attack_damage);

	virtual ~Unit();

	/**
	 * Get the unit's speed stat
	 *
	 * @return     Unit Speed
	 */
	int64_t GetSpeed();

	/**
	 * Get the soldier's attack range
	 *
	 * @return     Soldier's Attack Range
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
	physics::Vector<int64_t> GetDestination();

	/**
	 * Set the unit's destination
	 *
	 * @param[in]  Unit's Destination
	 */
	void SetDestination(physics::Vector<int64_t> destination);

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
	physics::Vector<int64_t> GetNewPosition();

	/**
	 * Set the unit's new_position
	 *
	 * @param[in]  Unit's new_position
	 */
	void SetNewPosition(physics::Vector<int64_t> new_position);

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
	 * Set the soldier's position
	 *
	 * @param[in]  Soldier's new position
	 */
	void SetPosition(physics::Vector<int64_t> position);

	/**
	 * Method to give the move command to this soldier
	 *
	 * @param[in]     destination    The destination to go to
	 */
	void Move(physics::Vector<int64_t> destination);

	/**
	 * Method to give the attack command to this soldier
	 *
	 * @param[in]     attack_target  The target to attack
	 */
	void Attack(Actor *attack_target);

	/**
	 * Returns soldier's new_hp
	 *
	 * @see Actor#GetLatestHp
	 *
	 * @return     The soldier's new_hp
	 */
	int64_t GetLatestHp() override;

	/**
	 * @see Actor#Damage
	 */
	void Damage(int64_t damage_amount) override;

	/**
	 * Late Update function of the soldier
	 */
	virtual void LateUpdate() = 0;

	/**
	 * Late Update function of the soldier
	 */
	virtual void Update() = 0;
};
} // namespace state

#endif
