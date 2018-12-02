/**
 * @file soldier_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/soldier.fwd.h"
#include "state/state_export.h"
#include <memory>

#pragma once

namespace state {

/**
 * Define a name for each state
 */
enum class SoldierStateName {
	// Soldier is doing nothing
	IDLE,
	// Soldier is in movement
	MOVE,
	// Soldier is currently attacking another actor
	ATTACK,
	// Soldier is currently moving to attack another actor
	PURSUIT,
	// Soldier is dead
	DEAD
};

/**
 * The base state that other SoldierStates inherit from
 */
class STATE_EXPORT SoldierState : public IActorState {
  protected:
	/**
	 * Name of this state
	 */
	SoldierStateName state_name;

	/**
	 * Soldier that this state is managing
	 */
	Soldier *soldier;

  public:
	/**
	 * Constructor for SoldierState
	 */
	SoldierState(SoldierStateName state_name, Soldier *soldier);

	/**
	 * Get this state's name
	 */
	SoldierStateName GetName();
};
} // namespace state
