/**
 * @file soldier.h
 * Declares the Base Soldier class for each troop
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/unit.h"

namespace state {

/**
 * Soldier class that defines properties of a single soldier
 */
class STATE_EXPORT Soldier : public Unit {
  protected:
  public:
	/**
	 * Soldier Constructor
	 */
	Soldier();

	Soldier(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector<int64_t> position, int64_t speed,
	        int64_t attack_range, int64_t attack_damage);

	/**
	 * Late Update function of the soldier
	 */
	void LateUpdate() override;

	/**
	 * Update function of the soldier
	 */
	void Update() override;
};
} // namespace state
