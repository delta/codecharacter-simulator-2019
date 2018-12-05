/**
 * @file factory_dead_state.cpp
 * Defines the factory dead state
 */

#include "state/actor/factory_states/factory_dead_state.h"
#include "state/actor/factory.h"

namespace state {

FactoryDeadState::FactoryDeadState(Factory *factory)
    : FactoryState(FactoryStateName::DEAD, factory) {}

void FactoryDeadState::Enter() {}

std::unique_ptr<IActorState> FactoryDeadState::Update() { return nullptr; }

void FactoryDeadState::Exit() {}
} // namespace state
