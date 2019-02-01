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
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If villager is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If the mine target is set, switch to mine state
	 * If the attack target is set, switch to attack state
	 * If the build target is in range, switch to build state
	 * Else, remain in move_to_build state. Move towards build target
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
