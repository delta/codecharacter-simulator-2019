/**
 * @file factory_unbuilt_state.h
 * Declares the factory unbuilt state
 */

#include "state/actor/factory.fwd.h"
#include "state/actor/factory_states/factory_state.h"

#pragma once

namespace state {

/**
 * The Unbuilt factory state class
 */
class STATE_EXPORT FactoryUnbuiltState : public FactoryState {
  public:
	FactoryUnbuiltState(Factory *factory);

	/**
	 * Called right after the factory switches to this state
	 */
	void Enter() override;

	/**
	 * Performs state transitions
	 *
	 * If factory construction completed, transision to idle state
	 * Else, remain in unbuilt state
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
