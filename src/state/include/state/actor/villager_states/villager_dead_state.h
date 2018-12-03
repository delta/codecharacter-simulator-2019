/**
 * @file dead_state.h
 * Declares the villager dead state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The dead villager state class
 */
class STATE_EXPORT VillagerDeadState : public VillagerState {
  public:
	VillagerDeadState(Villager *villager);

	/**
	 * Called right after the villager switches to this state
	 */
	void Enter() override;

	/**
	 * Performs state transitions
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
