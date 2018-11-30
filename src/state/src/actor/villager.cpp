/**
 * @file villager.cpp
 * Defines the methods of the Villager class
 */

#include "state/actor/villager.h"
#include "physics/vector.hpp"
#include "state/actor/unit.h"

namespace state {

Villager::Villager() {
	// Init none
}
Villager::Villager(ActorId id, PlayerId player_id, ActorType actor_type,
                   int64_t hp, int64_t max_hp,
                   physics::Vector<int64_t> position, GoldManager *gold_manager,
                   int64_t speed, int64_t attack_range, int64_t attack_damage)
    : Unit(id, player_id, actor_type, hp, max_hp, position, gold_manager, speed,
           attack_range, attack_damage) {}

void Villager::LateUpdate() {
	// TODO : State transitions such as to dead and damage
}

void Villager::Update() {
	// TODO : State update for the Villager
}
} // namespace state
