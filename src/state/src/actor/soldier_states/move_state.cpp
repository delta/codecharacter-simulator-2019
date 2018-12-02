/**
 * @file move_state.cpp
 * Defines the soldier move state
 */

#include "state/actor/soldier_states/move_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier_states/pursuit_state.h"

namespace state {

MoveState::MoveState(Soldier *soldier)
    : SoldierState(SoldierStateName::MOVE, soldier) {}

void MoveState::Enter() {}

std::unique_ptr<IActorState> MoveState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		return std::make_unique<DeadState>(soldier);
	}

	// Check if there's an attack target set
	if (soldier->IsAttackTargetSet()) {
		if (not soldier->IsAttackTargetInRange()) {
			return std::make_unique<PursuitState>(soldier);
		} else {
			return std::make_unique<AttackState>(soldier);
		}
	}

	// Check if destination has been reached
	if (soldier->GetPosition() == soldier->GetDestination()) {
		return std::make_unique<IdleState>(soldier);
	}

	// TODO: Move to next position of the soldier

	return nullptr;
}

void MoveState::Exit() {
	// Unset the destination on exit
	soldier->ClearDestination();
}
} // namespace state
