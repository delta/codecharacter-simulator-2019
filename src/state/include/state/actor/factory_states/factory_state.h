/**
 * @file factory_state.h
 * Declares an interface for factory states
 */

#include "state/actor/factory.fwd.h"
#include "state/actor/interfaces/i_actor_state.h"
#include "state/state_export.h"
#include <memory>

#pragma once

namespace state {

/**
 * Define a name for each state
 */
enum class FactoryStateName {
	// Factory is yet to be built
	UNBUILT,
	// Factory is not producing any units
	IDLE,
	// Factory is producing units
	PRODUCTION,
	// Factory is dead
	DEAD
};

/**
 * The base state that other FactoryStates inherit from
 */
class STATE_EXPORT FactoryState : public IActorState {
  protected:
	/**
	 * Name of this state
	 */
	FactoryStateName state_name;

	/**
	 * Factory that this state is managing
	 */
	Factory *factory;

  public:
	virtual ~FactoryState(){};

	/**
	 * Constructor for FactoryState
	 */
	FactoryState(FactoryStateName state_name, Factory *factory);

	/**
	 * Get this state's name
	 */
	FactoryStateName GetName();
};
} // namespace state
