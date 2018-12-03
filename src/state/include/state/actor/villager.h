/**
 * @file villager.h
 * Declares the Base Villager class for each troop
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/factory.h"
#include "state/actor/unit.h"
#include "state/actor/villager_states/villager_state.h"
#include "state/gold_manager/gold_manager.h"

namespace state {

/**
 * Villager class that defines properties of a single Villager
 */
class STATE_EXPORT Villager : public Unit {
  protected:
	/*
	 * State class that controls logic for villager's current state
	 */
	std::unique_ptr<VillagerState> state;

	/*
	 * Factory which the villager needs to build
	 * nullptr if unset
	 */
	Factory *build_target;

	/**
	 * Amount of effort by the villager on the factory in single turn
	 */
	int64_t build_effort;

	/**
	 * Distance in units around the villager where factories can be built
	 */
	int64_t build_range;

  public:
	/**
	 * Villager Constructor
	 */
	Villager();

	Villager(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	         int64_t max_hp, physics::Vector<int64_t> position,
	         GoldManager *gold_manager, int64_t speed, int64_t attack_range,
	         int64_t attack_damage, int64_t build_effort, int64_t build_range);

	/**
	 * Get the villager's build range
	 *
	 * @return     Villager's Build Range
	 */
	int64_t GetBuildRange();

	/**
	 * Get the villager's build effort
	 *
	 * @return     Villager's Build Effort
	 */
	int64_t GetBuildEffort();

	/**
	 * Get the villager's build target
	 *
	 * @return     build_target  villager's build Target
	 */
	Factory *GetBuildTarget();

	/**
	 * Set the villager's build target
	 *
	 * @param[in]  build_target  Villager's Build Target
	 */
	void SetBuildTarget(Factory *build_target);

	/**
	 * Check if the build target is set
	 *
	 * @return     true if build_target is set, false otherwise
	 */
	bool IsBuildTargetSet();

	/**
	 * Check if the build target is in range
	 *
	 * @return     true if target is in range, false otherwise
	 */
	bool IsBuildTargetInRange();

	/**
	 * Method to give the build command to this villager
	 *
	 * @param[in]     build_target  The target factory to build
	 */
	void Build(Factory *build_target);

	/**
	 * Get the name of the current state
	 *
	 * @return     Name of current state of type VillagerStateName
	 */
	VillagerStateName GetState();

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
