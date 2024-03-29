/**
 * @file pursuit_state.h
 * Declares the soldier pursuit state
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#pragma once

namespace state {

/**
 * The pursuit soldier state class
 */
class STATE_EXPORT SoldierPursuitState : public SoldierState {
  public:
	SoldierPursuitState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear destination
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If there target is dead, switch to idle state
	 * If the target is in range, switch to attack state
	 * Else, remain in pursuit state. Move towards the attack target
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
