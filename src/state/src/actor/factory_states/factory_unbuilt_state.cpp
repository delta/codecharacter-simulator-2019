/**
 * @file factory_unbuilt_state.cpp
 * Defines the Factory unbuilt state
 */

#include "state/actor/factory_states/factory_unbuilt_state.h"
#include "state/actor/factory.h"
#include "state/actor/factory_states/factory_dead_state.h"
#include "state/actor/factory_states/factory_idle_state.h"
#include "state/actor/factory_states/factory_production_state.h"
#include "state/actor/factory_states/factory_state.h"

namespace state {

FactoryUnbuiltState::FactoryUnbuiltState(Factory *factory)
    : FactoryState(FactoryStateName::UNBUILT, factory) {}

void FactoryUnbuiltState::Enter() {}

std::unique_ptr<IActorState> FactoryUnbuiltState::Update() {
	// If HP is 0, transition to dead state
	if (factory->GetHp() == 0) {
		return std::make_unique<FactoryDeadState>(factory);
	}

	// Check if construction is completed. If so, move to Idle state
	// Replenish factory HP to full
	if (factory->IsConstructionComplete()) {
		factory->SetHp(factory->GetMaxHp());
		return std::make_unique<FactoryIdleState>(factory);
	}

	// Else, do nothing and remain unbuilt

	return nullptr;
}

void FactoryUnbuiltState::Exit() {}
} // namespace state
