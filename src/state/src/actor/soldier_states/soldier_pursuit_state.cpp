/**
 * @file pursuit_state.cpp
 * Defines the soldier pursuit state
 */

#include "state/actor/soldier_states/soldier_pursuit_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/soldier_attack_state.h"
#include "state/actor/soldier_states/soldier_dead_state.h"
#include "state/actor/soldier_states/soldier_idle_state.h"
#include "state/actor/soldier_states/soldier_move_state.h"

namespace state {

SoldierPursuitState::SoldierPursuitState(Soldier *soldier)
    : SoldierState(SoldierStateName::PURSUIT, soldier) {}

void SoldierPursuitState::Enter() {}

std::unique_ptr<IActorState> SoldierPursuitState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierDeadState>(soldier);
	}

	// Check if destination is set
	if (soldier->IsDestinationSet()) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierMoveState>(soldier);
	}

	// Check if the target is dead
	auto target = soldier->GetAttackTarget();
	if (target == nullptr || target->GetLatestHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierIdleState>(soldier);
	}

	// Check if target in range
	if (soldier->IsAttackTargetInRange()) {
		return std::make_unique<SoldierAttackState>(soldier);
	}

	auto path_planner = soldier->GetPathPlanner();
	auto current_position = soldier->GetPosition();
	auto destination = soldier->GetAttackTarget()->GetPosition();
	auto speed = soldier->GetSpeed();

	auto next_position =
	    path_planner->GetNextPosition(current_position, destination, speed);

	soldier->SetNewPosition(next_position);

	return nullptr;
}

void SoldierPursuitState::Exit() {}
} // namespace state
