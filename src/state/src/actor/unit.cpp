/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/unit.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"

namespace state {

Unit::Unit() {
	// Init none
}
Unit::Unit(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
           int64_t max_hp, physics::Vector<int64_t> position, int64_t speed,
           int64_t attack_range, int64_t attack_damage)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      attack_range(attack_range), attack_damage(attack_damage),
      attack_target(nullptr), destination(physics::Vector<int64_t>(0, 0)),
      is_destination_set(false), new_position(physics::Vector<int64_t>(0, 0)),
      is_new_position_set(false), damage_incurred(0) {}

int64_t Unit::GetSpeed() { return speed; }

int64_t Unit::GetAttackRange() { return attack_range; }

int64_t Unit::GetAttackDamage() { return attack_damage; }

Actor *Unit::GetAttackTarget() { return attack_target; }

void Unit::SetAttackTarget(Actor *attack_target) {
	this->attack_target = attack_target;
};

bool Unit::IsAttackTargetInRange() {
	if (attack_target == nullptr) {
		throw std::logic_error("Attack target is not set");
	}
	auto target_position = attack_target->GetPosition();

	// Return true if the distance between the Unit and the target is
	// lesser than the attack_range
	return position.distance(target_position) <= attack_range;
}

physics::Vector<int64_t> Unit::GetDestination() { return destination; }

void Unit::SetDestination(physics::Vector<int64_t> destination) {
	this->destination = destination;
	this->is_destination_set = true;
}

void Unit::ClearDestination() { this->is_destination_set = false; }

bool Unit::IsDestinationSet() { return is_destination_set; }

physics::Vector<int64_t> Unit::GetNewPosition() { return new_position; }

void Unit::SetNewPosition(physics::Vector<int64_t> new_position) {
	this->new_position = new_position;
	this->is_new_position_set = true;
}

void Unit::ClearNewPosition() { this->is_new_position_set = false; }

bool Unit::IsNewPositionSet() { return is_new_position_set; }

bool Unit::IsAttackTargetSet() {
	return attack_target == nullptr ? false : true;
}

void Unit::SetPosition(physics::Vector<int64_t> position) {
	this->position = position;
}

void Unit::Move(physics::Vector<int64_t> destination) {
	this->destination = destination;
	this->is_destination_set = true;
	this->attack_target = nullptr;
}

void Unit::Attack(Actor *attack_target) {
	this->attack_target = attack_target;
	this->is_destination_set = false;
}

int64_t Unit::GetLatestHp() { return hp - damage_incurred; }

void Unit::Damage(int64_t damage_amount) {
	this->damage_incurred =
	    std::min<int64_t>(this->hp, this->damage_incurred + damage_amount);
}

} // namespace state
