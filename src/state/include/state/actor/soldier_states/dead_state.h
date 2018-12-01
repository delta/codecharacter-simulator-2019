/**
 * @file dead_state.h
 * Declares the soldier dead state
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#pragma once

namespace state {

/**
 * The dead soldier state class
 */
class STATE_EXPORT DeadState : public SoldierState {
  public:
	DeadState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<SoldierState> Update();

	/**
	 * Called before the Soldier switches to another state
	 */
	void Exit();
};
} // namespace state
