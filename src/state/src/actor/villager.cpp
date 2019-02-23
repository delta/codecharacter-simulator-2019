/**
 * @file villager.cpp
 * Defines the methods of the Villager class
 */

#include "state/actor/villager.h"
#include "physics/vector.hpp"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/unit.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_state.h"

namespace state {

Villager::Villager() {
	// Init none
}
Villager::Villager(ActorId id, PlayerId player_id, ActorType actor_type,
                   int64_t hp, int64_t max_hp, DoubleVec2D position,
                   GoldManager *gold_manager, PathPlanner *path_planner,
                   int64_t speed, int64_t attack_range, int64_t attack_damage,
                   int64_t build_effort, int64_t build_range,
                   int64_t mine_range)
    : Unit(id, player_id, actor_type, hp, max_hp, position, gold_manager,
           path_planner, speed, attack_range, attack_damage),
      state(std::make_unique<VillagerIdleState>(this)),
      build_range(build_range), build_effort(build_effort),
      build_target(nullptr), mine_target(Vec2D{}), mine_target_set(false),
      mine_range(mine_range) {}

VillagerStateName Villager::GetState() { return state->GetName(); }

Factory *Villager::GetBuildTarget() { return build_target; }

int64_t Villager::GetBuildRange() { return build_range; }

int64_t Villager::GetBuildEffort() { return build_effort; }

void Villager::SetBuildTarget(Factory *factory) {
	this->build_target = factory;
}

bool Villager::IsBuildTargetSet() {
	return build_target == nullptr ? false : true;
}

bool Villager::IsBuildTargetInRange() {
	if (build_target == nullptr) {
		throw std::logic_error("Build target is not set");
	}
	auto build_position = build_target->GetPosition();

	// Return true if the distance between the Villager and the build target is
	// lesser than the build_range
	return position.distance(build_position) <= build_range;
}

void Villager::Build(Factory *build_target) {
	this->build_target = build_target;
	this->is_destination_set = false;
}

void Villager::Mine(Vec2D mine_target) { this->SetMineTarget(mine_target); }

void Villager::SetMineTarget(Vec2D mine_target) {
	this->mine_target = mine_target;
	this->mine_target_set = true;
}

Vec2D Villager::GetMineTarget() {
	if (!IsMineTargetSet()) {
		throw std::logic_error("No Mine target set!");
	}

	return this->mine_target;
}

int64_t Villager::GetMineRange() { return mine_range; }

void Villager::ClearMineTarget() { this->mine_target_set = false; }

bool Villager::IsMineTargetSet() { return this->mine_target_set; }

bool Villager::IsMineTargetInRange() {
	if (!IsMineTargetSet()) {
		throw std::logic_error("No Mine target set!");
	}

	return this->position.distance(mine_target.to_double()) <= mine_range;
}

void Villager::LateUpdate() {
	// If a move was performed, copy new_position into position
	if (IsNewPositionSet()) {
		SetPosition(new_position);
		ClearNewPosition();
	}

	// Update HP
	this->SetHp(this->GetLatestHp());
	this->damage_incurred = 0;

	// Allow villager to transition to dead state if it's dead
	if (this->hp == 0 && state->GetName() != VillagerStateName::DEAD) {
		auto new_state = state->Update();
		state->Exit();
		state = std::unique_ptr<VillagerState>(
		    static_cast<VillagerState *>(new_state.release()));
		state->Enter();
		state->Update();
	}
}

void Villager::Update() {
	age++;
	auto new_state = state->Update();

	while (new_state != nullptr) {
		// State transition has occured
		state->Exit();
		state = std::unique_ptr<VillagerState>(
		    static_cast<VillagerState *>(new_state.release()));
		state->Enter();
		new_state = state->Update();
	}
}
} // namespace state
