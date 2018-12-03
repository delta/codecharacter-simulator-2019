/**
 * @file idle_state.h
 * Declares the villager idle state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The idle villager state class
 */
class STATE_EXPORT VillagerIdleState : public VillagerState {
  public:
	VillagerIdleState(Villager *villager);

	/**
	 * Called right after the villager switches to this state
	 *
	 * Clear any attack target or destination
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If villager is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If the build target is set, switch to build state
	 * If there's an attack target in range, switch to attack state
	 * If it's not in range, switch to pursuit state
	 * Else, remain in idle state. Do nothing
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the villager switches to another state
	 */
	void Exit() override;
};
} // namespace state
