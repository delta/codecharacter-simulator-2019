/**
 * @file pursuit_state.h
 * Declares the villager pursuit state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The pursuit villager state class
 */
class STATE_EXPORT VillagerPursuitState : public VillagerState {
  public:
	VillagerPursuitState(Villager *villager);

	/**
	 * Called right after the villager switches to this state
	 *
	 * Clear destination
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If villager is dead, switch to dead state
	 * If the build target is set, switch to build state
	 * If there's a destination set, switch to move state
	 * If there target is dead, switch to idle state
	 * If the target is in range, switch to attack state
	 * Else, remain in pursuit state. Move towards the attack target
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