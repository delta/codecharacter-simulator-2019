/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/unit.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/path_planner/path_planner.h"

namespace state {

Unit::Unit() {
	// Init none
}

Unit::Unit(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
           int64_t max_hp, DoubleVec2D position, GoldManager *gold_manager,
           ScoreManager *score_manager, PathPlanner *path_planner,
           int64_t speed, int64_t attack_range, int64_t attack_damage)
    : Actor(id, player_id, actor_type, hp, max_hp, position, gold_manager,
            score_manager),
      speed(speed), attack_range(attack_range), attack_damage(attack_damage),
      path_planner(path_planner), attack_target(nullptr), destination(Vec2D{}),
      is_destination_set(false), new_position(DoubleVec2D{}),
      is_new_position_set(false) {}

int64_t Unit::GetSpeed() { return speed; }

int64_t Unit::GetAttackRange() { return attack_range; }

int64_t Unit::GetAttackDamage() { return attack_damage; }

Actor *Unit::GetAttackTarget() { return attack_target; }

PathPlanner *Unit::GetPathPlanner() { return path_planner; }

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

Vec2D Unit::GetDestination() { return destination; }

void Unit::SetDestination(Vec2D destination) {
	this->destination = destination;
	this->is_destination_set = true;
}

void Unit::ClearDestination() { this->is_destination_set = false; }

bool Unit::IsDestinationSet() { return is_destination_set; }

DoubleVec2D Unit::GetNewPosition() { return new_position; }

void Unit::SetNewPosition(DoubleVec2D new_position) {
	this->new_position = new_position;
	this->is_new_position_set = true;
}

void Unit::ClearNewPosition() { this->is_new_position_set = false; }

bool Unit::IsNewPositionSet() { return is_new_position_set; }

bool Unit::IsAttackTargetSet() {
	return attack_target == nullptr ? false : true;
}

void Unit::SetPosition(DoubleVec2D position) { this->position = position; }

void Unit::Move(Vec2D destination) {
	this->destination = destination;
	this->is_destination_set = true;
	this->attack_target = nullptr;
}

void Unit::Attack(Actor *attack_target) {
	this->attack_target = attack_target;
	this->is_destination_set = false;
}

} // namespace state
