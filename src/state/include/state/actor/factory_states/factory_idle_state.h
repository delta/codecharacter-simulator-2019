/**
 * @file factory_idle_state.h
 * Declares the factory idle state
 */

#include "state/actor/factory.fwd.h"
#include "state/actor/factory_states/factory_state.h"

#pragma once

namespace state {

/**
 * The idle factory state class
 */
class STATE_EXPORT FactoryIdleState : public FactoryState {
  public:
	FactoryIdleState(Factory *factory);

	/**
	 * Called right after the factory switches to this state
	 *
	 * Clear any attack target or destination
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If factory is dead, switch to dead state
	 * If we can afford to produce units, switch to production state
	 * Else, remain in idle state
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<IActorState> Update() override;

	/**
	 * Called before the factory switches to another state
	 */
	void Exit() override;
};
} // namespace state
