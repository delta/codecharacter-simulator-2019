/**
 * @file factory.cpp
 * Defines the Factory class
 */

#include "state/actor/factory.h"
#include "state/actor/factory_states/factory_unbuilt_state.h"

#include <algorithm>

namespace state {

Factory::Factory() {}

Factory::Factory(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector<int64_t> position,
                 GoldManager *gold_manager, int64_t construction_complete,
                 int64_t construction_total, ActorType production_state,
                 int64_t villager_frequency, int64_t soldier_frequency,
                 UnitProductionCallback unit_production_callback)
    : Actor(id, player_id, actor_type, hp, max_hp, position, gold_manager),
      construction_complete(construction_complete),
      construction_total(construction_total),
      production_state(production_state), stopped(false),
      villager_frequency(villager_frequency),
      soldier_frequency(soldier_frequency),
      state(std::make_unique<FactoryUnbuiltState>(this)),
      unit_production_callback(unit_production_callback) {}

void Factory::ProduceUnit() {
	unit_production_callback(player_id, production_state, position);
}

void Factory::SetUnitProductionCallback(UnitProductionCallback callback) {
	this->unit_production_callback = callback;
}

void Factory::IncrementConstructionCompletion(int64_t value) {
	construction_complete =
	    std::min(construction_complete + value, construction_total);
}

int64_t Factory::GetConstructionCompletion() { return construction_complete; }

int64_t Factory::GetTotalConstructionCompletion() { return construction_total; }

bool Factory::IsConstructionComplete() {
	return construction_complete == construction_total;
}

bool Factory::IsStopped() { return stopped; }

void Factory::Stop() { stopped = true; }

void Factory::Start() { stopped = false; }

int64_t Factory::GetLatestHp() { return hp - damage_incurred; }

void Factory::Damage(int64_t damage_amount) {
	this->damage_incurred =
	    std::min<int64_t>(this->hp, this->damage_incurred + damage_amount);
}

int64_t Factory::GetVillagerFrequency() { return this->villager_frequency; }

int64_t Factory::GetSoldierFrequency() { return this->soldier_frequency; }

ActorType Factory::GetProductionState() { return production_state; }

void Factory::SetProductionState(ActorType production_state) {
	this->production_state = production_state;
}

FactoryStateName Factory::GetState() { return state->GetName(); }

void Factory::LateUpdate() {}

void Factory::Update() {
	auto new_state = state->Update();

	while (new_state != nullptr) {
		// State transition has occured
		state->Exit();
		state = std::unique_ptr<FactoryState>(
		    static_cast<FactoryState *>(new_state.release()));
		state->Enter();
		new_state = state->Update();
	}
}

Factory Factory::Clone() {
	auto new_factory = Factory(
	    id, player_id, actor_type, hp, max_hp, position, gold_manager,
	    construction_complete, construction_total, production_state,
	    villager_frequency, soldier_frequency, unit_production_callback);

	return new_factory;
}

} // namespace state
