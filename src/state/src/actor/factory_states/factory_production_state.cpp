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

		// Make a villager and reset production
		// Checking if the player has enough gold to create unit
		auto current_gold = gold_manager->GetBalance(factory->GetPlayerId());
		auto create_unit_cost =
		    gold_manager->GetCreateUnitCost(ActorType::VILLAGER);

		if (current_gold >= create_unit_cost) {
			factory->ProduceUnit();
			current_production_tick = 0;
		}

		// If the player dosen't have enough gold, switching the factory back to
		// idle state
		else {
			return std::make_unique<FactoryIdleState>(factory);
		}

	} else {
		// Do we need to make a soldier?
		if (current_production_tick % factory->GetSoldierFrequency() != 0) {
			current_production_tick++;
			return nullptr;
		}

		// Make a soldier and reset production
		// Checking if the player has enough gold to produce a soldier
		auto current_gold = gold_manager->GetBalance(factory->GetPlayerId());
		auto create_unit_cost =
		    gold_manager->GetCreateUnitCost(ActorType::SOLDIER);

		if (current_gold >= create_unit_cost) {
			factory->ProduceUnit();
			current_production_tick = 0;
		}

		// If the player dosen't have enough gold, switching the factory back to
		// idle state
		else {
			return std::make_unique<FactoryIdleState>(factory);
		}
	}

	current_production_tick++;
	return nullptr;
}

void FactoryProductionState::Exit() {}
} // namespace state
