/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/soldier.h"
#include "physics/vector.hpp"
#include "state/actor/soldier_states/soldier_idle_state.h"
#include "state/actor/soldier_states/soldier_state.h"
#include "state/actor/unit.h"

namespace state {

Soldier::Soldier() {
	// Init none
}
Soldier::Soldier(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, DoubleVec2D position,
                 GoldManager *gold_manager, PathPlanner *path_planner,
                 int64_t speed, int64_t attack_range, int64_t attack_damage)
    : Unit(id, player_id, actor_type, hp, max_hp, position, gold_manager,
           path_planner, speed, attack_range, attack_damage),
      state(std::make_unique<SoldierIdleState>(this)) {}

SoldierStateName Soldier::GetState() { return state->GetName(); }

void Soldier::LateUpdate() {
	// If a move was performed, copy new_position into position
	if (IsNewPositionSet()) {
		SetPosition(new_position);
		ClearNewPosition();
	}

	// Update HP
	this->SetHp(this->GetLatestHp());

	// Allow soldier to transition to dead state if it's dead
	if (this->hp == 0 && state->GetName() != SoldierStateName::DEAD) {
		auto new_state = state->Update();
		state->Exit();
		state = std::unique_ptr<SoldierState>(
		    static_cast<SoldierState *>(new_state.release()));
		state->Enter();
		state->Update();
	}
}

void Soldier::Update() {
	auto new_state = state->Update();

	while (new_state != nullptr) {
		// State transition has occured
		state->Exit();
		state = std::unique_ptr<SoldierState>(
		    static_cast<SoldierState *>(new_state.release()));
		state->Enter();
		new_state = state->Update();
	}
}
} // namespace state
