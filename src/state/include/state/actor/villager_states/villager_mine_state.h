/**
 * @file mine_state.h
 * Declares the villager mine state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The mine villager state class
 */
class STATE_EXPORT VillagerMineState : public VillagerState {
  public:
	VillagerMineState(Villager *villager);

	/**
	 * Called right after the villager switches to this state
	 *
	 * Clear destination
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * If villager is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If the build target is set, switch to build state
	 * If the attack target is set, switch to attack state
	 * If the mine is out of range, switch to move_to_mine_state
	 * Else, remain in build state. Increment Effort to build target factory
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the villager switches to another state
	 */
	void Exit();
};
} // namespace state
