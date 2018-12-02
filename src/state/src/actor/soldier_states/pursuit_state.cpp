/**
 * @file pursuit_state.cpp
 * Defines the soldier pursuit state
 */

#include "state/actor/soldier_states/pursuit_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier_states/move_state.h"

namespace state {

PursuitState::PursuitState(Soldier *soldier)
    : SoldierState(SoldierStateName::PURSUIT, soldier) {}

void PursuitState::Enter() {}

std::unique_ptr<IActorState> PursuitState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<DeadState>(soldier);
	}

	// Check if destination is set
	if (soldier->IsDestinationSet()) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<MoveState>(soldier);
	}

	// Check if the target is dead
	if (soldier->GetAttackTarget()->GetLatestHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<IdleState>(soldier);
	}

	// Check if target in range
	if (soldier->IsAttackTargetInRange()) {
		return std::make_unique<AttackState>(soldier);
	}

	// TODO: Set position of soldier to next position

	return nullptr;
}

void PursuitState::Exit() {}
} // namespace state
