/**
 * @file factory_idle_state.cpp
 * Defines the Factory idle state
 */

#include "state/actor/factory_states/factory_idle_state.h"
#include "state/actor/factory.h"
#include "state/actor/factory_states/factory_dead_state.h"
#include "state/actor/factory_states/factory_production_state.h"
#include "state/actor/factory_states/factory_state.h"

namespace state {

FactoryIdleState::FactoryIdleState(Factory *factory)
    : FactoryState(FactoryStateName::IDLE, factory) {}

void FactoryIdleState::Enter() {}

std::unique_ptr<IActorState> FactoryIdleState::Update() {
	// If HP is 0, transition to dead state
	if (factory->GetHp() == 0) {
		return std::make_unique<FactoryDeadState>(factory);
	}

	// If we're in Idle state, it means there was not enough money to produce
	// any units. Check it there's enough money again, and resume production

	auto gold_manager = factory->GetGoldManager();
	auto gold = gold_manager->GetBalance(factory->GetPlayerId());

	auto curr_production = factory->GetProductionState();
	auto curr_unit_cost = gold_manager->GetCreateUnitCost(curr_production);

	// If we can afford to produce the current unit and factory is not stopped
	if (gold >= curr_unit_cost && not factory->IsStopped()) {
		return std::make_unique<FactoryProductionState>(factory);
	}

	return nullptr;
}

void FactoryIdleState::Exit() {}
} // namespace state
