/**
 * @file move_state.h
 * Declares the soldier move state
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#pragma once

namespace state {

/**
 * The move soldier state class
 */
class STATE_EXPORT SoldierMoveState : public SoldierState {
  public:
	SoldierMoveState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear attack target
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a target in range, switch to attack state
	 * If there's a target out of range, switch to pursuit state
	 * If destination has been reached, switch to idle state
	 * Else, remain in move state. Move towards the destination
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the Soldier switches to another state
	 *
	 * Clear destination
	 */
	void Exit() override;
};
} // namespace state
