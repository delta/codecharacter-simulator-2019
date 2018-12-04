/**
 * @file move_state.h
 * Declares the villager move state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The move villager state class
 */
class STATE_EXPORT VillagerMoveState : public VillagerState {
  public:
	VillagerMoveState(Villager *villager);

	/**
	 * Called right after the villager switches to this state
	 *
	 * Clear attack target
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If villager is dead, switch to dead state
	 * If the build target is set, switch to build state
	 * If there's a target in range, switch to attack state
	 * If there's a target out of range, switch to pursuit state
	 * If destination has been reached, switch to idle state
	 * Else, remain in move state. Move towards the destination
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the villager switches to another state
	 *
	 * Clear destination
	 */
	void Exit() override;
};
} // namespace state