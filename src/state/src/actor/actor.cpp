/**
 * @file actor.cpp
 * Defines the methods of the Actor class
 */

#include "state/actor/actor.h"
#include "physics/vector.hpp"
#include <memory>

namespace state {

Actor::Actor() {
	// Init None
}

Actor::Actor(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
             int64_t max_hp, DoubleVec2D position, GoldManager *gold_manager)
    : id(id), player_id(player_id), actor_type(actor_type), hp(hp),
      max_hp(max_hp), position(position), gold_manager(gold_manager),
      damage_incurred(0), age(0) {}

ActorId Actor::GetActorId() { return id; }

int64_t Actor::actor_id_increment = 0;
void Actor::SetActorIdIncrement(ActorId actor_id) {
	if (actor_id < 0) {
		throw std::out_of_range("`actor_id` cannot be negative");
	}
	actor_id_increment = actor_id;
}

ActorId Actor::GetNextActorId() { return actor_id_increment++; }

PlayerId Actor::GetPlayerId() { return player_id; }

ActorType Actor::GetActorType() { return actor_type; }

GoldManager *Actor::GetGoldManager() { return gold_manager; }

int64_t Actor::GetHp() { return hp; }

int64_t Actor::GetMaxHp() { return max_hp; }

void Actor::SetHp(int64_t hp) {
	if (hp < 0) {
		throw std::out_of_range("`hp` must be a positive value");
	}
	if (hp > this->max_hp) {
		throw std::out_of_range("`hp` cannot be greater than max_hp");
	}
	this->hp = hp;
}

int64_t Actor::GetLatestHp() { return hp - damage_incurred; }

void Actor::Damage(int64_t damage_amount) {
	this->damage_incurred =
	    std::min<int64_t>(this->hp, this->damage_incurred + damage_amount);
}

DoubleVec2D Actor::GetPosition() { return position; }

int64_t Actor::GetAge() { return age; }
} // namespace state
