/**
 * @file factory_state.cpp
 * Defines the base state that FactoryStates inherit from
 */

#include "state/actor/factory_states/factory_state.h"
#include "state/actor/factory.h"

namespace state {

FactoryState::FactoryState(FactoryStateName state_name, Factory *factory)
    : state_name(state_name), factory(factory) {}

FactoryStateName FactoryState::GetName() { return state_name; }
} // namespace state
