/**
 * @file Mine_state.h
 * Declares the villager move_to_mine state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The move_to_mine villager state class
 */
class STATE_EXPORT VillagerMoveToMineState : public VillagerState {
  public:
	VillagerMoveToMineState(Villager *villager);

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
	 * If there's an attack target in range, switch to attack state
	 * If it's not in range, switch to pursuit state
	 * If the build target is complete, switch to idle state
	 * If the build target is not in range, switch to move_to_build state
	 * If the build target is in range, switch to build state
	 * If the mine target is set and in range, move to mine state
	 * Else, remain in move_to_mine state. Move towards Mine target
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
