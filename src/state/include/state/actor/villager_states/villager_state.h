/**
 * @file villager_state.h
 * Declares an interface for villager states
 */

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/villager.fwd.h"
#include "state/state_export.h"
#include <memory>

#pragma once

namespace state {

/**
 * Define a name for each state
 */
enum class VillagerStateName {
	// Villager is doing nothing
	IDLE,
	// Villager is in movement
	MOVE,
	// Villager is currently attacking another actor
	ATTACK,
	// Villager is currently moving to attack another actor
	PURSUIT,
	// Villager is moving towards build target
	MOVE_TO_BUILD,
	// Villager is building build target
	BUILD,
	// Villager is moving to mine mine
	MOVE_TO_MINE,
	// Villager is mining
	MINE,
	// Villager is dead
	DEAD
};

/**
 * The base state that other VillagerStates inherit from
 */
class STATE_EXPORT VillagerState : public IActorState {
  protected:
	/**
	 * Name of this state
	 */
	VillagerStateName state_name;

	/**
	 * Villager that this state is managing
	 */
	Villager *villager;

  public:
	virtual ~VillagerState(){};

	/**
	 * Constructor for VillagerState
	 */
	VillagerState(VillagerStateName state_name, Villager *villager);

	/**
	 * Get this state's name
	 */
	VillagerStateName GetName();
};
} // namespace state
