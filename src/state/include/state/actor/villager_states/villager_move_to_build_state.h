/**
 * @file build_state.h
 * Declares the villager move_to_build state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The move_to_build villager state class
 */
class STATE_EXPORT VillagerMoveToBuildState : public VillagerState {
  public:
	VillagerMoveToBuildState(Villager *villager);

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
	 * If there's an attack target in range, switch to attack state
	 * If it's not in range, switch to pursuit state
	 * If the build target is complete, switch to idle state
	 * If the build target is in range, switch to build state
	 * Else, remain in move_to_build state. Move towards build target
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
