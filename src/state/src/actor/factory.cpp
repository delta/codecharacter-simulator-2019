/**
 * @file factory.cpp
 * Defines the Factory class
 */

#include "state/actor/factory.h"

#include <algorithm>

namespace state {

Factory::Factory() {}

Factory::Factory(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector<int64_t> position,
                 GoldManager *gold_manager, int64_t construction_complete,
                 int64_t construction_total, ProductionState production_state)
    : Actor(id, player_id, actor_type, hp, max_hp, position, gold_manager),
      construction_complete(construction_complete),
      construction_total(construction_total),
      production_state(production_state) {}

void Factory::IncrementConstructionCompletion(int64_t value) {
	construction_complete =
	    std::min(construction_complete + value, construction_total);
}

int64_t Factory::GetConstructionCompletion() { return construction_complete; }

int64_t Factory::GetTotalConstructionCompletion() { return construction_total; }

bool Factory::IsConstructionComplete() {
	return construction_complete == construction_total;
}

} // namespace state
