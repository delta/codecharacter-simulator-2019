/**
 * @file i_actor_state.h
 * Declares an interface for actor states
 */

#include "state/state_export.h"
#include <memory>

#pragma once

namespace state {

class STATE_EXPORT IActorState {
  public:
	/**
	 * Called right after the actor switches to this state
	 */
	virtual void Enter() = 0;

	/**
	 * Executes state code when called
	 * Returns the next actor state when a state transition occurs
	 * Returns nullptr if state code has been executed (no transition)
	 *
	 * @return      A pointer to the new state
	 */
	virtual std::unique_ptr<IActorState> Update() = 0;

	/**
	 * Called before the actor switches to another state
	 */
	virtual void Exit() = 0;
};
} // namespace state
