/**
 * @file villager.h
 * Declares the Base Villager class for each troop
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/unit.h"
#include "state/gold_manager/gold_manager.h"

namespace state {

/**
 * Villager class that defines properties of a single Villager
 */
class STATE_EXPORT Villager : public Unit {
  protected:
  public:
	/**
	 * Villager Constructor
	 */
	Villager();

	Villager(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	         int64_t max_hp, physics::Vector<int64_t> position,
	         GoldManager *gold_manager, int64_t speed, int64_t attack_range,
	         int64_t attack_damage);

	/**
	 * Late Update function of the Villager
	 */
	void LateUpdate() override;

	/**
	 * Update function of the Villager
	 */
	void Update() override;
};
} // namespace state
