/**
 * @file idle_state.h
 * Declares the soldier idle state
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#pragma once

namespace state {

/**
 * The idle soldier state class
 */
class STATE_EXPORT SoldierIdleState : public SoldierState {
  public:
	SoldierIdleState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear any attack target or destination
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If there's an attack target in range, switch to attack state
	 * If it's not in range, switch to pursuit state
	 * Else, remain in idle state. Do nothing
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the Soldier switches to another state
	 */
	void Exit() override;
};
} // namespace state
