/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/soldier.h"
#include "physics/vector.hpp"
#include "state/actor/unit.h"

namespace state {

Soldier::Soldier() {
	// Init none
}
Soldier::Soldier(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector<int64_t> position,
                 int64_t speed, int64_t attack_range, int64_t attack_damage)
    : Unit(id, player_id, actor_type, hp, max_hp, position, speed, attack_range,
           attack_damage) {}

void Soldier::LateUpdate() {
	// TODO : State transitions such as to dead and damage
}

void Soldier::Update() {
	// TODO : State update for the soldier
}
} // namespace state
