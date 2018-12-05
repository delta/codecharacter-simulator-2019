/**
 * @file factory_dead_state.h
 * Declares the factory dead state
 */

#include "state/actor/factory.fwd.h"
#include "state/actor/factory_states/factory_state.h"

#pragma once

namespace state {

/**
 * The dead factory state class
 */
class STATE_EXPORT FactoryDeadState : public FactoryState {
  public:
	FactoryDeadState(Factory *factory);

	/**
	 * Called right after the factory switches to this state
	 */
	void Enter() override;

	/**
	 * Performs state transitions
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
