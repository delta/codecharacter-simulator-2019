/**
 * @file factory_production_state.h
 * Declares the factory production state
 */

#include "state/actor/factory.fwd.h"
#include "state/actor/factory_states/factory_state.h"

#pragma once

namespace state {

/**
 * The production factory state class
 */
class STATE_EXPORT FactoryProductionState : public FactoryState {
  private:
	/**
	 * Counter maintains the number of updates since the last production
	 */
	int64_t current_production_tick;

  public:
	FactoryProductionState(Factory *factory);

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
	 * If we do not have enough gold, switch to idle state
	 * Else, produce units
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
