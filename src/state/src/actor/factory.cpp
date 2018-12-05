/**
 * @file factory.cpp
 * Defines the Factory class
 */

#include "state/actor/factory.h"
#include "state/actor/factory_states/factory_unbuilt_state.h"

#include <algorithm>

namespace state {

Factory::Factory(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector<int64_t> position,
                 GoldManager *gold_manager, int64_t construction_complete,
                 int64_t construction_total, ActorType production_state,
                 int64_t villager_frequency, int64_t soldier_frequency,
                 Villager model_villager, Soldier model_soldier,
                 std::vector<std::unique_ptr<Villager>> &villager_list,
                 std::vector<std::unique_ptr<Soldier>> &soldier_list)
    : Actor(id, player_id, actor_type, hp, max_hp, position, gold_manager),
      construction_complete(construction_complete),
      construction_total(construction_total),
      production_state(production_state), stopped(false),
      villager_frequency(villager_frequency),
      soldier_frequency(soldier_frequency),
      model_villager(std::move(model_villager)),
      model_soldier(std::move(model_soldier)), villager_list(villager_list),
      soldier_list(soldier_list),
      state(std::make_unique<FactoryUnbuiltState>(this)) {}

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

Villager &Factory::GetModelVillager() { return model_villager; }

Soldier &Factory::GetModelSoldier() { return model_soldier; }

ActorType Factory::GetProductionState() { return production_state; }

void Factory::SetProductionState(ActorType production_state) {
	this->production_state = production_state;
}

std::vector<std::unique_ptr<Soldier>> &Factory::GetSoldierList() {
	return soldier_list;
}

std::vector<std::unique_ptr<Villager>> &Factory::GetVillagerList() {
	return villager_list;
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

} // namespace state
