/**
 * @file move_state.cpp
 * Defines the soldier move state
 */

#include "state/actor/soldier_states/soldier_move_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/soldier_attack_state.h"
#include "state/actor/soldier_states/soldier_dead_state.h"
#include "state/actor/soldier_states/soldier_idle_state.h"
#include "state/actor/soldier_states/soldier_pursuit_state.h"

namespace state {

SoldierMoveState::SoldierMoveState(Soldier *soldier)
    : SoldierState(SoldierStateName::MOVE, soldier) {}

void SoldierMoveState::Enter() {}

std::unique_ptr<IActorState> SoldierMoveState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		return std::make_unique<SoldierDeadState>(soldier);
	}

	// Check if there's an attack target set
	if (soldier->IsAttackTargetSet()) {
		if (not soldier->IsAttackTargetInRange()) {
			return std::make_unique<SoldierPursuitState>(soldier);
		} else {
			return std::make_unique<SoldierAttackState>(soldier);
		}
	}

	// Check if destination has been reached
	if (soldier->GetPosition() == soldier->GetDestination().to_double()) {
		return std::make_unique<SoldierIdleState>(soldier);
	}

	auto path_planner = soldier->GetPathPlanner();
	auto current_position = soldier->GetPosition();
	auto destination = soldier->GetDestination().to_double();
	auto speed = soldier->GetSpeed();

	auto next_position =
	    path_planner->GetNextPosition(current_position, destination, speed);

	if (next_position != DoubleVec2D::null) {
		soldier->SetNewPosition(next_position);
	}

	return nullptr;
}

void SoldierMoveState::Exit() {
	// Unset the destination on exit
	soldier->ClearDestination();
}
} // namespace state
