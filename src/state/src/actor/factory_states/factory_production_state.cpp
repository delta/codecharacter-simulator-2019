/**
 * @file factory_production_state.cpp
 * Defines the Factory production state
 */

#include "state/actor/factory_states/factory_production_state.h"
#include "state/actor/factory.h"
#include "state/actor/factory_states/factory_dead_state.h"
#include "state/actor/factory_states/factory_idle_state.h"
#include "state/actor/factory_states/factory_state.h"

namespace state {

FactoryProductionState::FactoryProductionState(Factory *factory)
    : FactoryState(FactoryStateName::PRODUCTION, factory),
      current_production_tick(0) {}

void FactoryProductionState::Enter() {}

std::unique_ptr<IActorState> FactoryProductionState::Update() {
	// If HP is 0, transition to dead state
	if (factory->GetHp() == 0) {
		return std::make_unique<FactoryDeadState>(factory);
	}

	// If we don't have enough gold to produce a unit, transition to idle state
	// If the factory has been stopped, transition to idle state
	auto gold_manager = factory->GetGoldManager();
	auto gold = gold_manager->GetBalance(factory->GetPlayerId());

	auto curr_production = factory->GetProductionState();
	auto curr_unit_cost = gold_manager->GetCreateUnitCost(curr_production);

	if (gold < curr_unit_cost || factory->IsStopped()) {
		return std::make_unique<FactoryIdleState>(factory);
	}

	// We only produce a unit when the tick is a multiple of the frequency
	// Produce unit, set position, and append to list, and reset the tick

	if (factory->GetProductionState() == ActorType::VILLAGER) {
		// Do we need to make a villager?
		if (current_production_tick % factory->GetVillagerFrequency() != 0) {
			current_production_tick++;
			return nullptr;
		}

		// Make a villager
		auto new_villager =
		    std::make_unique<Villager>(std::move(factory->GetModelVillager()));
		new_villager->SetPosition(factory->GetPosition());

		factory->GetVillagerList().push_back(std::move(new_villager));

		// Reset
		current_production_tick = 0;

	} else {
		// Do we need to make a soldier?
		if (current_production_tick % factory->GetSoldierFrequency() != 0) {
			current_production_tick++;
			return nullptr;
		}

		// Make a soldier
		auto new_soldier =
		    std::make_unique<Soldier>(std::move(factory->GetModelSoldier()));
		new_soldier->SetPosition(factory->GetPosition());

		factory->GetSoldierList().push_back(std::move(new_soldier));

		// Reset
		current_production_tick = 0;
	}

	current_production_tick++;
	return nullptr;
}

void FactoryProductionState::Exit() {}
} // namespace state
