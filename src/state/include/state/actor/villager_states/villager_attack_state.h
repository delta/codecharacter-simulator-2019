/**
 * @file attack_state.h
 * Declares the villager attack state
 */

#include "state/actor/villager.fwd.h"
#include "state/actor/villager_states/villager_state.h"

#pragma once

namespace state {

/**
 * The attack villager state class
 */
class STATE_EXPORT VillagerAttackState : public VillagerState {
  public:
	VillagerAttackState(Villager *villager);

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
	 * If the build target is set, switch to build state
	 * If the mine target is set, switch to mine state
	 * If there target is dead, switch to idle state
	 * If the target is out of range, switch to pursuit state
	 * Else, remain in attack state. Inflict damage on the target
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
